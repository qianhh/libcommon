#pragma once

#define CHUNK_SIZE 1024*1024

#pragma pack(push, 1)
typedef struct tagChunk
{
	void				*pChunk;			//数据块的地址
	struct tagChunk		*pNextChunk;		//下一个数据块节点的地址
}CHUNK, *PCHUNK;

typedef struct tagBlock
{
	size_t				size;				// 这个Block的大小
	struct tagBlock		*pNextFreeBlock;	// 下一个空闲节点的地址
	char				address[1];			// 分配的数据地址
}BLOCK, *PBLOCK;
#pragma pack(pop)

/**
 ** 内存池单元管理
 ** designed by eric,2016/08/20
 **/
template<class Lock, size_t BLOCK_SIZE>
class MemoryPoolUnitT
{
public:
	MemoryPoolUnitT(void) : m_pMemoryFreeList(NULL), m_pMemoryChunkList(NULL)
	{
	}


	virtual ~MemoryPoolUnitT(void)
	{
	}

	// 初始化单元内存池
	bool InitMemoryPoolUnit(void)
	{
		if (CHUNK_SIZE < BLOCK_SIZE)
			return false;

		m_pMemoryChunkList = CreateChunkList();
		m_pMemoryFreeList = CreateBlockList();

		if (m_pMemoryChunkList == NULL || m_pMemoryFreeList == NULL)
		{
			FreeMemoryPoolUnit();
			return false;
		}

		return AllocateChunk();
	}

	// 释放单元内存池
	void FreeMemoryPoolUnit(void)
	{
		FreeChunkList(m_pMemoryChunkList);
		FreeBlockList(m_pMemoryFreeList);
	}

	// 分配内存块
	void * AllocateBlock(void)
	{
		m_lock.Lock();

		PBLOCK pBlock = PopFront(m_pMemoryFreeList);
		if (pBlock == NULL)
		{
			AllocateChunk();
			pBlock = PopFront(m_pMemoryFreeList);
		}

		m_lock.Unlock();
		return pBlock == NULL ? NULL : pBlock->address;
	}

	// 释放内存块
	void FreeBlock(PBLOCK pBlock)
	{
		m_lock.Lock();
		PushFront(m_pMemoryFreeList, pBlock);
		m_lock.Unlock();
	}


protected:
	PBLOCK m_pMemoryFreeList;		// 存放未使用的内存块头指针
	PCHUNK m_pMemoryChunkList;		// 存放所有数据块的链表指针

	Lock m_lock;

protected:
	// 分配数据块加入到空闲列表
	bool AllocateChunk(void)
	{
		if (m_pMemoryChunkList == NULL || m_pMemoryFreeList == NULL)
			return false;

		size_t chunkNodeSize = sizeof(CHUNK);
		PCHUNK pNode = (PCHUNK)malloc(chunkNodeSize);
		if (pNode == NULL)
			return false;
		memset(pNode, 0, chunkNodeSize);

		size_t count = CHUNK_SIZE / BLOCK_SIZE;
		size_t blockNodeSize = sizeof(BLOCK);
		size_t chunkSize = CHUNK_SIZE + count * (blockNodeSize - sizeof(char));
		if ((pNode->pChunk = malloc(chunkSize)) == NULL)
		{
			free(pNode);
			return false;
		}
		memset(pNode->pChunk, 0, chunkSize);
		PushFrontToChunkList(m_pMemoryChunkList, pNode);

		// 划分数据块加入到空闲列表
		PBLOCK pBlock = reinterpret_cast<PBLOCK> (pNode->pChunk);
		size_t blockSize = blockNodeSize - sizeof(char) + BLOCK_SIZE;
		for (size_t i = 0; i < count; ++i)
		{
			pBlock->size = BLOCK_SIZE;
			PushFront(m_pMemoryFreeList, pBlock);
			pBlock = reinterpret_cast<PBLOCK> ((char *)pBlock + blockSize);
		}
		return true;
	}

	// 在链表中取出节点 若空则返回NULL
	PBLOCK PopFront(PBLOCK pListHead)
	{
		if (pListHead == NULL || pListHead->pNextFreeBlock == NULL)
			return NULL;
		
		PBLOCK pBlock = pListHead->pNextFreeBlock;
		pListHead->pNextFreeBlock = pBlock->pNextFreeBlock;
		return pBlock;
	}

	// 把节点加入到内存块空闲链表中
	bool PushFront(PBLOCK pListHead, PBLOCK pBlock)
	{
		if (pListHead == NULL || pBlock == NULL)
			return false;

		pBlock->pNextFreeBlock = pListHead->pNextFreeBlock;
		pListHead->pNextFreeBlock = pBlock;
		return true;
	}

	// 把节点加入到数据链表中
	bool PushFrontToChunkList(PCHUNK pListHead, PCHUNK pChunk)
	{
		if (pListHead == NULL || pChunk == NULL)
			return false;

		pChunk->pNextChunk = pListHead->pNextChunk;
		pListHead->pNextChunk = pChunk;
		return true;
	}

	// 释放数据块链表
	void FreeChunkList(PCHUNK pListHead)
	{
		if (pListHead == NULL)
			return;

		PCHUNK pChunk = pListHead->pNextChunk;
		PCHUNK pNextNode = NULL;
		while (pChunk != NULL)
		{
			pNextNode = pChunk->pNextChunk;
			free(pChunk->pChunk);
			free(pChunk);
			pChunk = pNextNode;
		}
		free(pListHead);
		pListHead = NULL;
	}

	// 释放内存块节点链表
	void FreeBlockList(PBLOCK pListHead)
	{
		if (pListHead == NULL)
			return;

		free(pListHead);
		pListHead = NULL;
	}

	// 创建数据块链表
	PCHUNK CreateChunkList(void)
	{
		size_t size = sizeof(CHUNK);
		PCHUNK pListHead = (PCHUNK)malloc(size);
		if (pListHead == NULL)
			return NULL;
		memset(pListHead, 0, size);
		return pListHead;
	}

	// 创建内存块链表
	PBLOCK CreateBlockList(void)
	{
		size_t size = sizeof(BLOCK);
		PBLOCK pListHead = (PBLOCK)malloc(size);
		if (pListHead == NULL)
			return NULL;
		memset(pListHead, 0, size);
		return pListHead;
	}
};

