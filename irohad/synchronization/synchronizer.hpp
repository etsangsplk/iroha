/*
Copyright Soramitsu Co., Ltd. 2016 All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

//
// Created by Takumi Yamashita on 2017/04/28.
//

#ifndef IROHA_PEER_SERVICE_SYNCHRONIZER_SYNCHRONIZER_H
#define IROHA_PEER_SERVICE_SYNCHRONIZER_SYNCHRONIZER_H

#include <block.pb.h>
#include <string>

namespace iroha {
  namespace synchronization {

  class Synchronizer {
    virtual rxcpp::observable<model::Block> process(model::Block commit) = 0;

    virtual rxcpp::observable<rxcpp::observable<model::Block>> on_commit_chain() = 0;
  };

  }  // namespace synchronization
}  // namespace iroha

#endif  // IROHA_PEER_SERVICE_SYNCHRONIZER_SYNCHRONIZER_H
