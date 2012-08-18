/*
 * locks.cpp
 *
 *  Created on: Apr 24, 2011
 *      Author: changokkim
 */
#include <time.h>
#include "locks.h"

namespace baseLock {

Spinlock::Spinlock()
  : locked_(false) {
}

Spinlock::~Spinlock() {
}


void Spinlock::lock() {

  int failed = 0; // for watching failed times
  struct timespec tim, tim2;  // for sleeping time

  // loop until locked_ is releasd
  while (__sync_lock_test_and_set(&locked_, true)) {

    // sleep time: 10 nano seconds
    tim.tv_nsec = 10;

    // test on local cache to see until lock is released
    while( locked_ == true) {

      // increase failed count
      failed++;

      // if failed to grab lock more than 10 times, sleep
      if( failed > 10) {

        nanosleep( &tim, &tim2);

        // if timed waited nanosecond is higher than 1000 nanosecond, increase by 1 instead of exponentially to prevent possibility of starvation
        if( tim.tv_nsec < 1000) {

          tim.tv_nsec = tim.tv_nsec * 2;

        } else if( tim.tv_nsec > 4000) {


          tim.tv_nsec = tim.tv_nsec = 10;

        } else {

          tim.tv_nsec = tim.tv_nsec + 10;
        }

        // initialize faled count
        failed = 0;
      }
    }
  }
}


void Spinlock::unlock() {
  __sync_lock_release(&locked_);
}

}  // namespace base
