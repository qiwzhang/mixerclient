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
#include <functional>
#include <map>
#include <memory>
#include <string>

namespace istio {
namespace mixer_client {

// A structure to represent a bag of attributes with
// different types.
struct Attributes {
  // A structure to hold different types of value.
  struct Value {
    // Data type
    enum ValueType {
      STRING,
      INT64,
      DOUBLE,
      BOOL,
      TIME,
      BYTES,
      DURATION,
      STRING_MAP
    } type;

    // Data value
    union {
      int64_t int64_v;
      double double_v;
      bool bool_v;
    } value;
    // Move types with constructor outside of union.
    // It is not easy for union to support them.
    std::string str_v;  // for both STRING and BYTES
    std::chrono::time_point<std::chrono::system_clock> time_v;
    std::chrono::nanoseconds duration_nanos_v;
    std::map<std::string, std::string> string_map_v;

    // compare operator
    bool operator==(const Value& v) const;
  };

  // Helper functions to construct different value types
  static Value StringValue(const std::string& str);
  static Value BytesValue(const std::string& bytes);
  static Value Int64Value(int64_t value);
  static Value DoubleValue(double value);
  static Value BoolValue(bool value);
  static Value TimeValue(
      std::chrono::time_point<std::chrono::system_clock> value);
  static Value DurationValue(std::chrono::nanoseconds value);
  static Value StringMapValue(std::map<std::string, std::string>&& string_map);

  // Generates a string for logging or debugging.
  std::string DebugString() const;

  // The attribute map.
  std::map<std::string, Value> attributes;
};

// The attribute key to fill "quota" field in the QuotaRequest.
extern const std::string kQuotaName;
// The attribute key to fill "amount" field in the QuotaRequest.
extern const std::string kQuotaAmount;

}  // namespace mixer_client
}  // namespace istio

#endif  // MIXERCLIENT_ATTRIBUTE_H
