/*
 * thread.hpp
 *
 *  Created on: Apr 28, 2011
 *      Author: xiaoguangye
 */

#ifndef THREAD_HPP_
#define THREAD_HPP_
#include <pthread.h>
#include <tr1/functional>

namespace base {

// makeThread creates a new thread and returns its thread ID. The
// thread will run 'body', which can be any function or object method
// with no arguments and no return.
//
//
// Usage:
//   #include <tr1/functional>
//   using std::tr1::bind;
//
//   class Server {
//   public:
//     Server(...);
//     void doTask();
//   ...
//   };
//
//   Server my_server(...);
//
//   // Let's run my_server.doTask() on a separate thread, go do something else,
//   // and then wait for that thread to finish
//   pthread_t my_thread = makeThread(bind(&Server::doTask, &my_server);
//   ... go do something else ...
//   pthread_join(my_thread, NULL);
//
typedef std::tr1::function<void()> ThreadBody;
pthread_t makeThread(ThreadBody body);

}  // namespace base

#endif /* THREAD_HPP_ */
