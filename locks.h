/*
 * locks.h
 *
 *  Created on: Apr 24, 2011
 *      Author: changokkim
 */

#ifndef LOCKS_H_
#define LOCKS_H_

#include <iostream>
#include <pthread.h>

using std::cout;
using std::endl;

namespace baseLock {

class Spinlock {
public:
  Spinlock();
  ~Spinlock();

  void lock();
  void unlock();

private:
  int locked_;

  // Non-copyable, non-assignable
  Spinlock(Spinlock&);
  Spinlock& operator=(Spinlock&);
};

class Mutex {
public:
  Mutex()         {
	  //cout << "hello" << endl;
	  pthread_mutex_init(&m_, NULL); }
  ~Mutex()        { pthread_mutex_destroy(&m_); }

  void lock()     { pthread_mutex_lock(&m_); }
  void unlock()   { pthread_mutex_unlock(&m_); }

private:

  pthread_mutex_t m_;

  // Non-copyable, non-assignable
  Mutex(Mutex &);
  Mutex& operator=(Mutex&);
};

class ScopedLock {
public:
  explicit ScopedLock(Mutex* lock) : m_(lock) {
	  //cout << "here" << endl;
	  m_->lock(); }
  ~ScopedLock()   {
	  //cout << "there" << endl;
	  m_->unlock(); }

private:
  Mutex* m_;

  // Non-copyable, non-assignable
  ScopedLock(ScopedLock&);
  ScopedLock& operator=(ScopedLock&);
};
}  // namespace base


#endif /* LOCKS_H_ */
