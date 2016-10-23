#pragma once

/**
 ** 单例模式基类(饿汉模式)，支持多线程操作
 ** designed by eric,2016/08/20
 **/
template<class T>
class CSingleton
{
public:
	static T* GetInstance(void)
	{
		static T instance;
		//这个DoNothing是确保CreateObject构造函数被调用,这跟模板的编译有关
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
