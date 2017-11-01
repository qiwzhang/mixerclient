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

#ifndef MIXERCONTROL_HTTP_REQUEST_HANDLER_H
#define MIXERCONTROL_HTTP_REQUEST_HANDLER_H

#include "http_check_data.h"
#include "http_report_data.h"
#include "include/client.h"

namespace istio {
namespace mixer_control {

// Interface class to handle a HTTP request.
class HttpRequestHandler {
 public:
  virtual ~HttpRequestHandler() {}

  virtual ::istio::mixer_client::CancelFunc Check(
      ::istio::mixer_client::TransportCheckFunc transport,
      ::istio::mixer_client::DoneFunc on_done) = 0;

  // Make remote report call.
  virtual void Report(std::unique_ptr<HttpReportData> report_data) = 0;
};

}  // namespace mixer_control
}  // namespace istio

#endif  // MIXERCONTROL_HTTP_REQUEST_HANDLER_H
