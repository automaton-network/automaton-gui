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

#include <JuceHeader.h>
#include "LoginComponent.h"
#include "../MainComponent.h"
#include "../Utils.h"


class AccountWindow : public DocumentWindow {
 public:
  AccountWindow(String title, String accountAddress, PropertySet* accountConfig)
      : DocumentWindow(title,
                    Desktop::getInstance()
                    .getDefaultLookAndFeel()
                    .findColour(ResizableWindow::backgroundColourId),
                    DocumentWindow::allButtons)
      , m_accountAddress(accountAddress)
      , m_accountConfig(accountConfig) {
    LookAndFeel::setDefaultLookAndFeel(&m_lnf);

    setUsingNativeTitleBar(true);
    setContentOwned(new DemosMainComponent(m_accountConfig), true);

    setFullScreen(false);
    setResizable(true, true);
    setResizeLimits(800, 600, 10000, 10000);
    centreWithSize(800, 600);

    setVisible(true);
  }

  ~AccountWindow() {
  }

  void closeButtonPressed() override {
    setVisible(false);
  }

  const String& getAddress() {
    return m_accountAddress;
  }

 private:
  LookAndFeel_V4 m_lnf;
  String m_accountAddress;
  PropertySet* m_accountConfig;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AccountWindow)
};

//==============================================================================
LoginComponent::LoginComponent(PropertiesFile* configFile) : m_configFile(configFile) {
  m_model = std::make_shared<AccountsModel>();
  m_model->addListener(this);
  m_accountsTable = std::make_unique<TableListBox>();
  m_accountsTable->setRowHeight(50);
  m_accountsTable->setRowSelectedOnMouseDown(true);
  m_accountsTable->setClickingTogglesRowSelection(true);
  m_accountsTable->setMultipleSelectionEnabled(false);
  m_accountsTable->setModel(this);
  addAndMakeVisible(m_accountsTable.get());

  auto& tableHeader = m_accountsTable->getHeader();
  tableHeader.setStretchToFitActive(true);
  tableHeader.addColumn(translate("Alias"), 1, 50);
  tableHeader.addColumn(translate("Address"), 2, 400);


  if (auto accountsXml = m_configFile->getXmlValue("accounts")) {
    PropertySet accounts;
    accounts.restoreFromXml(*accountsXml);
    auto keys = accounts.getAllProperties().getAllKeys();

    for (int i = 0; i < keys.size(); ++i) {
      Account account(keys[i]);
      accounts.getAllProperties().getValue(account.getAddress(), "");
      auto accountConfig = accounts.getXmlValue(account.getAddress());
      account.getConfig().restoreFromXml(*accountConfig);
      m_model->addItem(account, false);
    }
  }

  m_importPrivateKeyBtn = std::make_unique<TextButton>("Import Private Key");
  addAndMakeVisible(m_importPrivateKeyBtn.get());
  m_importPrivateKeyBtn->addListener(this);

  setSize(600, 400);
}

LoginComponent::~LoginComponent() {
  PropertySet accounts;
  for (int i = 0; i < m_model->size(); ++i) {
    auto& item = m_model->getReferenceAt(i);
    auto accountConfig = item.getConfig().createXml("account");
    accounts.setValue(item.getAddress(), accountConfig.get());
  }
  auto accountsXml = accounts.createXml("accounts");
  m_configFile->setValue("accounts", accountsXml.get());
  m_configFile->save();
}

void LoginComponent::paint(Graphics& g) {
}

void LoginComponent::resized() {
  auto bounds = getLocalBounds().reduced(20);
  m_accountsTable->setBounds(bounds.removeFromTop(300));
  bounds.removeFromTop(20);
  auto btnBounds = bounds.removeFromTop(40);
  m_importPrivateKeyBtn->setBounds(btnBounds.withSizeKeepingCentre(200, 40));
}

void LoginComponent::buttonClicked(Button* btn) {
  if (btn == m_importPrivateKeyBtn.get()) {
    AlertWindow w("Import Private Key",
                  "Enter your private key and it will be imported.",
                  AlertWindow::QuestionIcon);

    w.addTextEditor("privkey", "", "Private Key:", true);
    w.addTextEditor("alias", "", "Alias:", false);
    w.addButton("OK",     1, KeyPress(KeyPress::returnKey, 0, 0));
    w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

    if (w.runModalLoop() == 1) {
      auto privkeyHex = w.getTextEditorContents("privkey");
      if (privkeyHex.startsWith("0x")) {
        privkeyHex = privkeyHex.substring(2);
      }

      if (privkeyHex.length() != 64) {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                                         "Invalid Private Key!",
                                         "Private key should be exactly 32 bytes!");
        return;
      }

      const auto accountAlias = w.getTextEditorContents("alias");

      auto eth_address_hex = Utils::gen_ethereum_address(privkeyHex.toStdString());
      Account account(eth_address_hex);
      account.getConfig().setValue("private_key", privkeyHex);
      account.getConfig().setValue("eth_address", String(eth_address_hex));
      account.getConfig().setValue("account_alias", accountAlias);
      m_model->addItem(account, true);
    }
  }
}

void LoginComponent::componentVisibilityChanged(Component& component) {
  if (auto accountWindow = dynamic_cast<AccountWindow*>(&component)) {
    if (!accountWindow->isVisible())
      m_accountWindows.removeObject(accountWindow, true);
  }
}

AccountWindow* LoginComponent::getWindowByAddress(const String& address) {
  for (auto item : m_accountWindows) {
    if (item->getAddress() == address)
      return item;
  }

  return nullptr;
}

void LoginComponent::modelChanged(AbstractListModelBase* base) {
  m_accountsTable->updateContent();
  m_accountsTable->repaint();
}

void LoginComponent::openAccount(Account* account) {
  if (auto accountWindow = getWindowByAddress(account->getAddress())) {
    accountWindow->toFront(true);
  } else {
    accountWindow = new AccountWindow("Account " + account->getAlias(), account->getAddress(), &account->getConfig());
    accountWindow->addComponentListener(this);
    m_accountWindows.add(accountWindow);
  }
}

void LoginComponent::removeAccount(const Account& account) {
  m_accountWindows.removeObject(getWindowByAddress(account.getAddress()), true);
  m_model->removeItem(account);
}

// TableListBoxModel
//==============================================================================

int LoginComponent::getNumRows() {
  return m_model->size();
}

void LoginComponent::paintCell(Graphics& g,
                               int rowNumber, int columnId,
                               int width, int height,
                               bool rowIsSelected) {
  auto item = m_model->getAt(rowNumber);
  g.setColour(Colours::white);

  switch (columnId) {
    case 1: {
      g.drawText(item.getAlias(), 0, 0, width, height, Justification::centred);
      break;
    }
    case 2: {
      g.drawText(item.getAddress(), 0, 0, width, height, Justification::centredLeft);
      break;
    }
    default: {
    }
  }
}

void LoginComponent::paintRowBackground(Graphics& g,
                                        int rowNumber,
                                        int width, int height,
                                        bool rowIsSelected) {
  auto colour = LookAndFeel::getDefaultLookAndFeel().findColour(TableListBox::backgroundColourId);
  g.setColour(rowIsSelected ? colour.darker(0.3f) : colour);
  g.fillRect(0, 0, width, height);
}

void LoginComponent::cellClicked(int rowNumber, int columnId, const MouseEvent& e) {
    if (e.mods.isRightButtonDown()) {
    PopupMenu menu;
    menu.addItem("Open", [=] {
      openAccount(&m_model->getReferenceAt(rowNumber));
    });

    menu.addItem("Remove", [=] {
      removeAccount(m_model->getReferenceAt(rowNumber));
    });

    menu.showAt(m_accountsTable->getCellComponent(columnId, rowNumber));
  }
}

void LoginComponent::cellDoubleClicked(int rowNumber, int columnId, const MouseEvent& e) {
  if (e.mods.isLeftButtonDown()) {
    openAccount(&m_model->getReferenceAt(rowNumber));
  }
}
