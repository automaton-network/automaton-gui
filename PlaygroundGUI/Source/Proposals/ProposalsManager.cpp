/*
 * Automaton Playground
 * Copyright (c) 2020 The Automaton Authors.
 * Copyright (c) 2020 The automaton.network Authors.
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

#include <json.hpp>

#include "ProposalsManager.h"
#include "../Data/AutomatonContractData.h"
#include "automaton/core/interop/ethereum/eth_contract_curl.h"
#include "automaton/core/interop/ethereum/eth_transaction.h"
#include "automaton/core/interop/ethereum/eth_helper_functions.h"

using automaton::core::interop::ethereum::dec_to_32hex;
using automaton::core::interop::ethereum::eth_transaction;
using automaton::core::interop::ethereum::eth_getTransactionCount;
using automaton::core::interop::ethereum::eth_getTransactionReceipt;
using automaton::core::interop::ethereum::encode;
using automaton::core::interop::ethereum::eth_contract;
using automaton::core::io::bin2hex;
using automaton::core::io::dec2hex;
using automaton::core::io::hex2dec;

ProposalsManager::ProposalsManager()
  : m_model (std::make_shared<ProposalsModel>())
{
}

ProposalsManager::~ProposalsManager()
{
  clearSingletonInstance();
}

bool ProposalsManager::addProposal (Proposal::Ptr proposal, const std::string& contributor)
{
  //TODO: pass address and privateKey
  const std::string privateKey = "af575525cab41534a57e0b0487992d5048eee7c8c72e4c39f2ec34c1a25ca385";
  const std::string address = "0xa6C8015476f6F4c646C95488c5fc7f5174A4E0ef";

  const auto cd = AutomatonContractData::getInstance();
  const auto contract = eth_contract::get_contract (cd->contract_address);
  if (contract == nullptr) {
    std::cout << "ERROR: Contract is NULL" << std::endl;
    AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                      "ERROR",
                                      "Contract is NULL. Read appropriate contract data first.");
    //s = status::internal("Contract is NULL!");
    return false;
  }

  auto s = eth_getTransactionCount (cd->eth_url, address);
  const auto nonce = s.is_ok() ? s.msg : "0";
  if (! s.is_ok())
  {
    AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                      "ERROR",
                                      s.msg);
    return false;
  }

  json jProposal;
  jProposal.push_back (contributor);
  jProposal.push_back (proposal->getTitle().toStdString());
  jProposal.push_back ("google.com");
  jProposal.push_back ("BA5EC0DE");
  jProposal.push_back (proposal->getLengthDays());
  jProposal.push_back (proposal->getNumPeriods());
  jProposal.push_back (proposal->getBudget());

  json jSignature;
  jSignature.push_back ("address");
  jSignature.push_back ("string");
  jSignature.push_back ("string");
  jSignature.push_back ("bytes");
  jSignature.push_back ("uint256");
  jSignature.push_back ("uint256");
  jSignature.push_back ("uint256");

  std::stringstream createProposalData;
  createProposalData << "f8a1ff12" << bin2hex (encode (jSignature.dump(), jProposal.dump()));

  eth_transaction transaction;
  transaction.nonce = nonce;
  transaction.gas_price = "1388";  // 5 000
  transaction.gas_limit = "5B8D80";  // 6M
  transaction.to = cd->contract_address.substr(2);
  transaction.value = "";
  transaction.data = createProposalData.str();
  transaction.chain_id = "01";
  s = contract->call ("createProposal", transaction.sign_tx (privateKey));

  if (! s.is_ok())
  {
    AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                      "ERROR",
                                      String("(") + String(s.code) + String(") :") + s.msg);
    printf("ERROR: (%u) %s", s.code, s.msg.c_str());
    return false;
  }

  DBG("Call result: " << s.msg << "\n");
  proposal->setStatus (Proposal::Status::Uninitialized);
  m_model->addItem (proposal);
  return true;
}

JUCE_IMPLEMENT_SINGLETON(ProposalsManager);
