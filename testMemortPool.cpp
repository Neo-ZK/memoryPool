#include "memoryPool.h"
#include <iostream>
#include <time.h>
#include <Windows.h>

using namespace std;

const int num = 10000000;

template<class T>
class A
{
public:
	A();
	A(T i):a(i){};
public:
	T a ;
};

int main(int argc, char** argv)
{
	CMemoryPool<A<char>>* alloc = new CMemoryPool<A<char>>();
	A<char>* a;
	double start,stop;
	start = GetTickCount();
	try
	{
		for (int i = 0;i < num;i++)
		{
			a = alloc->newElement('a');
		}
	}
	catch(std::bad_alloc& e)
	{
		cout << "out of space" << endl;
		return -1;
	}
	stop = GetTickCount();
	cout << (stop -start)*1.0/1000 << endl;
	start = GetTickCount();
	try
	{
		for (int i = 0;i < num;i++)
		{
			new int();
		}
	}
	catch(std::bad_alloc& e)
	{
		cout << "out of space" << endl;
		return -1;
	}
	stop = GetTickCount();
	cout << (stop -start)*1.0/1000<< endl;	
	return 0;
}
