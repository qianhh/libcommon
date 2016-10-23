#pragma once

/**
 ** 缓存管理
 ** designed by eric,2016/08/20
 **/
namespace eric { namespace common {
class Buffer
{
public:
	Buffer(void);
	Buffer(const Buffer &buf);
	virtual ~Buffer(void);

	/**
	 ** 赋值操作
	 ** @param[in] buf				源Buffer
	 **/
	void operator = (const Buffer &buf);

	/**
	 ** 获取缓存
	 **/
	void* GetBuffer(void) const;
	/**
	 ** 获取指定字节的缓存，使用完之后需调用ReleaseBufferSetSize
	 ** @param[in] lBufferSize		缓存大小
	 **/
	void* GetBuffer(long lBufferSize);
	/**
	 ** 获取指定字节的缓存并设置缓存大小
	 ** @param[in] lSize			缓存大小
	 **/
	void* GetBufferSetSize(long lSize);
	/**
	 ** 设置缓存大小
	 ** @param[in] lNewSize(lSize)	缓存大小
	 **/
	void ReleaseBufferSetSize(long lNewSize);
	void SetSize(long lSize);
	/**
	 ** 获取缓存大小
	 **/
	long GetSize(void) const;
	/**
	 ** 清空缓存
	 **/
	void Empty(void);
	/**
	 ** 插入指定长度的字节
	 ** @param[in] pSrc				起始地址
	 ** @param[in] lSize			字节数
	 ** @param[in] lStart			插入位置
	 **/
	void Insert(const void* pSrc, long lSize, long lStart);
	/**
	 ** 追加指定长度的字节
	 ** @param[in] pSrc				起始地址
	 ** @param[in] lSize			字节数
	 **/
	void Append(const void* pSrc, long lSize);
	/**
	 ** 追加字符串
	 ** @param[in] pStr				字符串
	 **/
	void AppendStringA(const char* pStr);
	/**
	 ** 拷贝
	 ** @param[out] dest			目标Buffer
	 **/
	void CopyTo(Buffer& dest) const;

private:
	void Reallocate(long lSize); //分配内存

private:
	void *m_pData;
	long m_lSize;
	long m_lAllocSize;
};

}}
