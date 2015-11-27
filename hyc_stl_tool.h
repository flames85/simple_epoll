#ifndef HYC_STL_TOOL_H
#define HYC_STL_TOOL_H

// stl
#include <string>
#include <sstream>
#include <iomanip>

using namespace std ;

namespace STL{

    //! 函数功能：string to int
    //! nScale表示string是何进制
    int str2int( const string &str, int nScale = 10);

    //! 函数功能：int to string
    //! nScale表示num是什么进制
    //! nWidth表示生成的字符串的宽度（不足0的在前面以0填充）
    string int2str( int num, int nScale = 10, int nWidth = 0);

}
#endif // HYC_STL_TOOL_H
