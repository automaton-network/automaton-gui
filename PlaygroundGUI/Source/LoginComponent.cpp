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
#include "MainComponent.h"
#include "LoginComponent.h"
#include "Utils.h"


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

    setFullScreen(true);
    setResizable(true, true);
    setResizeLimits(1200, 800, 10000, 10000);
    centreWithSize(600, 400);

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
  m_accountsComboBox = std::make_unique<ComboBox>();

  if (auto accountsXml = m_configFile->getXmlValue("accounts")) {
    PropertySet accounts;
    accounts.restoreFromXml(*accountsXml);
    auto keys = accounts.getAllProperties().getAllKeys();

    for (int i = 0; i < keys.size(); ++i) {
      auto account = keys[i];
      accounts.getAllProperties().getValue(account, "");
      auto accountConfig = accounts.getXmlValue(account);
      m_accountsConfigs[account].restoreFromXml(*accountConfig);
      m_accountsComboBox->addItem(account, i + 1);
    }
  }

  m_accountsComboBox->addListener(this);
  addAndMakeVisible(m_accountsComboBox.get());

  m_importPrivateKeyBtn = std::make_unique<TextButton>("Import Private Key");
  addAndMakeVisible(m_importPrivateKeyBtn.get());
  m_importPrivateKeyBtn->addListener(this);

  m_openAccountBtn = std::make_unique<TextButton>("Open account");
  m_openAccountBtn->setEnabled(false);
  addAndMakeVisible(m_openAccountBtn.get());
  m_openAccountBtn->addListener(this);

  setSize(600, 400);
}

LoginComponent::~LoginComponent() {
  PropertySet accounts;
  for (auto& item : m_accountsConfigs) {
    auto accountConfig = item.second.createXml("account");
    accounts.setValue(item.first, accountConfig.get());
  }
  auto accountsXml = accounts.createXml("accounts");
  m_configFile->setValue("accounts", accountsXml.get());
  m_configFile->save();
}

void LoginComponent::paint(Graphics& g) {
}

void LoginComponent::resized() {
  auto bounds = getLocalBounds().reduced(20);
  m_accountsComboBox->setBounds(bounds.removeFromTop(40));
  bounds.removeFromTop(20);
  auto btnBounds = bounds.removeFromTop(40);
  m_importPrivateKeyBtn->setBounds(btnBounds.removeFromLeft(200));
  btnBounds.removeFromLeft(20);
  m_openAccountBtn->setBounds(btnBounds.removeFromLeft(200));
}

void LoginComponent::buttonClicked(Button* btn) {
  if (btn == m_importPrivateKeyBtn.get()) {
    AlertWindow w("Import Private Key",
                  "Enter your private key and it will be imported.",
                  AlertWindow::QuestionIcon);

    w.addTextEditor("privkey", "", "Private Key:", true);
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

      auto eth_address_hex = Utils::gen_ethereum_address(privkeyHex.toStdString());
      m_accountsConfigs[eth_address_hex].setValue("private_key", privkeyHex);
      m_accountsConfigs[eth_address_hex].setValue("eth_address", String(eth_address_hex));
      m_accountsComboBox->addItem(eth_address_hex, m_accountsComboBox->getNumItems() + 1);
    }
  } else if (btn == m_openAccountBtn.get()) {
    const auto account = m_accountsComboBox->getText();

    if (auto accountWindow = getWindowByAddress(account)) {
      accountWindow->toFront(true);
    } else {
      accountWindow = new AccountWindow("Account " + account, account, &m_accountsConfigs[account]);
      accountWindow->addComponentListener(this);
      m_accountWindows.add(accountWindow);
    }
  }
}

void LoginComponent::comboBoxChanged(ComboBox* comboBoxThatHasChanged) {
  const auto selectedIndex = m_accountsComboBox->getSelectedItemIndex();
  m_openAccountBtn->setEnabled(selectedIndex >= 0);
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
