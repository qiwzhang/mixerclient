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

#ifndef MIXERCONTROL_HTTP_REQUEST_HANDLER_IMPL_H
#define MIXERCONTROL_HTTP_REQUEST_HANDLER_IMPL_H

#include "client_context.h"
#include "control/include/http_request_handler.h"
#include "http_request_context.h"

namespace istio {
namespace mixer_control {

class HttpRequestHandlerImpl : public HttpRequestHandler {
 public:
  HttpRequestHandlerImpl(
      std::unique_ptr<HttpCheckData> check_data,
      std::shared_ptr<ClientContext> client_context,
      std::unique_ptr<::istio::mixer::v1::config::client::MixerControlConfig>
          per_route_config);

  ::istio::mixer_client::CancelFunc Check(
      ::istio::mixer_client::TransportCheckFunc transport,
      ::istio::mixer_client::DoneFunc on_done) override;

  void Report(std::unique_ptr<HttpReportData> report_data) override;

 private:
  std::unique_ptr<HttpRequestContext> request_context_;
};

}  // namespace mixer_control
}  // namespace istio

#endif  // MIXERCONTROL_HTTP_REQUEST_HANDLER_IMPL_H
