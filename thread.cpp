/*
 * thread.cpp
 *
 *  Created on: Apr 28, 2011
 *      Author: xiaoguangye
 */

#include "thread.hpp"

namespace base {

static void* threadFunction(void* arg) {
  ThreadBody* c = reinterpret_cast<ThreadBody*>(arg);
  (*c)();
  delete c;
  return 0;
}

pthread_t makeThread(ThreadBody body) {
  // 'makeThread' may return before the thread it is creating is
  // actually scheduled. Therefore we pass a heap allocated copy of
  // the thread body down to 'threadFuction' instead of using 'body'
  // itself. Ownership of the copy is transferred if the thread is
  // successfully created.
  ThreadBody* copy = new ThreadBody(body);

  void* arg = reinterpret_cast<void*>(copy);
  pthread_t thread;
  if (pthread_create(&thread, NULL, threadFunction, arg) != 0) {
    delete copy;
  }
  return thread;
}

}  // namespace base
