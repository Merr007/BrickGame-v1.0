#include "timer.h"

Timer_t new_timer(double timeout) {
  struct timespec prev = {0};
  timespec_get(&prev, TIME_UTC);
  Timer_t timer;
  timer.prev = prev;
  timer.timeout = timeout;
  return timer;
}

bool check_timeout(Timer_t *timer) {
  struct timespec curr = {0};
  bool is_timeout = false;
  timespec_get(&curr, TIME_UTC);

  is_timeout = timer->timeout <= ((curr.tv_sec - timer->prev.tv_sec) +
                                  ((curr.tv_nsec - timer->prev.tv_nsec) / 1e9));

  if (is_timeout) timer->prev = curr;

  return is_timeout;
}