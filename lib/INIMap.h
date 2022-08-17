/* INIMap - INI Files Processor
    Copyright (C) 2006  João Moreno (alph.pt@gmail.com)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef INIMAP_H
#define INIMAP_H

#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <regex>
#include <algorithm>
// #include "Tokenizer.h"
#include <TCHAR.h>


using namespace std;
using namespace std::tr1;

typedef const wstring cstring;
typedef map<wstring, wstring> keymap;
typedef map<wstring, keymap> inimap;
typedef vector<wstring> SnipList;

class INIMap
{
    private:
        inimap data;
    public:
        INIMap ();
        virtual ~INIMap ();
        virtual bool ReadFile ( const TCHAR *filename );
        virtual bool WriteFile ( const TCHAR *filename, vector<wstring> lang_menu ) const;
        virtual keymap &operator[]( cstring &section );
        virtual keymap &operator[]( const TCHAR *section );
        virtual bool Exists( cstring &section ) const;
        virtual bool Exists( cstring &section, cstring &snip ) const;
        virtual bool DeleteSection ( cstring &section );
        virtual bool DeleteKey( cstring &section, cstring &key );
        virtual void Clear();
        virtual bool FileExists( const TCHAR *filename );
        // query for a list of matching Snip with input wstring
        virtual vector<wstring> querySnips( cstring &section, cstring &key );
};

#endif
