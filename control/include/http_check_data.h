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

#ifndef MIXERCONTROL_HTTP_CHECK_DATA_H
#define MIXERCONTROL_HTTP_CHECK_DATA_H

namespace istio {
namespace mixer_control {

// Interface class to extract data for Mixer check call.
class HttpCheckData {
 public:
  // Find "x-istio-attributes" headers, if found base64 decode
  // its value and remove it from the headers.
  virtual bool ExtractIstioAttributes(std::string* data) = 0;
  // base64 encode data, and add it to the HTTP header.
  virtual void AddIstoAttributes(const std::string& data) = 0;

  // Get client tcp connection ip and port.
  virtual bool GetSourceIpPort(std::string* ip, int* port) const = 0;

  // If SSL is used, get origin user name.
  virtual bool GetSourceUser(std::string* user) const = 0;

  // Get reqeust HTTP headers
  virtual std::map<std::string, std::string> GetRequestHeaders() const = 0;

  // These headers are extracted into top level attributes
  // They can be retrieved at O(1) speed by platform.
  enum HeaderType {
    HEADER_PATH = 0,
    HEADER_HOST,
    HEADER_SCHEME,
    HEADER_USER_AGENT,
    HEADER_METHOD,
    HEADER_REFERER,
  };
  virtual bool GetHeader(HeaderType header_type, std::string* value) const = 0;
};

}  // namespace mixer_control
}  // namespace istio

#endif  // MIXERCONTROL_HTTP_CHECK_DATA_H
