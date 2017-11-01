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

#ifndef MIXERCONTROL_HTTP_REPORT_DATA_H
#define MIXERCONTROL_HTTP_REPORT_DATA_H

namespace istio {
namespace mixer_control {

// Interface class to extract data for Mixer report call.
class HttpReportData {
 public:
  struct ReportInfo {
    uint64_t send_bytes;
    uint64_t received_bytes;
    std::chrono::nanoseconds duration;
    int response_code;
  };
  virtual void GetReportInfo(ReportInfo* info) const = 0;

  // Get reqeust HTTP headers
  virtual std::map<std::string, std::string> GetResponseHeaders() const = 0;
};

}  // namespace mixer_control
}  // namespace istio

#endif  // MIXERCONTROL_HTTP_REPORT_DATA_H
