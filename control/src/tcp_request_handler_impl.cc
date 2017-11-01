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

using ::istio::mixer_client::CancelFunc;
using ::istio::mixer_client::DoneFunc;

namespace istio {
namespace mixer_control {

TcpRequestHandlerImpl::TcpRequestHandlerImpl(
    std::unique_ptr<TcpCheckData> check_data,
    std::shared_ptr<ClientContext> client_context) {
  request_context_.reset(
      new TcpRequestContext(std::move(check_data), client_context));
}

CancelFunc TcpRequestHandlerImpl::Check(DoneFunc on_done) {
  return request_context_->Check(on_done);
}

// Make remote report call.
void TcpRequestHandlerImpl::Report(std::unique_ptr<TcpReportData> response) {
  request_context_->Report(std::move(response));
}

}  // namespace mixer_control
}  // namespace istio
