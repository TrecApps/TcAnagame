#pragma once
#include "TrecLib.h"
#include "TrecPointer.h"
#include <vector>



/*
* Class: TDataArray (Regular Type version)
* Purpose: AnaGame array designed to hold raw data
*
* SuperClass: TDataArrayBase - common base class for all TDataArrays
*/
template<typename T> class _TREC_LIB_DLL TDataArray 
{
	friend class TDataArray<T>;

	T* array;
	UINT size;
	UINT capacity;

public:

	UINT Size() const
	{
		return size;
	}

	UINT Capacity() const
	{
		return capacity;
	}

	/*
	* Method: TDataArray::TDataArray
	* Purpose: Copy Constructor
	* Parameters: TDataArray<T>& newArray - the Data Array to copy
	* Returns: void
	*/
	TDataArray(const TDataArray<T>& newArray) : TDataArrayBase()
	{
		UINT cap = newArray.Capacity()
			array = new T[cap];

		for (UINT c = 0; c < cap; c++)
		{
			if (c < cap)
				array[c] = newArray.array[c];
		}
		size = newArray.Size(), capacity = newArray.Capacity();
	}

	/*
	* Method: TDataArray::data
	* Purpose: Retrieves the location of the underlying array
	* Parameters: void
	* Returns: T* - data address in memory
	*
	* Attributes: const
	*/
	T* data() const
	{
		return array;
	}

	/*
	* Method: TDataArray::operator[]
	* Purpose: Gives the TDataArray array like functionality
	* Parameters: size_t c - index to target
	* Returns: T& - element at index
	*/
	T& operator[](size_t c)const
	{
		if (c >= size)
			throw 2;
		return array[c];
	}

	/*
	* Method: TDataArray::operator=
	* Purpose: Copies the contents of an exisitng data array to the TDataArray
	* Parameters: TDataArray<T>& newArray - the Data Array to copy
	* Returns: void
	*/
	TDataArray<T>& operator=(const TDataArray<T>& newArray)
	{
		if (array)
			delete[] array;
		capacity = newArray.Capacity();
		array = new T[capacity];
		size = newArray.Size();

		for (UINT c = 0; c < size && c < capacity; c++)
		{
			if (c < capacity)
				array[c] = newArray.array[c];
		}
	}

	/*
	* Method: TDataArray::at
	* Purpose: returns element at a given index
	* Parameters: UINT c - the index to target
	* Returns: T& - element at index
	*/
	T& at(UINT c)
	{
		if (c >= size)
			throw 2;
		return array[c];
	}

	/*
	* Method: TDataArray::operator=
	* Purpose: Copies the contents of a C++ standard vector to the TDataArray
	* Parameters: std::vector<T, std::allocator<T>>& vectorSource - the vector to copy
	* Returns: void
	*/
	void operator=(const std::vector<T, std::allocator<T>>& vectorSource)
	{
		if (array)
			delete[] array;
		capacity = vectorSource.capacity();
		array = new T[capacity];
		size = vectorSource.size();
		for (UINT c = 0; c < vectorSource.capacity(); c++)
			array[c] = vectorSource[c];
	}

	/*
	* Method: (TDataArray) (Constructor)
	* Purpose: Sets up a TDataArray
	* Parameters: void
	* Returns: void
	*/
	TDataArray()
	{
		array = new T[5];
		size = 0; 
		capacity = 5;
	}



	/*
	* Method: (TDataArray) (Destructor)
	* Purpose: Cleans up the Data Array
	* Parameters: void
	* Returns: void
	*/
	virtual ~TDataArray()
	{
		delete[] array;
	}



	/*
	* Method: TDataArray::Capacity
	* Purpose: Reports the size of the underlying array
	* Parameters: void
	* Returns: UINT - the size of the unerlying array being used
	*
	* Attributes: const
	*/
	UINT Capacity() const
	{
		return capacity;
	}

	/*
	* Method: TDataArray::push_back
	* Purpose: Appends an element to the DataArray
	* Parameters: T element - the data to append
	* Returns: UINT - the index of the new element (1 less than the size)
	*/
	UINT push_back(T element)
	{
		if (size >= capacity)
		{
			T* newArray = new T[capacity + 5];
			for (UINT c = 0; c < capacity; c++)
				newArray[c] = array[c];
			
			delete[] array;
			array = newArray;
			capacity += 5;
		}
		array[size++] = element;
		return size - 1;
	}

	/**
	 * Method: TDataArray::InsertAt
	 * Purpose: Adds element at the specified location
	 * Parameters: T element - the data to insert
	 *				UINT index - the index of th array to insert at
	 * Returns: UINT - the new size of the array
	 */
	UINT InsertAt(T element, UINT index)
	{
		if (index >= size)
			return push_back(element);
		UINT ret = push_back(element);

		for (UINT Rust = size - 1; Rust > index; Rust--)
		{
			array[Rust] = array[Rust - 1];
		}
		array[index] = element;

		return ret;
	}

	/*
	* Method: TDataArray::RemoveAt
	* Purpose: Removes an element at a certain location
	* Parameters: UINT c - the location to remove
	* Returns: T - the data stored at that location
	*/
	T RemoveAt(UINT c)
	{
		if (c >= size)
			throw 2;
		T returnable = array[c];

		for (; c < size - 1; c++)
			array[c] = array[c + 1];
		size--;
		// Set to the Default
		array[size] = T();
		return returnable;
	}

	/*
	* Method: TDataArray::RemoveAll
	* Purpose: Removes all emements and sets count to 0
	* Parameters: void
	* Returns: void
	*/
	void RemoveAll()
	{
		if (array)
			delete[] array;
		array = new T[capacity = 5];
		size = 0;
	}

};
