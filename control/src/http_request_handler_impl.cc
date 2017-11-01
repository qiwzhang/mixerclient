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

#include "http_request_handler_impl.h"

using ::istio::mixer::v1::config::client::MixerControlConfig;

using ::google::protobuf::util::Status;
using ::google::protobuf::util::error::Code;
using ::istio::mixer::v1::Attributes;
using ::istio::mixer_client::CheckOptions;
using ::istio::mixer_client::ReportOptions;
using ::istio::mixer_client::QuotaOptions;
using ::istio::mixer_client::CancelFunc;
using ::istio::mixer_client::TransportCheckFunc;
using ::istio::mixer_client::DoneFunc;

namespace istio {
namespace mixer_control {

HttpRequestHandlerImpl::HttpRequestHandlerImpl(
    std::unique_ptr<HttpCheckData> check_data,
    std::shared_ptr<ClientContext> client_context,
    std::unique_ptr<MixerControlConfig> per_route_config) {
  request_context_.reset(new HttpRequestContext(
      std::move(check_data), client_context, std::move(per_route_config)));
}

istio::mixer_client::CancelFunc HttpRequestHandlerImpl::Check(
    ::istio::mixer_client::TransportCheckFunc transport,
    ::istio::mixer_client::DoneFunc on_done) {
  return request_context_->Check(transport, on_done);
}

// Make remote report call.
void HttpRequestHandlerImpl::Report(std::unique_ptr<HttpReportData> response) {
  request_context_->Report(std::move(response));
}

}  // namespace mixer_control
}  // namespace istio
