/*
 * skiplist.h
 *
 *  Created on: Apr 24, 2011
 *      Author: changokkim
 */

#ifndef SKIPLIST_H_
#define SKIPLIST_H_

#include <limits.h>
#include <stdlib.h>
#include <ctime>
#include "locks.h"
#include <pthread.h>
#define MAX_LEVEL 24
using namespace baseLock;
namespace MCP_SKIPLIST{

template<class K, class V> class Node;
template<class K, class V> class PredsSuccs;

template <typename K, typename V>
class LazySkipList {
public:
  LazySkipList();
  ~LazySkipList();

  int size();
  bool traverse_check();
  Node<K, V>* get_entry(K key, int thread_num_);
    V* get_value(K key, int thread_num_ /*thread index*/);
  int find( K , Node<K, V> **preds, Node<K, V> **succs);
  bool contains( K , int thread_num_);
  int remove(K ,int thread_num_);
  bool add(K , V ,int thread_num_);
  inline int random_level(){
    int random_l = 0;
    while(coin_flip())
      random_l++;
    if( random_l > MAX_LEVEL)
      random_l = MAX_LEVEL;
    //cout << random_l << endl;
    return random_l;
  }
  double pseudo_random() { return random();}
  bool coin_flip() { return ( ( (rand() % 2) == 0 ) ? true : false);}

  void print_list();  // okido Apr 28
  PredsSuccs<K,V> *pred_and_succ;
private:
  Node<K, V> *head_;
  Node<K, V> *tail_;

  LazySkipList(LazySkipList &);
  LazySkipList& operator=(LazySkipList&);

};

template<typename K, typename V>
class PredsSuccs{
public:
  PredsSuccs(){
    preds = new Node<K, V> *[MAX_LEVEL+1];
          succs = new Node<K, V> *[MAX_LEVEL+1];
  }
  Node<K, V> **preds;
        Node<K, V> **succs;
};

template<typename K, typename V>
class Node{
public:
  // sentinel node constructor
  //Node( int key);
  //Node(K key, V value);
  //Node( int x, int height);
  Node();
  explicit Node(K key);
  explicit Node(K key, V value, int height);
  ~Node();

  void lock();
  void unlock();

  void spin_lock();
  void spin_unlock();

  Node **nexts_;  // next node
  K key_;  // key
  V value_;  // value
  int top_level_;  // max level
  bool marked_;  // logically deleted
  bool fully_linked_;  // abstract set

  Mutex mutex_;  // mutex lock
private:
  //static const int MAX_LEVEL = 1024;
  Spinlock spin_;  // spin lock
};

}



#endif /* SKIPLIST_H_ */
