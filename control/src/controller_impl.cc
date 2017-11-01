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

#include "control/src/http_request_handler_impl.h"

using ::google::protobuf::util::Status;
using ::google::protobuf::util::error::Code;
using ::istio::mixer::v1::Attributes;
using ::istio::mixer::v1::config::client::MixerFilterConfig;
using ::istio::mixer::v1::config::client::MixerControlConfig;
using ::istio::mixer_client::CheckOptions;
using ::istio::mixer_client::ReportOptions;
using ::istio::mixer_client::QuotaOptions;
using ::istio::mixer_client::CancelFunc;
using ::istio::mixer_client::TransportCheckFunc;
using ::istio::mixer_client::DoneFunc;

namespace istio {
namespace mixer_control {

ControllerImpl::ControllerImpl(const Controller::FactoryData& data) {
  client_context.reset(new ClientContext(data));
}

std::unique_ptr<HttpRequestHandler> ControllerImpl::CreateHttpRequestHandler(
    std::unique_ptr<HttpCheckData> check_data,
    std::unique_ptr<MixerControlConfig> per_route_config) {
  return std::unique_ptr<HttpRequestHandler>(new HttpRequestHandlerImpl(
      check_data, client_context_, std::move(per_route_config)));
}

std::unique_ptr<TcpRequestHandler> ControllerImpl::CreateTcpRequestHandler(
    std::unique_ptr<TcpCheckData> check_data,
    const PerRouteConfig& per_route_config) {
    return std::unique_ptr<TcpRequestHandler>(new TcpRequestHandlerImpl(client_context_);
}

std::unique_ptr<Controller> Controller::Create(
    const FactoryData& factory_data) {
  return std::unique_ptr<Controller>(new ControllerImpl(factory_data));
}

}  // namespace mixer_control
}  // namespace istio
