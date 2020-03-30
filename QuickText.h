/*
 *  SYSTEM INCLUDES
 */
#include <sstream>
#include <stdlib.h>
#include "resource.h"
#include "lib/INIMap.h"
#include "menuCmdID.h"
#include "PluginInterface.h"
#include "lib/IniFile.h"
#include "lib/QTString.h"
// #include "sqlite3/CppSQLite3.h"

//////////////////////////////////////////////////////////////////////////
/*
 *  Prototypes
 */
//////////////////////////////////////////////////////////////////////////

void QuickText();
void _refreshINIFiles();
void refreshINIMap();
void openConfigFile();
void openTagsFile();
void jump( HWND &scintilla );
void clear();
void loadConfig();
bool restoreKeyStroke( int cursorPos, HWND &scintilla );
HWND &getCurrentHScintilla();
void stripBreaks( string &str, bool doc, cstring &indent );
void revStripBreaks( string &str );
void decodeStr( cstring &str, int start,
                string &indent ); // Uses global var cQuickText.
BOOL CALLBACK DlgConfigProc( HWND hwndDlg, UINT message, WPARAM wParam,
                             LPARAM lParam );

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
