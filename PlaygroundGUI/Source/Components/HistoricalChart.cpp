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
    for (const auto& series : m_owner->m_seriesList) {
      g.setColour(series.m_colour);

      Path path;
      const auto bounds = m_owner->m_margins.subtractedFrom(getLocalBounds()).toFloat();
      const float moveY = series.m_minMaxByY.first.y < 0 ? std::fabs(series.m_minMaxByY.first.y) : 0;
      const float moveX = series.m_minMaxByX.first.x < 0 ?
          std::fabs(series.m_minMaxByX.first.x) : -1 * series.m_minMaxByX.first.x;

      const float scalePathY = bounds.getHeight() / (series.m_minMaxByY.second.y + moveY);
      const float scalePathX = bounds.getWidth() / (series.m_minMaxByX.second.x + moveX);
      const Point<float> margin(m_owner->m_margins.getLeft(), m_owner->m_margins.getBottom());
      auto scalePoint = [&](const Point<float>& point) {
        auto p = Point<float>((point.x + moveX) * scalePathX, (point.y + moveY) * scalePathY) + margin;
        return p;
      };

      const auto firstPoint = scalePoint(series.m_series[0]);
      path.startNewSubPath(firstPoint);
      for (int i = 1; i < series.m_series.size(); ++i) {
        path.lineTo(scalePoint(series.m_series[i]));
      }

      g.strokePath(path, PathStrokeType(1, PathStrokeType::curved), AffineTransform::verticalFlip(getHeight()));
    }
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

void HistoricalChart::clear() {
  m_seriesList.clearQuick();
}

void HistoricalChart::addSeries(const Array<Point<float>>& series, Colour colour) {
  m_seriesList.resize(m_seriesList.size() + 1);
  SeriesData& seriesData = m_seriesList.getReference(m_seriesList.size() - 1);
  seriesData.m_series = series;
  seriesData.m_colour = colour;

  auto minMaxByY = std::minmax_element(seriesData.m_series.begin(), seriesData.m_series.end()
      , [](const Point<float>& p1, const Point<float>& p2){
    return p1.y < p2.y;
  });
  seriesData.m_minMaxByY = {*minMaxByY.first, *minMaxByY.second};

  auto minMaxByX = std::minmax_element(seriesData.m_series.begin(), seriesData.m_series.end()
      , [](const Point<float>& p1, const Point<float>& p2){
    return p1.x < p2.x;
  });
  seriesData.m_minMaxByX = {*minMaxByX.first, *minMaxByX.second};

  m_seriesList.add(seriesData);
}

void HistoricalChart::addSeries(const Array<Point<float>>& series,
                               std::pair<Point<float>, Point<float>> minMaxByY,
                               Colour colour) {
  m_seriesList.resize(m_seriesList.size() + 1);
  SeriesData& seriesData = m_seriesList.getReference(m_seriesList.size() - 1);
  seriesData.m_series = series;
  seriesData.m_minMaxByY = minMaxByY;
  seriesData.m_colour = colour;

  auto minMaxByX = std::minmax_element(seriesData.m_series.begin(), seriesData.m_series.end()
      , [](const Point<float>& p1, const Point<float>& p2){
        return p1.x < p2.x;
      });
  seriesData.m_minMaxByX = {*minMaxByX.first, *minMaxByX.second};
}

void HistoricalChart::update() {
  resized();
}
