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

#include "controller_impl.h"
#include "http_request_handler_impl.h"
#include "tcp_request_handler_impl.h"

using ::istio::mixer::v1::config::client::MixerControlConfig;

namespace istio {
namespace mixer_control {

ControllerImpl::ControllerImpl(const Controller::FactoryData& data) {
  client_context_.reset(new ClientContext(data));
}

std::unique_ptr<HttpRequestHandler> ControllerImpl::CreateHttpRequestHandler(
    std::unique_ptr<HttpCheckData> check_data,
    std::unique_ptr<MixerControlConfig> per_route_config) {
  return std::unique_ptr<HttpRequestHandler>(new HttpRequestHandlerImpl(
      GetHttpServiceContext(*check_data, std::move(per_route_config)),
      std::move(check_data)));
}

std::unique_ptr<TcpRequestHandler> ControllerImpl::CreateTcpRequestHandler(
    std::unique_ptr<TcpCheckData> check_data) {
  return std::unique_ptr<TcpRequestHandler>(new TcpRequestHandlerImpl(
      GetTcpServiceContext(*check_data), std::move(check_data)));
}

std::shared_ptr<ServiceContext> ControllerImpl::GetHttpServiceContext(
    const HttpCheckData&,
    std::unique_ptr<MixerControlConfig> per_route_config) {
  // TODO: use check data to find config from control_configs
  // TODO: cache the service config per destination.service.
  return std::make_shared<ServiceContext>(client_context_, *per_route_config);
}

std::shared_ptr<ServiceContext> ControllerImpl::GetTcpServiceContext(
    const TcpCheckData&) {
  if (!tcp_service_context_) {
    MixerControlConfig config;
    // Report is always on
    config.set_enable_mixer_report(true);
    config.set_enable_mixer_check(
        !client_context_->config().disable_tcp_check_calls());
    tcp_service_context_ =
        std::make_shared<ServiceContext>(client_context_, config);
  }
  return tcp_service_context_;
}

std::unique_ptr<Controller> Controller::Create(
    const FactoryData& factory_data) {
  return std::unique_ptr<Controller>(new ControllerImpl(factory_data));
}

}  // namespace mixer_control
}  // namespace istio
