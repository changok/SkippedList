all:
	g++ -g -lpthread locks.cpp skiplist.cpp tester.h thread.cpp test.cpp -o test_skip_list
 
