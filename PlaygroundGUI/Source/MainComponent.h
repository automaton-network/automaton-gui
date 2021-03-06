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

#include <memory>
#include <Login/Account.h>

#include "../JuceLibraryCode/JuceHeader.h"

class ProposalsManager;
class DEXManager;
class TasksPanel;

class DemosMainComponent:
  public Component,
  public ApplicationCommandTarget,
  public MenuBarModel,
  public Button::Listener,
  public ChangeListener,
  private Timer {
 public:
  enum CommandIDs {
  };

  DemosMainComponent(Account::Ptr accountData);
  ~DemosMainComponent();

  void paint(Graphics& g) override;
  void resized() override;
  void buttonClicked(Button* button) override;
  void changeListenerCallback(ChangeBroadcaster* source) override;

  StringArray getMenuBarNames() override {
    return { "Menu Position", "Outer Colour", "Inner Colour" };
  }

  void getAllCommands(Array<CommandID>& c) override {
  }

  PopupMenu getMenuForIndex(int topLevelMenuIndex, const String& menuName) override {
    PopupMenu menu;
    return menu;
  }

  bool perform(const InvocationInfo& info) override {
    return false;
  }

  void menuItemSelected(int /*menuItemID*/, int /*topLevelMenuIndex*/) override {}

  ApplicationCommandTarget* getNextCommandTarget() override {
    return nullptr;
  }

  void getCommandInfo(
     CommandID commandID,
     ApplicationCommandInfo& result) override {
  }

 private:
  void updateContractState();

  void timerCallback() override;

  std::unique_ptr<MenuBarComponent> m_menuBar;
  std::unique_ptr<TabbedComponent> m_tabbedComponent;
  std::unique_ptr<TasksPanel> m_tasksPanel;
  std::unique_ptr<TextButton> m_refreshButton;
  std::unique_ptr<Component> m_loadingComponent;
  Account::Ptr m_accountData;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DemosMainComponent)
};
