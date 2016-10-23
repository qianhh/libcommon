#pragma once

/**
 ** �������
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
	 ** ��ֵ����
	 ** @param[in] buf				ԴBuffer
	 **/
	void operator = (const Buffer &buf);

	/**
	 ** ��ȡ����
	 **/
	void* GetBuffer(void) const;
	/**
	 ** ��ȡָ���ֽڵĻ��棬ʹ����֮�������ReleaseBufferSetSize
	 ** @param[in] lBufferSize		�����С
	 **/
	void* GetBuffer(long lBufferSize);
	/**
	 ** ��ȡָ���ֽڵĻ��沢���û����С
	 ** @param[in] lSize			�����С
	 **/
	void* GetBufferSetSize(long lSize);
	/**
	 ** ���û����С
	 ** @param[in] lNewSize(lSize)	�����С
	 **/
	void ReleaseBufferSetSize(long lNewSize);
	void SetSize(long lSize);
	/**
	 ** ��ȡ�����С
	 **/
	long GetSize(void) const;
	/**
	 ** ��ջ���
	 **/
	void Empty(void);
	/**
	 ** ����ָ�����ȵ��ֽ�
	 ** @param[in] pSrc				��ʼ��ַ
	 ** @param[in] lSize			�ֽ���
	 ** @param[in] lStart			����λ��
	 **/
	void Insert(const void* pSrc, long lSize, long lStart);
	/**
	 ** ׷��ָ�����ȵ��ֽ�
	 ** @param[in] pSrc				��ʼ��ַ
	 ** @param[in] lSize			�ֽ���
	 **/
	void Append(const void* pSrc, long lSize);
	/**
	 ** ׷���ַ���
	 ** @param[in] pStr				�ַ���
	 **/
	void AppendStringA(const char* pStr);
	/**
	 ** ����
	 ** @param[out] dest			Ŀ��Buffer
	 **/
	void CopyTo(Buffer& dest) const;

private:
	void Reallocate(long lSize); //�����ڴ�

private:
	void *m_pData;
	long m_lSize;
	long m_lAllocSize;
};

}}
