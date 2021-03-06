#ifndef QUICKTEXT_H
#define QUICKTEXT_H

#include <string>
#include <vector>
#include "lib/INIMap.h"
//////////////////////////////////////////////////////////////////////////
/*
 *  Prototypes
 */
//////////////////////////////////////////////////////////////////////////

using namespace std;
typedef const string cstring;

void QuickText();
HWND getCurrentHScintilla();
std::string wstrtostr( const std::wstring & );
void _refreshINIFiles();
void refreshINIMap();
void openConfigFile();
void openTagsFile();
void jump( HWND );
void clear();
void loadConfig();
bool restoreKeyStroke( int, HWND );
void stripBreaks( string &, bool, cstring & );
void revStripBreaks( string & );
void decodeStr( cstring &, int ,string & ); // Uses global var cQuickText.
BOOL CALLBACK DlgConfigProc( HWND, UINT, WPARAM, LPARAM );

//////////////////////////////////////////////////////////////////////////
/*
 *  QuickText specific configs
 */
//////////////////////////////////////////////////////////////////////////
struct
{
    string text;
    vector<int> hotSpotsPos;
    vector<int> hotSpotsLen;
    bool editing;
    /*unsigned*/ int cHotSpot; // Current HotSpot
} cQuickText; // Current QuickText tag

struct
{
    bool indenting;
} Config;

struct
{
    HWND langCB, tag, text, add, del, tagname;
    bool changed;
} ConfigWin;

HINSTANCE appInstance;

// *** Variables
INIMap tags, tags_replica;
basic_string<TCHAR> tagsFileName;

#endif
