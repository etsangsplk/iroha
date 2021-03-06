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

#include <ametsuchi/ametsuchi.h>
#include <gtest/gtest.h>
#include <endpoint_generated.h>
#include <ametsuchi/exception.h>
#include "../generator/tx_generator.h"

class Ametsuchi_Test : public ::testing::Test {
 protected:
  virtual void SetUp() { system(("rm -rf " + folder).c_str()); }
  virtual void TearDown() { system(("rm -rf " + folder).c_str()); }

  std::string folder = "/tmp/ametsuchi/";
  ametsuchi::Ametsuchi ametsuchi_;

  Ametsuchi_Test() : ametsuchi_(folder) {}
};

TEST_F(Ametsuchi_Test, AssetTest) {
  // ASSERT_NO_THROW({
  flatbuffers::FlatBufferBuilder fbb(2048);

  // Create asset dollar
  auto blob = generator::random_transaction(
      fbb, iroha::Command::AssetCreate,
      generator::random_AssetCreate(fbb, "Dollar", "USA", "l1").Union());
  ametsuchi_.append(&blob);

  // Create account with id 1
  blob = generator::random_transaction(
      fbb, iroha::Command::AccountAdd,
      generator::random_AccountAdd(fbb, generator::random_account("1")).Union()
  );
  ametsuchi_.append(&blob);

  // Create account with id 2
  blob = generator::random_transaction(
      fbb, iroha::Command::AccountAdd,
      generator::random_AccountAdd(fbb, generator::random_account("2")).Union()
  );
  ametsuchi_.append(&blob);

  // Add currency to account 1
  blob = generator::random_transaction(
      fbb, iroha::Command::Add,
      generator::random_Add(
          fbb, "1",
          generator::random_asset_wrapper_currency(
             345,
             2,
             "Dollar",
             "USA",
             "l1")
      ).Union()
  );
  ametsuchi_.append(&blob);

  {
    flatbuffers::FlatBufferBuilder fbb2(2048);
    auto reference_tx = flatbuffers::GetRoot<iroha::Transaction>(generator::random_transaction(
        fbb2, iroha::Command::Transfer,
        generator::random_Transfer(
            fbb2,
            generator::random_asset_wrapper_currency(100,
                                                     2,
                                                     "Dollar",
                                                     "USA",
                                                     "l1"),
            "1", "2").Union()
    ).data())->command_as_Transfer();

    auto reference_1 = reference_tx->sender();
    auto reference_ln = reference_tx->asset_nested_root()->asset_as_Currency()->ledger_name();
    auto reference_dn = reference_tx->asset_nested_root()->asset_as_Currency()->domain_name();
    auto reference_cn = reference_tx->asset_nested_root()->asset_as_Currency()->currency_name();
    EXPECT_NE(reference_1, nullptr);
    EXPECT_NE(reference_ln, nullptr);
    EXPECT_NE(reference_dn, nullptr);
    EXPECT_NE(reference_cn, nullptr);
    try {
      auto asset1 = ametsuchi_.accountGetAsset(reference_1, reference_ln, reference_dn, reference_cn, true);
      EXPECT_NE(asset1, nullptr);
      EXPECT_NE(asset1->asset_as_Currency(), nullptr);
      EXPECT_NE(asset1->asset_as_Currency()->ledger_name(), nullptr);
      EXPECT_NE(asset1->asset_as_Currency()->domain_name(), nullptr);
      EXPECT_NE(asset1->asset_as_Currency()->currency_name(), nullptr);
      EXPECT_NE(asset1->asset_as_Currency()->description(), nullptr);
      EXPECT_NE(asset1->asset_as_Currency()->amount(), nullptr);
      EXPECT_EQ(asset1->asset_as_Currency()->amount()->str(), "345");
    } catch (ametsuchi::exception::InvalidTransaction e){
      if (e == ametsuchi::exception::InvalidTransaction::ASSET_NOT_FOUND) {
        std::cout << "Exception! ASSET_NOT_FOUND!! in test" << std::endl;
        ASSERT_TRUE(false);
      }
    }
  }


  // Transfer from 1 to 2
  blob = generator::random_transaction(
      fbb, iroha::Command::Transfer,
      generator::random_Transfer(
          fbb,
          generator::random_asset_wrapper_currency(100,
                                                   2,
                                                   "Dollar",
                                                   "USA",
                                                   "l1"),
          "1", "2").Union()
  );
  ametsuchi_.append(&blob);

  {
    flatbuffers::FlatBufferBuilder fbb2(2048);
    auto reference_tx = flatbuffers::GetRoot<iroha::Transaction>(generator::random_transaction(
        fbb2, iroha::Command::Transfer,
        generator::random_Transfer(
            fbb2,
            generator::random_asset_wrapper_currency(100,
                                                     2,
                                                     "Dollar",
                                                     "USA",
                                                     "l1"),
            "1", "2").Union()
    ).data())->command_as_Transfer();
    auto reference_1 = reference_tx->sender();
    auto reference_2 = reference_tx->receiver();
    auto reference_ln = reference_tx->asset_nested_root()->asset_as_Currency()->ledger_name();
    auto reference_dn = reference_tx->asset_nested_root()->asset_as_Currency()->domain_name();
    auto reference_cn = reference_tx->asset_nested_root()->asset_as_Currency()->currency_name();

    auto asset1 = ametsuchi_.accountGetAsset(reference_1, reference_ln, reference_dn, reference_cn, true);
    ASSERT_EQ(asset1->asset_as_Currency()->amount()->str(), "245");

    auto asset2 = ametsuchi_.accountGetAsset(reference_2, reference_ln, reference_dn, reference_cn, true);
    ASSERT_EQ(asset2->asset_as_Currency()->amount()->str(), "100");
  }

  // Transfer from 2 to 1
  blob = generator::random_transaction(
      fbb, iroha::Command::Transfer,
      generator::random_Transfer(
          fbb,
          generator::random_asset_wrapper_currency(50,
                                                   2,
                                                   "Dollar",
                                                   "USA",
                                                   "l1"),
          "2", "1").Union()
  );
  ametsuchi_.append(&blob);

  {
    flatbuffers::FlatBufferBuilder fbb2(2048);
    auto reference_tx = flatbuffers::GetRoot<iroha::Transaction>(generator::random_transaction(
        fbb2, iroha::Command::Transfer,
        generator::random_Transfer(
            fbb2,
            generator::random_asset_wrapper_currency(100,
                                                     2,
                                                     "Dollar",
                                                     "USA",
                                                     "l1"),
            "1", "2").Union()
    ).data())->command_as_Transfer();
    auto reference_1 = reference_tx->sender();
    auto reference_2 = reference_tx->receiver();
    auto reference_ln = reference_tx->asset_nested_root()->asset_as_Currency()->ledger_name();
    auto reference_dn = reference_tx->asset_nested_root()->asset_as_Currency()->domain_name();
    auto reference_cn = reference_tx->asset_nested_root()->asset_as_Currency()->currency_name();

    auto asset1 = ametsuchi_.accountGetAsset(reference_1, reference_ln, reference_dn, reference_cn, true);
    ASSERT_EQ(asset1->asset_as_Currency()->amount()->str(), "295");

    auto asset2 = ametsuchi_.accountGetAsset(reference_2, reference_ln, reference_dn, reference_cn, true);
    ASSERT_EQ(asset2->asset_as_Currency()->amount()->str(), "50");
  }

  ametsuchi_.commit();

  {
    flatbuffers::FlatBufferBuilder fbb2(2048);
    auto reference_tx = flatbuffers::GetRoot<iroha::Transaction>(generator::random_transaction(
        fbb2, iroha::Command::Transfer,
        generator::random_Transfer(
            fbb2,
            generator::random_asset_wrapper_currency(100,
                                                     2,
                                                     "Dollar",
                                                     "USA",
                                                     "l1"),
            "1", "2").Union()
    ).data())->command_as_Transfer();
    auto reference_1 = reference_tx->sender();
    auto reference_ln = reference_tx->asset_nested_root()->asset_as_Currency()->ledger_name();
    auto reference_dn = reference_tx->asset_nested_root()->asset_as_Currency()->domain_name();
    auto reference_cn = reference_tx->asset_nested_root()->asset_as_Currency()->currency_name();

    auto asset1 = ametsuchi_.accountGetAsset(reference_1, reference_ln, reference_dn, reference_cn, false);
    ASSERT_EQ(asset1->asset_as_Currency()->amount()->str(), "295");
  }
  {
    flatbuffers::FlatBufferBuilder fbb2(2048);
    auto reference_tx = flatbuffers::GetRoot<iroha::Transaction>(generator::random_transaction(
        fbb2, iroha::Command::Transfer,
        generator::random_Transfer(
            fbb2,
            generator::random_asset_wrapper_currency(100,
                                                     2,
                                                     "Dollar",
                                                     "USA",
                                                     "l1"),
            "1", "2").Union()
    ).data())->command_as_Transfer();
    auto reference_2 = reference_tx->receiver();
    auto reference_ln = reference_tx->asset_nested_root()->asset_as_Currency()->ledger_name();
    auto reference_dn = reference_tx->asset_nested_root()->asset_as_Currency()->domain_name();
    auto reference_cn = reference_tx->asset_nested_root()->asset_as_Currency()->currency_name();
    auto asset2 = ametsuchi_.accountGetAsset(reference_2, reference_ln, reference_dn, reference_cn, false);
    ASSERT_EQ(asset2->asset_as_Currency()->amount()->str(), "50");
  }
}

TEST_F(Ametsuchi_Test, PeerTest) {
  std::string ledger_name = "ShinkaiHideo";
  std::string pubkey1 = "SOULCATCHER_S";
  std::string ip1 = "KamineShota";
  std::string pubkey2 = "LIGHTWING";
  std::string ip2 = "AmagaiRihito";


  // Create asset dollar

  {  // Create peer1
    flatbuffers::FlatBufferBuilder fbb(2048);
    auto blob = generator::random_transaction(
        fbb, iroha::Command::PeerAdd,
        generator::random_PeerAdd(fbb, generator::random_peer(ledger_name,pubkey1, ip1))
            .Union());
    auto txs = flatbuffers::GetRoot<iroha::Transaction>(blob.data());
    ametsuchi_.append(&blob);
  }

  {  // Create peer2
    flatbuffers::FlatBufferBuilder fbb(2048);
    auto blob = generator::random_transaction(
        fbb, iroha::Command::PeerAdd,
        generator::random_PeerAdd(fbb, generator::random_peer(ledger_name,pubkey2, ip2))
            .Union());
    ametsuchi_.append(&blob);
  }

  { // Check Peer1
    flatbuffers::FlatBufferBuilder fbb(256);
    auto tmp_pubkey = fbb.CreateString(pubkey1);
    fbb.Finish(tmp_pubkey);
    auto query_pubkey = flatbuffers::GetRoot<flatbuffers::String>(fbb.GetBufferPointer());

    auto cur = ametsuchi_.pubKeyGetPeer(query_pubkey,true);
    ASSERT_TRUE(cur->publicKey()->str() == pubkey1);
    ASSERT_TRUE(cur->ip()->str() ==  ip1);
  }

  { // Check Peer2
    flatbuffers::FlatBufferBuilder fbb(256);
    auto tmp_pubkey = fbb.CreateString(pubkey2);
    fbb.Finish(tmp_pubkey);
    auto query_pubkey = flatbuffers::GetRoot<flatbuffers::String>(fbb.GetBufferPointer());

    auto cur = ametsuchi_.pubKeyGetPeer(query_pubkey,true);

    ASSERT_TRUE(cur->publicKey()->str() == pubkey2);
    ASSERT_TRUE(cur->ip()->str() ==  ip2);
  }

  {  // Remove peer1
    flatbuffers::FlatBufferBuilder fbb(2048);
    auto blob = generator::random_transaction(
        fbb, iroha::Command::PeerRemove,
        generator::random_PeerRemove(fbb, pubkey1).Union());
    ametsuchi_.append(&blob);
  }

  { // Check Peer1
    flatbuffers::FlatBufferBuilder fbb(256);
    auto tmp_pubkey = fbb.CreateString(pubkey1);
    fbb.Finish(tmp_pubkey);
    auto query_pubkey = flatbuffers::GetRoot<flatbuffers::String>(fbb.GetBufferPointer());

    bool exception_flag = false;
    try {
      ametsuchi_.pubKeyGetPeer(query_pubkey, true);
    } catch ( ... ) {
      exception_flag = true;
    }
    ASSERT_TRUE( exception_flag );
  }

  {  // Remove peer2
    flatbuffers::FlatBufferBuilder fbb(2048);
    auto blob = generator::random_transaction(
        fbb, iroha::Command::PeerRemove,
        generator::random_PeerRemove(fbb, pubkey2).Union());
    ametsuchi_.append(&blob);
  }

  { // Check Peer2
    flatbuffers::FlatBufferBuilder fbb(256);
    auto tmp_pubkey = fbb.CreateString(pubkey2);
    fbb.Finish(tmp_pubkey);
    auto query_pubkey = flatbuffers::GetRoot<flatbuffers::String>(fbb.GetBufferPointer());

    bool exception_flag = false;
    try {
      ametsuchi_.pubKeyGetPeer(query_pubkey, true);
    } catch ( ... ) {
      exception_flag = true;
    }
    ASSERT_TRUE( exception_flag );
  }

  ametsuchi_.commit();

}