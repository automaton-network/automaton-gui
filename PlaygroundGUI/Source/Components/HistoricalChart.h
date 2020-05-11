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

#include <JuceHeader.h>

//==============================================================================
/*
*/

class ChartView;

class HistoricalChart : public Component {
 public:
  HistoricalChart();
  ~HistoricalChart();

  void paint(Graphics& g) override;
  void resized() override;

  void setMargins(int leftMargin, int topMargin, int rightMargin, int bottomMargin);
  void scale(float scaleValue);
  void clear();
  void addSeries(const Array<Point<float>>& series, Colour colour);
  void addSeries(const Array<Point<float>>& series, std::pair<Point<float>, Point<float>> minMaxByY, Colour colour);
  void update();

 private:
  struct SeriesData {
    Array<Point<float>> m_series;
    Colour m_colour;
    std::pair<Point<float>, Point<float>> m_minMaxByY;
    std::pair<Point<float>, Point<float>> m_minMaxByX;
  };

  Array<SeriesData> m_seriesList;
  std::unique_ptr<ChartView> m_chartView;
  std::unique_ptr<Viewport> m_viewPort;

  float m_scale = 1.0f;
  BorderSize<int> m_margins;

  friend class ChartView;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HistoricalChart)
};
