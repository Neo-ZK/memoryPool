#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H
#include "memoryPool.cpp"
#include <new>
#include <cstddef>
#include <stdlib.h>
#include <stdexcept>
#include <iostream>

template<typename T>


class CMemoryPool
{
public:
	typedef T        value_type;
	typedef T*       pointer;
	typedef T&       reference;
	typedef const T* const_pointer;
	typedef const T& const_reference;
	typedef size_t   size_type;

	
	CMemoryPool();
	~CMemoryPool();
	pointer newElement();
	template<class U> pointer newElement(const U& args);
	void deleteElement(pointer p);
	
	pointer allocate();
	void deallocate(pointer p);

	void* fillList(size_t n);

	char* chunkAlloc(size_t n, int& nodeNum);


private:
	union obj
	{
		obj* next;
		value_type clientData();
	};
	typedef union obj  list_type;
	typedef union obj* list_pointer;

	list_pointer _freeList;

private:
	char* _startPool;
	char* _endPool;
};


template<typename T>
CMemoryPool<T>::CMemoryPool():_freeList(0),_startPool(0),_endPool(0)
{

}
/***********************************************************************
fun:allocate address ,then placement new the address(without args)
entrance:nothing
return:the pointer to Assigned address
***********************************************************************/
template<typename T>
inline typename CMemoryPool<T>::pointer
	CMemoryPool<T>::newElement()
{
	pointer result = allocate();
	new(result) T();//placement new
	return result;
}

/***********************************************************************
fun:allocate address ,then placement new the address(with args)
entrance:the args of class T
return:the pointer to Assigned address
***********************************************************************/
template<typename T>
template<class U>
inline typename CMemoryPool<T>::pointer
	CMemoryPool<T>::newElement(const U& args)
{
	pointer result = allocate();
	new(result) T(args);;
	return result;
}
/***********************************************************************
fun:delete the element of memory pool,deallocate the address
entrance:the pointer of class T
return:nothing
***********************************************************************/
template<typename T>
inline void
	CMemoryPool<T>::deleteElement(pointer p)
{
	if(p != 0)
	{
		p->~T();//call destructor
		deallocate(p);
	}
}

/***********************************************************************
fun:allocate address ,first look for address in _freelist,if there is
    no free address in _freelist,then allocate address to _freelist
entrance:nothing
return:the pointer to Assigned address
***********************************************************************/
template<typename T>
typename CMemoryPool<T>::pointer 
	CMemoryPool<T>::allocate()
{
	list_pointer result = _freeList;

	
	if(0 == result)
	{
		_freeList = reinterpret_cast<list_pointer>(fillList(sizeof(obj)));
		result = _freeList;
	}

	_freeList = result->next;
	return reinterpret_cast<pointer>(result);
}

/***********************************************************************
fun:recycle the address ,insert the address into _freelist,set the 
	address as the begin of _freelist
entrance:the pointer of class T
return:nothing
***********************************************************************/
template<typename T>
inline void 
	CMemoryPool<T>::deallocate(pointer p)
{
	if(p != 0)
	{
		reinterpret_cast<list_pointer>(p)->next = _freeList;
		_freeList = reinterpret_cast<list_pointer>(p);
	}
}



/***********************************************************************
fun:when there is no available address in _freelist,call this fun to get
    address chunck from memorypool
entrance:the sizeof obj
return:uninitialized address
***********************************************************************/
template<typename T>
void* CMemoryPool<T>::fillList(size_t n)
{
	
	int nodeNum = 10;

	char* chunk = chunkAlloc(n, nodeNum);

	list_pointer result;
	list_pointer curPointer, nextPointer;

	result = reinterpret_cast<list_pointer>(chunk);

	if(1 == nodeNum)
	{
		(reinterpret_cast<list_pointer>(chunk))->next = 0;
		return result;
	}		
	nextPointer = reinterpret_cast<list_pointer>(chunk + n);
	result->next = nextPointer;
	for(int i = 1; ; ++i)
	{
		curPointer = nextPointer;
		nextPointer = reinterpret_cast<list_pointer>(reinterpret_cast<char*>(nextPointer) + n);
		if(nodeNum - 1 == i)
		{
			curPointer->next = 0;
			break;
		}
		else
		{
			curPointer->next = nextPointer;
		}
	}

	return result;
}

/***********************************************************************
fun:allocate address chunk 
entrance:n:the sizeof obj, nodeNum:the num of nodes of _freelist
return:the begin of available address 
***********************************************************************/
template<typename T>
char* CMemoryPool<T>::chunkAlloc(size_t n, int & nodeNum)
{
	char* result;
	size_t totalBytes = n * nodeNum;
	size_t bytesRemain = _endPool - _startPool;

	if(bytesRemain >= totalBytes)
	{
		result = _startPool;
		_startPool += totalBytes;
		return result;
	}/*  
	 //Because there is no memory pool allocation in the first layer, this piece doesn't work
	else if(bytesRemain >= n)//memory pool cannot contains all, but can alloc some
	{
		nodeNum = bytesRemain / n;
		totalBytes = n * nodeNum;
		result = _startPool;
		_startPool += totalBytes;
		return result;
	}*/
	else  //one also connot alloc
	{
		//submit 2 times of totalBytes memory
		size_t bytesToGet = totalBytes * 2;
		_startPool = reinterpret_cast<char*>(malloc(bytesToGet));

		if(0 == _startPool) //if failure, throw exception
		{
			throw std::bad_alloc();
		}
		else
		{
			_endPool = _startPool + bytesToGet;
			return chunkAlloc(n, nodeNum);  //recall the function to fix nodeNum
		}
	}
}

#endif
