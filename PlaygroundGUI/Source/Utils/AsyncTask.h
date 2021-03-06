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
                , public AsyncUpdater
                , public std::enable_shared_from_this<AsyncTask>{
 public:
  using Ptr = std::shared_ptr<AsyncTask>;

  class Listener {
   public:
    virtual ~Listener() {}
    virtual void taskStarted(AsyncTask::Ptr task) {}
    virtual void taskFinished(AsyncTask::Ptr task) {}
    virtual void taskMessageChanged(AsyncTask::Ptr task) {}
    virtual void taskProgressChanged(AsyncTask::Ptr task) {}
  };

  AsyncTask(std::function<bool(AsyncTask*)> fun,
            std::function<void(AsyncTask*)> postAsyncAction,
            const String& title,
            int64 ownerId)
      : m_status(status::ok())
      , m_title(title)
      , m_fun(fun)
      , m_postAsyncAction(postAsyncAction)
      , m_ownerId(ownerId)
      , Thread(title) {
    static uint64 globalTaskId = 0;
    ++globalTaskId;
    m_taskId = globalTaskId;
  }

  virtual ~AsyncTask() {
    cancelPendingUpdate();
  }

  void runThread(std::function<void(AsyncTask*)> onComplete) {
    m_listeners.call(&Listener::taskStarted, shared_from_this());
    m_onComplete = onComplete;
    startThread();
  }

  void setProgress(const double newProgress) {
    m_progress = newProgress;
    m_listeners.call(&Listener::taskProgressChanged, shared_from_this());
  }

  void setStatusMessage(const String& newStatusMessage) {
    const ScopedLock sl(m_messageLock);
    m_message = newStatusMessage;
    m_taskLog.add(newStatusMessage);
    Logger::writeToLog(newStatusMessage);

    m_listeners.call(&Listener::taskMessageChanged, shared_from_this());
  }

  void handleAsyncUpdate() override {
    if (m_postAsyncAction != nullptr)
      m_postAsyncAction(this);

    logStatus(m_status);

    m_listeners.call(&Listener::taskFinished, shared_from_this());

    if (m_onComplete != nullptr)
      m_onComplete(this);
  }

  void logStatus(status _status, const String& description = String()) {
    const auto descriptionBlock = description.isNotEmpty() ? "[" + description + "]" : "";
    const auto logMessage = Time::getCurrentTime().toString(true, true, true, true)
        + String(" code(") + String(_status.code) + String(") ") + descriptionBlock + String(": ") + _status.msg;
    m_taskLog.add(logMessage);
    Logger::writeToLog(logMessage);
  }

  uint64 getTaskId() const noexcept {
    return m_taskId;
  }

  StringArray getTaskLog() const noexcept {
    const ScopedLock sl(m_messageLock);
    return m_taskLog;
  }

  int64 getOwnerId() const noexcept {
    return m_ownerId;
  }

  double& getProgress() {
    return m_progress;
  }

  String getStatusMessage() {
    const ScopedLock sl(m_messageLock);
    return m_message;
  }

  const String& getTitle() {
    return m_title;
  }

  void addListener(Listener* listener) {
    m_listeners.add(listener);
  }

  void removeListener(Listener* listener) {
    m_listeners.remove(listener);
  }

  status m_status;

 private:
  void run() override {
    if (!threadShouldExit())
      m_fun(this);

    triggerAsyncUpdate();
  }

 private:
  uint64 m_taskId;
  int64 m_ownerId;
  double m_progress;

  String m_title;
  String m_message;
  StringArray m_taskLog;

  ListenerList<Listener> m_listeners;
  CriticalSection m_messageLock;

  std::function<bool(AsyncTask*)> m_fun;
  std::function<void(AsyncTask*)> m_postAsyncAction;
  std::function<void(AsyncTask*)> m_onComplete;
};
