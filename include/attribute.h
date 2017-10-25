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

#ifndef MIXERCLIENT_ATTRIBUTE_H
#define MIXERCLIENT_ATTRIBUTE_H

#include <chrono>
#include <map>
#include <string>

#include "mixer/v1/attributes.pb.h"

namespace istio {
namespace mixer_client {

// Helper functions to add attribute different value types
struct AttributesHelper {
  static void AddString(const std::string key, const std::string& str,
                        ::istio::mixer::v1::Attributes* attributes);
  static void AddBytes(const std::string key, const std::string& bytes,
                       ::istio::mixer::v1::Attributes* attributes);
  static void AddInt64(const std::string key, int64_t value,
                       ::istio::mixer::v1::Attributes* attributes);
  static void AddDouble(const std::string key, double value,
                        ::istio::mixer::v1::Attributes* attributes);
  static void AddBool(const std::string key, bool value,
                      ::istio::mixer::v1::Attributes* attributes);
  static void AddTime(const std::string key,
                      std::chrono::time_point<std::chrono::system_clock> value,
                      ::istio::mixer::v1::Attributes* attributes);
  static void AddDuration(const std::string key, std::chrono::nanoseconds value,
                          ::istio::mixer::v1::Attributes* attributes);
  static void AddStringMap(const std::string key,
                           std::map<std::string, std::string>&& string_map,
                           ::istio::mixer::v1::Attributes* attributes);

  // The attribute key to fill "quota" field in the QuotaRequest.
  static const std::string kQuotaName;
  // The attribute key to fill "amount" field in the QuotaRequest.
  static const std::string kQuotaAmount;
};

}  // namespace mixer_client
}  // namespace istio

#endif  // MIXERCLIENT_ATTRIBUTE_H
