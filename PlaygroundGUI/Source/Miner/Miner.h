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

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Components/FormMaker.h"

#include "automaton/core/crypto/cryptopp/secure_random_cryptopp.h"
#include "automaton/core/crypto/cryptopp/SHA256_cryptopp.h"
#include "automaton/core/io/io.h"

class Miner:
  public FormMaker,
  private Timer {
 public:
  //==============================================================================
  Miner();
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
  void createSignature();

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
  // Mining
  uint32 totalSlots = 1024;
  std::vector<mined_slot> mined_slots;

  unsigned int total_keys_generated = 0;
  unsigned int last_keys_generated = 0;
  unsigned int slots_claimed = 0;
  int64 last_time = 0;
  OwnedArray<Thread> miners;
  unsigned char mask[32];
  unsigned int minDifficulty;
  unsigned char difficulty[32];
  unsigned char minerAddress[32];

  std::string private_key;
  std::string eth_address;

  // UI
  TextEditor* txtRpcServer;
  Button* btnContract;
  TextEditor* txtContract;
  // TextEditor* txtMask;
  TextEditor* txtMaskHex;
  TextEditor* txtMinerAddress;
  TextEditor* txtMinerInfo;
  // TextEditor* txtMinDifficulty;
  TextEditor* txtMinDifficultyHex;
  TextEditor* txtSlotsNum;
  TableListBox* tblSlots;
  TextEditor* txtClaim;

  void importPrivateKey();
  void updateContractData();
  void claimMinedSlots();

  void timerCallback() override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Miner)
};
