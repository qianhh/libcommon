#include "StdAfx.h"
#include "Buffer.h"

#ifndef E_INVALIDARG
#define E_INVALIDARG 1 //无效参数
#endif

#ifndef DELETE_PTRS
#define DELETE_PTRS(ptr)	\
	if (ptr) { delete[] ptr; ptr = NULL; }
#endif

namespace eric { namespace common {

template< typename N >
inline N AlignUp( N n, unsigned long nAlign ) throw()
{
	return( N( (n+(nAlign-1))&~(N( nAlign )-1) ) );
}

template< typename N >
inline N AlignDown( N n, unsigned long nAlign ) throw()
{
	return( N( n&~(N( nAlign )-1) ) );
}

Buffer::Buffer(void) : m_pData(NULL), m_lSize(0), m_lAllocSize(0)
{
}

Buffer::Buffer(const Buffer &buf) : m_pData(NULL), m_lSize(0), m_lAllocSize(0)
{
	buf.CopyTo(*this);
}

Buffer::~Buffer(void)
{
	DELETE_PTRS(m_pData);
}

void Buffer::operator = (const Buffer &buf)
{
	buf.CopyTo(*this);
}

void Buffer::Reallocate(long lSize)
{
	if (m_lAllocSize>=lSize || lSize<=0) throw(E_INVALIDARG);

	long lTotalSize = AlignUp( lSize+1, 8 );
	void* p = new char[lTotalSize];
	memset(p, 0, lTotalSize);
	if (m_pData) memcpy(p, m_pData, m_lAllocSize);
	DELETE_PTRS(m_pData);

	m_pData = p;
	m_lAllocSize = lTotalSize;
}

void* Buffer::GetBuffer(long lBufferSize)
{
	if (lBufferSize < 0) throw(E_INVALIDARG);

	if (m_lAllocSize < lBufferSize) Reallocate(lBufferSize);
	return m_pData;
}

void* Buffer::GetBuffer(void) const
{
	return m_pData;
}

void Buffer::SetSize(long lSize)
{
	if (lSize<0 || lSize>m_lAllocSize) throw(E_INVALIDARG);
	m_lSize = lSize;
}

long Buffer::GetSize(void) const
{
	return m_lSize;
}

void Buffer::Empty()
{
	if (m_pData && m_lAllocSize>0) memset(m_pData, 0, m_lAllocSize);
	m_lSize = 0;
}

void* Buffer::GetBufferSetSize(long lSize)
{
	void *pBuffer = GetBuffer(lSize);
	SetSize(lSize);
	return pBuffer;
}

void Buffer::ReleaseBufferSetSize(long lNewSize)
{
	SetSize(lNewSize);
}

void Buffer::Insert(const void* pSrc, long lSize, long lStart)
{
	if (!pSrc || lSize<0 || lStart<0) throw(E_INVALIDARG);

	if (lStart > m_lSize) lStart = m_lSize;
	char *pData = static_cast<char*>(GetBuffer(lSize + m_lSize));
	if (lStart < m_lSize) memmove(pData+lStart+lSize, pData+lStart, m_lSize-lStart);
	memcpy(pData+lStart, pSrc, lSize);
	m_lSize += lSize;
}

void Buffer::Append(const void* pSrc, long lSize)
{
	Insert(pSrc, lSize, m_lSize);
}

void Buffer::AppendStringA(const char* pStr)
{
	if (!pStr) throw(E_INVALIDARG);

	long lLen = (long)strlen(pStr);
	Insert(pStr, lLen, m_lSize);
}

void Buffer::CopyTo(Buffer& dest) const
{
	dest.Empty();
	
	if (m_lSize > 0)
	{
		void *pBuffer = dest.GetBuffer(m_lSize);
		memcpy(pBuffer, m_pData, m_lSize);
		dest.SetSize(m_lSize);
	}	
}

}}
