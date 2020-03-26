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
#include "../DEX/OrdersManager.h"

using json = nlohmann::json;

using automaton::core::common::status;
using automaton::core::interop::ethereum::dec_to_i256;
using automaton::core::interop::ethereum::eth_contract;
using automaton::core::io::bin2hex;
using automaton::core::io::dec2hex;
using automaton::core::io::hex2bin;
using automaton::core::io::hex2dec;
using automaton::core::crypto::cryptopp::Keccak_256_cryptopp;

// TODO(asen): Move Ethereum utilities to automaton/core/interop.

static std::string gen_ethereum_address(const unsigned char* priv_key) {
  Keccak_256_cryptopp hash;
  secp256k1_context* context = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
  secp256k1_pubkey* pubkey = new secp256k1_pubkey();

  if (!secp256k1_ec_pubkey_create(context, pubkey, priv_key)) {
    LOG(WARNING) << "Invalid priv_key " << bin2hex(std::string(reinterpret_cast<const char*>(priv_key), 32));
    delete pubkey;
    secp256k1_context_destroy(context);
    return "";
  }

  uint8_t pub_key_serialized[65];
  uint8_t eth_address[32];
  size_t outLen = 65;
  secp256k1_ec_pubkey_serialize(context, pub_key_serialized, &outLen, pubkey, SECP256K1_EC_UNCOMPRESSED);
  hash.calculate_digest(pub_key_serialized + 1, 64, eth_address);
  delete pubkey;
  secp256k1_context_destroy(context);
  return std::string(reinterpret_cast<char*>(eth_address + 12), 20);
}

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
    auto conf = Config::getInstance();
    eth_contract::register_contract(url, contract_addr, conf->get_abi());
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
  auto cd = AutomatonContractData::getInstance();
  ScopedLock lock(cd->criticalSection);

  // startTimer(1000);
  int y = 100;
  LBL("Ethereum RPC: ", 20, y, 100, 24);
  txtURL = TXT("URL", 120, y, 500, 24);
  txtURL->setText(cd->eth_url);

  y += 30;
  LBL("Contract Address: ", 20, y, 100, 24);
  txtContractAddress = TXT("ADDR", 120, y, 500, 24);
  txtContractAddress->setInputRestrictions(42, "0123456789abcdefABCDEFx");
  txtContractAddress->setText(cd->contract_address);

  y += 30;
  LBL("Ethereum Address: ", 20, y, 100, 24);
  txtEthAddress = TXT("ADDR", 120, y, 500, 24);
  txtEthAddress->setReadOnly(true);
  txtEthAddress->setText(cd->eth_address);

  y += 30;
  TB("Read Contract", 120, y, 120, 24);
  TB("Import Private Key", 300, y, 120, 24);
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
    std::string url = txtURL->getText().toStdString();
    std::string address = txtContractAddress->getText().toStdString();
    auto conf = Config::getInstance();
    conf->lock();
    conf->set_string("eth_url", url);
    conf->set_string("contract_address", address);
    conf->unlock();
    ReadContractThread t(url, address);
    if (t.runThread(9)) {
      if (!t.s.is_ok()) {
      AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                                       "Error occurred!",
                                       t.s.msg);
      } else {
        auto cd = AutomatonContractData::getInstance();
        ScopedLock lock(cd->criticalSection);
        cd->contract_address = t.contract_addr;
        cd->mask = t.mask;
        cd->min_difficulty = t.min_difficulty;
        cd->slots = t.slots;
        cd->slots_number = t.slots_number;
        cd->slots_claimed = t.slots_claimed;

        auto conf = Config::getInstance();
        conf->lock();
        conf->set_string("mask", t.mask);
        conf->set_string("min_difficulty", t.min_difficulty);
        conf->set_number("slots_number", t.slots_number);
        conf->set_number("slots_claimed", t.slots_claimed);
        conf->unlock();

        AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon,
                                         "Operation successful!",
                                         "Contract data loaded successfully and settings updated");
      }
    } else {
      AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                                       "Operation aborted!",
                                       "Current settings were not affected.");
    }
    ProposalsManager::getInstance()->fetchProposals();
    OrdersManager::getInstance()->fetchOrders();
  } else if (txt == "Import Private Key") {
    AlertWindow w("Import Private Key",
                  "Enter your private key and it will be imported.",
                  AlertWindow::QuestionIcon);

    w.addTextEditor("privkey", "", "Private Key:", true);
    w.addButton("OK",     1, KeyPress(KeyPress::returnKey, 0, 0));
    w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

    if (w.runModalLoop() == 1) {
      // this is the text they entered..
      auto privkey_hex = w.getTextEditorContents("privkey");
      if (privkey_hex.startsWith("0x")) {
        privkey_hex = privkey_hex.substring(2);
      }
      if (privkey_hex.length() != 64) {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                "Invalid Private Key!",
                "Private key should be exactly 32 bytes!");
        return;
      }

      auto private_key = hex2bin(privkey_hex.toStdString());
      auto eth_address_hex = "0x" + bin2hex(gen_ethereum_address((unsigned char *)private_key.c_str()));
      txtEthAddress->setText(eth_address_hex);

      auto cd = AutomatonContractData::getInstance();
      cd->eth_address = eth_address_hex;
      cd->private_key = privkey_hex.toStdString();

      auto conf = Config::getInstance();
      conf->lock();
      conf->set_string("private_key", privkey_hex.toStdString());
      conf->set_string("eth_address", eth_address_hex);
      conf->unlock();
    }
  } else if (txt == "Import Private Key") {
    AlertWindow w("Import Private Key",
                  "Enter your private key and it will be imported.",
                  AlertWindow::QuestionIcon);

    w.addTextEditor("privkey", "", "Private Key:", true);
    w.addButton("OK",     1, KeyPress(KeyPress::returnKey, 0, 0));
    w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

    if (w.runModalLoop() == 1) {
      // this is the text they entered..
      auto privkey_hex = w.getTextEditorContents("privkey");
      if (privkey_hex.startsWith("0x")) {
        privkey_hex = privkey_hex.substring(2);
      }
      if (privkey_hex.length() != 64) {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                "Invalid Private Key!",
                "Private key should be exactly 32 bytes!");
        return;
      }

      auto private_key = hex2bin(privkey_hex.toStdString());
      auto eth_address_hex = "0x" + bin2hex(gen_ethereum_address((unsigned char *)private_key.c_str()));
      txtEthAddress->setText(eth_address_hex);

      auto cd = AutomatonContractData::getInstance();
      cd->eth_address = eth_address_hex;
      cd->private_key = privkey_hex.toStdString();

      auto conf = Config::getInstance();
      conf->lock();
      conf->set_string("private_key", privkey_hex.toStdString());
      conf->set_string("eth_address", eth_address_hex);
      conf->unlock();
    }
  }
}

void NetworkView::resized() {
}

void NetworkView::textEditorTextChanged(TextEditor &) {
}

void NetworkView::timerCallback() {
}
