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

#ifndef MIXERCONTROL_MOCK_TCP_REPORT_DATA_H
#define MIXERCONTROL_MOCK_TCP_REPORT_DATA_H

#include "control/include/tcp_report_data.h"
#include "gmock/gmock.h"

namespace istio {
namespace mixer_control {

// The mock object for TcpReportData interface.
class MockTcpReportData : public TcpReportData {
 public:
  MOCK_CONST_METHOD2(GetDestinationIpPort, bool(std::string* ip, int* port));
  MOCK_CONST_METHOD1(GetReportInfo, void(ReportInfo* info));
};

}  // namespace mixer_control
}  // namespace istio

#endif  // MIXERCONTROL_MOCK_TCP_REPORT_DATA_H
