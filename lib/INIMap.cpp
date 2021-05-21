/* INIMap - INI Files Processor
    Copyright (C) 2006  Joao Moreno (alph.pt@gmail.com)

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

#include <windows.h>
#include "INIMap.h"

INIMap::INIMap ()
{
}

INIMap::~INIMap ()
{
}

// check if a file exists by checking attribute of a file
bool INIMap::FileExists( const TCHAR *filename )
{
    ifstream f;
    f.open( filename );

    if ( !f )
        return false;
    else
    {
        f.close();
        return true;
    }

}

bool INIMap::ReadFile ( const TCHAR *filename )
{
    // empty filename?
    if ( !filename || !filename[0] )
        return false;

/*
    // config files (.ini) exist?
    bool newConfigExist = FileExists( filename );
    bool oldConfigExist = FileExists( filename_Old );

    // moving old .ini to new directory (\plugins\Config\)
    if ( newConfigExist && oldConfigExist )
    {
        // remove new ini file and use old one
        _tremove( filename );
        _trename( filename_Old, filename );
    }
    else if ( oldConfigExist )
    {
        // moving old ini file to new directory
        _trename( filename_Old, filename );
    }
*/

    // reading config files
    ifstream configFile;
    configFile.open( filename );

    if ( !configFile )
        return false;

    string lineBuffer;
    vector<string> curSections; // Current section

    while ( getline( configFile, lineBuffer ) )
    {
        cmatch matchResults;
        regex tagNumberMatch( "\\[([\\d]+)\\]" ); // matching [(1)], [(2)] and etc.
        // 2019-03-22:MVINCENT: tagAndSubstitutionTextMatch not used
        //     the noComment is useful to not match the LANGUAGE_NAME prefix
        //     which can add documentation / comments to the INI file
//      regex tagAndSubstitutionTextMatch("([^=]+)=([\\S]+)"); // match (a)=(<a> </a>)
        regex noComment( "^#LANGUAGE_NAME" ); // match "comments"

        // e.g. [1]
        // parsing tag number
        if ( regex_search( lineBuffer.c_str(), matchResults, tagNumberMatch ) )
        {
            curSections.clear();
            string tagNumber = matchResults[1];

            if ( tagNumber.length() == 0 )
                return false;
            else
                curSections.push_back( tagNumber );

            //Tokenizer t;
            //t.Tokenize(tagNumber, curSections, " ");

            if ( curSections.size() == 0 )
                return false;

            //curSection = string(buffer, s1+1, s2-s1-1);
            //if (curSection.c_str() == "") return false;
        }
        // e.g. if=if ()
        // 2019-03-22:MVINCENT: add else IF ( ... ) to not match "comments"
        else if ( ! regex_search( lineBuffer.c_str(), matchResults, noComment ) )
        {

            string::size_type equalSignIndex = lineBuffer.find( '=' );

            if ( equalSignIndex != string::npos )
            {
                string tagName = string( lineBuffer.substr( 0, equalSignIndex ) );
                string substitutionText = string( lineBuffer.substr( equalSignIndex + 1 ) );

                //if (curSection.c_str() == "" || k.c_str() == "" || d.c_str() == "") return false;
                //if (curSections.size() == 0 || tagName.c_str() == "" || substitutionText.c_str() == "")
                if ( curSections.size() == 0 || tagName.empty()
                        || substitutionText.empty() )
                    return false;

                for ( vector<string>::const_iterator i = curSections.begin();
                        i != curSections.end(); i++ )
                    data[*i][tagName] = substitutionText;
            }
        }
    }

    configFile.close();
    return true;
}

bool INIMap::WriteFile ( const TCHAR *filename, vector<string> lang_menu) const
{
    if ( !filename || !filename[0] )
        return false;

    ofstream f;
    f.open( filename );

    if ( !f )
        return false;

    for ( inimap::const_iterator i = data.begin(); i != data.end(); i++ )
    {
        if ( i->second.begin() == i->second.end() )
            continue;

        f << '[' << i->first << ']' << endl;
        int lang = std::stoi( i->first );
        if ( lang == 255 )
            lang = ( int ) lang_menu.size() - 1;

        f << "#LANGUAGE_NAME=" << lang_menu[lang] << endl;

        for ( keymap::const_iterator j = i->second.begin(); j != i->second.end();
                j++ )
            f << j->first << '=' << j->second << endl;
        f << endl;
    }

    return true;
}

keymap &INIMap::operator[]( cstring &section )
{
    return data[section];
}

keymap &INIMap::operator[]( const char *section )
{
    return data[string( section )];
}

bool INIMap::DeleteSection ( cstring &section )
{
    inimap::iterator i = data.find( section );

    if ( i == data.end() )
        return false;

    data.erase( i );
    return true;
}

bool INIMap::DeleteKey( cstring &section, cstring &key )
{
    inimap::iterator i = data.find( section );

    if ( i == data.end() )
        return false;

    keymap::iterator j = i->second.find( key );

    if ( j == i->second.end() )
        return false;

    i->second.erase( j );
    return true;
}

// check if language section existance in config
bool INIMap::Exists( cstring &section ) const
{
    if ( data.find( section ) != data.end() )
        return true;

    return false;
}

// check if language and tag existance in config
bool INIMap::Exists( cstring &section, cstring &tag ) const
{
    inimap::const_iterator i = data.find( section );

    if ( i == data.end() )
        return false;

    if ( i->second.find( tag ) != i->second.end() )
        return true;

    return false;
}

// query for a list of matching Tags with input string
TagList INIMap::queryTags( cstring &section, cstring &tag )
{
    TagList tagList;
    cmatch matchResults;
    //+@TonyM: "([\\S]*" + tag + "([\\S]*)" -> "^" + tag + "([\\S]*)"
    regex tagsRegex( "^" + tag +
                     "([\\S]*)" ); // match any string with tag as substring

    // use regex to match string
    if ( Exists( section ) )
    {
        keymap curSection = data[section];
        keymap::const_iterator tagPointer = curSection.begin();

        while ( tagPointer != curSection.end() )
        {
            if ( regex_search( tagPointer->first.c_str(), matchResults, tagsRegex ) )
                tagList.insert( tagList.begin(), tagPointer->first );

            tagPointer++;
        }

        // Nice to return a sorted list, but if we get a language list (sort)
        // then a global list (sort) and then combine, still need to sort the 
        // result - so 3 sorts?  How about skip it and only sort the one time, 
        // since we only call queryTags() from QuickText()
        // sort( tagList.begin(), tagList.end() );
    }

    return tagList;
}

void INIMap::Clear()
{
    data.clear();
}