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

#ifndef MIXERCONTROL_CLIENT_CONTEXT_H
#define MIXERCONTROL_CLIENT_CONTEXT_H

#include "control/include/controller.h"

namespace istio {
namespace mixer_control {

class ClientContext {
 public:
  ClientContext(const Controller::FactoryData& data);

  ::istio::mixer_client::CancelFunc SendCheck(
      const ::istio::mixer::v1::Attributes& attributes,
      ::istio::mixer_client::TransportCheckFunc transport,
      ::istio::mixer_client::DoneFunc on_done);

  void SendReport(const ::istio::mixer::v1::Attributes& attributes);

  const ::istio::mixer::v1::config::client::MixerFilterConfig& config() const {
    return config_;
  }

 private:
  // The mixer client
  std::unique_ptr<::istio::mixer_client::MixerClient> mixer_client_;
  const ::istio::mixer::v1::config::client::MixerFilterConfig& config_;
};

}  // namespace mixer_control
}  // namespace istio

#endif  // MIXERCONTROL_CLIENT_CONTEXT_H
