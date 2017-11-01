/* Copyright 2017 Istio Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "http_request_context.h"

#include "attribute_names.h"
#include "include/attributes_builder.h"

#include <arpa/inet.h>

using ::google::protobuf::util::Status;
using ::google::protobuf::util::error::Code;
using ::istio::mixer::v1::Attributes;
using ::istio::mixer::v1::Attributes_StringMap;
using ::istio::mixer::v1::config::client::MixerFilterConfig;
using ::istio::mixer::v1::config::client::MixerControlConfig;
using ::istio::mixer_client::AttributesBuilder;
using ::istio::mixer_client::CancelFunc;
using ::istio::mixer_client::TransportCheckFunc;
using ::istio::mixer_client::DoneFunc;

namespace istio {
namespace mixer_control {
namespace {

// Pilot mesh attributes with the suffix will be treated as ipv4.
// They will use BYTES attribute type.
const std::string kIPSuffix = ".ip";

// Mesh attributes from Pilot are all string type.
// The attributes with ".ip" suffix will be treated
// as ipv4 and use BYTES attribute type.
void SetMeshAttribute(const std::string& name, const std::string& value,
                      Attributes* attr) {
  // Check with ".ip" suffix,
  if (name.length() <= kIPSuffix.length() ||
      name.compare(name.length() - kIPSuffix.length(), kIPSuffix.length(),
                   kIPSuffix) != 0) {
    AttributesBuilder(attr).AddString(name, value);
    return;
  }

  in_addr ipv4_bytes;
  if (inet_pton(AF_INET, value.c_str(), &ipv4_bytes) == 1) {
    AttributesBuilder(attr).AddBytes(
        name, std::string(reinterpret_cast<const char*>(&ipv4_bytes),
                          sizeof(ipv4_bytes)));
    return;
  }

  in6_addr ipv6_bytes;
  if (inet_pton(AF_INET6, value.c_str(), &ipv6_bytes) == 1) {
    AttributesBuilder(attr).AddBytes(
        name, std::string(reinterpret_cast<const char*>(&ipv6_bytes),
                          sizeof(ipv6_bytes)));
    return;
  }

  // TODO: add error log.
  // ENVOY_LOG(warn, "Could not convert to ip: {}: {}", name, value);
  AttributesBuilder(attr).AddString(name, value);
}

}  //  namespace

HttpRequestContext::HttpRequestContext(
    std::unique_ptr<HttpCheckData> check_data,
    std::shared_ptr<ClientContext> client_context,
    std::unique_ptr<MixerControlConfig> per_route_config)
    : check_data_(std::move(check_data)),
      client_context_(client_context),
      per_route_config_(std::move(per_route_config)) {}

void HttpRequestContext::FillRequestHeaderAttributes() {
  AttributesBuilder builder(&attributes_);
  std::map<std::string, std::string> headers = check_data_->GetRequestHeaders();
  builder.AddStringMap(AttributeName::kRequestHeaders, headers);

  struct TopLevelAttr {
    HttpCheckData::HeaderType header_type;
    const std::string& name;
    bool set_default;
    const char* default_value;
  };
  static TopLevelAttr attrs[] = {
      {HttpCheckData::HEADER_PATH, AttributeName::kRequestPath, true, ""},
      {HttpCheckData::HEADER_HOST, AttributeName::kRequestHost, true, ""},
      {HttpCheckData::HEADER_SCHEME, AttributeName::kRequestScheme, true,
       "http"},
      {HttpCheckData::HEADER_USER_AGENT, AttributeName::kRequestUserAgent,
       false, ""},
      {HttpCheckData::HEADER_METHOD, AttributeName::kRequestMethod, false, ""},
      {HttpCheckData::HEADER_REFERER, AttributeName::kRequestReferer, false,
       ""},
  };

  for (const auto& it : attrs) {
    std::string data;
    if (check_data_->FindRequestHeader(it.header_type, &data)) {
      builder.AddString(it.name, data);
    } else if (it.set_default) {
      builder.AddString(it.name, it.default_value);
    }
  }
}

void HttpRequestContext::AddForwardedAttributes(
    const std::string& forwarded_data) {
  Attributes v2_format;
  if (v2_format.ParseFromString(forwarded_data)) {
    attributes_.MergeFrom(v2_format);
    return;
  }

  // Legacy format from old proxy.
  Attributes_StringMap forwarded_attributes;
  if (forwarded_attributes.ParseFromString(forwarded_data)) {
    for (const auto& it : forwarded_attributes.entries()) {
      SetMeshAttribute(it.first, it.second, &attributes_);
    }
  }
}

void HttpRequestContext::ExtractCheckAttributes() {
  // Quota should be part of service_config().mixer_attributes.
  if (client_context()->config().has_mixer_attributes()) {
    attributes_.MergeFrom(client_context()->config().mixer_attributes());
  }
  if (per_route_config_->has_mixer_attributes()) {
    attributes_.MergeFrom(per_route_config_->mixer_attributes());
  }

  FillRequestHeaderAttributes();

  AttributesBuilder builder(&attributes_);

  std::string source_ip;
  int source_port;
  if (check_data_->GetSourceIpPort(&source_ip, &source_port)) {
    builder.AddBytes(AttributeName::kSourceIp, source_ip);
    builder.AddInt64(AttributeName::kSourcePort, source_port);
  }

  std::string source_user;
  if (check_data_->GetSourceUser(&source_user)) {
    builder.AddString(AttributeName::kSourceUser, source_user);
  }
  builder.AddTimestamp(AttributeName::kRequestTime,
                       std::chrono::system_clock::now());
  builder.AddString(AttributeName::kContextProtocol, "http");
}

void HttpRequestContext::ForwardAttributes() {
  if (client_context()->config().has_forward_attributes()) {
    std::string str;
    client_context()->config().forward_attributes().SerializeToString(&str);
    check_data_->AddIstioAttributes(str);
  }
}

void HttpRequestContext::ExtractReportAttributes(
    std::unique_ptr<HttpReportData> report_data) {
  AttributesBuilder builder(&attributes_);
  std::map<std::string, std::string> headers =
      report_data->GetResponseHeaders();
  builder.AddStringMap(AttributeName::kResponseHeaders, headers);

  builder.AddTimestamp(AttributeName::kResponseTime,
                       std::chrono::system_clock::now());

  HttpReportData::ReportInfo info;
  report_data->GetReportInfo(&info);
  builder.AddInt64(AttributeName::kRequestSize, info.received_bytes);
  builder.AddInt64(AttributeName::kResponseSize, info.send_bytes);
  builder.AddDuration(AttributeName::kResponseDuration, info.duration);
  if (check_status_code_ != 0) {
    builder.AddInt64(AttributeName::kResponseCode, check_status_code_);
  } else {
    builder.AddInt64(AttributeName::kResponseCode, info.response_code);
  }
}

CancelFunc HttpRequestContext::Check(TransportCheckFunc transport,
                                     DoneFunc on_done) {
  std::string forwarded_data;
  bool has_forwarded_data =
      check_data_->ExtractIstioAttributes(&forwarded_data);

  if (per_route_config_->enable_mixer_check() ||
      per_route_config_->enable_mixer_report()) {
    if (has_forwarded_data) {
      AddForwardedAttributes(forwarded_data);
    }

    ExtractCheckAttributes();
  }

  ForwardAttributes();

  if (!per_route_config_->enable_mixer_check()) {
    on_done(Status::OK);
    return nullptr;
  }

  auto my_on_done = [this,
                     on_done](const ::google::protobuf::util::Status& status) {
    // save the check status code
    check_status_code_ = status.error_code();
    on_done(status);
  };
  return client_context()->SendCheck(attributes_, transport, my_on_done);
}

void HttpRequestContext::Report(std::unique_ptr<HttpReportData> report_data) {
  // If attributes is empty, it means Mixer filter is not called.
  // not call Report for this case.
  // Report() is called at Log stage which can be called even mixer filter
  // is not called yet.
  if (!per_route_config_->enable_mixer_report() ||
      attributes_.attributes_size() == 0) {
    return;
  }
  ExtractReportAttributes(std::move(report_data));

  client_context()->SendReport(attributes_);
}

}  // namespace mixer_control
}  // namespace istio
