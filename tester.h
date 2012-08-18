/*
 * mapconcurrent.hpp
 *
 *  Created on: Apr 28, 2011
 *      Author: xiaoguangye
 */

#ifndef MAPCONCURRENT_HPP_
#define MAPCONCURRENT_HPP_
#include <pthread.h>
#include "thread.hpp"
#include <sys/time.h>
#include <stdlib.h>
#include "skiplist.h"
#include "skiplist.cpp"
#include "ticks_clock.hpp"

#include <iterator>
#include "locks.h"
#include <map>

using base::TicksClock;
using namespace MCP_SKIPLIST;

class Tester{

public:
LazySkipList<int, int> *skipList;

std::map<int, int> *words;

Tester(){
  skipList = new LazySkipList<int, int>();
  words = new std::map<int,int>();
  _succ_insert_ = 0;
  _succ_delete_ = 0;
  _succ_get_ = 0;
  _succ_get_null_ = 0;
}

~Tester(){
  delete skipList;
  delete words;
}


void performance_test(int num_thread, int work_load, int range, double portion_get, double portion_add, double portion_remove, int debug){
  int num_add = num_thread * portion_add;
  if( num_add == 0 && portion_add != 0.0) {
    num_add = 1;
  }

  int num_remove = num_thread * portion_remove;
  if( num_remove == 0 && portion_remove != 0.00) {
    num_remove = 1;
  }

  int num_get = num_thread - (num_add + num_remove);
  if( num_get == 0 && portion_get != 0.0) {
    num_get = 1;
  }

  if( debug > 0)
    cout << "TEST(Performance): " << num_thread << " threads[" << num_get << " get, " << num_add << " add, " << num_remove << " remove]" << endl;

  if( num_thread != num_get + num_add + num_remove) {
    cout << "ERROR: The number of thread must be same as the sum of each num" << endl;
    exit(0);
  }
  work_load = work_load/num_thread;
  pthread_t *threads = new pthread_t[num_thread];

  TicksClock::Ticks before = TicksClock::getTicks();

  for(int i=0; i<num_thread;){
    if( num_add > 0) {
      threads[i] = base::makeThread(std::tr1::bind(&Tester::add_randomly, this, work_load, i, range));
      num_add--;
      i++;
    }
    if( num_remove > 0) {
      threads[i] = base::makeThread(std::tr1::bind(&Tester::remove_randomly, this, work_load, i, range));
      num_remove--;
      i++;
    }
    if( num_get > 0) {
      threads[i] = base::makeThread(std::tr1::bind(&Tester::get_randomly, this, work_load, i, range));
      num_get--;
      i++;
    }
  }

  for(int i=0; i<num_thread; i++){
    pthread_join(threads[i], NULL);
  }
  
  TicksClock::Ticks duration = TicksClock::getTicks() - before;
  if( debug > 0) {
    cout << "duration(cpu cycle): ";
    cout << duration << endl;
  }
  else {
    cout << duration << "\t";
  }

  delete threads;
}

void add_randomly(int work_load, int thread_num, int range){
  for(int i=0; i<work_load; i++){
    int l_random = rand() % range;
    // add successfully
    if(skipList->add(l_random, l_random, thread_num)){
    //  __sync_fetch_and_add(&_succ_add_, 1);
    }
  }
}

void remove_randomly(int work_load, int thread_num, int range){
  for(int i=0; i<work_load; i++){
    int l_random = rand() % range;
    // remove successfully
    if(skipList->remove(l_random, thread_num)){
    //  __sync_fetch_and_add(&_succ_remove_, 1);
    }
  }
}

void get_randomly(int work_load, int thread_num, int range){
  int l_random;
  for(int i=0; i<work_load; i++){
    l_random = rand() % range;
    skipList->get_value(l_random, thread_num);
  }
}

void find_randomly(int work_load, int thread_num, int range){
  int l_random;
  Node<int, int> **preds = new Node<int, int> *[MAX_LEVEL + 1];
  Node<int, int> **succs = new Node<int, int> *[MAX_LEVEL + 1];
  for(int i=0; i<work_load; i++){
    l_random = rand() % range;
    skipList->find(l_random, preds, succs);
  }
}


inline int find_random(int scope){
  int l_scope = scope;
  int l_random = rand() % l_scope;
  return l_random;
}

void rb_performance_test(int num_thread, int work_load, int range, double portion_find, double portion_add, double portion_remove, int debug){
  int num_add = num_thread * portion_add;
  if( num_add == 0 && portion_add != 0.0) {
    num_add = 1;
  }

  int num_remove = num_thread * portion_remove;
  if( num_remove == 0 && portion_remove != 0.00) {
    num_remove = 1;
  }

  int num_find = num_thread - (num_add + num_remove);
  if( num_find == 0 && portion_find != 0.0) {
    num_find = 1;
  }

  if( debug > 0)
    cout << "TEST(Performance): " << num_thread << " threads[" << num_find << " find, " << num_add << " add, " << num_remove << " remove]" << endl;

  if( num_thread != num_find + num_add + num_remove) {
    cout << "ERROR: The number of thread must be same as the sum of each num" << endl;
    exit(0);
  }
  work_load = work_load/num_thread;
  pthread_t *threads = new pthread_t[num_thread];

  TicksClock::Ticks before = TicksClock::getTicks();

  for(int i=0; i<num_thread;){
    if( num_add > 0) {
      threads[i] = base::makeThread(std::tr1::bind(&Tester::rb_add_randomly, this, work_load, i, range));
      num_add--;
      i++;
    }
    if( num_remove > 0) {
      threads[i] = base::makeThread(std::tr1::bind(&Tester::rb_remove_randomly, this, work_load, i, range));
      num_remove--;
      i++;
    }
    if( num_find > 0) {
      threads[i] = base::makeThread(std::tr1::bind(&Tester::rb_find_randomly, this, work_load, i, range));
      num_find--;
      i++;
    }
  }

  for(int i=0; i<num_thread; i++){
    pthread_join(threads[i], NULL);
  }
  
  TicksClock::Ticks duration = TicksClock::getTicks() - before;
  if( debug > 0) {
    cout << "duration(cpu cycles): ";
    cout << duration << endl;
  }
  else {
    cout << duration << "\t";
  }

  delete threads;
}

void rb_add_randomly(int work_load, int thread_num, int range){
  for(int i=0; i<work_load; i++){
    int l_random = rand() % range;
    baseLock::ScopedLock l(&mutex_);
    // add successfully
     words->insert(std::pair<int,int>(l_random,l_random));
  }
}

void rb_remove_randomly(int work_load, int thread_num, int range){
  for(int i=0; i<work_load; i++){
    int l_random = rand() % range;
    baseLock::ScopedLock l(&mutex_);
    words->erase(l_random);
  }
}

void rb_find_randomly(int work_load, int thread_num, int range){
  int l_random;
  for(int i=0; i<work_load; i++){
    baseLock::ScopedLock l(&mutex_);
    l_random = rand() % range;
    words->find(l_random);
  }
}

// Correctness Test Part
void sequential_add_test(int range){
  pthread_t threads[1000];
  struct timeval    tp1;
  struct timeval    tp2;

  gettimeofday(&tp1, NULL);

  for(int i=0; i<1; i++){
    threads[i] = base::makeThread(std::tr1::bind(&Tester::insert_randomly, this, 1048576, i, range));
  }
  for(int i=0; i<1; i++){
    pthread_join(threads[i], NULL);
  }

  gettimeofday(&tp2, NULL);

  std::cout<<(tp2.tv_sec-tp1.tv_sec)<<" Seconds "
     <<(tp2.tv_usec-tp1.tv_usec)<<" Mircoseconds"<<std::endl;

  printf("TEST(Sequential insert): \n");
  (_succ_insert_ == skipList->size() && skipList->traverse_check()) ?
    printf("PASS succ %d::lenght %d\n", _succ_insert_, skipList->size())
    :printf("FAILURE: Exact value (%d)::Error value (%d)\n", _succ_insert_, skipList->size());

}

 void concurrent_add_test(int num_threads, int range){
  int work_load_ = 1048576/num_threads;
  pthread_t threads[1000];
  struct timeval    tp1;
  struct timeval    tp2;

  gettimeofday(&tp1, NULL);

  for(int i=0; i<num_threads; i++){
    threads[i] = base::makeThread(std::tr1::bind(&Tester::insert_randomly, this, work_load_, i, range));
  }
  for(int i=0; i<num_threads; i++){
    pthread_join(threads[i], NULL);
  }

  gettimeofday(&tp2, NULL);

  std::cout<<(tp2.tv_sec-tp1.tv_sec)<<" Seconds "
     <<(tp2.tv_usec-tp1.tv_usec)<<" Mircoseconds"<<std::endl;

  printf("TEST(Concurrent insert %d threads): \n", num_threads);
  (_succ_insert_ == skipList->size() && skipList->traverse_check()) ?
    printf("PASS succ %d::lenght %d\n", _succ_insert_, skipList->size())
    :printf("FAILURE: Exact value (%d)::Error value (%d)\n", _succ_insert_, skipList->size());
}

void sequential_erase_test(int size, int range){
  pthread_t threads[1000];
  struct timeval    tp1;
  struct timeval    tp2;

  gettimeofday(&tp1, NULL);

  int l_size = size;

  for(int i=0; i<1; i++){
      threads[i] = base::makeThread(std::tr1::bind(&Tester::erase_randomly, this, 1048576, i, range));
  }
  for(int i=0; i<1; i++){
      pthread_join(threads[i], NULL);
  }

  gettimeofday(&tp2, NULL);

  std::cout<<(tp2.tv_sec-tp1.tv_sec)<<" Seconds "
    <<(tp2.tv_usec-tp1.tv_usec)<<" Mircoseconds"<<std::endl;

  printf("TEST(Sequential delete): \n");
  (_succ_delete_ == (l_size - skipList->size()) && skipList->traverse_check()) ?
    printf("PASS succ %d::lenght %d\n", _succ_delete_, skipList->size())
    :printf("FAILURE: Exact value (%d)::Error value (%d)\n", _succ_insert_, skipList->size());
}

  void concurrent_erase_test(int size,int num_threads, int range){
    int work_load_ = 1048576/num_threads;
    int l_size = size;
    pthread_t threads[1000];
    struct timeval    tp1;
    struct timeval    tp2;

    gettimeofday(&tp1, NULL);

  for(int i=0; i<num_threads; i++){
    threads[i] = base::makeThread(std::tr1::bind(&Tester::erase_randomly, this, work_load_, i, range));
  }
  for(int i=0; i<num_threads; i++){
    pthread_join(threads[i], NULL);
  }

  gettimeofday(&tp2, NULL);

  std::cout<<(tp2.tv_sec-tp1.tv_sec)<<" Seconds "
     <<(tp2.tv_usec-tp1.tv_usec)<<" Mircoseconds"<<std::endl;

  printf("TEST(Concurrent delete %d threads): \n", num_threads);
  (_succ_delete_ == (l_size - skipList->size()) && skipList->traverse_check()) ?
    printf("PASS succ %d::lenght %d\n", _succ_delete_, skipList->size())
    :printf("FAILURE: Exact value (%d)::Error value (%d)\n", _succ_insert_, skipList->size());
}

void concurrent_add_remove_test(int num_add_threads, int num_remove_threads, int range){
  int add_work_load_ = 1048576/num_add_threads;
  int remove_work_load_ = 1048576/num_remove_threads;
  pthread_t threads[1000];
  struct timeval    tp1;
  struct timeval    tp2;

  gettimeofday(&tp1, NULL);

  for(int i=0; i<num_add_threads; i++){
    threads[i] = base::makeThread(std::tr1::bind(&Tester::insert_randomly, this, add_work_load_, i, range));
  }
  for(int i= num_add_threads; i< num_add_threads+num_remove_threads; i++){
    threads[i] = base::makeThread(std::tr1::bind(&Tester::erase_randomly, this, remove_work_load_, i/*thread index*/, range));
  }
  for(int i=0; i<num_remove_threads+num_add_threads; i++){
    pthread_join(threads[i], NULL);
  }

  gettimeofday(&tp2, NULL);

  std::cout<<(tp2.tv_sec-tp1.tv_sec)<<" Seconds "
     <<(tp2.tv_usec-tp1.tv_usec)<<" Mircoseconds"<<std::endl;

  printf("TEST(Concurrent insert %d threads; remove %d threads): \n", num_add_threads, num_remove_threads);
  (_succ_insert_ - _succ_delete_ == skipList->size() && skipList->traverse_check()) ?
    printf("PASS insert(%d):: delete (%d) :: lenght (%d)\n", _succ_insert_, _succ_delete_, skipList->size())
    :printf("FAILURE: Insert(%d):: delete(%d)::Error value (%d)\n", _succ_insert_, _succ_delete_,skipList->size());
}

void concurrent_add_remove_getvalue_test(int num_add_threads, int num_remove_threads, int num_get_value_threads, int range){
  int add_work_load_ = 1048576/num_add_threads;
  int remove_work_load_ = 1048576/num_remove_threads;
  int get_value_load_ = 1048576/num_get_value_threads;
  pthread_t threads[1000];
  struct timeval    tp1;
  struct timeval    tp2;

  gettimeofday(&tp1, NULL);

  for(int i=0; i<num_add_threads; i++){
    threads[i] = base::makeThread(std::tr1::bind(&Tester::insert_randomly, this, add_work_load_, i, range));
  }
  for(int i= num_add_threads; i< num_add_threads+num_remove_threads; i++){
    threads[i] = base::makeThread(std::tr1::bind(&Tester::erase_randomly, this, remove_work_load_, i/*thread index*/, range));
  }
  for(int i= num_add_threads+num_remove_threads; i< num_add_threads+num_remove_threads+num_get_value_threads ; i++){
    threads[i] = base::makeThread(std::tr1::bind(&Tester::get_value, this, get_value_load_, i/*thread index*/, range));
  }
  for(int i=0; i<num_remove_threads+num_add_threads+num_get_value_threads; i++){
    pthread_join(threads[i], NULL);
  }

  gettimeofday(&tp2, NULL);

  std::cout<<(tp2.tv_sec-tp1.tv_sec)<<" Seconds "
     <<(tp2.tv_usec-tp1.tv_usec)<<" Mircoseconds"<<std::endl;

  printf("TEST(Concurrent insert %d threads; remove %d threads; get_value %d threads): \n",
    num_add_threads, num_remove_threads, num_get_value_threads);
  (_succ_insert_ - _succ_delete_ == skipList->size() && skipList->traverse_check() && _succ_get_ + _succ_get_null_ == 1048576) ?
    printf("PASS insert(%d):: delete (%d) :: lenght (%d) :: getnull (%d) :: get(%d)\n",
        _succ_insert_, _succ_delete_, skipList->size(), _succ_get_null_, _succ_get_)
    :printf("FAILURE: Insert(%d):: delete(%d):: lenght (%d):: getnull (%d) :: get(%d)\n",
        _succ_insert_, _succ_delete_,skipList->size(), _succ_get_null_, _succ_get_);

}

void concurrent_add_remove_getentry_test(int num_add_threads, int num_remove_threads, int num_get_entry_threads, int range){
  int add_work_load_ = 1048576/num_add_threads;
  int remove_work_load_ = 1048576/num_remove_threads;
  int get_entry_load_ = 1048576/num_get_entry_threads;
  pthread_t threads[1000];
  struct timeval    tp1;
  struct timeval    tp2;
  
  gettimeofday(&tp1, NULL);
  
  for(int i=0; i<num_add_threads; i++){
    threads[i] = base::makeThread(std::tr1::bind(&Tester::insert_randomly, this, add_work_load_, i, range));
  }
  for(int i= num_add_threads; i< num_add_threads+num_remove_threads; i++){
    threads[i] = base::makeThread(std::tr1::bind(&Tester::erase_randomly, this, remove_work_load_, i/*thread index*/, range));
  }
  for(int i= num_add_threads+num_remove_threads; i< num_add_threads+num_remove_threads+num_get_entry_threads ; i++){
    threads[i] = base::makeThread(std::tr1::bind(&Tester::get_entry, this, get_entry_load_, i/*thread index*/, range));
  }
  for(int i=0; i<num_remove_threads+num_add_threads+num_get_entry_threads; i++){
    pthread_join(threads[i], NULL);
  }
  
  gettimeofday(&tp2, NULL);
  
  std::cout<<(tp2.tv_sec-tp1.tv_sec)<<" Seconds "
    <<(tp2.tv_usec-tp1.tv_usec)<<" Mircoseconds"<<std::endl;
  
  printf("TEST(Concurrent insert %d threads; remove %d threads; get_entry %d threads): \n",
    num_add_threads, num_remove_threads, num_get_entry_threads);
  (_succ_insert_ - _succ_delete_ == skipList->size() && skipList->traverse_check() && _succ_get_ + _succ_get_null_ == 1048576) ?
  printf("PASS insert(%d):: delete (%d) :: lenght (%d) :: getnull (%d) :: get(%d)\n",
    _succ_insert_, _succ_delete_, skipList->size(), _succ_get_null_, _succ_get_)
  :printf("FAILURE: Insert(%d):: delete(%d):: lenght (%d):: getnull (%d) :: get(%d)\n",
    _succ_insert_, _succ_delete_,skipList->size(), _succ_get_null_, _succ_get_);
  
}

void get_value(int times, int thread_num_, int range){
  for(int i=0; i<times; i++){
    int key = rand()%range;
    int *value = skipList->get_value(key, thread_num_);
    if(value == NULL){
      __sync_fetch_and_add(&_succ_get_null_, 1);
      continue;
    }
    if(*value == key){
      __sync_fetch_and_add(&_succ_get_, 1);
      continue;
    }
  }
}

void get_entry(int times, int thread_num_, int range){
  for(int i=0; i<times; i++){
    int key = rand()%range;
    Node<int, int> *value = skipList->get_entry(key, thread_num_);
    if(value == NULL){
      __sync_fetch_and_add(&_succ_get_null_, 1);
      continue;
    }
    if(value->value_ == key){
      __sync_fetch_and_add(&_succ_get_, 1);
      continue;
    }
  }
}

void insert_randomly(int times, int thread_num_, int range){
  for(int i=0; i<times; i++){
    int l_random = rand() % range;
    // insert successfully
    if(skipList->add(l_random, l_random, thread_num_)){
      __sync_fetch_and_add(&_succ_insert_, 1);
    }
  }
}

void erase_randomly(int times, int thread_num_, int range){
  for(int i=0; i<times; i++){
    int l_random = rand() % range;
    // delete successfully
    if(skipList->remove(l_random, thread_num_)){
      __sync_fetch_and_add(&_succ_delete_, 1);
    }
  }
}

inline int get_random(int scope){
  int l_scope = scope;
  int l_random = rand() % l_scope;
  return l_random;
}

private:

  baseLock::Mutex mutex_;

  int _succ_insert_;
  int _succ_delete_;
  int _succ_get_;
  int _succ_get_null_;
};

#endif /* MAPCONCURRENT_HPP_ */
