#include <iostream>
#include <string>
#include <map>
#include <iostream>
#include "tester.h"

using namespace std;

void TEST_CORRECT_NESS() {
  cout<<"Sequential"<<endl;
  Tester *mt = new Tester();
  mt->sequential_add_test(1000);
  int size_ = mt->skipList->size();
  mt->sequential_erase_test(size_, 100);
  delete mt;
  cout<<endl;

  for(int i=2; i<64;i=i*2){
    Tester *mt = new Tester();
    mt->concurrent_add_test(i, 1000); 
    int size_ = mt->skipList->size();
    mt->concurrent_erase_test(size_,i,100);
    delete mt;
    cout<<endl;
  }
      
  for(int i=2; i<128;i=i*2){
    Tester *mt = new Tester();
    mt->concurrent_add_remove_test(i,i*2,10);
    delete mt;
      cout<<endl;
  }

  for(int i=1; i<128;i=i*2){
    Tester *mt = new Tester();
    mt->concurrent_add_remove_getvalue_test(i,i,i,10);
    delete mt;
    cout<<endl;
  }

  for(int i=1; i<128; i=i*2){
    Tester *mt = new Tester();
    mt->concurrent_add_remove_getentry_test(i,i,i,10);
    delete mt;
    cout<<endl;
  }
}

// the drifting number of threads test with range 200,000 and 100,000 operations
void SKIP_LIST_TEST( int num_repeats, int work_load, int range, double ratio_get, double ratio_add, double ratio_remove, int debug) {
  cout << "SKIP LIST PERFORMANCE TEST: " << endl;
  cout << "\nget " << ratio_get << ", add " << ratio_add << ", remove " << ratio_remove << endl;
  for( int i = 0; i < num_repeats; i++){
    Tester *mt = new Tester();
    // arguments: 
    // 1.the number of threads, 2.work load(operation), 3.range, 
    // 4.the ratio of get(), 5. the ratio of add(), 6. the ratio of remove(), 7. Debug
    mt->performance_test( 10*(i+1), work_load, range, ratio_get, ratio_add, ratio_remove, debug);
    delete mt;
  }
  cout << endl;
}

// the drifting number of threads test with range 200,000 and 100,000 operations
void RB_TREE_TEST( int num_repeats, int work_load, int range, double ratio_get, double ratio_add, double ratio_remove, int debug) {
  cout << "RB TREE PERFORMANCE TEST: " << endl;
  cout << "\nget " << ratio_get << ", add " << ratio_add << ", remove " << ratio_remove << endl;
  for( int i = 0; i < num_repeats; i++){
    Tester *rb = new Tester();
    rb->rb_performance_test( 10*(i+1), work_load, range, ratio_get, ratio_add, ratio_remove, debug);
    delete rb;
  }
  cout << endl;
}

int main()
{
  int debug = 1;
  TEST_CORRECT_NESS();

  SKIP_LIST_TEST( 10, 100000, 200000, 0.9, 0.09, 0.01, debug);
  RB_TREE_TEST( 10, 100000, 200000, 0.9, 0.09, 0.01, debug);
}
