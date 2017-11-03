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

#ifndef MIXERCONTROL_TCP_REQUEST_CONTEXT_H
#define MIXERCONTROL_TCP_REQUEST_CONTEXT_H

#include "client_context.h"
#include "control/include/tcp_check_data.h"
#include "control/include/tcp_request_handler.h"

namespace istio {
namespace mixer_control {

// The context to hold TCP request data.
class TcpRequestContext {
 public:
  TcpRequestContext(std::unique_ptr<TcpCheckData> check_data,
                    std::shared_ptr<ClientContext> client_context);

  // Make a Check call.
  ::istio::mixer_client::CancelFunc Check(
      ::istio::mixer_client::DoneFunc on_done);

  // Make a Report call.
  void Report(std::unique_ptr<TcpReportData> report_data);

 private:
  // Extract attributes for Check.
  void ExtractCheckAttributes();
  // Extract attributes for Report.
  void ExtractReportAttributes(std::unique_ptr<TcpReportData> report_data);

  // The check data object.
  std::unique_ptr<TcpCheckData> check_data_;
  // The client context object.
  std::shared_ptr<ClientContext> client_context_;

  // The attributes for both Check and Report.
  ::istio::mixer::v1::Attributes attributes_;
  // The check status code.
  int check_status_code_ = 0;
};

}  // namespace mixer_control
}  // namespace istio

#endif  // MIXERCONTROL_TCP_REQUEST_CONTEXT_H
