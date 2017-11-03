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

#ifndef MIXERCONTROL_HTTP_REQUEST_CONTEXT_H
#define MIXERCONTROL_HTTP_REQUEST_CONTEXT_H

#include "client_context.h"
#include "control/include/http_check_data.h"
#include "control/include/http_request_handler.h"

namespace istio {
namespace mixer_control {

// The context for each HTTP request.
class HttpRequestContext {
 public:
  HttpRequestContext(
      std::unique_ptr<HttpCheckData> check_data,
      std::shared_ptr<ClientContext> client_context,
      std::unique_ptr<::istio::mixer::v1::config::client::MixerControlConfig>
          per_route_config);

  // Make a Check call.
  ::istio::mixer_client::CancelFunc Check(
      ::istio::mixer_client::TransportCheckFunc transport,
      ::istio::mixer_client::DoneFunc on_done);

  // Make a Report call.
  void Report(std::unique_ptr<HttpReportData> report_data);

 private:
  // Extract attributes for Check call.
  void ExtractCheckAttributes();
  // Extract attributes for Report call.
  void ExtractReportAttributes(std::unique_ptr<HttpReportData> report_data);
  // Forward attributes to upstream proxy.
  void ForwardAttributes();
  // Extract forwarded attributes from client proxy.
  void ExtractForwardedAttributes(const std::string& forwarded_data);
  // Extract HTTP header attributes
  void ExtractRequestHeaderAttributes();

  // The check data object.
  std::unique_ptr<HttpCheckData> check_data_;
  // The client context object.
  std::shared_ptr<ClientContext> client_context_;
  // The per-route config.
  std::unique_ptr<::istio::mixer::v1::config::client::MixerControlConfig>
      per_route_config_;

  // The attributes used for both Check and Report.
  // All Check attributes will be send to Report too.
  ::istio::mixer::v1::Attributes attributes_;
  // The check call status
  int check_status_code_;
};

}  // namespace mixer_control
}  // namespace istio

#endif  // MIXERCONTROL_HTTP_REQUEST_CONTEXT_H
