/*
 * Automaton Playground
 * Copyright (c) 2019 The Automaton Authors.
 * Copyright (c) 2019 The automaton.network Authors.
 *
 * Automaton Playground is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Automaton Playground is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Automaton Playground.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "NetworkView.h"

#include "../Data/AutomatonContractData.h"  // NOLINT

#include <secp256k1_recovery.h>
#include <secp256k1.h>
#include <json.hpp>

#include "automaton/core/io/io.h"
#include "automaton/core/interop/ethereum/eth_contract_curl.h"
#include "automaton/core/interop/ethereum/eth_transaction.h"
#include "automaton/core/interop/ethereum/eth_helper_functions.h"
#include "automaton/core/crypto/cryptopp/Keccak_256_cryptopp.h"

#include "../Proposals/ProposalsManager.h"
#include "../DEX/DEXManager.h"

using json = nlohmann::json;

using automaton::core::common::status;
using automaton::core::interop::ethereum::dec_to_i256;
using automaton::core::interop::ethereum::eth_contract;
using automaton::core::io::bin2hex;
using automaton::core::io::dec2hex;
using automaton::core::io::hex2bin;
using automaton::core::io::hex2dec;
using automaton::core::crypto::cryptopp::Keccak_256_cryptopp;

//==============================================================================
//  ReadContractThread
//==============================================================================

class ReadContractThread: public ThreadWithProgressWindow {
 public:
  std::string url;
  std::string contract_addr;
  status s;

  // Data imported from smart contract.
  uint32_t slots_number;
  uint32_t slots_claimed;
  std::vector<ValidatorSlot> slots;
  std::string mask;
  std::string min_difficulty;

  ReadContractThread(std::string _url, std::string _contract_addr):
    ThreadWithProgressWindow("Reading Contract...", true, true),
    url(_url),
    contract_addr(_contract_addr),
    s(status::ok())
  {}

  void run() override {
    std::string abi;
    eth_contract::register_contract(url, contract_addr, abi);
    auto contract = eth_contract::get_contract(contract_addr);
    if (contract == nullptr) {
      std::cout << "ERROR: Contract is NULL" << std::endl;
      s = status::internal("Contract is NULL!");
      return;
    }

    setProgress(0.1);
    s = contract->call("numSlots", "");
    if (!s.is_ok()) {
      std::cout << "ERROR: " << s.msg << std::endl;
      return;
    }
    if (s.msg.empty()) {
      s = status::internal("Invalid contract address!");
      std::cout << "ERROR: Invalid contract address" << std::endl;
      return;
    }
    json j_output = json::parse(s.msg);
    slots_number = std::stoul((*j_output.begin()).get<std::string>());
    slots.resize(slots_number);

    uint32_t step = 1024;
    for (uint32_t slot = 0; slot < slots_number; slot += step) {
      if (threadShouldExit()) {
        s = status::internal("Aborted");
        std::cout << "ERROR: Aborted!" << std::endl;
        return;
      }
      if (step > slots_number - slot) {
        step = slots_number - slot;
      }
      setProgress((1.0 * slot) / slots_number);
      setStatusMessage(
        "Getting slot " + String(slot + step) + " of " + String(slots_number));

      json j_input;
      j_input.push_back(slot);
      j_input.push_back(step);
      std::string params = j_input.dump();

      // Fetch owners.
      s = contract->call("getOwners", params);
      if (!s.is_ok()) {
        std::cout << "ERROR: " << s.msg << std::endl;
        return;
      }

      // Parse owners.
      j_output = json::parse(s.msg);
      std::vector<std::string> owners = (*j_output.begin()).get<std::vector<std::string> >();
      for (uint32_t i = 0; i < owners.size(); i++) {
        slots[slot + i].owner = owners[i];
      }

      // Fetch difficulties.
      s = contract->call("getDifficulties", params);
      if (!s.is_ok()) {
        std::cout << "ERROR: " << s.msg << std::endl;
        return;
      }

      // Parse difficulties.
      j_output = json::parse(s.msg);
      std::vector<std::string> diff = (*j_output.begin()).get<std::vector<std::string> >();
      for (uint32_t i = 0; i < diff.size(); i++) {
        slots[slot + i].difficulty = dec_to_i256(false, diff[i]);
      }

      // Fetch last claim times.
      s = contract->call("getLastClaimTimes", params);
      if (!s.is_ok()) {
        std::cout << "ERROR: " << s.msg << std::endl;
        return;
      }

      // Parse last claim times.
      j_output = json::parse(s.msg);
      std::vector<std::string> last_claims = (*j_output.begin()).get<std::vector<std::string> >();
      for (uint32_t i = 0; i < last_claims.size(); i++) {
        slots[slot + i].last_claim_time = last_claims[i];
      }
    }
    setProgress(0);

    s = contract->call("mask", "");
    j_output = json::parse(s.msg);
    mask = bin2hex(dec_to_i256(false, (*j_output.begin()).get<std::string>()));
    setStatusMessage("Mask: " + mask);

    s = contract->call("minDifficulty", "");
    j_output = json::parse(s.msg);
    min_difficulty = bin2hex(dec_to_i256(false, (*j_output.begin()).get<std::string>()));
    setStatusMessage("MinDifficulty: " + min_difficulty);

    s = contract->call("numTakeOvers", "");
    j_output = json::parse(s.msg);
    std::string slots_claimed_string = (*j_output.begin()).get<std::string>();
    slots_claimed = std::stoul(slots_claimed_string);
    setStatusMessage("Number of slot claims: " + slots_claimed_string);
    setProgress(1.0);

    s = status::ok();
  }

  void threadComplete(bool userPressedCancel) override {
  }
};

//==============================================================================
//  NetworkView
//==============================================================================

NetworkView::NetworkView() {
//  auto cd = AutomatonContractData::getInstance();
//  ScopedLock lock(cd->m_criticalSection);
//
//  // startTimer(1000);
//  int y = 100;
//  LBL("Ethereum RPC: ", 20, y, 100, 24);
//  txtURL = TXT("URL", 120, y, 500, 24);
//  txtURL->setText(cd->m_ethUrl);
//
//  y += 30;
//  LBL("Contract Address: ", 20, y, 100, 24);
//  txtContractAddress = TXT("ADDR", 120, y, 500, 24);
//  txtContractAddress->setInputRestrictions(42, "0123456789abcdefABCDEFx");
//  txtContractAddress->setText(cd->m_contractAddress);
//
//  y += 30;
//  TB("Read Contract", 120, y, 120, 24);
}

NetworkView::~NetworkView() {
}

void NetworkView::paint(Graphics& g) {
  g.setColour(Colours::white);
  g.setFont(32.0f);
  g.drawText("Network Configuration", 0, 20, getWidth(), 40, Justification::centred);
  g.setFont(8.0f);
  g.drawText(String(__DATE__) + " " + String(__TIME__), 4, 4, 100, 20, Justification::topLeft);
}

void NetworkView::buttonClicked(Button* btn) {
//  auto txt = btn->getButtonText();
//  if (txt == "Read Contract") {
//    const auto url = txtURL->getText();
//    const auto address = txtContractAddress->getText();
//
//    ReadContractThread t(url.toStdString(), address.toStdString());
//    if (t.runThread(9)) {
//      if (!t.s.is_ok()) {
//      AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
//                                       "Error occurred!",
//                                       t.s.msg);
//      } else {
//        auto cd = AutomatonContractData::getInstance();
//        cd->setData(url.toStdString(), t.contract_addr, t.mask,
//                    t.min_difficulty, t.slots_number,
//                    t.slots_claimed, t.slots);
//
//        AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon,
//                                         "Operation successful!",
//                                         "Contract data loaded successfully and settings updated");
//      }
//    } else {
//      AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
//                                       "Operation aborted!",
//                                       "Current settings were not affected.");
//    }
//  }
}

void NetworkView::resized() {
}

void NetworkView::textEditorTextChanged(TextEditor &) {
}

void NetworkView::timerCallback() {
}
