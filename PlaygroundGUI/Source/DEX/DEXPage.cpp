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
#include "OrdersManager.h"

enum Columns
{
  Auto = 1
  , Eth
  , Owner
};

DEXPage::DEXPage()
{
  m_sellingProxyModel = std::make_shared<OrdersProxyModel>();
  m_sellingProxyModel->setFilter (OrderFilter::Sell);
  m_sellingProxyModel->setModel (OrdersManager::getInstance()->getModel());
  m_buyingProxyModel = std::make_shared<OrdersProxyModel>();
  m_buyingProxyModel->setFilter (OrderFilter::Buy);
  m_sellingProxyModel->setModel (OrdersManager::getInstance()->getModel());

  m_sellingLabel = std::make_unique<Label>("m_sellingLabel", "Selling:");
  m_sellingLabel->setColour (Label::textColourId, Colours::red);
  m_sellingLabel->setFont (m_sellingLabel->getFont().withHeight (35));
  m_buyingLabel = std::make_unique<Label>("m_buyingLabel", "Buying:");
  m_buyingLabel->setColour (Label::textColourId, Colours::green);
  m_buyingLabel->setFont (m_sellingLabel->getFont().withHeight (35));

  m_sellingTable = std::make_unique<TableListBox>();
  auto& sellingHeader = m_sellingTable->getHeader();
  sellingHeader.setStretchToFitActive (true);
  sellingHeader.addColumn (translate ("Auto"), Columns::Auto, 50);
  sellingHeader.addColumn (translate ("Eth"), Columns::Eth, 50);
  sellingHeader.addColumn (translate ("Owner"), Columns::Owner, 200);

  m_buyingTable = std::make_unique<TableListBox>();
  auto& buyingHeader = m_buyingTable->getHeader();
  buyingHeader.setStretchToFitActive (true);
  buyingHeader.addColumn (translate ("Auto"), Columns::Auto, 50);
  buyingHeader.addColumn (translate ("Eth"), Columns::Eth, 50);
  buyingHeader.addColumn (translate ("Owner"), Columns::Owner, 200);

  addAndMakeVisible (m_sellingLabel.get());
  addAndMakeVisible (m_buyingLabel.get());
  addAndMakeVisible (m_sellingTable.get());
  addAndMakeVisible (m_buyingTable.get());
}

DEXPage::~DEXPage()
{
}

void DEXPage::paint (Graphics& g)
{
}

void DEXPage::resized()
{
  auto tablesBounds = getLocalBounds();
  const int tablesMargin = 10;
  const int titlesHeight = 50;
  auto sellingBounds = tablesBounds.removeFromLeft (getWidth() / 2).reduced (tablesMargin);
  m_sellingLabel->setBounds (sellingBounds.removeFromTop (titlesHeight));
  m_sellingTable->setBounds (sellingBounds);
  auto buyingBounds = tablesBounds.reduced (tablesMargin);
  m_buyingLabel->setBounds (buyingBounds.removeFromTop (titlesHeight));
  m_buyingTable->setBounds (buyingBounds);
}
