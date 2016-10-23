#include "StdAfx.h"
#include "MemoryPool.h"
#include <cassert>

MemoryPool::MemoryPool(void)
{
}


MemoryPool::~MemoryPool(void)
{
}


bool MemoryPool::InitMemoryPool(void)
{
	bool bSuccess = m_memoryPoolUnit64.InitMemoryPoolUnit();
	bSuccess &= m_memoryPoolUnit128.InitMemoryPoolUnit();
	bSuccess &= m_memoryPoolUnit256.InitMemoryPoolUnit();
	bSuccess &= m_memoryPoolUnit512.InitMemoryPoolUnit();
	bSuccess &= m_memoryPoolUnit1k.InitMemoryPoolUnit();
	bSuccess &= m_memoryPoolUnit4k.InitMemoryPoolUnit();
	bSuccess &= m_memoryPoolUnit16k.InitMemoryPoolUnit();
	bSuccess &= m_memoryPoolUnit64k.InitMemoryPoolUnit();
	bSuccess &= m_memoryPoolUnit256k.InitMemoryPoolUnit();
	bSuccess &= m_memoryPoolUnit512k.InitMemoryPoolUnit();
	bSuccess &= m_memoryPoolUnit1m.InitMemoryPoolUnit();

	if (bSuccess)
		return true;
	else
	{
		FreeMemoryPool();
		return false;
	}
}

void MemoryPool::FreeMemoryPool(void)
{
	m_memoryPoolUnit64.FreeMemoryPoolUnit();
	m_memoryPoolUnit128.FreeMemoryPoolUnit();
	m_memoryPoolUnit256.FreeMemoryPoolUnit();
	m_memoryPoolUnit512.FreeMemoryPoolUnit();
	m_memoryPoolUnit1k.FreeMemoryPoolUnit();
	m_memoryPoolUnit4k.FreeMemoryPoolUnit();
	m_memoryPoolUnit16k.FreeMemoryPoolUnit();
	m_memoryPoolUnit64k.FreeMemoryPoolUnit();
	m_memoryPoolUnit256k.FreeMemoryPoolUnit();
	m_memoryPoolUnit512k.FreeMemoryPoolUnit();
	m_memoryPoolUnit1m.FreeMemoryPoolUnit();
}


void * MemoryPool::AllocateMemory(size_t allocSize)
{
	if (allocSize > BLOCK_MAX_BYTES)
		return AllocateOtherMemory(allocSize);

	size_t blockSize = GetBlockSize(allocSize);
	switch (blockSize)
	{
	case BLOCK_64_BYTES:
		return m_memoryPoolUnit64.AllocateBlock();
	case BLOCK_128_BYTES:
		return m_memoryPoolUnit128.AllocateBlock();
	case BLOCK_256_BYTES:
		return m_memoryPoolUnit256.AllocateBlock();
	case BLOCK_512_BYTES:
		return m_memoryPoolUnit512.AllocateBlock();
	case BLOCK_1K_BYTES:
		return m_memoryPoolUnit1k.AllocateBlock();
	case BLOCK_4K_BYTES:
		return m_memoryPoolUnit4k.AllocateBlock();
	case BLOCK_16K_BYTES:
		return m_memoryPoolUnit16k.AllocateBlock();
	case BLOCK_64K_BYTES:
		return m_memoryPoolUnit64k.AllocateBlock();
	case BLOCK_256K_BYTES:
		return m_memoryPoolUnit256k.AllocateBlock();
	case BLOCK_512K_BYTES:
		return m_memoryPoolUnit512k.AllocateBlock();
	case BLOCK_1M_BYTES:
		return m_memoryPoolUnit1m.AllocateBlock();
	default:
		break;
	}
	return NULL;
}

void MemoryPool::FreeMemory(void *pointer)
{
	if (pointer == NULL)
		return;

	char *pTemp = (char *)pointer - (sizeof(BLOCK) - sizeof(char)) / sizeof(char);
	PBLOCK pBlock = reinterpret_cast<PBLOCK> (pTemp);
	switch (pBlock->size)
	{
	case BLOCK_64_BYTES:
		return m_memoryPoolUnit64.FreeBlock(pBlock);
	case BLOCK_128_BYTES:
		return m_memoryPoolUnit128.FreeBlock(pBlock);
	case BLOCK_256_BYTES:
		return m_memoryPoolUnit256.FreeBlock(pBlock);
	case BLOCK_512_BYTES:
		return m_memoryPoolUnit512.FreeBlock(pBlock);
	case BLOCK_1K_BYTES:
		return m_memoryPoolUnit1k.FreeBlock(pBlock);
	case BLOCK_4K_BYTES:
		return m_memoryPoolUnit4k.FreeBlock(pBlock);
	case BLOCK_16K_BYTES:
		return m_memoryPoolUnit16k.FreeBlock(pBlock);
	case BLOCK_64K_BYTES:
		return m_memoryPoolUnit64k.FreeBlock(pBlock);
	case BLOCK_256K_BYTES:
		return m_memoryPoolUnit256k.FreeBlock(pBlock);
	case BLOCK_512K_BYTES:
		return m_memoryPoolUnit512k.FreeBlock(pBlock);
	case BLOCK_1M_BYTES:
		return m_memoryPoolUnit1m.FreeBlock(pBlock);
	default:
		return FreeOtherMemory(pBlock);
	}
}


// 获得Block的大小
size_t MemoryPool::GetBlockSize(size_t allocSize)
{
	assert(allocSize <= BLOCK_MAX_BYTES);

	size_t blockSize(BLOCK_64_BYTES);
	const size_t ARY_BLOCK[] =
	{
		BLOCK_64_BYTES,
		BLOCK_128_BYTES,
		BLOCK_256_BYTES,
		BLOCK_512_BYTES,
		BLOCK_1K_BYTES,
		BLOCK_4K_BYTES,
		BLOCK_16K_BYTES,
		BLOCK_64K_BYTES,
		BLOCK_256K_BYTES,
		BLOCK_512K_BYTES,
		BLOCK_1M_BYTES,
	};
	for (long i = 0; i < ARRAYSIZE(ARY_BLOCK); ++i)
	{
		if (allocSize <= ARY_BLOCK[i])
		{
			blockSize = ARY_BLOCK[i];
			break;
		}
	}
	return blockSize;
}


void * MemoryPool::AllocateOtherMemory(size_t allocSize)
{
	PBLOCK pBlock = (PBLOCK)malloc(sizeof(BLOCK) - sizeof(char) + allocSize);
	if (pBlock != NULL)
	{
		pBlock->size = allocSize;
		pBlock->pNextFreeBlock = NULL;
	}
	return pBlock == NULL ? NULL : pBlock->address;
}


void MemoryPool::FreeOtherMemory(void *pointer)
{
	free(pointer);
}
