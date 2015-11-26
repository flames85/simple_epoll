#include <iostream>
#include <string>
using namespace std;

class Base {

};

class CA : public Base
{
	public:

		void NoneArg(void){
			cout << "None-arg" << endl;
		}

		string OnRead(int arg1, int arg2)
		{
			arg1 = 500;
			string str = "Hi!!!";
			cout << "bind OnRead:" << arg1 << " " << arg2 << endl;
			return str;
		}

		void OnLogin(const char* sCode, const char* sInfo)
		{
			cout << sCode << " " << sInfo << endl;
		}

};

class CB : public Base
{
	public:

		long OnWrite(int arg1, int arg2)
		{
			long ret = 9999;
			cout << "bind OnWrite:" << arg1 << " " << arg2 << endl;
			return ret;
		}

};

template <typename T, typename Class>
T Bind(Class* obj, T(Class::*pClassFun)(void))
{
	Class *oo = obj;

	T(Class::*pClassFun2)(void) = pClassFun;

	return (obj->*pClassFun)();
}
template <typename T, typename Class, typename Param1, typename Arg1>
T Bind(Class* obj, T(Class::*pClassFun)(Param1), const Arg1 &arg1)
{
	return (obj->*pClassFun)(arg1);
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2>
T Bind(Class* obj, T(Class::*pClassFun)(Param1, Param2), const Arg1 &arg1, const Arg2 &arg2)
{
	return (obj->*pClassFun)(arg1, arg2);
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3>
T Bind(Class* obj, T(Class::*pClassFun)(Param1, Param2, Param3), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
{
	return (obj->*pClassFun)(arg1, arg2, arg3);
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4>
T Bind(Class* obj, T(Class::*pClassFun)(Param1, Param2, Param3, Param4), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4)
{
	return (obj->*pClassFun)(arg1, arg2, arg3, arg4);
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4, typename Param5, typename Arg5>
T Bind(Class* obj, T(Class::*pClassFun)(Param1, Param2, Param3, Param4, Param5), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5)
{
	return (obj->*pClassFun)(arg1, arg2, arg3, arg4, arg5);
}


template <typename T, typename Class>
T Bind(Class* obj, T(Class::*pClassFun)(void) const)
{
	return (obj->*pClassFun)();
}
template <typename T, typename Class, typename Param1, typename Arg1>
T Bind(Class* obj, T(Class::*pClassFun)(Param1) const, const Arg1 &arg1)
{
	return (obj->*pClassFun)(arg1);
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2>
T Bind(Class* obj, T(Class::*pClassFun)(Param1, Param2) const, const Arg1 &arg1, const Arg2 &arg2)
{
	return (obj->*pClassFun)(arg1, arg2);
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3>
T Bind(Class* obj, T(Class::*pClassFun)(Param1, Param2, Param3) const, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
{
	return (obj->*pClassFun)(arg1, arg2, arg3);
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4>
T Bind(Class* obj, T(Class::*pClassFun)(Param1, Param2, Param3, Param4) const, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4)
{
	return (obj->*pClassFun)(arg1, arg2, arg3, arg4);
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4, typename Param5, typename Arg5>
T Bind(Class* obj, T(Class::*pClassFun)(Param1, Param2, Param3, Param4, Param5) const, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5)
{
	return (obj->*pClassFun)(arg1, arg2, arg3, arg4, arg5);
}



int main()
{
	CA ca;
	CB cb;
	const int arg1 = 123;
	int arg2 = 321;

	cout << "ret:" << Bind(&ca, &CA::OnRead, arg1, arg2) << endl;;
	cout << "ret:" << Bind(&cb, &CB::OnWrite, arg1, arg2) << endl;


	auto my = arg1;

	Bind(&ca, &CA::OnLogin, "code", "info");
	Bind(&ca, &CA::NoneArg);

	return 0;
}
