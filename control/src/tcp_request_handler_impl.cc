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

#include "tcp_request_handler_impl.h"
#include "tcp_attributes_builder.h"

using ::google::protobuf::util::Status;
using ::istio::mixer_client::CancelFunc;
using ::istio::mixer_client::DoneFunc;

namespace istio {
namespace mixer_control {

TcpRequestHandlerImpl::TcpRequestHandlerImpl(
    std::shared_ptr<ServiceContext> service_context,
    std::unique_ptr<TcpCheckData> check_data)
    : service_context_(service_context), check_data_(std::move(check_data)) {}

CancelFunc TcpRequestHandlerImpl::Check(DoneFunc on_done) {
  if (service_context_->enable_mixer_check() ||
      service_context_->enable_mixer_report()) {
    service_context_->AddStaticAttributes(&request_context_);

    TcpAttributesBuilder builder(&request_context_);
    builder.ExtractCheckAttributes(*check_data_);
  }

  if (!service_context_->enable_mixer_check()) {
    on_done(Status::OK);
    return nullptr;
  }

  return service_context_->client_context()->SendCheck(nullptr, on_done,
                                                       &request_context_);
}

// Make remote report call.
void TcpRequestHandlerImpl::Report(std::unique_ptr<TcpReportData> report_data) {
  if (!service_context_->enable_mixer_report()) {
    return;
  }
  TcpAttributesBuilder builder(&request_context_);
  builder.ExtractReportAttributes(*report_data);

  service_context_->client_context()->SendReport(request_context_);
}

}  // namespace mixer_control
}  // namespace istio
