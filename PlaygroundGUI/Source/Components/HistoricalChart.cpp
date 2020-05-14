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
    const auto bounds = m_owner->m_margins.subtractedFrom(getLocalBounds()).toFloat();
    const auto minMaxByY = m_owner->m_minMaxByY;
    const float moveY = minMaxByY.first.y <= 0 ? 0 : -1 * minMaxByY.first.y;
    const float maxY = minMaxByY.second.y + moveY;
    const float scalePathY = maxY == 0 ? 1 : bounds.getHeight() / maxY;

    for (const auto& series : m_owner->m_seriesList) {
      g.setColour(series.m_colour);
      const auto minMaxByX = series.m_minMaxByX;

      Path path;
      const auto origin = bounds.getHeight() - minMaxByY.first.y;
      const float moveX = minMaxByX.first.x < 0 ?
          std::fabs(minMaxByX.first.x) : -1 * minMaxByX.first.x;
      const float maxX = minMaxByX.second.x + moveX;

      const float scalePathX = maxX == 0 ? 1 : bounds.getWidth() / maxX;
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

      PathStrokeType strokeType(1, PathStrokeType::curved);

      if (series.m_isDashed) {
        const Array<float> dashLengths = { 2.0f, 3.0f, 4.0f, 5.0f };
        strokeType.createDashedStroke(path, path, dashLengths.getRawDataPointer(), dashLengths.size());
      }
      g.strokePath(path, strokeType, AffineTransform::verticalFlip(getHeight()));
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

void HistoricalChart::addSeries(const Array<Point<float>>& series, Colour colour, bool isDashed) {
  m_seriesList.resize(m_seriesList.size() + 1);
  SeriesData& seriesData = m_seriesList.getReference(m_seriesList.size() - 1);
  seriesData.m_series = series;
  seriesData.m_colour = colour;
  seriesData.m_isDashed = isDashed;

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
}

void HistoricalChart::addSeries(const Array<Point<float>>& series, Colour colour, bool isDashed,
                                const std::pair<Point<float>, Point<float>>& minMaxByY) {
  m_seriesList.resize(m_seriesList.size() + 1);
  SeriesData& seriesData = m_seriesList.getReference(m_seriesList.size() - 1);
  seriesData.m_series = series;
  seriesData.m_minMaxByY = minMaxByY;
  seriesData.m_colour = colour;
  seriesData.m_isDashed = isDashed;

  auto minMaxByX = std::minmax_element(seriesData.m_series.begin(), seriesData.m_series.end()
      , [](const Point<float>& p1, const Point<float>& p2){
        return p1.x < p2.x;
      });
  seriesData.m_minMaxByX = {*minMaxByX.first, *minMaxByX.second};
}

void HistoricalChart::update() {
  auto minByY = std::min_element(m_seriesList.begin(), m_seriesList.end()
      , [](const SeriesData& s1, const SeriesData& s2){
        return s1.m_minMaxByY.first.y < s2.m_minMaxByY.first.y;
      })->m_minMaxByY.first;

  auto maxByY = std::max_element(m_seriesList.begin(), m_seriesList.end()
      , [](const SeriesData& s1, const SeriesData& s2){
        return s1.m_minMaxByY.second.y < s2.m_minMaxByY.second.y;
      })->m_minMaxByY.second;

  m_minMaxByY = {minByY, maxByY};
  resized();
}
