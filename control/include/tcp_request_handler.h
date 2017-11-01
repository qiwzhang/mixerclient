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

#ifndef MIXERCONTROL_TCP_REQUEST_HANDLER_H
#define MIXERCONTROL_TCP_REQUEST_HANDLER_H

#include "include/client.h"
#include "tcp_check_data.h"
#include "tcp_report_data.h"

namespace istio {
namespace mixer_control {

// Interface class to handle a TCP request.
class TcpRequestHandler {
 public:
  virtual ~TcpRequestHandler() {}

  virtual ::istio::mixer_client::CancelFunc Check(
      ::istio::mixer_client::DoneFunc on_done) = 0;

  // Make remote report call.
  virtual void Report(std::unique_ptr<TcpReportData> report_data) = 0;
};

}  // namespace mixer_control
}  // namespace istio

#endif  // MIXERCONTROL_TCP_REQUEST_HANDLER_H
