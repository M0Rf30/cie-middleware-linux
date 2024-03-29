#pragma once

#include <condition_variable>
#include <mutex>

class auto_reset_event {
 public:
  auto_reset_event(bool signaled = false) : signaled_(signaled) {}

  void set();
  void wait();

 private:
  std::mutex m_;
  std::condition_variable cv_;
  bool signaled_;
};
