/*
 *    This is QTournament, a badminton tournament management program.
 *    Copyright (C) 2014 - 2017  Volker Knollmann
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <cassert>

using namespace std;

template <typename T>
class ThreadSafeQueue
{
public:
  void push(const T& val)
  {
    // acquire a lock on the mutex
    lock_guard<mutex> lk{queueMutex};

    // store the value
    q.push(val);

    // notify potentially waiting threads that
    // the queue is not empty anymore
    emptyCondition.notify_one();

    // lk is automatically unlocked by the
    // destructor of the lock_guard
  }

  bool hasData()
  {
    // acquire a lock on the mutex
    lock_guard<mutex> lk{queueMutex};

    return !(q.empty());

    // lk is automatically unlocked by the
    // destructor of the lock_guard
  }

  shared_ptr<T> blockingPop()
  {
    // acquire a lock on the mutex
    unique_lock<mutex> lk{queueMutex};  // "unique_lock", not "lock_guard"!

    // if the queue is empty (identified by the lambda expression)
    // lk is unlocked so that other threads can fll the queue.
    // if a filling thread calls push(), push issues a
    // notifyOne that wakes us up, re-acquires the lock
    // and continues
    emptyCondition.wait(lk, [this]{return !(q.empty());});
    assert(!q.empty());

    auto result = shared_ptr<T>(make_shared<T>(q.front()));
    q.pop();

    return result;

    // lk is automatically unlocked by the
    // destructor of the unique_lock
  }

  shared_ptr<T> nonblockingPop()
  {
    // acquire a lock on the mutex
    lock_guard<mutex> lk{queueMutex};

    if (q.empty()) return shared_ptr<T>(); // return nullptr if empty

    auto result = shared_ptr<T>(make_shared<T>(q.front()));
    q.pop();

    return result;

    // lk is automatically unlocked by the
    // destructor of the lock_guard
  }

protected:
  queue<T> q;
  mutex queueMutex;
  condition_variable emptyCondition;
};

using ThreadSafeIntQueue = ThreadSafeQueue<int>;

class ProgressQueue : public ThreadSafeIntQueue
{
public:
  ProgressQueue(int _maxVal = 100);
  void step(int numSteps = 1);
  void reset(int _maxVal = 100);

private:
  int maxVal = 100;
  int counter = 0;
  double scaleFac = 1.0;
};

#endif // THREADSAFEQUEUE_H
