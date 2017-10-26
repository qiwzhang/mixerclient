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

#include "referenced.h"

#include "global_dictionary.h"
#include "utils/md5.h"

#include <map>
#include <set>
#include <sstream>

using ::istio::mixer::v1::Attributes;
using ::istio::mixer::v1::Attributes_AttributeValue;
using ::istio::mixer::v1::ReferencedAttributes;

namespace istio {
namespace mixer_client {
namespace {
const char kDelimiter[] = "\0";
const int kDelimiterLength = 1;
const std::string kWordDelimiter = ":";
}  // namespace

bool Referenced::Fill(const ReferencedAttributes& reference) {
  const std::vector<std::string>& global_words = GetGlobalWords();

  for (const auto& match : reference.attribute_matches()) {
    std::string name;
    // name index from Mixer server needs to be decoded into string
    // based on the global dictionary and per-message word list.
    int idx = match.name();
    if (idx >= 0) {
      if ((unsigned int)idx >= global_words.size()) {
        GOOGLE_LOG(ERROR) << "Global word index is too big: " << idx
                          << " >= " << global_words.size();
        return false;
      }
      name = global_words[idx];
    } else {
      // per-message index is negative, its format is:
      //    per_message_idx = -(array_idx + 1)
      idx = -idx - 1;
      if (idx >= reference.words_size()) {
        GOOGLE_LOG(ERROR) << "Per message word index is too big: " << idx
                          << " >= " << reference.words_size();
        return false;
      }
      name = reference.words(idx);
    }

    if (match.condition() == ReferencedAttributes::ABSENCE) {
      absence_keys_.push_back(name);
    } else if (match.condition() == ReferencedAttributes::EXACT) {
      exact_keys_.push_back(name);
    } else if (match.condition() == ReferencedAttributes::REGEX) {
      // Don't support REGEX yet, return false to no caching the response.
      GOOGLE_LOG(ERROR) << "Received REGEX in ReferencedAttributes for "
                        << name;
      return false;
    }
  }

  return true;
}

bool Referenced::Signature(const Attributes& attributes,
                           const std::string& extra_key,
                           std::string* signature) const {
  const auto& attributes_map = attributes.attributes();
  for (const std::string& key : absence_keys_) {
    // if an "absence" key exists, return false for mis-match.
    if (attributes_map.find(key) != attributes_map.end()) {
      return false;
    }
  }

  MD5 hasher;
  for (const std::string& key : exact_keys_) {
    const auto it = attributes_map.find(key);
    // if an "exact" attribute not present, return false for mismatch.
    if (it == attributes_map.end()) {
      return false;
    }

    hasher.Update(it->first);
    hasher.Update(kDelimiter, kDelimiterLength);

    const Attributes_AttributeValue& value = it->second;
    switch (value.value_case()) {
      case Attributes_AttributeValue::kStringValue:
        hasher.Update(value.string_value());
        break;
      case Attributes_AttributeValue::kBytesValue:
        hasher.Update(value.bytes_value());
        break;
      case Attributes_AttributeValue::kInt64Value: {
        auto data = value.int64_value();
        hasher.Update(&data, sizeof(data));
      } break;
      case Attributes_AttributeValue::kDoubleValue: {
        auto data = value.double_value();
        hasher.Update(&data, sizeof(data));
      } break;
      case Attributes_AttributeValue::kBoolValue: {
        auto data = value.bool_value();
        hasher.Update(&data, sizeof(data));
      } break;
      case Attributes_AttributeValue::kTimestampValue: {
        auto seconds = value.timestamp_value().seconds();
        auto nanos = value.timestamp_value().nanos();
        hasher.Update(&seconds, sizeof(seconds));
        hasher.Update(kDelimiter, kDelimiterLength);
        hasher.Update(&nanos, sizeof(nanos));
      } break;
      case Attributes_AttributeValue::kDurationValue: {
        auto seconds = value.duration_value().seconds();
        auto nanos = value.duration_value().nanos();
        hasher.Update(&seconds, sizeof(seconds));
        hasher.Update(kDelimiter, kDelimiterLength);
        hasher.Update(&nanos, sizeof(nanos));
      } break;
      case Attributes_AttributeValue::kStringMapValue: {
        // protobuf map doesn't have deterministic order:
        // each run the order is different.
        // copy to std::map with deterministic order.
        const auto& pb_map = value.string_map_value().entries();
        std::map<std::string, std::string> stl_map(pb_map.begin(),
                                                   pb_map.end());
        for (const auto& sub_it : stl_map) {
          hasher.Update(sub_it.first);
          hasher.Update(kDelimiter, kDelimiterLength);
          hasher.Update(sub_it.second);
          hasher.Update(kDelimiter, kDelimiterLength);
        }
      } break;
      case Attributes_AttributeValue::VALUE_NOT_SET:
        break;
    }
    hasher.Update(kDelimiter, kDelimiterLength);
  }
  hasher.Update(extra_key);

  *signature = hasher.Digest();
  return true;
}

std::string Referenced::Hash() const {
  MD5 hasher;
  // Use std::set to sort the words
  std::set<std::string> sorted_absence_keys(absence_keys_.begin(),
                                            absence_keys_.end());
  for (const auto& key : sorted_absence_keys) {
    hasher.Update(key);
    hasher.Update(kDelimiter, kDelimiterLength);
  }
  hasher.Update(kWordDelimiter);
  std::set<std::string> sorted_exact_keys(exact_keys_.begin(),
                                          exact_keys_.end());
  for (const auto& key : sorted_exact_keys) {
    hasher.Update(key);
    hasher.Update(kDelimiter, kDelimiterLength);
  }

  return hasher.Digest();
}

std::string Referenced::DebugString() const {
  std::stringstream ss;
  ss << "Absence-keys: ";
  for (const auto& key : absence_keys_) {
    ss << key + ", ";
  }
  ss << "Exact-keys: ";
  for (const auto& key : exact_keys_) {
    ss << key + ", ";
  }
  return ss.str();
}

}  // namespace mixer_client
}  // namespace istio
