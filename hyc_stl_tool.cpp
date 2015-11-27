#include "hyc_stl_tool.h"

int STL::str2int( const string &str, int nScale)
{
   stringstream ss( str);
    int num;

    switch( nScale)
    {
    case 16:
        if(( ss >> hex >> num). fail())
        {
            // ERROR
        }
        break;
    case 10:
        if(( ss >> dec >> num). fail())
        {
            //ERROR
        }
        break;
    case 8:
        if(( ss >> oct >> num). fail())
        {
            //ERROR
        }
        break;
    default:
        // ERROR
        break;
    }

    return num;
}


string STL::int2str( int num, int nScale , int nWidth)
{
    stringstream strStream;
    switch(nScale)
    {
    case 16:
        strStream << hex << setw(nWidth) << setfill('0') << num;
        break;
    case 10:
        strStream << dec << setw(nWidth) << setfill('0') << num;
        break;
    case 8:
        strStream << oct << setw(nWidth) << setfill('0') << num;
        break;
    default:
        // ERROR
        break;
    }

    return strStream.str();
}
