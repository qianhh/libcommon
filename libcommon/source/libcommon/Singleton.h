#pragma once

/**
 ** ����ģʽ����(����ģʽ)��֧�ֶ��̲߳���
 ** designed by eric,2016/08/20
 **/
template<class T>
class CSingleton
{
public:
	static T* GetInstance(void)
	{
		static T instance;
		//���DoNothing��ȷ��CreateObject���캯��������,���ģ��ı����й�
		s_CreateObject.DoNothing();
		return &instance;
	}

	class CreateObject
	{
	public:
		CreateObject(void) { CSingleton<T>::GetInstance(); }
		void DoNothing(void) {};
	};

protected:
	CSingleton(void) {}
	~CSingleton(void) {}
	CSingleton(const CSingleton &);
	CSingleton& operator = (const CSingleton &);

private:
	static CreateObject s_CreateObject;
};

template<typename T>
typename CSingleton<T>::CreateObject CSingleton<T>::s_CreateObject;
