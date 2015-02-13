#include "memory/NDBufferPool.h"

#include <cassert>

#include "NDCLibFun.h"
#include "NDShareBaseGlobal.h"




_NDSHAREBASE_BEGIN

NDBufferPool::NDBufferPool()
{
	totalMallocs         = 0;

	mallocsFromTinyPool  = 0;
	mallocsFromSmallPool = 0;
	mallocsFromMedPool   = 0;

	bytesAllocated       = 0;

	//tinyPoolSize         = 0;
	//tinyHeap             = NULL;

	smallPoolSize        = 0;

	medPoolSize          = 0;


	// Initialize the tiny heap as a bunch of pointers into one
	// pre-allocated buffer;
	tinyHeap = ::malloc(maxTinyBuffers * tinyBufferSize);
	for (int i = 0; i < maxTinyBuffers; ++i) {
		tinyPool[i] = (NDUint8*)tinyHeap + (tinyBufferSize * i);
	}
	tinyPoolSize = maxTinyBuffers;
}

NDBufferPool::~NDBufferPool()
{
	::free(tinyHeap);
	flushPool(smallPool, smallPoolSize);
	flushPool(medPool, medPoolSize);
}

void* NDBufferPool::realloc( void* ptr, size_t bytes )
{
	if (ptr == NULL) {
		return malloc(bytes);
	}

	if (inTinyHeap(ptr)) {
		if (bytes <= tinyBufferSize) {
			// The old pointer actually had enough space.
			return ptr;
		} else {
			// Free the old pointer and malloc

			UserPtr newPtr = malloc(bytes);
			memcpy(newPtr, ptr, tinyBufferSize);
			tinyFree(ptr);
			return newPtr;

		}
	} else {
		// In one of our heaps.
		// See how big the block really was;
		size_t userSize = USERSIZE_FROM_USERPTR(ptr);
		if (bytes <= userSize) {
			// The old block was big enough.
			return ptr;
		}

		// Need to reallocate and move;
		UserPtr newPtr = malloc(bytes);
		memcpy(newPtr, ptr, userSize);
		free(ptr);
		return newPtr;
	}	
}

void* NDBufferPool::malloc( size_t bytes )
{
	do 
	{
		lock();
		++totalMallocs;

		if (bytes <= tinyBufferSize) {

			UserPtr ptr = tinyMalloc(bytes);

			if (ptr) {
				++mallocsFromTinyPool;
				unlock();
				return ptr;
			}

		} 

		// Failure to allocate a tiny buffer is allowed to flow
		// through to a small buffer;
		if (bytes <= smallBufferSize) {

			UserPtr ptr = malloc(smallPool, smallPoolSize, bytes);

			if (ptr) {
				++mallocsFromSmallPool;
				unlock();
				return ptr;
			}

		} else if (bytes <= medBufferSize) {
			// Note that a small allocation failure does *not* fall
			// through into a medium allocation because that would
			// waste the medium buffer's resources.

			UserPtr ptr = malloc(medPool, medPoolSize, bytes);

			if (ptr) {
				++mallocsFromMedPool;
				unlock();
				//debugAssertM(ptr != NULL, "BufferPool::malloc returned NULL");
				return ptr;
			}
		}

		bytesAllocated += (int)USERSIZE_TO_REALSIZE(bytes);
		unlock();
	} while (0);
	

	// Heap allocate

	// Allocate 4 extra bytes for our size header (unfortunate,
	// since malloc already added its own header).
	RealPtr ptr = ::malloc(USERSIZE_TO_REALSIZE(bytes));

	if (ptr == NULL) {
		// Flush memory pools to try and recover space
		flushPool(smallPool, smallPoolSize);
		flushPool(medPool, medPoolSize);
		ptr = ::malloc(USERSIZE_TO_REALSIZE(bytes));
	}

//	if (ptr == NULL) {
//		if ((System::outOfMemoryCallback() != NULL) &&
//			(System::outOfMemoryCallback()(USERSIZE_TO_REALSIZE(bytes), true) == true)) {
//				// Re-attempt the malloc
//				ptr = ::malloc(USERSIZE_TO_REALSIZE(bytes));
//		}
//	}
//
//	if (ptr == NULL) {
//		if (System::outOfMemoryCallback() != NULL) {
//			// Notify the application
//			System::outOfMemoryCallback()(USERSIZE_TO_REALSIZE(bytes), false);
//		}
//#           ifdef G3D_DEBUG
//		debugPrintf("::malloc(%d) returned NULL\n", (int)USERSIZE_TO_REALSIZE(bytes));
//#           endif
//		debugAssertM(ptr != NULL, 
//			"::malloc returned NULL. Either the "
//			"operating system is out of memory or the "
//			"heap is corrupt.");
//		return NULL;
//	}

	*(NDUint32*)ptr = (NDUint32)bytes;

	return REALPTR_TO_USERPTR(ptr);	
}

void NDBufferPool::free( void* ptr )
{
	if (ptr == NULL) {
		// Free does nothing on null pointers
		return;
	}

	assert(NDShareBaseGlobal::isValidPointer(ptr));

	if (inTinyHeap(ptr)) {
		lock();
		tinyFree(ptr);
		unlock();
		return;
	}

	NDUint32 bytes = USERSIZE_FROM_USERPTR(ptr);

	do 
	{
		lock();
		if (bytes <= smallBufferSize) {
			if (smallPoolSize < maxSmallBuffers) {
				smallPool[smallPoolSize] = MemBlock(ptr, bytes);
				++smallPoolSize;
				unlock();
				return;
			}
		} else if (bytes <= medBufferSize) {
			if (medPoolSize < maxMedBuffers) {
				medPool[medPoolSize] = MemBlock(ptr, bytes);
				++medPoolSize;
				unlock();
				return;
			}
		}
		bytesAllocated -= USERSIZE_TO_REALSIZE(bytes);
		unlock();
	} while (0);

	// Free; the buffer pools are full or this is too big to store.;
	::free(USERPTR_TO_REALPTR(ptr));	
}


std::string NDBufferPool::performance() const
{
	if (totalMallocs > 0) {
		int pooled	= mallocsFromTinyPool + mallocsFromSmallPool + mallocsFromMedPool;
		int total	= totalMallocs;

		//ostringstream	m_strPerfmStream; //声明在头文件中;
		//m_strPerfmStream.str("");
		//m_strPerfmStream << "malloc performance: ";
		//m_strPerfmStream << (100.0f * mallocsFromTinyPool  / total)		<< "%% <= " << NDBufferPool::tinyBufferSize		<< "b, ";
		//m_strPerfmStream << (100.0f * mallocsFromSmallPool / total)		<< "%% <= " << NDBufferPool::smallBufferSize	<< "b, ";
		//m_strPerfmStream << (100.0f * mallocsFromMedPool / total)		<< "%% <= " << NDBufferPool::medBufferSize		<< "b, ";
		//m_strPerfmStream << (100.0f * (1.0 - (double)pooled / total))	<< "%% <= " << NDBufferPool::medBufferSize		<< "b ";
		//return m_strPerfmStream.str();

		char szBuf[128] = {0};
		ND_SNPRINTF( szBuf, 127, "malloc performance: %5.1f%% <= %db, %5.1f%% <= %db, "
							   "%5.1f%% <= %db, %5.1f%% > %db",
							   100.0f * mallocsFromTinyPool  / total,
							   NDBufferPool::tinyBufferSize,
							   100.0f * mallocsFromSmallPool / total,
							   NDBufferPool::smallBufferSize,
							   100.0f * mallocsFromMedPool   / total,
							   NDBufferPool::medBufferSize,
							   100.0f * (1.0 - (double)pooled / total),
							   NDBufferPool::medBufferSize);
	
		return string(szBuf);
	} else {
		return "No System::malloc calls made yet.";
	}
}

std::string NDBufferPool::status() const
{
	char szBuf[128] = {0};
	ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, "preallocated shared buffers: %5d/%d x %db", (maxTinyBuffers - tinyPoolSize), maxTinyBuffers, tinyBufferSize );
	return string(szBuf);
}


NDBufferPool::UserPtr NDBufferPool::tinyMalloc( size_t bytes )
{
	// Note that we ignore the actual byte size
	// and create a constant size block.
	(void)bytes;
	assert(tinyBufferSize >= bytes);

	UserPtr ptr = NULL;

	if (tinyPoolSize > 0) {
		--tinyPoolSize;

		// Return the old last pointer from the freelist;
		ptr = tinyPool[tinyPoolSize];

#ifdef _DEBUG
		if (tinyPoolSize > 0) {
			assert(tinyPool[tinyPoolSize - 1] != ptr); 
			//   "System::malloc heap corruption detected: "
			//   "the last two pointers on the freelist are identical (during tinyMalloc).");
		}
#endif

		// NULL out the entry to help detect corruption
		tinyPool[tinyPoolSize] = NULL;
	}

	return ptr;
}

bool NDBufferPool::inTinyHeap( UserPtr ptr )
{
	return (ptr >= tinyHeap) && (ptr < (NDUint8*)tinyHeap + maxTinyBuffers * tinyBufferSize);
}

void NDBufferPool::tinyFree( UserPtr ptr )
{
	assert(ptr);
	assert(tinyPoolSize < maxTinyBuffers);
	//           "Tried to free a tiny pool buffer when the tiny pool freelist is full.");

#ifdef _DEBUG
	if (tinyPoolSize > 0) {
		UserPtr prevOnHeap = tinyPool[tinyPoolSize - 1];
		assert(prevOnHeap != ptr); 
		//                    "System::malloc heap corruption detected: "
		//                    "the last two pointers on the freelist are identical (during tinyFree).");
	}
#endif

	assert(tinyPool[tinyPoolSize] == NULL);

	// Put the pointer back into the free list
	tinyPool[tinyPoolSize] = ptr;
	++tinyPoolSize;
}

void NDBufferPool::flushPool( MemBlock* pool, int& poolSize )
{
	for (int i = 0; i < poolSize; ++i) {
		bytesAllocated -= (int)USERSIZE_TO_REALSIZE(pool[i].bytes);
		::free(USERPTR_TO_REALPTR(pool[i].ptr));
		pool[i].ptr = NULL;
		pool[i].bytes = 0;
	}
	poolSize = 0;
}


NDBufferPool::UserPtr NDBufferPool::malloc( MemBlock* pool, int& poolSize, size_t bytes )
{
	// OPT: find the smallest block that satisfies the request.

	// See if there's something we can use in the buffer pool.
	// Search backwards since usually we'll re-use the last one.
	for (int i = (int)poolSize - 1; i >= 0; --i) {
		if (pool[i].bytes >= bytes) {
			// We found a suitable entry in the pool.

			// No need to offset the pointer; it is already offset
			UserPtr ptr = pool[i].ptr;

			// Remove this element from the pool, replacing it with
			// the one from the end (same as Array::fastRemove)
			--poolSize;
			pool[i] = pool[poolSize];

			return ptr;
		}
	}

	return NULL;
}




_NDSHAREBASE_END

