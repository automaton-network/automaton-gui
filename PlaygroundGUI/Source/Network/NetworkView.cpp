/*
 * Automaton Playground
 * Copyright (C) 2019 Asen Kovachev (@asenski, GitHub: akovachev)
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

#include "secp256k1/include/secp256k1_recovery.h"
#include "secp256k1/include/secp256k1.h"
#include "secp256k1/src/hash_impl.h"
#include "secp256k1/src/hash.h"
#include "automaton/core/io/io.h"

#include "automaton/core/interop/ethereum/eth_contract_curl.h"

using automaton::core::common::status;
using automaton::core::interop::ethereum::dec_to_32hex;
using automaton::core::interop::ethereum::eth_contract;
using automaton::core::interop::ethereum::hex_to_dec;
using automaton::core::io::bin2hex;
using automaton::core::io::hex2bin;

//==============================================================================
//  ReadContractThread
//==============================================================================

class ReadContractThread: public ThreadWithProgressWindow {
 public:
  std::string url;
  std::string contract_addr;
  status s;

  uint32_t slots_number;
  uint32_t slots_claimed;
  std::vector<std::string> slot_difficulty;
  std::vector<std::string> slot_owner;
  std::vector<std::string> slot_last_claim_time;

  ReadContractThread(std::string _url, std::string _contract_addr):
    ThreadWithProgressWindow("Reading Contract...", true, true),
    s(status::ok()),
    url(_url),
    contract_addr(_contract_addr) {}

  void run() override {
    eth_contract contract(url, contract_addr, {
      {"getSlotsNumber", {"getSlotsNumber()", false}},
      {"getSlotOwner", {"getSlotOwner(uint256)", false}},
      {"getSlotDifficulty", {"getSlotDifficulty(uint256)", false}},
      {"getSlotLastClaimTime", {"getSlotLastClaimTime(uint256)", false}},
      {"getOwners", {"getOwners(uint256,uint256)", false}},
      {"getDifficulties", {"getDifficulties(uint256,uint256)", false}},
      {"getLastClaimTimes", {"getLastClaimTimes(uint256,uint256)", false}},
      {"getMask", {"getMask()", false}},
      {"getClaimed", {"getClaimed()", false}},
      {"claimSlot", {"claimSlot(bytes32,bytes32,uint8,bytes32,bytes32)", true}}
    });

    setProgress(0.1);
    s = contract.call("", "getSlotsNumber", "");
    if (!s.is_ok()) {
      return;
    }
    if (s.msg.empty()) {
      s = status::internal("Invalid contract address!");
      return;
    }
    slots_number = hex_to_dec(s.msg);

    uint32_t step = 1000;
    for (uint32_t slot = 0; slot < slots_number; slot += step) {
      if (threadShouldExit()) {
        s = status::internal("Aborted");
        return;
      }
      if (step > slots_number - slot) {
        step = slots_number - slot;
      }
      setProgress(0.2 + (0.5 * slot) / slots_number);
      setStatusMessage(
        "Getting slots [" +
        String(slot) + " .. " + String(slot + step - 1) +
        "] of " + String(slots_number));

      s = contract.call("", "getOwners", dec_to_32hex(slot) + dec_to_32hex(step));
      if (!s.is_ok()) {
        return;
      }

      s = contract.call("", "getDifficulties", dec_to_32hex(slot) + dec_to_32hex(step));
      if (!s.is_ok()) {
        return;
      }

      // s = contract->call("", "getLastClaimTimes", dec_to_32hex(slot) + dec_to_32hex(step));
      // if (!s.is_ok()) {
      //   return;
      // }
    }

    setProgress(0.9);
    s = contract.call("", "getMask", "");
    if (!s.is_ok()) {
      return;
    }
    setStatusMessage("Mask: " + s.msg);
    wait(500);

    setProgress(0.9);
    s = contract.call("", "getClaimed", "");
    if (!s.is_ok()) {
      return;
    }
    setStatusMessage("Number of slot claims: " + String(hex_to_dec(s.msg)));
    wait(500);

    setProgress(1.0);
    s = status::ok();
  }

  void threadComplete(bool userPressedCancel) override {
    wait(500);
    if (userPressedCancel) {
      AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                                       "Operation aborted!",
                                       "Current settings were not affected.");
    } else if (!s.is_ok()) {
      AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                                       "Error occurred!",
                                       s.msg);
    } else {
      AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon,
                                       "Operation successful!",
                                       "Contract data loaded successfully and settings updated");
    }

    delete this;
  }
};

//==============================================================================
//  NetworkView
//==============================================================================

NetworkView::NetworkView() {
  // startTimer(1000);
  int y = 100;
  LBL("Ethereum RPC: ", 20, y, 100, 24);
  txtURL = TXT("URL", 120, y, 500, 24);
  txtURL->setText("http://127.0.0.1:7545");

  y += 30;
  LBL("Contract Address: ", 20, y, 100, 24);
  txtContractAddress = TXT("ADDR", 120, y, 500, 24);
  txtContractAddress->setInputRestrictions(42, "0123456789abcdefABCDEFx");

  y += 30;
  TB("Read Contract", 120, y, 120, 24);
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
  auto txt = btn->getButtonText();
  if (txt == "Read Contract") {
    (new ReadContractThread(txtURL->getText().toStdString(), txtContractAddress->getText().toStdString()))->launchThread();
  }
}

void NetworkView::resized() {
}

void NetworkView::textEditorTextChanged(TextEditor &) {
}

void NetworkView::timerCallback() {
}
