//+@TonyM

#ifndef QTSTRING_H
#define QTSTRING_H

#include <sstream>
#include <string>
#include <vector>
#include <windows.h>

// QT - from QuickText not from Nokia's Qt library
class QTString
{
    public:
        static const std::string WHITE_SPACES;
        // Strip whitespace (or other characters) from the beginning and end of a string
        static std::string trim( std::string str,
                                 const std::string chars = WHITE_SPACES );

        // Split a string by string
        static std::vector<std::string> vexplode ( std::string delimiter,
                std::string _string, bool _trim = false );

        // Convert string to wide string
        static std::wstring s2ws( const std::string &s );

        // Convert wide string to string
        static std::string ws2s ( const std::wstring &ws );
};

#endif
