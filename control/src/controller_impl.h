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
#include "service_context.h"

namespace istio {
namespace mixer_control {

// The class to implement Controller interface.
class ControllerImpl : public Controller {
 public:
  ControllerImpl(const Controller::FactoryData& data);

  // Creates a HTTP request handler
  std::unique_ptr<HttpRequestHandler> CreateHttpRequestHandler(
      std::unique_ptr<HttpCheckData> check_data,
      std::unique_ptr<::istio::mixer::v1::config::client::MixerControlConfig>
          per_route_config) override;

  // Creates a TCP request handler
  std::unique_ptr<TcpRequestHandler> CreateTcpRequestHandler(
      std::unique_ptr<TcpCheckData> check_data) override;

 private:
  // Create service config context for HTTP.
  std::shared_ptr<ServiceContext> GetHttpServiceContext(
      const HttpCheckData& check_data,
      std::unique_ptr<::istio::mixer::v1::config::client::MixerControlConfig>
          per_route_config);

  // Create service config context for TCP
  std::shared_ptr<ServiceContext> GetTcpServiceContext(
      const TcpCheckData& check_data);

  // The client context object to hold client config and client cache.
  std::shared_ptr<ClientContext> client_context_;

  // tcp service context
  std::shared_ptr<ServiceContext> tcp_service_context_;
};

}  // namespace mixer_control
}  // namespace istio

#endif  // MIXERCONTROL_CONTROLLER_IMPL_H
