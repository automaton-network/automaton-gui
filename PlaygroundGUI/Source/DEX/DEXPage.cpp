/*
 * Automaton Playground
 * Copyright (c) 2020 The Automaton Authors.
 * Copyright (c) 2020 The automaton.network Authors.
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
#include "DEXPage.h"
#include "DEXManager.h"
#include "Utils/Utils.h"

static const String ETH_BALANCE_PREFIX_LABEL = "Eth Balance: ";
static const String AUTO_BALANCE_PREFIX_LABEL = "AUTO Balance: ";

class OrdersUIModel : public TableListBoxModel {
 public:
  enum Columns {
    Auto = 1
    , Eth
    , Owner
  };

  OrdersUIModel() {
  }

  void setModel(std::shared_ptr<AbstractListModel<Order::Ptr>> model) {
    m_model = model;
  }

  int getNumRows() override {
    return m_model != nullptr ? m_model->size() : 0;
  }

  void paintCell(Graphics& g,
                 int rowNumber, int columnId,
                 int width, int height,
                 bool rowIsSelected) override {
    auto item = m_model->getAt(rowNumber);
    g.setColour(Colours::white);

    switch (columnId) {
      case Auto: {
        g.drawText(item->getAuto().toString(10), 0, 0, width, height, Justification::centredLeft);
        break;
      }
      case Eth: {
        g.drawText(item->getEth().toString(10), 0, 0, width, height, Justification::centredLeft);
        break;
      }
      case Owner: {
        g.drawText(String(item->getOwner()), 0, 0, width, height, Justification::centredLeft);
        break;
      }
      default:
        break;
    }
  }

  void paintRowBackground(Graphics& g,
                          int rowNumber,
                          int width, int height,
                          bool rowIsSelected) override {
    auto colour = LookAndFeel::getDefaultLookAndFeel().findColour(TableListBox::backgroundColourId);
    g.setColour(rowIsSelected ? colour.darker(0.3f) : colour);
    g.fillRect(0, 0, width, height);
  }

 private:
  std::shared_ptr<AbstractListModel<Order::Ptr>> m_model;
};

DEXPage::DEXPage(Account::Ptr accountData) : m_accountData(accountData) {
  m_dexManager = m_accountData->getDexManager();

  m_sellingProxyModel = std::make_shared<OrdersProxyModel>();
  m_sellingProxyModel->setFilter(OrderFilter::Sell);
  m_sellingProxyModel->setModel(m_dexManager->getModel());
  m_sellingProxyModel->addListener(this);
  m_buyingProxyModel = std::make_shared<OrdersProxyModel>();
  m_buyingProxyModel->setFilter(OrderFilter::Buy);
  m_buyingProxyModel->setModel(m_dexManager->getModel());
  m_buyingProxyModel->addListener(this);

  m_sellingUIModel = std::make_unique<OrdersUIModel>();
  m_sellingUIModel->setModel(m_sellingProxyModel);
  m_buyingUIModel = std::make_unique<OrdersUIModel>();
  m_buyingUIModel->setModel(m_buyingProxyModel);

  m_ethBalanceLabel = std::make_unique<Label>("m_balanceLabel");
  m_ethBalanceLabel->setText(ETH_BALANCE_PREFIX_LABEL
                                + Utils::fromWei(CoinUnit::ether, m_accountData->getEthBalance()) + String(" ETH"),
                             NotificationType::dontSendNotification);
  m_autoBalanceLabel = std::make_unique<Label>("m_autoBalanceLabel");
  m_autoBalanceLabel->setText(AUTO_BALANCE_PREFIX_LABEL
                                + Utils::fromWei(CoinUnit::AUTO, m_accountData->getAutoBalance()) + String(" AUTO"),
                              NotificationType::dontSendNotification);


  m_sellingLabel = std::make_unique<Label>("m_sellingLabel", "Selling:");
  m_sellingLabel->setColour(Label::textColourId, Colours::red);
  m_sellingLabel->setFont(m_sellingLabel->getFont().withHeight(35));
  m_buyingLabel = std::make_unique<Label>("m_buyingLabel", "Buying:");
  m_buyingLabel->setColour(Label::textColourId, Colours::green);
  m_buyingLabel->setFont(m_sellingLabel->getFont().withHeight(35));

  m_createSellOrderBtn = std::make_unique<TextButton>(translate("Sell order"));
  m_createSellOrderBtn->addListener(this);

  m_createBuyOrderBtn = std::make_unique<TextButton>(translate("Buy order"));
  m_createBuyOrderBtn->addListener(this);

  m_sellingTable = std::make_unique<TableListBox>();
  m_sellingTable->setModel(m_sellingUIModel.get());
  auto& sellingHeader = m_sellingTable->getHeader();
  sellingHeader.setStretchToFitActive(true);
  sellingHeader.addColumn(translate("Auto"), OrdersUIModel::Auto, 50);
  sellingHeader.addColumn(translate("Eth"), OrdersUIModel::Eth, 50);
  sellingHeader.addColumn(translate("Owner"), OrdersUIModel::Owner, 200);

  m_buyingTable = std::make_unique<TableListBox>();
  m_buyingTable->setModel(m_buyingUIModel.get());
  auto& buyingHeader = m_buyingTable->getHeader();
  buyingHeader.setStretchToFitActive(true);
  buyingHeader.addColumn(translate("Auto"), OrdersUIModel::Auto, 50);
  buyingHeader.addColumn(translate("Eth"), OrdersUIModel::Eth, 50);
  buyingHeader.addColumn(translate("Owner"), OrdersUIModel::Owner, 200);

  addAndMakeVisible(m_ethBalanceLabel.get());
  addAndMakeVisible(m_autoBalanceLabel.get());
  addAndMakeVisible(m_sellingLabel.get());
  addAndMakeVisible(m_buyingLabel.get());
  addAndMakeVisible(m_createSellOrderBtn.get());
  addAndMakeVisible(m_createBuyOrderBtn.get());
  addAndMakeVisible(m_sellingTable.get());
  addAndMakeVisible(m_buyingTable.get());
}

DEXPage::~DEXPage() {
}

void DEXPage::paint(Graphics& g) {
}

void DEXPage::resized() {
  const int buttonsHeight = 50;
  const int buttonsSpacing = 10;
  const int buttonsWidth = (getLocalBounds().getWidth() - buttonsSpacing) / 2;
  auto bounds = getLocalBounds();
  auto buttonsArea = bounds.removeFromBottom(buttonsHeight);
  m_createSellOrderBtn->setBounds(buttonsArea.removeFromLeft(buttonsWidth));
  buttonsArea.removeFromLeft(buttonsSpacing);
  m_createBuyOrderBtn->setBounds(buttonsArea.removeFromLeft(buttonsWidth));
  bounds.removeFromBottom(buttonsSpacing);

  auto labelsBounds = bounds.removeFromTop(30);
  m_ethBalanceLabel->setBounds(labelsBounds.removeFromLeft(getWidth() / 2));
  m_autoBalanceLabel->setBounds(labelsBounds);

  auto tablesBounds = bounds;
  const int tablesMargin = 10;
  const int titlesHeight = 50;
  auto sellingBounds = tablesBounds.removeFromLeft(getWidth() / 2).reduced(tablesMargin);
  m_sellingLabel->setBounds(sellingBounds.removeFromTop(titlesHeight));
  m_sellingTable->setBounds(sellingBounds);
  auto buyingBounds = tablesBounds.reduced(tablesMargin);
  m_buyingLabel->setBounds(buyingBounds.removeFromTop(titlesHeight));
  m_buyingTable->setBounds(buyingBounds);
}

void DEXPage::buttonClicked(Button* buttonThatWasClicked) {
  if (buttonThatWasClicked == m_createSellOrderBtn.get()
      || buttonThatWasClicked == m_createBuyOrderBtn.get()) {
    const bool isSellOrder = buttonThatWasClicked == m_createSellOrderBtn.get();
    AlertWindow w(isSellOrder ? "Create a sell order" : "Create a buy order",
                  isSellOrder ? "Enter how many AUTO you want to sell to get ETH"
                              : "Enter how many ETH you want to spend to buy AUTO",
                  AlertWindow::QuestionIcon);

    w.addTextEditor("amountAUTO", "", "Amount AUTO:", false);
    w.addTextEditor("amountETH", "", "Amount ETH:", false);
    w.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
    w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

    w.getTextEditor("amountAUTO")->setInputRestrictions(8, Utils::numericalFloatAllowed);
    w.getTextEditor("amountETH")->setInputRestrictions(8, Utils::numericalFloatAllowed);

    if (w.runModalLoop() == 1) {
      const auto amountAUTO = w.getTextEditorContents("amountAUTO").getDoubleValue();
      const auto amountETH = w.getTextEditorContents("amountETH").getDoubleValue();
      if (amountAUTO > 0.0 && amountETH > 0.0) {
        const auto amountAUTOWei = Utils::toWei(CoinUnit::AUTO, w.getTextEditorContents("amountAUTO"));
        const auto amountETHWei = Utils::toWei(CoinUnit::ether, w.getTextEditorContents("amountETH"));
        if (isSellOrder)
          m_dexManager->createSellOrder(amountAUTOWei, amountETHWei);
        else
          m_dexManager->createBuyOrder(amountAUTOWei, amountETHWei);
      } else {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                                         "Invalid data",
                                         "Enter correct AUTO and ETH amounts");
      }
    }
  }
}

void DEXPage::modelChanged(AbstractListModelBase* model) {
  m_ethBalanceLabel->setText(ETH_BALANCE_PREFIX_LABEL
                                + Utils::fromWei(CoinUnit::ether, m_accountData->getEthBalance()) + String(" ETH"),
                             NotificationType::dontSendNotification);
  m_autoBalanceLabel->setText(AUTO_BALANCE_PREFIX_LABEL
                                + Utils::fromWei(CoinUnit::AUTO, m_accountData->getAutoBalance()) + String(" AUTO"),
                              NotificationType::dontSendNotification);

  if (model == m_sellingProxyModel.get()) {
    m_sellingTable->updateContent();
    m_sellingTable->repaint();
  } else if (model == m_buyingProxyModel.get()) {
    m_buyingTable->updateContent();
    m_buyingTable->repaint();
  }
}
