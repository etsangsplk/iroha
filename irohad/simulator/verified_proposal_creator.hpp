/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef IROHA_VERIFIED_PROPOSAL_CREATOR_HPP
#define IROHA_VERIFIED_PROPOSAL_CREATOR_HPP

#include <model/model.hpp>

namespace iroha {
namespace simulator{
class VerifiedProposalCreator {
  /**
   *
   */
  virtual model::Proposal process_proposal(model::Proposal);

  virtual rxcpp::observervable<model::Proposal> on_verified_proposal();

};
}
}

#endif //IROHA_VERIFIED_PROPOSAL_CREATOR_HPP
