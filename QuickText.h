#ifndef QUICKTEXT_H
#define QUICKTEXT_H

#include <string>
#include <vector>
#include "Scintilla.h"
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
void openSnipsFile();
void jump( HWND );
void clear();
void doSettings();
bool restoreKeyStroke( Sci_Position, HWND );
void replaceTabs( string & );
void stripBreaks( string &, bool, cstring & );
void revStripBreaks( string & );
void decodeStr( cstring &, Sci_Position ,string & ); // Uses global var cQuickText.
void ChangeFont( HWND, int, int, LPCWSTR );
void SetNppColors( void );
void SetSysColors( void );
void ChangeColors( void );
LRESULT CALLBACK DlgConfigProc( HWND, UINT, WPARAM, LPARAM );

//////////////////////////////////////////////////////////////////////////
/*
 *  QuickText specific configs
 */
//////////////////////////////////////////////////////////////////////////
struct
{
    string text;
    vector<Sci_Position> hotSpotsPos;
    vector<Sci_Position> hotSpotsLen;
    bool editing;
    /*unsigned*/ int cHotSpot; // Current HotSpot
} cQuickText; // Current QuickText snip

struct
{
    bool indenting;
} Config;

struct
{
    HWND langCB, snip, text, add, del, snipname;
    bool changed;
} ConfigWin;

HINSTANCE appInstance;

// *** Variables
INIMap snips, snips_replica;
basic_string<TCHAR> snipsFileName;

#endif
