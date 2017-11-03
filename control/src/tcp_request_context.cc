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

#include "tcp_request_context.h"

#include "attribute_names.h"
#include "include/attributes_builder.h"

using ::google::protobuf::util::Status;
using ::istio::mixer_client::AttributesBuilder;
using ::istio::mixer_client::CancelFunc;
using ::istio::mixer_client::TransportCheckFunc;
using ::istio::mixer_client::DoneFunc;

namespace istio {
namespace mixer_control {

TcpRequestContext::TcpRequestContext(
    std::unique_ptr<TcpCheckData> check_data,
    std::shared_ptr<ClientContext> client_context)
    : check_data_(std::move(check_data)), client_context_(client_context) {}

void TcpRequestContext::ExtractCheckAttributes() {
  if (client_context_->config().has_mixer_attributes()) {
    attributes_.MergeFrom(client_context_->config().mixer_attributes());
  }

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
  builder.AddTimestamp(AttributeName::kContextTime,
                       std::chrono::system_clock::now());
  builder.AddString(AttributeName::kContextProtocol, "tcp");
}

void TcpRequestContext::ExtractReportAttributes(
    std::unique_ptr<TcpReportData> report_data) {
  AttributesBuilder builder(&attributes_);

  TcpReportData::ReportInfo info;
  report_data->GetReportInfo(&info);
  builder.AddInt64(AttributeName::kConnectionReceviedBytes,
                   info.received_bytes);
  builder.AddInt64(AttributeName::kConnectionReceviedTotalBytes,
                   info.received_bytes);
  builder.AddInt64(AttributeName::kConnectionSendBytes, info.send_bytes);
  builder.AddInt64(AttributeName::kConnectionSendTotalBytes, info.send_bytes);
  builder.AddDuration(AttributeName::kConnectionDuration, info.duration);
  builder.AddInt64(AttributeName::kCheckStatusCode, check_status_code_);

  std::string dest_ip;
  int dest_port;
  if (report_data->GetDestinationIpPort(&dest_ip, &dest_port)) {
    builder.AddBytes(AttributeName::kDestinationIp, dest_ip);
    builder.AddInt64(AttributeName::kDestinationPort, dest_port);
  }

  builder.AddTimestamp(AttributeName::kContextTime,
                       std::chrono::system_clock::now());
}

CancelFunc TcpRequestContext::Check(DoneFunc on_done) {
  ExtractCheckAttributes();

  if (client_context_->config().disable_tcp_check_calls()) {
    on_done(Status::OK);
    return nullptr;
  }

  auto my_on_done = [this, on_done](const Status& status) {
    // save the check status code
    check_status_code_ = status.error_code();
    on_done(status);
  };
  return client_context_->SendCheck(attributes_, nullptr, my_on_done);
}

void TcpRequestContext::Report(std::unique_ptr<TcpReportData> report_data) {
  ExtractReportAttributes(std::move(report_data));

  client_context_->SendReport(attributes_);
}

}  // namespace mixer_control
}  // namespace istio
