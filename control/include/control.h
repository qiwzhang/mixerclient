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

#ifndef MIXERCONTROL_CONTROLLER_H
#define MIXERCONTROL_CONTROLLER_H

#include "http_check_data.h"
#include "http_request_handler.h"

namespace istio {
namespace mixer_control {

class Controller {
 public:
  struct FactoryData {
    // Mixer filter config
    const ::istio::mixer::v1::config::client::MixerFilterConfig& mixer_config;

    // Transport functions.
    ::istio::mixer_client::TransportCheckFunc check_transport;
    ::istio::mixer_client::TransportReportFunc report_transport;
    ::istio::mixer_client::TimerCreateFunc timer_create_func;
    ::istio::mixer_client::UUIDGenerateFunc uuid_generate_func;
  };

  virtual std::unique_ptr<HttpRequestHandler> CreateHttpRequestHandler(
								       std::unique_ptr<HttpCheckData> check_data,
								       const ::istio::mixer::v1::config::client::MixerControlConfig& per_route_config) = 0;

  static std::unique_ptr<Controller> Create(std::unique_ptr<FactoryData> factory_data);
};

}  // namespace mixer_control
}  // namespace istio

#endif  // MIXERCONTROL_CONTROLLER_H
