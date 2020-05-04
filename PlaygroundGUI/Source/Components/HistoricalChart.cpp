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
#include <algorithm>
#include <utility>
#include "HistoricalChart.h"

class ChartView : public Component {
 public:
  ChartView(HistoricalChart* owner) : m_owner(owner) {
  }

  void paint(Graphics& g) override {
    g.setColour(Colours::black);

    Path path;
    const auto bounds = m_owner->m_margins.subtractedFrom(getLocalBounds()).toFloat();
    const auto origin = m_owner->m_minMaxByX.first;
    const float scalePathY = bounds.getHeight() / (m_owner->m_minMaxByY.second.y - origin.y);
    const float scalePathX = bounds.getWidth() / (m_owner->m_minMaxByX.second.x - origin.x);
    const Point<float> margin(m_owner->m_margins.getLeft(), m_owner->m_margins.getBottom());
    auto scalePoint = [&](const Point<float>& point) {
      return Point<float>((point.x - origin.x) * scalePathX, (point.y - origin.y) * scalePathY) + margin;
    };

    const auto firstPoint = scalePoint(m_owner->m_series[0]);
    path.startNewSubPath(firstPoint);
    for (int i = 1; i < m_owner->m_series.size(); ++i) {
      path.lineTo(scalePoint(m_owner->m_series[i]));
    }

    g.strokePath(path, PathStrokeType(1, PathStrokeType::curved), AffineTransform::verticalFlip(getHeight()));
  }

 private:
  HistoricalChart* m_owner;
};

//==============================================================================
HistoricalChart::HistoricalChart() {
  m_chartView = std::make_unique<ChartView>(this);
  m_viewPort = std::make_unique<Viewport>();
  m_viewPort->setViewedComponent(m_chartView.get(), false);
  addAndMakeVisible(m_viewPort.get());
}

HistoricalChart::~HistoricalChart() {
}

void HistoricalChart::paint(Graphics& g) {
  g.fillAll(Colours::white);
}

void HistoricalChart::resized() {
  m_viewPort->setBounds(getLocalBounds());
  m_chartView->setSize(static_cast<int>(getWidth() * m_scale), static_cast<int>(getHeight() * m_scale));
}

void HistoricalChart::setMargins(int leftMargin, int topMargin, int rightMargin, int bottomMargin) {
  m_margins = BorderSize<int>(topMargin, leftMargin, bottomMargin, rightMargin);
}

void HistoricalChart::scale(float scaleValue) {
  m_scale = scaleValue;
}

void HistoricalChart::setSeries(const Array<Point<float>>& series) {
  m_series = series;

  auto minMaxByY = std::minmax_element(m_series.begin(), m_series.end()
      , [](const Point<float>& p1, const Point<float>& p2){
    return p1.y < p2.y;
  });
  m_minMaxByY = {*minMaxByY.first, *minMaxByY.second};

  auto minMaxByX = std::minmax_element(m_series.begin(), m_series.end()
      , [](const Point<float>& p1, const Point<float>& p2){
    return p1.x < p2.x;
  });
  m_minMaxByX = {*minMaxByX.first, *minMaxByX.second};

  resized();
  repaint();
}
