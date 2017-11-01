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

#ifndef MIXERCONTROL_CONTROLLER_IMPL_H
#define MIXERCONTROL_CONTROLLER_IMPL_H

#include <memory>

#include "client_context.h"
#include "control/include/controller.h"

namespace istio {
namespace mixer_control {

class ControllerImpl : public Controller {
 public:
  ControllerImpl(const Controller::FactoryData& data);

  std::unique_ptr<HttpRequestHandler> CreateHttpRequestHandler(
      std::unique_ptr<HttpCheckData> check_data,
      std::unique_ptr<::istio::mixer::v1::config::client::MixerControlConfig>
          per_route_config) override;

#if 0  
  std::unique_ptr<TcpRequestHandler> CreateTcpRequestHandler(
      std::unique_ptr<TcpRequestData> request) override;
#endif

 private:
  std::shared_ptr<ClientContext> client_context_;
};

}  // namespace mixer_control
}  // namespace istio

#endif  // MIXERCONTROL_CONTROLLER_IMPL_H
