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
#include <Components/HistoricalChart.h>
#include <Utils/TasksManager.h>
#include "LoginComponent.h"
#include "../MainComponent.h"
#include "../Utils/Utils.h"
#include "../Config/Config.h"
#include "../Data/AutomatonContractData.h"

class AccountWindow : public DocumentWindow {
 public:
  AccountWindow(String title, Account::Ptr accountData)
      : DocumentWindow(title,
                    Desktop::getInstance()
                    .getDefaultLookAndFeel()
                    .findColour(ResizableWindow::backgroundColourId),
                    DocumentWindow::allButtons)
      , m_accountAddress(accountData->getAddress()) {
    LookAndFeel::setDefaultLookAndFeel(&m_lnf);

    setUsingNativeTitleBar(false);
    setContentOwned(new DemosMainComponent(accountData), true);
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

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AccountWindow)
};

static const Array<RPCConfig> RPC_DEFAULT_LIST = {RPCConfig("http://127.0.0.1:7545", "Localhost 7545"),
                                                  RPCConfig("http://127.0.0.1:8545", "Localhost 8545")};

HistoricalChart* hc;

//==============================================================================
LoginComponent::LoginComponent(ConfigFile* configFile) : m_configFile(configFile) {
  m_rpcList = RPC_DEFAULT_LIST;
  m_accountsModel = std::make_shared<AccountsModel>();
  m_accountsModel->addListener(this);

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

  m_selectedRPC = m_configFile->get_string("selectedRPC", "");
  m_selectedContract = m_configFile->get_string("selectedContract", "");

  auto rpcJson = m_configFile->get_json("rpcList");
  for (const auto& el : rpcJson.items()) {
    m_rpcList.addIfNotAlreadyThere(RPCConfig(el.key(), el.value()));
  }

  auto contractsJson = m_configFile->get_json("contracts");
  for (const auto& el : contractsJson.items()) {
    Config config;
    config.restoreFrom_json(el.value());
    auto contract = std::make_shared<AutomatonContractData>(config);
    m_contracts[contract->getUrl()].add(contract);
  }


  auto accountsJson = m_configFile->get_json("accounts");
  for (const auto& el : accountsJson.items()) {
    AccountConfig account(el.key());
    account.getConfig().restoreFrom_json(el.value());
    m_accountsModel->addItem(account, NotificationType::dontSendNotification);
  }

  m_rpcComboBox = std::make_unique<ComboBox>();
  initRPCComboBox(m_rpcList);
  m_rpcComboBox->addListener(this);
  addAndMakeVisible(m_rpcComboBox.get());

  m_contractComboBox = std::make_unique<ComboBox>();
  initContractsComboBox(getCurrentContracts());
  m_contractComboBox->addListener(this);
  addAndMakeVisible(m_contractComboBox.get());

  m_importPrivateKeyBtn = std::make_unique<TextButton>("Import");
  addAndMakeVisible(m_importPrivateKeyBtn.get());
  m_importPrivateKeyBtn->addListener(this);

  m_logo = Drawable::createFromImageData(
      BinaryData::logo_white_on_transparent_8x8_svg,
      BinaryData::logo_white_on_transparent_8x8_svgSize);
  addAndMakeVisible(m_logo.get());

  m_rpcLabel = std::make_unique<Label>("m_rpcLabel", "Ethereum RPC:");
  addAndMakeVisible(m_rpcLabel.get());
  m_contractAddrLabel = std::make_unique<Label>("m_contractAddrLabel", "Contract Address: ");
  addAndMakeVisible(m_contractAddrLabel.get());

  setSize(350, 600);
}

LoginComponent::~LoginComponent() {
  m_accountWindows.clear();

  json contracts;
  for (auto& iter : m_contracts) {
    for (auto& item : iter.second)
      contracts[iter.first.toStdString() + item->getAddress()] = item->getConfig().to_json();
  }
  m_configFile->set_json("contracts", contracts);

  json accounts;
  for (int i = 0; i < m_accountsModel->size(); ++i) {
    auto& item = m_accountsModel->getReferenceAt(i);
    accounts[item.getAddress().toStdString()] = item.getConfig().to_json();
  }
  m_configFile->set_json("accounts", accounts);

  json rpcList;
  for (int i = 0; i < m_rpcList.size(); ++i) {
    auto item = m_rpcList[i];
    rpcList[item.m_url] = item.m_alias;
  }
  m_configFile->set_json("rpcList", rpcList);

  m_configFile->set_string("selectedRPC", m_selectedRPC);
  m_configFile->set_string("selectedContract", m_selectedContract);

  m_configFile->save_to_local_file();
}

void LoginComponent::paint(Graphics& g) {
}

void LoginComponent::resized() {
  auto bounds = getLocalBounds().reduced(20, 40);

  auto logoBounds = bounds.removeFromTop(39);
  m_logo->setBounds(logoBounds.withSizeKeepingCentre(231, 39));

  bounds.removeFromTop(20);
  auto rpcBounds = bounds.removeFromTop(20);
  m_rpcLabel->setBounds(rpcBounds.removeFromLeft(100));
  m_rpcComboBox->setBounds(rpcBounds);
  bounds.removeFromTop(20);
  auto addrBounds = bounds.removeFromTop(20);
  m_contractAddrLabel->setBounds(addrBounds.removeFromLeft(100));
  m_contractComboBox->setBounds(addrBounds);

  bounds.removeFromTop(20);
  m_accountsTable->setBounds(bounds.removeFromTop(200));
  bounds.removeFromTop(20);
  auto btnBounds = bounds.removeFromTop(40);
  m_importPrivateKeyBtn->setBounds(btnBounds);
}

void LoginComponent::buttonClicked(Button* btn) {
  if (btn == m_importPrivateKeyBtn.get()) {
    auto currentContract = getCurrentContract();
    if (currentContract == nullptr) {
      AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                                       "Invalid contract!",
                                       "Select valid RPC!");
      return;
    }

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
      AccountConfig account(eth_address_hex);
      account.getConfig().set_string("private_key", privkeyHex.toStdString());
      account.getConfig().set_string("eth_address", eth_address_hex);
      account.getConfig().set_string("account_alias", accountAlias.toStdString());
      m_accountsModel->addItem(account, NotificationType::sendNotification);
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

void LoginComponent::openAccount(AccountConfig* accountConfig) {
  if (auto accountWindow = getWindowByAddress(accountConfig->getAddress())) {
    accountWindow->toFront(true);
  } else {
    if (auto currentContract = getCurrentContract()) {
      if (currentContract->readContract()) {
        accountWindow = new AccountWindow("Account " + accountConfig->getAlias(),
                                          std::make_shared<Account>(accountConfig, currentContract));
        accountWindow->addComponentListener(this);
        m_accountWindows.add(accountWindow);
      }
    } else {
      AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                                       "Invalid contract!",
                                       "Select valid Contract and RPC!");
    }
  }
}

void LoginComponent::removeAccount(const AccountConfig& accountConfig) {
  m_accountWindows.removeObject(getWindowByAddress(accountConfig.getAddress()), true);
  m_accountsModel->removeItem(accountConfig, NotificationType::sendNotification);
}

void LoginComponent::comboBoxChanged(ComboBox* comboBoxThatHasChanged) {
  if (comboBoxThatHasChanged == m_rpcComboBox.get()) {
    if (m_rpcComboBox->getSelectedItemIndex() == m_rpcComboBox->getNumItems() - 1) {
      AlertWindow w("Add Custom RPC",
                    "Enter RPC Info.",
                    AlertWindow::QuestionIcon);

      w.addTextEditor("rpc", "", "Ethereum RPC:", false);
      w.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
      w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

      m_rpcComboBox->setSelectedId(-1, NotificationType::dontSendNotification);

      if (w.runModalLoop() == 1) {
        auto rpc = w.getTextEditorContents("rpc");
        addRPC(rpc);
      }
    } else {
      m_selectedRPC = getCurrentRPC().toStdString();
    }
    initContractsComboBox(getCurrentContracts());
  } else if (comboBoxThatHasChanged == m_contractComboBox.get()) {
    if (m_contractComboBox->getSelectedItemIndex() == m_contractComboBox->getNumItems() - 1) {
      const auto rpc = getCurrentRPC();
      if (rpc.isEmpty()) {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                                         "Invalid contract!",
                                         "Select valid RPC!");
        m_contractComboBox->setSelectedId(-1, NotificationType::dontSendNotification);
      } else {
        AlertWindow w("Add contract",
                      "Enter contract Info.",
                      AlertWindow::QuestionIcon);

        w.addTextEditor("contract", "", "Contract Address: ", false);
        w.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
        w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

        if (w.runModalLoop() == 1) {
          auto contractAddr = w.getTextEditorContents("contract");
          Config contractConfig;
          contractConfig.set_string("eth_url", getCurrentRPC().toStdString());
          contractConfig.set_string("contract_address", contractAddr.toStdString());
          addContract(contractConfig);
        }
      }
    } else {
      m_selectedContract = getCurrentContract()->getAddress();
    }
    initContractsComboBox(getCurrentContracts());
  }
}

// TableListBoxModel
//==============================================================================

int LoginComponent::getNumRows() {
  return m_accountsModel->size();
}

void LoginComponent::paintCell(Graphics& g,
                               int rowNumber, int columnId,
                               int width, int height,
                               bool rowIsSelected) {
  auto item = m_accountsModel->getAt(rowNumber);
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
      openAccount(&m_accountsModel->getReferenceAt(rowNumber));
    });

    menu.addItem("Remove", [=] {
      removeAccount(m_accountsModel->getReferenceAt(rowNumber));
    });

    menu.showAt(m_accountsTable->getCellComponent(columnId, rowNumber));
  }
}

void LoginComponent::cellDoubleClicked(int rowNumber, int columnId, const MouseEvent& e) {
  if (e.mods.isLeftButtonDown()) {
    openAccount(&m_accountsModel->getReferenceAt(rowNumber));
  }
}

void LoginComponent::initContractsComboBox(const Array<std::shared_ptr<AutomatonContractData>>& contracts) {
  m_contractComboBox->clear(NotificationType::dontSendNotification);
  for (auto& contract : contracts) {
    m_contractComboBox->addItem(contract->getAddress(), m_contractComboBox->getNumItems() + 1);

    if (contract->getAddress() == m_selectedContract)
      m_contractComboBox->setSelectedItemIndex(m_contractComboBox->getNumItems() - 1,
                                               NotificationType::dontSendNotification);
  }
  m_contractComboBox->addSeparator();
  m_contractComboBox->addItem("Add contract", m_contractComboBox->getNumItems() + 1);
}

void LoginComponent::initRPCComboBox(const Array<RPCConfig>& rpcList) {
  m_rpcComboBox->clear(NotificationType::dontSendNotification);
  for (auto& rpc : rpcList) {
    m_rpcComboBox->addItem(rpc.m_alias, m_rpcComboBox->getNumItems() + 1);

    if (rpc.m_url == m_selectedRPC)
      m_rpcComboBox->setSelectedItemIndex(m_rpcComboBox->getNumItems() - 1, NotificationType::dontSendNotification);
  }
  m_rpcComboBox->addSeparator();
  m_rpcComboBox->addItem("Add Custom RPC", m_rpcComboBox->getNumItems() + 1);
}

String LoginComponent::getCurrentRPC() {
  return m_rpcList[m_rpcComboBox->getSelectedItemIndex()].m_url;
}

Array<std::shared_ptr<AutomatonContractData>> LoginComponent::getCurrentContracts() {
  const auto rpc = getCurrentRPC();
  if (rpc.isNotEmpty()) {
    return m_contracts[rpc];
  }

  return Array<std::shared_ptr<AutomatonContractData>>();
}

std::shared_ptr<AutomatonContractData> LoginComponent::getCurrentContract() {
  if (m_rpcComboBox->getSelectedItemIndex() >= 0) {
    const auto rpc = getCurrentRPC();
    if (rpc.isNotEmpty()) {
      return m_contracts[rpc][m_contractComboBox->getSelectedItemIndex()];
    }
  }

  return nullptr;
}

void LoginComponent::addContract(const Config& config) {
  auto contract = std::make_shared<AutomatonContractData>(config);
  const auto rpc = getCurrentRPC();
  if (rpc.isEmpty())
    return;

  m_contracts[contract->getUrl()].add(contract);
}

void LoginComponent::addRPC(const String& rpc) {
  m_rpcList.addIfNotAlreadyThere(RPCConfig(rpc.toStdString(), rpc.toStdString()));
  initRPCComboBox(m_rpcList);
}
