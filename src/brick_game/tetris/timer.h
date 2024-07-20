#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>
#include <time.h>

#include "defines.h"

typedef struct {
  struct timespec prev;
  double timeout;
} Timer_t;

Timer_t new_timer(double timeout);
bool check_timeout(Timer_t *timer);

#endif