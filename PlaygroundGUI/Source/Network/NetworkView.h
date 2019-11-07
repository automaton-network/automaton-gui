/*
 * Automaton Playground
 * Copyright (c) 2017-2018 The Automaton Authors.
 * Copyright (c) 2017-2018 The automaton.network Authors.
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

class NetworkView:
  public FormMaker,
  private Timer {
 public:
  //==============================================================================
  NetworkView();
  ~NetworkView();

  void paint(Graphics& g) override;
  void resized() override;

  void update();

  // Button::Listener overrides.
  void buttonClicked(Button* btn) override;

  // TextEditor::Listener overrides.
  void textEditorTextChanged(TextEditor &) override;

  void timerCallback() override;

 private:
  TextEditor* txtURL;
  TextEditor* txtContractAddress;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NetworkView)
};
