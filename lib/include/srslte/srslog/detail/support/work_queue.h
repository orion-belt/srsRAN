/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLOG_DETAIL_SUPPORT_WORK_QUEUE_H
#define SRSLOG_DETAIL_SUPPORT_WORK_QUEUE_H

#include "srslte/srslog/detail/support/thread_utils.h"
#include <queue>

#ifndef SRSLOG_QUEUE_CAPACITY
#define SRSLOG_QUEUE_CAPACITY 8192
#endif

namespace srslog {

namespace detail {

//:TODO: this is a temp work queue.

/// Thread safe generic data type work queue.
template <typename T, size_t capacity = SRSLOG_QUEUE_CAPACITY>
class work_queue
{
  std::queue<T> queue;
  mutable condition_variable cond_var;
  static constexpr size_t threshold = capacity * 0.98;

public:
  work_queue() = default;

  work_queue(const work_queue&) = delete;
  work_queue& operator=(const work_queue&) = delete;

  /// Inserts a new element into the back of the queue. Returns false when the
  /// queue is full, otherwise true.
  bool push(const T& value)
  {
    cond_var.lock();
    // Discard the new element if we reach the maximum capacity.
    if (queue.size() > capacity) {
      cond_var.unlock();
      return false;
    }
    queue.push(value);
    cond_var.signal();
    cond_var.unlock();

    return true;
  }

  /// Inserts a new element into the back of the queue. Returns false when the
  /// queue is full, otherwise true.
  bool push(T&& value)
  {
    cond_var.lock();
    // Discard the new element if we reach the maximum capacity.
    if (queue.size() > capacity) {
      cond_var.unlock();
      return false;
    }
    queue.push(std::move(value));
    cond_var.signal();
    cond_var.unlock();

    return true;
  }

  /// Extracts the top most element from the queue.
  /// NOTE: This method blocks while the queue is empty.
  T pop()
  {
    cond_var.lock();

    while (queue.empty()) {
      cond_var.wait();
    }

    T elem = std::move(queue.front());
    queue.pop();

    cond_var.unlock();

    return elem;
  }

  /// Extracts the top most element from the queue.
  /// NOTE: This method blocks while the queue is empty or or until the
  /// programmed timeout expires. Returns a pair with a bool indicating if the
  /// pop has been successful.
  std::pair<bool, T> timed_pop(unsigned timeout_ms)
  {
    // Build an absolute time reference for the expiration time.
    timespec ts = condition_variable::build_timeout(timeout_ms);

    cond_var.lock();

    bool timedout = false;
    while (queue.empty() && !timedout) {
      timedout = cond_var.wait(ts);
    }

    // Did we wake up on timeout?
    if (timedout && queue.empty()) {
      cond_var.unlock();
      return {false, T{}};
    }

    // Here we have been woken up normally.
    T Item = std::move(queue.front());
    queue.pop();

    cond_var.unlock();

    return {true, std::move(Item)};
  }

  /// Capacity of the queue.
  size_t get_capacity() const { return capacity; }

  /// Returns true when the queue is almost full, otherwise returns false.
  bool is_almost_full() const
  {
    cond_var_scoped_lock lock(cond_var);

    return queue.size() > threshold;
  }
};

} // namespace detail

} // namespace srslog

#endif // SRSLOG_DETAIL_SUPPORT_WORK_QUEUE_H
