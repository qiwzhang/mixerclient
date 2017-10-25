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

#include "include/attribute.h"
#include "utils/protobuf.h"

using ::istio::mixer::v1::Attributes;
using ::istio::mixer::v1::Attributes_AttributeValue;
using ::istio::mixer::v1::Attributes_StringMap;

namespace istio {
namespace mixer_client {

const std::string AttributesHelper::kQuotaName = "quota.name";
const std::string AttributesHelper::kQuotaAmount = "quota.amount";

void AttributesHelper::AddString(const std::string key, const std::string& str,
                                 Attributes* attributes) {
  Attributes_AttributeValue value_pb;
  value_pb.set_string_value(str);
  (*attributes->mutable_attributes())[key] = value_pb;
}

void AttributesHelper::AddBytes(const std::string key, const std::string& bytes,
                                Attributes* attributes) {
  Attributes_AttributeValue value_pb;
  value_pb.set_bytes_value(bytes);
  (*attributes->mutable_attributes())[key] = value_pb;
}

void AttributesHelper::AddInt64(const std::string key, int64_t value,
                                Attributes* attributes) {
  Attributes_AttributeValue value_pb;
  value_pb.set_int64_value(value);
  (*attributes->mutable_attributes())[key] = value_pb;
}

void AttributesHelper::AddDouble(const std::string key, double value,
                                 Attributes* attributes) {
  Attributes_AttributeValue value_pb;
  value_pb.set_double_value(value);
  (*attributes->mutable_attributes())[key] = value_pb;
}

void AttributesHelper::AddBool(const std::string key, bool value,
                               Attributes* attributes) {
  Attributes_AttributeValue value_pb;
  value_pb.set_bool_value(value);
  (*attributes->mutable_attributes())[key] = value_pb;
}

void AttributesHelper::AddTime(
    const std::string key,
    std::chrono::time_point<std::chrono::system_clock> value,
    Attributes* attributes) {
  Attributes_AttributeValue value_pb;
  *value_pb.mutable_timestamp_value() = CreateTimestamp(value);
  (*attributes->mutable_attributes())[key] = value_pb;
}

void AttributesHelper::AddDuration(const std::string key,
                                   std::chrono::nanoseconds value,
                                   Attributes* attributes) {
  Attributes_AttributeValue value_pb;
  *value_pb.mutable_duration_value() = CreateDuration(value);
  (*attributes->mutable_attributes())[key] = value_pb;
}

void AttributesHelper::AddStringMap(
    const std::string key, std::map<std::string, std::string>&& string_map,
    Attributes* attributes) {
  Attributes_AttributeValue value_pb;
  auto entries = value_pb.mutable_string_map_value()->mutable_entries();
  for (const auto& map_it : string_map) {
    (*entries)[map_it.first] = map_it.second;
  }
  (*attributes->mutable_attributes())[key] = value_pb;
}

}  // namespace mixer_client
}  // namespace istio
