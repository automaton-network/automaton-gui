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
  struct slot {
    std::string difficulty;
    std::string owner;
    std::string private_key;

    slot() {
      // Set min difficulty.
      char buf[32];
      memset(buf, 0, 32);
      buf[0] = 0xFF;
      buf[1] = 0xF0;
      difficulty = std::string(buf, 32);
    }
  };

  void initSlots();
  void processMinedKey(std::string _pk, int keys_generated);

  void addMinerThread();
  void stopMining();
  void createSignature();

  size_t getSlotsNumber() { return slots.size(); }
  slot& getSlot(int _slot) { return slots[_slot]; }
  unsigned char * getMask() { return mask; }
  unsigned char * getDifficulty() { return difficulty; }

  void setMask(std::string _mask);
  void setMinDifficulty(unsigned int _minDifficulty);
  void setSlotsNumber(int _slotsNum);
  void setMinerAddress(std::string _address);

 private:
  // Mining
  int totalSlots = 1024;
  std::vector<slot> slots;

  unsigned int total_keys_generated = 0;
  unsigned int last_keys_generated = 0;
  unsigned int slots_claimed = 0;
  int64 last_time = 0;
  OwnedArray<Thread> miners;
  unsigned char mask[32];
  unsigned int minDifficulty;
  unsigned char difficulty[32];
  unsigned char minerAddress[32];

  // UI
  TextEditor* txtRpcServer;
  Button* btnContract;
  TextEditor* txtContract;
  TextEditor* txtMask;
  TextEditor* txtMaskHex;
  TextEditor* txtMinerAddress;
  TextEditor* txtMinerInfo;
  TextEditor* txtMinDifficulty;
  TextEditor* txtMinDifficultyHex;
  TextEditor* txtSlotsNum;
  TableListBox* tblSlots;
  TextEditor* txtClaim;

  void timerCallback() override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Miner)
};
