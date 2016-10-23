#pragma once

#define CHUNK_SIZE 1024*1024

#pragma pack(push, 1)
typedef struct tagChunk
{
	void				*pChunk;			//���ݿ�ĵ�ַ
	struct tagChunk		*pNextChunk;		//��һ�����ݿ�ڵ�ĵ�ַ
}CHUNK, *PCHUNK;

typedef struct tagBlock
{
	size_t				size;				// ���Block�Ĵ�С
	struct tagBlock		*pNextFreeBlock;	// ��һ�����нڵ�ĵ�ַ
	char				address[1];			// ��������ݵ�ַ
}BLOCK, *PBLOCK;
#pragma pack(pop)

/**
 ** �ڴ�ص�Ԫ����
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

	// ��ʼ����Ԫ�ڴ��
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

	// �ͷŵ�Ԫ�ڴ��
	void FreeMemoryPoolUnit(void)
	{
		FreeChunkList(m_pMemoryChunkList);
		FreeBlockList(m_pMemoryFreeList);
	}

	// �����ڴ��
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

	// �ͷ��ڴ��
	void FreeBlock(PBLOCK pBlock)
	{
		m_lock.Lock();
		PushFront(m_pMemoryFreeList, pBlock);
		m_lock.Unlock();
	}


protected:
	PBLOCK m_pMemoryFreeList;		// ���δʹ�õ��ڴ��ͷָ��
	PCHUNK m_pMemoryChunkList;		// ����������ݿ������ָ��

	Lock m_lock;

protected:
	// �������ݿ���뵽�����б�
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

		// �������ݿ���뵽�����б�
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

	// ��������ȡ���ڵ� �����򷵻�NULL
	PBLOCK PopFront(PBLOCK pListHead)
	{
		if (pListHead == NULL || pListHead->pNextFreeBlock == NULL)
			return NULL;
		
		PBLOCK pBlock = pListHead->pNextFreeBlock;
		pListHead->pNextFreeBlock = pBlock->pNextFreeBlock;
		return pBlock;
	}

	// �ѽڵ���뵽�ڴ�����������
	bool PushFront(PBLOCK pListHead, PBLOCK pBlock)
	{
		if (pListHead == NULL || pBlock == NULL)
			return false;

		pBlock->pNextFreeBlock = pListHead->pNextFreeBlock;
		pListHead->pNextFreeBlock = pBlock;
		return true;
	}

	// �ѽڵ���뵽����������
	bool PushFrontToChunkList(PCHUNK pListHead, PCHUNK pChunk)
	{
		if (pListHead == NULL || pChunk == NULL)
			return false;

		pChunk->pNextChunk = pListHead->pNextChunk;
		pListHead->pNextChunk = pChunk;
		return true;
	}

	// �ͷ����ݿ�����
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

	// �ͷ��ڴ��ڵ�����
	void FreeBlockList(PBLOCK pListHead)
	{
		if (pListHead == NULL)
			return;

		free(pListHead);
		pListHead = NULL;
	}

	// �������ݿ�����
	PCHUNK CreateChunkList(void)
	{
		size_t size = sizeof(CHUNK);
		PCHUNK pListHead = (PCHUNK)malloc(size);
		if (pListHead == NULL)
			return NULL;
		memset(pListHead, 0, size);
		return pListHead;
	}

	// �����ڴ������
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

