#pragma once

template <class T, class N = int>
class CUtlMemory
{
public:
	T& operator[](N i)
	{
		return pMemory[i];
	}

	const T& operator[](N i) const
	{
		return pMemory[i];
	}

	T* Base()
	{
		return pMemory;
	}

	[[nodiscard]] int NumAllocated() const
	{
		return iAllocationCount;
	}

	[[nodiscard]] bool IsExternallyAllocated() const
	{
		return iGrowSize < 0;
	}

	void Grow(const int iCount = 1)
	{
		if (IsExternallyAllocated())
			return;

		int iAllocationRequested = iAllocationCount + iCount;
		int iNewAllocationCount = 0;

		if (iGrowSize)
			iAllocationCount = ((1 + ((iAllocationRequested - 1) / iGrowSize)) * iGrowSize);
		else
		{
			if (!iAllocationCount)
				iAllocationCount = (31 + sizeof(T)) / sizeof(T);

			while (iAllocationCount < iAllocationRequested)
				iAllocationCount <<= 1;
		}

		if (static_cast<int>(static_cast<N>(iNewAllocationCount)) < iAllocationRequested)
		{
			if (static_cast<int>(static_cast<N>(iNewAllocationCount)) == 0 && static_cast<int>(static_cast<N>(iNewAllocationCount - 1)) >= iAllocationRequested)
				--iNewAllocationCount;
			else
			{
				if (static_cast<int>(static_cast<N>(iAllocationRequested)) != iAllocationRequested)
					return;

				while (static_cast<int>(static_cast<N>(iNewAllocationCount)) < iAllocationRequested)
					iNewAllocationCount = (iNewAllocationCount + iAllocationRequested) / 2;
			}
		}

		iAllocationCount = iNewAllocationCount;

		if (pMemory != nullptr)
			pMemory = static_cast<T*>(realloc(pMemory, iAllocationCount * sizeof(T)));
		else
			pMemory = static_cast<T*>(malloc(iAllocationCount * sizeof(T)));
	}

protected:
	T* pMemory;
	int iAllocationCount;
	int iGrowSize;
};

template< typename T, size_t SIZE, int nAlignment = 0 >
class CUtlMemoryFixed
{
public:
	// constructor, destructor
	CUtlMemoryFixed( int nGrowSize = 0, int nInitSize = 0 ) { assert( nInitSize == 0 || nInitSize == SIZE ); }
	CUtlMemoryFixed( T* pMemory, int numElements ) { assert( 0 ); }

	// Can we use this index?
	bool IsIdxValid( int i ) const { return ( i >= 0 ) && ( i < SIZE ); }

	// Specify the invalid ('null') index that we'll only return on failure
	static const int INVALID_INDEX = -1; // For use with COMPILE_TIME_ASSERT
	static int InvalidIndex( ) { return INVALID_INDEX; }

	// Gets the base address
	T* Base( ) { if (nAlignment == 0) return ( T* )( &m_Memory[ 0 ] ); else return ( T* )AlignValue( &m_Memory[ 0 ], nAlignment ); }
	const T* Base( ) const { if (nAlignment == 0) return ( T* )( &m_Memory[ 0 ] ); else return ( T* )AlignValue( &m_Memory[ 0 ], nAlignment ); }

	// element access
	T& operator[]( int i ) { assert( IsIdxValid( i ) ); return Base( )[ i ]; }
	const T& operator[]( int i ) const { assert( IsIdxValid( i ) ); return Base( )[ i ]; }
	T& Element( int i ) { assert( IsIdxValid( i ) ); return Base( )[ i ]; }
	const T& Element( int i ) const { assert( IsIdxValid( i ) ); return Base( )[ i ]; }

	// Attaches the buffer to external memory....
	void SetExternalBuffer( T* pMemory, int numElements ) { assert( 0 ); }

	// Size
	int NumAllocated( ) const { return SIZE; }
	int Count( ) const { return SIZE; }

	// Grows the memory, so that at least allocated + num elements are allocated
	void Grow( int num = 1 ) { assert( 0 ); }

	// Makes sure we've got at least this much memory
	void EnsureCapacity( int num ) { assert( num <= SIZE ); }

	// Memory deallocation
	void Purge( ) {}

	// Purge all but the given number of elements (NOT IMPLEMENTED IN CUtlMemoryFixed)
	void Purge( int numElements ) { assert( 0 ); }

	// is the memory externally allocated?
	bool IsExternallyAllocated( ) const { return false; }

	// Set the size by which the memory grows
	void SetGrowSize( int size ) {}

	class Iterator_t
	{
	public:
		Iterator_t( int i ) : index( i ) {}
		int index;
		bool operator==( const Iterator_t it ) const { return index == it.index; }
		bool operator!=( const Iterator_t it ) const { return index != it.index; }
	};
	Iterator_t First( ) const { return Iterator_t( IsIdxValid( 0 ) ? 0 : InvalidIndex( ) ); }
	Iterator_t Next( const Iterator_t& it ) const { return Iterator_t( IsIdxValid( it.index + 1 ) ? it.index + 1 : InvalidIndex( ) ); }
	int GetIndex( const Iterator_t& it ) const { return it.index; }
	bool IsIdxAfter( int i, const Iterator_t& it ) const { return i > it.index; }
	bool IsValidIterator( const Iterator_t& it ) const { return IsIdxValid( it.index ); }
	Iterator_t InvalidIterator( ) const { return Iterator_t( InvalidIndex( ) ); }

private:
	char m_Memory[ SIZE * sizeof( T ) + nAlignment ];
};