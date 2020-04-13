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
#include "../Utils/Utils.h"
#include "../Config/Config.h"
#include "../Data/AutomatonContractData.h"

class AccountWindow : public DocumentWindow {
 public:
  AccountWindow(String title, String accountAddress, Config* accountConfig)
      : DocumentWindow(title,
                    Desktop::getInstance()
                    .getDefaultLookAndFeel()
                    .findColour(ResizableWindow::backgroundColourId),
                    DocumentWindow::allButtons)
      , m_accountAddress(accountAddress)
      , m_accountConfig(accountConfig) {
    LookAndFeel::setDefaultLookAndFeel(&m_lnf);

    setUsingNativeTitleBar(false);
    setContentOwned(new DemosMainComponent(m_accountConfig), true);
    setTitleBarButtonsRequired(closeButton | minimiseButton | maximiseButton, false);

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
  Config* m_accountConfig;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AccountWindow)
};

//==============================================================================
LoginComponent::LoginComponent(ConfigFile* configFile) : m_configFile(configFile) {
  m_model = std::make_shared<AccountsModel>();
  m_model->addListener(this);
  m_accountsTable = std::make_unique<TableListBox>();
  m_accountsTable->setRowHeight(32);
  m_accountsTable->setRowSelectedOnMouseDown(true);
  m_accountsTable->setClickingTogglesRowSelection(true);
  m_accountsTable->setMultipleSelectionEnabled(false);
  m_accountsTable->setModel(this);
  addAndMakeVisible(m_accountsTable.get());

  auto& tableHeader = m_accountsTable->getHeader();
  tableHeader.setStretchToFitActive(true);
  tableHeader.addColumn(translate("Alias"), 1, 50);
  tableHeader.addColumn(translate("Address"), 2, 300);


  auto accountsJson = m_configFile->get_json("accounts");
  for (const auto& el : accountsJson.items()) {
    Account account(el.key());
    account.getConfig().restoreFrom_json(el.value());
    m_model->addItem(account, false);
  }

  m_importPrivateKeyBtn = std::make_unique<TextButton>("Import");
  addAndMakeVisible(m_importPrivateKeyBtn.get());
  m_importPrivateKeyBtn->addListener(this);

  m_logo = Drawable::createFromImageData(
      BinaryData::logo_white_on_transparent_8x8_svg,
      BinaryData::logo_white_on_transparent_8x8_svgSize);
  addAndMakeVisible(m_logo.get());

  m_rpcLabel = std::make_unique<Label>("m_rpcLabel", "Ethereum RPC:");
  addAndMakeVisible(m_rpcLabel.get());
  m_rpcEditor = std::make_unique<TextEditor>("m_rpcEditor");
  addAndMakeVisible(m_rpcEditor.get());

  m_contractAddrLabel = std::make_unique<Label>("m_contractAddrLabel", "Contract Address: ");
  addAndMakeVisible(m_contractAddrLabel.get());
  m_contractAddrEditor = std::make_unique<TextEditor>("m_contractAddrEditor");
  m_contractAddrEditor->setInputRestrictions(42, "0123456789abcdefABCDEFx");
  addAndMakeVisible(m_contractAddrEditor.get());

  m_readContractBtn = std::make_unique<TextButton>("Login");
  m_readContractBtn->addListener(this);
  addAndMakeVisible(m_readContractBtn.get());

  auto cd = AutomatonContractData::getInstance();
  m_rpcEditor->setText(cd->getUrl());
  m_contractAddrEditor->setText(cd->getAddress());
  switchLoginState(true);

  setSize(350, 600);
}

LoginComponent::~LoginComponent() {
  json accounts;
  for (int i = 0; i < m_model->size(); ++i) {
    auto& item = m_model->getReferenceAt(i);
    accounts[item.getAddress().toStdString()] = item.getConfig().to_json();
  }

  m_configFile->set_json("accounts", accounts);
  m_configFile->save_to_local_file();
}

void LoginComponent::paint(Graphics& g) {
}

void LoginComponent::resized() {
  auto bounds = getLocalBounds().reduced(20, 40);

  auto logoBounds = bounds.removeFromTop(39);
  m_logo->setBounds(logoBounds.withSizeKeepingCentre(231, 39));

  auto configBounds = bounds;
  configBounds.removeFromTop(20);
  auto rpcBounds = configBounds.removeFromTop(20);
  m_rpcLabel->setBounds(rpcBounds.removeFromLeft(100));
  m_rpcEditor->setBounds(rpcBounds);
  configBounds.removeFromTop(20);
  auto addrBounds = configBounds.removeFromTop(20);
  m_contractAddrLabel->setBounds(addrBounds.removeFromLeft(100));
  m_contractAddrEditor->setBounds(addrBounds);
  configBounds.removeFromTop(40);
  m_readContractBtn->setBounds(configBounds.removeFromTop(40).withSizeKeepingCentre(120, 40));

  bounds.removeFromTop(40);
  m_accountsTable->setBounds(bounds.removeFromTop(250));
  bounds.removeFromTop(40);
  auto btnBounds = bounds.removeFromTop(40);
  // m_importPrivateKeyBtn->setBounds(btnBounds.withSizeKeepingCentre(120, 40));

  // Temporarily display refresh button on the same screen after switching view
  if (m_importPrivateKeyBtn->isVisible()) {
      const auto btnWidth = 120;
      m_importPrivateKeyBtn->setBounds(btnBounds.removeFromLeft(btnWidth));
      m_readContractBtn->setBounds(btnBounds.removeFromRight(btnWidth));
  }
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
      account.getConfig().set_string("private_key", privkeyHex.toStdString());
      account.getConfig().set_string("eth_address", eth_address_hex);
      account.getConfig().set_string("account_alias", accountAlias.toStdString());
      m_model->addItem(account, true);
    }
  } else if (btn == m_readContractBtn.get()) {
    bool res = AutomatonContractData::getInstance()->readContract(m_rpcEditor->getText().toStdString(),
                                                                  m_contractAddrEditor->getText().toStdString());
    if (res)
      switchLoginState(false);
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
      g.setFont(14);
      g.drawText(item.getAlias(), 0, 0, width, height, Justification::centred);
      break;
    }
    case 2: {
      g.setFont(10);
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

void LoginComponent::switchLoginState(bool isNetworkConfig) {
  m_accountsTable->setVisible(!isNetworkConfig);
  m_importPrivateKeyBtn->setVisible(!isNetworkConfig);
  m_rpcLabel->setVisible(isNetworkConfig);
  m_rpcEditor->setVisible(isNetworkConfig);
  m_contractAddrLabel->setVisible(isNetworkConfig);
  m_contractAddrEditor->setVisible(isNetworkConfig);
  // m_readContractBtn->setVisible(isNetworkConfig);

  // Temporarily always display read contract button after switching view
  m_readContractBtn->setVisible(true);
  if (!isNetworkConfig)
    m_readContractBtn->setButtonText("Refresh");
  resized();
}
