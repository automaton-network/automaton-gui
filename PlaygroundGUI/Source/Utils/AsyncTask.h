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

#include "JuceHeader.h"
#include "automaton/core/common/status.h"

using automaton::core::common::status;

class AsyncTask : public Thread
                , public AsyncUpdater {
 public:
  AsyncTask(std::function<bool(AsyncTask*)> fun,
            std::function<void(AsyncTask*)> postAsyncAction,
            const String& title)
      : m_status(status::ok())
      , m_title(title)
      , m_fun(fun)
      , m_postAsyncAction(postAsyncAction)
      , Thread(title) {
  }

  void runThread(std::function<void(AsyncTask*)> onComplete) {
    m_onComplete = onComplete;
    startThread();
  }

  void setProgress(const double newProgress) {
    m_progress = newProgress;
  }

  void setStatusMessage(const String& newStatusMessage) {
    const ScopedLock sl(m_messageLock);
    m_message = newStatusMessage;
  }

  void handleAsyncUpdate() override {
    if (m_postAsyncAction != nullptr)
      m_postAsyncAction(this);

    writeStatusToLog();

    if (m_onComplete != nullptr)
      m_onComplete(this);
  }

  void writeStatusToLog() {
    Logger::writeToLog(String("(") + String(m_status.code) + String(") :") + m_status.msg);
  }

  const String& getOwnerId() const noexcept {
    return m_ownerId;
  }

  double& getProgress() {
    return m_progress;
  }

  status m_status;

  using Ptr = std::shared_ptr<AsyncTask>;

 private:
  void run() override {
    m_fun(this);
    triggerAsyncUpdate();
  }

 private:
  String m_ownerId;
  String m_title;
  double m_progress;
  String m_message;
  CriticalSection m_messageLock;
  std::function<bool(AsyncTask*)> m_fun;
  std::function<void(AsyncTask*)> m_postAsyncAction;
  std::function<void(AsyncTask*)> m_onComplete;
};


class TaskWithProgressWindow : public ThreadWithProgressWindow {
 public:
  TaskWithProgressWindow(std::function<bool(TaskWithProgressWindow*)> fun, const String& title)
      : ThreadWithProgressWindow(title, true, true)
      , m_status(status::ok()) {
    m_fun = fun;
  }

  void run() {
    m_fun(this);
  }

  status m_status;

 private:
  std::function<bool(TaskWithProgressWindow*)> m_fun;
};
