#include <iostream>
#include <string>
using namespace std;

class Base {
public:

};

class CA : public Base
{
	public:

		void NoneArg(void){
			cout << "None-arg" << endl;
		}

};

class CB : public Base
{
	public:

		long OnWrite(void)
		{
			cout << "bind OnWrite:" << endl;
			return 0;
		}

};


template <typename Class, typename T>
class Binded 
{
	typedef T (Class::*ClassFun)(void);
	
public:
	Binded(Class* obj, ClassFun fun)
	{
		m_obj = obj;
		m_fun = fun;		
	}

	T excute() {
		return (m_obj->*m_fun)();
	}

	Class 		*m_obj;
	ClassFun    m_fun;
	
};	

template <typename Class, typename T>
Binded<Class, T>* Bind(Class* obj, T (Class::*pClassFun)(void))
{
	Binded<Class, T> *bind_t = new Binded<Class, T>(obj, pClassFun);
	return bind_t;
}

int main()
{
	CA ca;
	CB cb;
	const int arg1 = 123;
	int arg2 = 321;

	Bind(&ca, &CA::NoneArg)->excute();

	return 0;
}
