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

#include <JuceHeader.h>

class DemosMainComponent;
class AccountWindow;


class LoginComponent  : public Component
                      , public ComponentListener
                      , public Button::Listener
                      , public ComboBox::Listener {
 public:
  LoginComponent(PropertiesFile* configFile);
  ~LoginComponent();

  void paint(Graphics&) override;
  void resized() override;
  void buttonClicked(Button* btn) override;
  void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
  void componentVisibilityChanged(Component& component) override;

 private:
  AccountWindow* getWindowByAddress(const String& address);

  std::unique_ptr<ComboBox> m_accountsComboBox;
  std::unique_ptr<TextButton> m_importPrivateKeyBtn;
  std::unique_ptr<TextButton> m_openAccountBtn;
  OwnedArray<AccountWindow> m_accountWindows;
  std::map<String, PropertySet> m_accountsConfigs;
  PropertiesFile* m_configFile;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoginComponent)
};
