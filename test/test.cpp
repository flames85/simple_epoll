//testtemplatecallback.h

#include <iostream>
#include <string>

using namespace std;

template<typename X, typename Y>
class TestCallBack
{
	typedef X (*P_FN)(Y);
	public:
	TestCallBack(P_FN pfn):m_pFn(pfn){}
	void operator()()
	{
		m_pFn(2);
	}
	private:
	P_FN m_pFn;
};

/*
//可以在类中使用回调函数模板,而不能定义全局回调函数模板
//错误如下:
//error C2823: typedef 模板 非法
//error C2998: “P (__cdecl *__cdecl P_FNZ)(Q)”: 不能是模板定义

template<typename P,typename Q>
typedef P (*P_FNZ)(Q);
 */

//这个能够行
template<typename M, typename N>
int FuncCallBack(M (*P_pFN)(N))
{
	return P_pFN(1);
}
int HelloCallBack(int )
{
	cout<<"hello world"<<endl;
	return 0;
}
void TestCall_Back()
{
	TestCallBack<int,int> TCB(HelloCallBack);
	TCB();
	//FuncCallBack(HelloCallBack);
}


int main() {
	TestCall_Back();
	return 0;
}
