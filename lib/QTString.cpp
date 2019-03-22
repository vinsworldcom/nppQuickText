//+@TonyM
#include "QTString.h"
#include <intsafe.h>

const std::string QTString::WHITE_SPACES = " \f\n\r\t\v";

std::string QTString::trim( std::string str, const std::string chars )
{
    return str.erase( str.find_last_not_of( chars ) + 1 ).erase( 0,
            str.find_first_not_of( chars ) );
}

std::vector<std::string> QTString::vexplode ( std::string delimiter,
        std::string _string, bool _trim )
{
    int _string_size;
    ULongPtrToInt ( _string.size(), &_string_size );

    int delimiter_size;
    ULongPtrToInt ( delimiter.size(), &delimiter_size );

    int pos = 0;
    std::vector <std::string> vr;

    if ( _trim )
    {
        while ( pos != std::string::npos && pos < _string_size )
        {
            ULongPtrToInt( _string.find( delimiter, 0 ), &pos );
            vr.push_back( trim( _string.substr( 0, pos ) ) );
            _string.erase( 0, pos + delimiter_size );
        };

        //vr.assign (vr.begin(),remove( vr.begin(), vr.end(), ""));
    }
    else
    {
        while ( pos != std::string::npos && pos < _string_size )
        {
            ULongPtrToInt( _string.find( delimiter, 0 ), &pos );
            vr.push_back( _string.substr( 0, pos ) );
            _string.erase( 0, pos + delimiter_size );
        };
    };

    return vr;
}

std::wstring QTString::s2ws( const std::string &s )
{
    int len;
    int slength = ( int )s.length() + 1;
    len = MultiByteToWideChar( CP_ACP, 0, s.c_str(), slength, 0, 0 );
    wchar_t *buf = new wchar_t[len];
    MultiByteToWideChar( CP_ACP, 0, s.c_str(), slength, buf, len );
    std::wstring r( buf );
    delete[] buf;
    return r;
}

std::string QTString::ws2s ( const std::wstring &ws )
{
    std::string s;
    s.assign ( ws.begin (), ws.end () );
    return s;
}