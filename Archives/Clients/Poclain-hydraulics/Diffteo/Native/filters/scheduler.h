#include <iostream>
#include <vector>

using namespace std;

template <class T> class scheduler
{
	int queueLength = 0;
	vector<T*> queue;
	vector<int> computeIntensity;

public:
	int getQueueLength();
	void enqueue(T* taskToDo);
	void run();
};

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
}