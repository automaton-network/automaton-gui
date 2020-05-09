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

#pragma once

#include <Login/Account.h>
#include <Utils/AsyncTask.h>
#include <Data/AutomatonContractData.h>
#include "../../JuceLibraryCode/JuceHeader.h"
#include "Components/FormMaker.h"

#include "automaton/core/crypto/cryptopp/secure_random_cryptopp.h"
#include "automaton/core/crypto/cryptopp/SHA256_cryptopp.h"
#include "automaton/core/io/io.h"

class ValidatorSlotsGrid;

class Miner : public Component,
              public Button::Listener,
              public TextEditor::Listener,
              private Timer {
 public:
  //==============================================================================
  Miner(Account::Ptr accountData);
  ~Miner();

  void paint(Graphics& g) override;
  void resized() override;

  void update();

  // Button::Listener overrides.
  void buttonClicked(Button* btn) override;

  // TextEditor::Listener overrides.
  void textEditorTextChanged(TextEditor &) override;

  // Mining
  struct mined_slot {
    std::string difficulty;
    std::string public_key;
    std::string private_key;
    uint32_t slot_index;
  };

  void initSlots();
  void processMinedKey(std::string _pk, int keys_generated);

  void addMinerThread();
  void stopMining();

  size_t getMinedSlotsNumber() { return mined_slots.size(); }
  mined_slot& getMinedSlot(int _slot) { return mined_slots[_slot]; }
  unsigned char * getMask() { return mask; }
  unsigned char * getDifficulty() { return difficulty; }

  // void setMinDifficulty(unsigned int _minDifficulty);

  void setMaskHex(std::string _mask);
  void setMinDifficultyHex(std::string _minDifficulty);
  void setSlotsNumber(int _slotsNum);
  void setMinerAddress(std::string _address);

 private:
  Account::Ptr m_accountData;

  // Mining
  uint32 totalSlots = 1024;
  std::vector<mined_slot> mined_slots;

  unsigned int total_keys_generated = 0;
  unsigned int last_keys_generated = 0;
  unsigned int slots_claimed = 0;
  int64 last_time = 0;
  Array<AsyncTask::Ptr> miners;
  unsigned char mask[32];
  unsigned int min_difficulty;
  unsigned char difficulty[32];
  unsigned char minerAddress[32];

  std::string private_key;
  std::string eth_address;

  // UI
  std::unique_ptr<Label> m_ownedSlotsNumEditor;
  std::unique_ptr<Label> m_timeLabel;
  std::unique_ptr<Label> m_slotsLabel;
  std::unique_ptr<Label> m_maskHexLabel;
  std::unique_ptr<Label> m_minDifficultyHexLabel;
  std::unique_ptr<Label> m_validatorSlotsLabel;
  std::unique_ptr<Label> m_claimSlotsLabel;

  std::unique_ptr<TextEditor> m_slotsNumEditor;
  std::unique_ptr<TextEditor> m_maskHexEditor;
  std::unique_ptr<TextEditor> m_minDifficultyHexEditor;
  std::unique_ptr<TextEditor> m_minerInfoEditor;
  std::unique_ptr<TextEditor> m_claimEditor;

  std::unique_ptr<TextButton> m_addMinerBtn;
  std::unique_ptr<TextButton> m_stopMinerBtn;

  std::unique_ptr<TableListBox> m_tblSlots;
  std::unique_ptr<ValidatorSlotsGrid> m_validatorSlotsGrid;

  void setNumOfOwnedSlots(const std::vector<ValidatorSlot>& validatorSlots);
  void updateContractData();
  void claimMinedSlots();

  void timerCallback() override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Miner)
};
