#pragma once
#include "MemoryPoolUnitT.h"
#include "CriticalSectionLock.h"
#include "Singleton.h"

#define BLOCK_64_BYTES		64
#define BLOCK_128_BYTES		128
#define BLOCK_256_BYTES		256
#define BLOCK_512_BYTES		512
#define BLOCK_1K_BYTES		1024
#define BLOCK_4K_BYTES		1024*4
#define BLOCK_16K_BYTES		1024*16
#define BLOCK_64K_BYTES		1024*64
#define BLOCK_256K_BYTES	1024*256
#define BLOCK_512K_BYTES	1024*512
#define BLOCK_1M_BYTES		1024*1024

#define BLOCK_MAX_BYTES		BLOCK_1M_BYTES

/**
 ** 内存池
 ** designed by eric,2016/08/20
 **/
namespace eric { namespace common {
class MemoryPool
{
public:
	MemoryPool(void);
	virtual ~MemoryPool(void);

	/**
	 ** 初始化内存池
	 **/
	bool InitMemoryPool(void);

	/**
	 ** 释放内存池
	 **/
	void FreeMemoryPool(void);

	/**
	 ** 分配内存
	 **/
	void * AllocateMemory(size_t allocSize);

	/**
	 ** 释放内存
	 **/
	void FreeMemory(void *pointer);

protected:
	size_t GetBlockSize(size_t allocSize); // 获得Block的大小
	void * AllocateOtherMemory(size_t allocSize); //分配超出内存池最大设置内存块的内存
	void FreeOtherMemory(void *pointer); // 释放超出内存池最大设置内存块的内存

protected:
	MemoryPoolUnitT<CriticalSectionLock, BLOCK_64_BYTES> m_memoryPoolUnit64;
	MemoryPoolUnitT<CriticalSectionLock, BLOCK_128_BYTES> m_memoryPoolUnit128;
	MemoryPoolUnitT<CriticalSectionLock, BLOCK_256_BYTES> m_memoryPoolUnit256;
	MemoryPoolUnitT<CriticalSectionLock, BLOCK_512_BYTES> m_memoryPoolUnit512;
	MemoryPoolUnitT<CriticalSectionLock, BLOCK_1K_BYTES> m_memoryPoolUnit1k;
	MemoryPoolUnitT<CriticalSectionLock, BLOCK_4K_BYTES> m_memoryPoolUnit4k;
	MemoryPoolUnitT<CriticalSectionLock, BLOCK_16K_BYTES> m_memoryPoolUnit16k;
	MemoryPoolUnitT<CriticalSectionLock, BLOCK_64K_BYTES> m_memoryPoolUnit64k;
	MemoryPoolUnitT<CriticalSectionLock, BLOCK_256K_BYTES> m_memoryPoolUnit256k;
	MemoryPoolUnitT<CriticalSectionLock, BLOCK_512K_BYTES> m_memoryPoolUnit512k;
	MemoryPoolUnitT<CriticalSectionLock, BLOCK_1M_BYTES> m_memoryPoolUnit1m;
};

/**
 ** 内存池，单例模式
 ** designed by eric,2016/08/20
 **/
class MemoryPoolSingleton : public CSingleton<MemoryPool>
{
	friend class CSingleton<MemoryPool>;
private:
	MemoryPoolSingleton(void);
	virtual ~MemoryPoolSingleton(void);
};

}}
