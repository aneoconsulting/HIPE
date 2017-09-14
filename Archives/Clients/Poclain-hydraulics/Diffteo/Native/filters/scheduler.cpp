#include <iostream>
#include <vector>
#include "scheduler.h"

using namespace std;

/* This is not compiled if left here, but if put in the header file, it links properly... */

/*
template<class T>
int scheduler<T>::getQueueLength() { return queueLength; }

template<class T>
void scheduler<T>::enqueue(T* taskToDo)
{
	queue.push_back(taskToDo);
	queueLength++;
}

template<class T>
void scheduler<T>::run()
{
	for (int i = 0; i < queueLength; i++)
		(*queue[i]).apply();
	cout << "Scheduler DONE." << endl;
}*/