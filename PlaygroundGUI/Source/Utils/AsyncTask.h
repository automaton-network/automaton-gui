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

#include "automaton/core/common/status.h"

using automaton::core::common::status;

class AsyncTask : public ThreadWithProgressWindow {
 public:
  AsyncTask(std::function<bool(AsyncTask*)> fun, const String& title)
      : ThreadWithProgressWindow(title, true, true)
      , m_status(status::ok()) {
    m_fun = fun;
  }

  void run() {
    m_fun(this);
  }

  status m_status;

 private:
  std::function<bool(AsyncTask*)> m_fun;
};
