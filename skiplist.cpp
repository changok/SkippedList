/*
 * skiplist.cpp
 *
 *  Created on: Apr 24, 2011
 *      Author: changokkim
 */

#include <stdio.h>
#include "skiplist.h"
namespace MCP_SKIPLIST{

template<typename K, typename V>
LazySkipList<K, V>::LazySkipList() :head_(new Node<K, V>(INT_MIN)), tail_(new Node<K, V>(INT_MAX)){
  for( int i = 0; i < MAX_LEVEL+1; i++) {
    (head_->nexts_)[i] = tail_;
    (tail_->nexts_)[i] = NULL;
  }
  
  pred_and_succ = new PredsSuccs<K,V> [512];
}

template<typename K, typename V>
LazySkipList<K, V>::~LazySkipList() {
  Node<K, V> *curr = head_;
  Node<K, V> *next = head_;
  // keep looping if list is not empty yet
  while(curr->nexts_[0] != NULL){
    next = curr->nexts_[0];
    delete curr;
    curr = next;
  }
  delete curr;
  delete pred_and_succ;
}

template<typename K, typename V>
int LazySkipList<K, V>::size() {
  int length = -1;
  Node<K, V> *curr = head_;
  if(curr->nexts_[0] == NULL)
    return 0;
  // keep looping if list is not empty yet
  while(curr->nexts_[0] != NULL){
    curr = curr->nexts_[0];
    length++;
  }
  return length;
}

template<typename K, typename V>
bool LazySkipList<K, V>::traverse_check(){
  Node<K, V> *curr = head_;
    for(int i=MAX_LEVEL; i>=0; i--){
        while(curr->nexts_[i] != NULL){
      Node<K, V> *pre = curr;
      curr = curr->nexts_[i];
      if(curr->key_ <= pre->key_)
        return false;
    }
    }
  return true;
}


template<typename K, typename V>
bool LazySkipList<K, V>::contains(K key, int thread_num_) {
  Node<K, V> **preds = pred_and_succ[thread_num_].preds;
  Node<K, V> **succs = pred_and_succ[thread_num_].succs;
  int lFound = find(key, preds, succs);
  return ( lFound != -1
      && succs[lFound]->fully_linked_
      && !succs[lFound]->marked_);
}
template<typename K, typename V>
int LazySkipList<K, V>::find(K key, Node<K, V> **preds, Node<K, V> **succs) {
  K l_key = key;  // hash will change value to key ******
  int lFound = -1;
  Node<K, V> *pred = head_;
  for( int level = MAX_LEVEL; level >= 0; level--) {
    Node<K, V> *curr = (pred->nexts_)[level];
    while( l_key > curr->key_) {
      pred = curr;
      curr = (pred->nexts_)[level];
    }
    if( lFound == -1 && l_key == curr->key_) {
      lFound = level;
    }
    preds[level] = pred;
    succs[level] = curr;
  }
  return lFound;
}

template<typename K, typename V>
bool LazySkipList<K, V>::add(K key, V value, int thread_num_) {
  int topLevel = random_level();
  Node<K, V> **preds = pred_and_succ[thread_num_].preds;
  Node<K, V> **succs = pred_and_succ[thread_num_].succs;

  while (true) {
    int lFound = find(key, preds, succs);
    if (lFound != -1) {
    Node<K, V> *nodeFound = succs[lFound];
      if (!nodeFound->marked_) {
        while (!nodeFound->fully_linked_) {}
        return false;
      }
      continue;
    }
    int highestLocked = -1;
    Node<K, V> *pred, *succ;
    bool valid = true;
    Node<K, V> *previous = NULL;
    for (int level = 0; valid && (level <= topLevel); level++) {
      pred = preds[level];
      succ = succs[level];
      if( previous != preds[level])
        pred->lock();
      previous = preds[level];
      highestLocked = level;
      valid = !pred->marked_ && !succ->marked_ && pred->nexts_[level]==succ;
    }
    if (!valid) {
      Node<K, V> *previous = NULL;
      for (int level = 0; level <= highestLocked; level++)
      {
        if(previous == NULL || previous != preds[level])
          preds[level]->unlock();
        previous = preds[level];
      }
      continue;
    }
    Node<K, V> *newNode = new Node<K, V>(key, value, topLevel);
    for (int level = 0; level <= topLevel; level++)
      newNode->nexts_[level] = succs[level];
    for (int level = 0; level <= topLevel; level++)
      preds[level]->nexts_[level] = newNode;
    newNode->fully_linked_ = true;
    previous = NULL;
    for (int level = 0; level <= highestLocked; level++)
    {
      if(previous == NULL || previous != preds[level])
        preds[level]->unlock();
      previous = preds[level];
    }
    return true;
    }
}

template<typename K, typename V>
V* LazySkipList<K, V>::get_value(K key, int thread_num_){
  Node<K, V> **preds = pred_and_succ[thread_num_].preds;
  Node<K, V> **succs = pred_and_succ[thread_num_].succs; 
  int lFound = find(key, preds, succs);
  if( lFound != -1){
    Node<K, V> *target = succs[lFound];
    if(target != NULL)
    {
      target->lock();
      if((lFound != -1 && (target->fully_linked_ && target->top_level_ == lFound && !target->marked_))){
        target->unlock();
        return &(target->value_);
      }
      target->unlock();
      return NULL;
    }
    return NULL;
  }
  return NULL;
}

template<typename K, typename V>
Node<K, V>* LazySkipList<K, V>::get_entry(K key, int thread_num_){
   Node<K, V> **preds = pred_and_succ[thread_num_].preds;
   Node<K, V> **succs = pred_and_succ[thread_num_].succs;
   int lFound = find(key, preds, succs);
   if( lFound != -1){
     Node<K, V> *target = succs[lFound];
     if(target != NULL)
     {
       target->lock();
       if((lFound != -1 && (target->fully_linked_ && target->top_level_ == lFound && !target->marked_))){
          target->unlock();
          return target;
       }
       target->unlock();
       return NULL;
       }
    return NULL;
  }
  return NULL;
}

template<typename K, typename V>
int LazySkipList<K, V>::remove(K key, int thread_num_)
{
  Node<K, V> * victim;
  bool is_marked = false;
  int top_level = -1;
  Node<K, V> **preds = pred_and_succ[thread_num_].preds;
    Node<K, V> **succs = pred_and_succ[thread_num_].succs;

  while(true)
  {
    int lFound = find(key, preds, succs);
    if(lFound != -1)
    {
      victim = succs[lFound];
    }
    if (is_marked | (lFound != -1 && (victim->fully_linked_ && victim->top_level_ == lFound && !victim->marked_))) {
      if (!is_marked) {
        top_level = victim->top_level_;
        victim->lock();
        if (victim->marked_) {
          victim->unlock();
          return false;
        }
        victim->marked_ = true;
        is_marked = true;
      }
      int highestLocked = -1;
      Node<K, V>* pred;//, *succ;
      bool valid = true;
      Node<K, V> *previous = NULL;
      for (int level = 0; valid && (level <= top_level); level++) {
        pred = preds[level];
        if(previous != preds[level])
          pred->lock();
        previous = preds[level];
        highestLocked = level;
        valid = !pred->marked_ && pred->nexts_[level]==victim;
      }
      if (!valid) {
        Node<K, V> *previous = NULL;
        for (int level = 0; level <= highestLocked; level++)
        {
          if(previous == NULL || previous != preds[level])
            preds[level]->unlock();
          previous = preds[level];
        }
        continue;
      }
      for (int level = top_level; level >= 0; level--) {
        preds[level]->nexts_[level] = victim->nexts_[level];
      }
      victim->unlock();
      previous = NULL;
      for (int level = 0; level <= highestLocked; level++)
      {
        if(previous == NULL || previous != preds[level])
          preds[level]->unlock();
        previous = preds[level];
      }
      return true;
    }
    else return false;
  }
}

// okido Apr 28
template<typename K, typename V>
void LazySkipList<K, V>::print_list() {
  int key;
  int height;
  cout << "Height" << "\tVisual Pads" << endl;
  for( Node<K, V> *node = (head_->nexts_)[0]; node != tail_ || (node->nexts_)[0] != NULL; node = (node->nexts_)[0]) {
    key = (int)(node->key_);
    height = node->top_level_;
    cout << height << "\t| ";
    for( int star = height+1; star > 0; star--)
      cout << "*";

    cout << endl;
  }
}

// sentinel node constructor
template<typename K, typename V>
Node<K, V>::Node(K key) {
  key_ = key;
  value_ = NULL;
  nexts_ = new Node *[MAX_LEVEL + 1];
  top_level_ = MAX_LEVEL;
  marked_ = false;
  fully_linked_ = false;
}

template<typename K, typename V>
Node<K, V>::Node(K key, V value, int height) {
  value_ = value;
  key_ = key;
  nexts_ = new Node *[height + 1];
  top_level_ = height;
  marked_ = false;
  fully_linked_ = false;
}

template<typename K, typename V>
Node<K, V>::~Node(){
  delete [] nexts_;
}

template<typename K, typename V>
void Node<K, V>::lock() {
  mutex_.lock();
}

template<typename K, typename V>
void Node<K, V>::unlock(){
  mutex_.unlock();
}

template<typename K, typename V>
void Node<K, V>::spin_lock() {
  spin_.lock();
}

template<typename K, typename V>
void Node<K, V>::spin_unlock(){
  spin_.unlock();
}


}

