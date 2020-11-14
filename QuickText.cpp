/* QuickText - Quick editing tags for Notepad++
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
#include <commctrl.h>
#include <intsafe.h>
#include <shlwapi.h>

#include "PluginInterface.h"
#include "menuCmdID.h"
#include "resource.h"
#include "QuickText.h"

#include "lib/INIMap.h"
#include "lib/IniFile.h"
#include "lib/QTString.h"

#define NEW_HOTSPOT -1
#define SZ_TAG 32

// *** Plugin specific variables
const TCHAR NPP_PLUGIN_NAME[] = _T( "QuickText" ); // Nome do plugin
const int nbFunc = 7; 

const TCHAR confFileName[]    = TEXT( "QuickText.conf.ini" );
const TCHAR dataFileName[]    = TEXT( "QuickText.ini" );
const TCHAR dataFileDefault[] = TEXT( "QuickText.default.ini" );
basic_string<TCHAR> confFilePath;
const char sectionName[]        = "General";
const char iniKeyAllowedChars[] = "AllowedChars";
const char iniInsertOnAutoC[]   = "InsertOnAutoC";

NppData nppData; // handles
FuncItem funcItems[nbFunc];

bool g_bInsertOnAutoC = false;

//+@TonyM: added some characters (._-). more characters I've added, more errors occure.
std::string allowedChars =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890._-";
//+@TonyM: string lang_menu[] -> vector<string> lang_menu(256) - for dynamic loading from configuration file.
vector<string> lang_menu;

#define REGIMGID 20820
/* XPM */
const char *xpmQt[] = {
    /* columns rows colors chars-per-pixel */
    "16 16 39 1 ",
    "  c black",
    ". c #3A0000",
    "X c #3A3A00",
    "o c #00003A",
    "O c #660000",
    "+ c #663A00",
    "@ c #663A3A",
    "# c #000066",
    "$ c #003A66",
    "% c #903A00",
    "& c #B66600",
    "* c #90663A",
    "= c #B6903A",
    "- c #DB903A",
    "; c #B69066",
    ": c #DB9066",
    "> c #DBB666",
    ", c #FFB666",
    "< c #003A90",
    "1 c #3A6690",
    "2 c #0066B6",
    "3 c #3A66B6",
    "4 c #6666B6",
    "5 c #3A90DB",
    "6 c #66B6DB",
    "7 c #66B6FF",
    "8 c #9090B6",
    "9 c #DBB690",
    "0 c #FFDBB6",
    "q c #FFFFB6",
    "w c #90B6DB",
    "e c #B6DBDB",
    "r c #90DBFF",
    "t c #B6FFFF",
    "y c gray86",
    "u c #FFFFDB",
    "i c #DBDBFF",
    "p c #DBFFFF",
    "a c white",
    /* pixels */
    "aaaa9@  $raaaaaa",
    "aa0+o6a0O 7aaaaa",
    "aqO 7aaa- <paaaa",
    "u% 2aaaa- #taaaa",
    ",  7aaa-       #",
    "% #taaa%3i- <p><",
    "O 2aaaa4 u% 5au8",
    "O 5aaau% qOoraaa",
    "% 5aaa- <, #taaa",
    ", <pu= <p& 2aaaa",
    "a>.  $waqO 7aaaa",
    "a0*3aaaa, #taaaa",
    ":.   1ea- 2aaaaa",
    "yau;X  0O 5aaaaa",
    "aaaaaa&    2aaaa",
    "aaaaaaaaaaaaaaaa"
};

void commandMenuCleanUp()
{
    // Don't forget to deallocate your shortcut here
    delete funcItems[0]._pShKey;
}

void pluginInit( HANDLE /* hModule */ )
{

}

void pluginCleanUp()
{
    std::string ini_file_path = wstrtostr( confFilePath.c_str() );

    ::WritePrivateProfileStringA( sectionName, iniKeyAllowedChars,
                                 allowedChars.c_str(), ini_file_path.c_str() );
    ::WritePrivateProfileStringA( sectionName, iniInsertOnAutoC,
                                 g_bInsertOnAutoC ? "1" : "0", ini_file_path.c_str() );
}

// *** Main
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  reasonForCall,
                       LPVOID /* lpReserved */ )
{
    switch ( reasonForCall )
    {
        case DLL_PROCESS_ATTACH:
            pluginInit( hModule );
            appInstance = ( HINSTANCE ) hModule;
            break;

        case DLL_PROCESS_DETACH:
            pluginCleanUp();
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;
    }

    return TRUE;
}

void commandMenuInit()
{
    // get path of plugin configuration
    TCHAR get_confFilePath[MAX_PATH];
    TCHAR get_dataFilePath[MAX_PATH];
    ::SendMessage( nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH,
                   ( LPARAM )get_confFilePath );
    ::SendMessage( nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH,
                   ( LPARAM )get_dataFilePath );

    // if config path doesn't exist, we create it
    if ( PathFileExists( get_confFilePath ) == FALSE )
        ::CreateDirectory( get_confFilePath, NULL );

    // make your plugin config file full file path name
    PathAppend( get_confFilePath, confFileName );
    PathAppend( get_dataFilePath, dataFileName );
    confFilePath = get_confFilePath;
    tagsFileName = get_dataFilePath;

    // Move data file to plugins/config if not there
    if ( ! PathFileExists( tagsFileName.c_str() ) )
    {
        TCHAR temp[256];
        basic_string<TCHAR> defaultDbTile;

        GetModuleFileName( ( HMODULE )appInstance, temp, sizeof( temp ) );
        defaultDbTile = temp;
        unsigned int pos;
        pos = static_cast<unsigned int>( defaultDbTile.rfind( _T( "\\" ) ) );
        defaultDbTile.erase( pos );
        defaultDbTile.append( _T( "\\" ) );
        defaultDbTile.append( dataFileDefault );

        if ( PathFileExists( defaultDbTile.c_str() ) )
        {
            CopyFile( defaultDbTile.c_str(), tagsFileName.c_str(), true );
        }
    }

    // funcItems setting
    funcItems[0]._pFunc = QuickText;
    lstrcpy( funcItems[0]._itemName, _T( "&Replace Tag" ) );
    funcItems[0]._init2Check = false;

    funcItems[1]._pFunc = NULL;
    lstrcpy( funcItems[1]._itemName, _T( "-SEPARATOR-" ) );
    funcItems[1]._init2Check = false;

    funcItems[2]._pFunc = openTagsFile;
    lstrcpy( funcItems[2]._itemName, _T( "Open &Tags File" ) );
    funcItems[2]._init2Check = false;

    funcItems[3]._pFunc = openConfigFile;
    lstrcpy( funcItems[3]._itemName, _T( "Open &Config File" ) );
    funcItems[3]._init2Check = false;

    funcItems[4]._pFunc = refreshINIMap;
    lstrcpy( funcItems[4]._itemName, _T( "Re&fresh Configuration" ) );
    funcItems[4]._init2Check = false;

    funcItems[5]._pFunc = NULL;
    lstrcpy( funcItems[5]._itemName, _T( "-SEPARATOR-" ) );
    funcItems[5]._init2Check = false;

    funcItems[6]._pFunc = loadConfig;
    lstrcpy( funcItems[6]._itemName, _T( "&Settings" ) );
    funcItems[6]._init2Check = false;

    Config.indenting = true;

    cQuickText.editing = false;
    //+@TonyM: refreshINIMap() -> _refreshINIFiles() - to skip displaying
    //+@TonyM: MessageBox with confirmation of INI files reload on the start of Notepad++.
    _refreshINIFiles();
}

// Notepad++ - handles
extern "C" __declspec( dllexport ) void setInfo( NppData notpadPlusData )
{
    nppData = notpadPlusData;
    commandMenuInit();
}

// Notepad++ - nome do plugin
extern "C" __declspec( dllexport ) const TCHAR *getName()
{
    return NPP_PLUGIN_NAME;
}

// Notepad++ - funcoes
extern "C" __declspec( dllexport ) FuncItem *getFuncsArray( int *nbF )
{
    *nbF = nbFunc;
    return funcItems;
}

// Notepad++ - messages from Scinttila
extern "C" __declspec( dllexport ) void beNotified( SCNotification
        *notifyCode )
{
    switch ( notifyCode->nmhdr.code )
    {
        // check if editing text in hotspot
        case SCN_MODIFIED:
        {
            if ( cQuickText.editing )
            {
                if ( notifyCode->modificationType & SC_MOD_INSERTTEXT )
                {
                    for ( vector<int>::iterator i = cQuickText.hotSpotsPos.begin();
                            i != cQuickText.hotSpotsPos.end(); i++ )
                        if ( *i > notifyCode->position )
                            ( *i ) += notifyCode->length;

                    // 2019-03-23:MVINCENT: if current position is at the end
                    //   of inserted tag text then clear the tag hotspots else
                    //   inserted chars add to the overall length
                    int currpos = static_cast<int>( SendMessage( getCurrentHScintilla(),
                                                    SCI_GETCURRENTPOS, 0, 0 ) );

                    if ( currpos == cQuickText.hotSpotsPos[cQuickText.hotSpotsPos.size() - 1] )
                        clear();
                    else
                        cQuickText.hotSpotsLen[cQuickText.cHotSpot] += notifyCode->length;
                }

                if ( notifyCode->modificationType & SC_MOD_DELETETEXT )
                {
                    if ( cQuickText.hotSpotsLen[cQuickText.cHotSpot] == 0 )
                    {
                        clear();
                        return;
                    }

                    for ( vector<int>::iterator i = cQuickText.hotSpotsPos.begin();
                            i != cQuickText.hotSpotsPos.end(); i++ )
                        if ( *i > notifyCode->position )
                            ( *i ) -= notifyCode->length;

                    cQuickText.hotSpotsLen[cQuickText.cHotSpot] -= notifyCode->length;
                }
            }
        }
        break;

        // check if outside a hotspot
        case SCN_UPDATEUI:
        {
            int currpos = static_cast<int>( SendMessage( getCurrentHScintilla(),
                                            SCI_GETCURRENTPOS, 0, 0 ) );
            bool r = true;

            for ( unsigned int i = 0; i < cQuickText.hotSpotsPos.size(); i++ )
                if ( currpos >= cQuickText.hotSpotsPos[i]
                        && currpos <= cQuickText.hotSpotsPos[i] + cQuickText.hotSpotsLen[i] )
                    r = false;

            if ( r )
                clear();
        }
        break;

        case SCN_AUTOCCOMPLETED:
        {
           if ( g_bInsertOnAutoC )
               QuickText();
        }
        break;

        case NPPN_SHUTDOWN:
        {
            commandMenuCleanUp();
        }
        break;

        default:
            return;
    }
}

// Notepad++ - messages from Npp
extern "C" __declspec( dllexport ) LRESULT messageProc( UINT /* Message */,
        WPARAM /* wParam */, LPARAM /* lParam */ )
{
    return TRUE;
}

#ifdef UNICODE
extern "C" __declspec( dllexport ) BOOL isUnicode()
{
    return TRUE;
}
#endif

/* Functions */
// get Scintilla's Handle
HWND getCurrentHScintilla()
{
    int currentEdit;
    SendMessage( nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0,
                 ( LPARAM )&currentEdit );
    return ( currentEdit == 0 ) ? nppData._scintillaMainHandle :
           nppData._scintillaSecondHandle;
}


std::string wstrtostr( const std::wstring &wstr )
{
    // Convert a Unicode string to an ASCII string
    std::string strTo;
    char *szTo = new char[wstr.length() + 1];
    szTo[wstr.size()] = '\0';
    WideCharToMultiByte( CP_ACP, 0, wstr.c_str(), -1, szTo,
                         ( int )wstr.length(), NULL, NULL );
    strTo = szTo;
    delete[] szTo;
    return strTo;
}

//+@TonyM: Used without wrapper (refreshINIMap()) to skip displaying
//+@TonyM: MessageBox with confirmation of INI files reload on the start of Notepad++.
void _refreshINIFiles()
{
    tags.Clear();
    tags.ReadFile( tagsFileName.c_str() );

    std::string ini_file_path    = wstrtostr( confFilePath.c_str() );
    std::string ini_file_section = sectionName;

    //+@TonyM: Reads allowedChars value from config file on each config refresh
    std::string ini_allowedChars = CIniFile::GetValue( iniKeyAllowedChars,
                                   ini_file_section, ini_file_path );
    if ( !ini_allowedChars.empty() )
        allowedChars = ini_allowedChars;

    std::string autoC = CIniFile::GetValue( iniInsertOnAutoC,
                                     ini_file_section, ini_file_path );
    if ( !autoC.empty() )
    {
        int val = std::stoi( autoC );
        if ( val != 0 )
            g_bInsertOnAutoC = true;
    }

    int i = 0;
    TCHAR langName[MAX_PATH];
    lang_menu.clear();
    do
    {
        SendMessage( nppData._nppHandle, NPPM_GETLANGUAGENAME, i, ( LPARAM ) langName );
        lang_menu.push_back( wstrtostr( langName ));
        i++;
    } while ( ( strcmp( wstrtostr( langName ).c_str(), "External" ) != 0 ) 
           && ( i < 255 ) );
    lang_menu.push_back( "GLOBAL" );
}

// Clears and loads the INI file
void refreshINIMap()
{
    _refreshINIFiles();
    MessageBox( nppData._nppHandle,
                _T( "QuickText.ini and QuickText.conf.ini files reloaded!" ),
                NPP_PLUGIN_NAME, MB_OK | MB_ICONINFORMATION );
}

void openConfigFile()
{
    SendMessage( nppData._nppHandle, NPPM_DOOPEN, 0,
                 ( LPARAM )confFilePath.c_str() );
}

void openTagsFile()
{
    SendMessage( nppData._nppHandle, NPPM_DOOPEN, 0,
                 ( LPARAM )tagsFileName.c_str() );
}

// Strip all the line breaks
void stripBreaks( string &str, bool doc = false, cstring &indent = "" )
{
    char newline[3] = "\r\n";

    if ( doc )
    {
        HWND scintilla = getCurrentHScintilla();
        int mode = static_cast<int>( SendMessage( scintilla, SCI_GETEOLMODE, 0,
                                     0 ) );

        switch ( mode )
        {
            case SC_EOL_CRLF:
                strcpy( newline, "\r\n" );
                break;

            case SC_EOL_CR:
                strcpy( newline, "\r" );
                break;

            case SC_EOL_LF:
                strcpy( newline, "\n" );
                break;
        }
    }

    unsigned i;
    int newlineLength = ( int )strlen( newline );

    // 2019-03-22:MVINCENT: static_cast<unsigned>(str.npos) for 64-bit
    while ( ( i = static_cast<unsigned>( str.find( "\\n" ) ) ) !=
            static_cast<unsigned>( str.npos ) )
    {
        str.erase( i, 2 );
        str.insert( i, newline, newlineLength );
        str.insert( i + newlineLength, indent );
    }
}

// Reverse Strip all the line breaks
void revStripBreaks( string &str )
{
    unsigned i;

    while ( ( i = static_cast<unsigned>( str.find( "\r\n" ) ) ) !=
            static_cast<unsigned>( str.npos ) )
    {
        str.erase( i, 2 );
        str.insert( i, "\\n" );
    }
}

// set cQuickText.text to substitution texts, setup hotspots hopping
void decodeStr( cstring &str, int start, string &indent )
{
    cQuickText.text = str;
    stripBreaks( cQuickText.text, true, indent );

    for ( string::iterator i = cQuickText.text.begin();
            i != cQuickText.text.end(); )
    {
        if ( ( *i ) == '$' )
        {
            if ( i != cQuickText.text.begin() )
            {
                if ( ( *( i - 1 ) ) == '\\' )
                {
                    cQuickText.text.erase( i - 1 );
                    continue;
                }
                else
                {
                    cQuickText.text.erase( i );
                    cQuickText.hotSpotsPos.push_back( start + static_cast<int>
                                                      ( i - cQuickText.text.begin() ) );
                    cQuickText.hotSpotsLen.push_back( 0 );
                    continue;
                }
            }
            else
            {
                cQuickText.text.erase( i );
                cQuickText.hotSpotsPos.push_back( start + static_cast<int>
                                                  ( i - cQuickText.text.begin() ) );
                cQuickText.hotSpotsLen.push_back( 0 );
                continue;
            }
        }

        i++;
    }
}

void QuickText()
{
    HWND scintilla = getCurrentHScintilla();

    int curPos, startPos, endPos;
    int textSelectionStart, textSelectionEnd ;
    char tag[256];
    LangType langtype;
    char sLangType[3];
    char sLangTypeGlobal[3];

    // define 'text' for scintilla
    SendMessage( scintilla, SCI_SETWORDCHARS, 0,
                 ( LPARAM )allowedChars.c_str() );

    // if a block of text is selected for tabbing
    textSelectionStart = static_cast<int>( SendMessage( scintilla,
                                           SCI_GETSELECTIONSTART, 0, 0 ) );
    textSelectionEnd = static_cast<int>( SendMessage( scintilla,
                                         SCI_GETSELECTIONEND, 0, 0 ) );

    // get 'text' location
    curPos = static_cast<int>( SendMessage( scintilla, SCI_GETCURRENTPOS, 0,
                                            0 ) );
    startPos = static_cast<int>( SendMessage( scintilla, SCI_WORDSTARTPOSITION,
                                 curPos, ( LPARAM )true ) );
    endPos = static_cast<int>( SendMessage( scintilla, SCI_WORDENDPOSITION,
                                            curPos, ( LPARAM )true ) );

    // copy 'text' to tag
    SendMessage( scintilla, SCI_SETSELECTIONSTART, startPos, 0 );
    SendMessage( scintilla, SCI_SETSELECTIONEND, endPos, 0 );
    SendMessage( scintilla, SCI_GETSELTEXT, 0, ( LPARAM )tag );

    if ( strlen( tag ) == 0 && !cQuickText.editing )
    {
        // Get current shortcut key (no modifiers necessary)
        ShortcutKey sk;
        SendMessage( nppData._nppHandle, NPPM_GETSHORTCUTBYCMDID,
                    ( WPARAM ) funcItems[0]._cmdID, ( LPARAM )&sk );

        if (( sk._key == VK_TAB ) || ( sk._key == VK_RETURN ) &&
              !sk._isCtrl && !sk._isAlt && !sk._isShift )
        {
            restoreKeyStroke( curPos, scintilla );
            return;
        }
    }

    // get the current langtype
    SendMessage( nppData._nppHandle, NPPM_GETCURRENTLANGTYPE, 0,
                 ( LPARAM )&langtype );
    _itoa( langtype, sLangType, 10 );
    _itoa( 255, sLangTypeGlobal, 10 );

    bool tagInCurrentLang = tags.Exists( sLangType, tag );
    bool tagInGlobalLang = tags.Exists( sLangTypeGlobal, tag );

    TagList tagList = tags.queryTags( sLangType, tag );

    //+@TonyM: add global tags list to current lang tags list (for autocompletion):
    TagList g_tagList = tags.queryTags( sLangTypeGlobal, tag );
    tagList.insert ( tagList.end(), g_tagList.begin(), g_tagList.end() );
    //+@TonyM: free memory:
    g_tagList.clear();

    // check exact tag match
    if ( tagInCurrentLang || tagInGlobalLang )
    {
        clear();
        string indent;

        // if indenting is ON
        if ( Config.indenting )
        {
            // get positions in text
            char temp[128];
            int lineNumber = static_cast<int>( SendMessage( scintilla,
                                               SCI_LINEFROMPOSITION, curPos, 0 ) );
            int startline = static_cast<int>( SendMessage( scintilla,
                                              SCI_POSITIONFROMLINE, lineNumber, 0 ) );
            // from start of line to start of tag
            SendMessage( scintilla, SCI_SETSELECTIONSTART, startline, 0 );
            SendMessage( scintilla, SCI_SETSELECTIONEND, startPos, 0 );
            SendMessage( scintilla, SCI_GETSELTEXT, 0, ( LPARAM )temp );

            indent = temp;

            // fix the indent
            unsigned i = static_cast<unsigned>( indent.find_first_not_of( " \t" ) );

            if ( i != static_cast<unsigned>( string::npos ) )
                indent.erase( i );

            // put it back
            SendMessage( scintilla, SCI_SETSELECTIONSTART, startPos, 0 );
            SendMessage( scintilla, SCI_SETSELECTIONEND, endPos, 0 );
        }

        // decode key into value
        // tags within Current lang takes priority over Global language
        if ( tagInCurrentLang )
            decodeStr( tags[sLangType][tag], startPos, indent );
        else if ( tagInGlobalLang )
            decodeStr( tags[sLangTypeGlobal][tag], startPos, indent );

        // replace it in scintilla
        SendMessage( scintilla, SCI_REPLACESEL, 0,
                     ( LPARAM )cQuickText.text.c_str() );

        SendMessage( scintilla, SCI_AUTOCCANCEL, 0, 0 );

        cQuickText.editing = true;

        cQuickText.cHotSpot = NEW_HOTSPOT;
        jump( scintilla );
    }
    // expecting to be hopping through hotspots
    else if ( cQuickText.editing )
        jump( scintilla );
    // autoComplete mode for tags
    else if ( tagList.size() > 0 )
    {
        SendMessage( scintilla, SCI_AUTOCSETSEPARATOR, WPARAM( ' ' ), 0 );
        SendMessage( scintilla, SCI_AUTOCSETTYPESEPARATOR, WPARAM('?'), 0 );
        SendMessage( scintilla, SCI_AUTOCSETIGNORECASE, true, 0 );
        SendMessage( scintilla, SCI_REGISTERIMAGE, REGIMGID, (LPARAM)xpmQt );

        // restoring original selection
        SendMessage( scintilla, SCI_SETCURRENTPOS, curPos, 0 );
        SendMessage( scintilla, SCI_SETSELECTIONSTART, curPos, ( LPARAM )true );
        SendMessage( scintilla, SCI_SETSELECTIONEND, curPos, ( LPARAM )true );

        stringstream tagList_ss;
        TagList::const_iterator tagListEnd = tagList.end();

        for ( TagList::const_iterator index = tagList.begin(); index != tagListEnd;
                index++ )
        {
            tagList_ss << *index;
            tagList_ss << '?';
            tagList_ss << REGIMGID;

            if ( ( index + 1 ) != tagListEnd )
                tagList_ss << ' ';
        }

        string newList = tagList_ss.str();

        SendMessage( scintilla, SCI_AUTOCSHOW, ( WPARAM ) strlen( tag ),
                     ( LPARAM )newList.c_str() );
    }
    else
        restoreKeyStroke( curPos, scintilla );

    // restore default settings
    SendMessage( scintilla, SCI_SETCHARSDEFAULT, 0, 0 );
}

// hopping through hotspots
void jump( HWND scintilla )
{
    if ( cQuickText.hotSpotsPos.size() != 0 )
    {
        // Browse through the hotspots
        cQuickText.cHotSpot = ( cQuickText.cHotSpot + 1 ) %
                              cQuickText.hotSpotsPos.size();
        // Select the text in the hotspot
        SendMessage( scintilla, SCI_SETANCHOR,
                     ( WPARAM )cQuickText.hotSpotsPos[cQuickText.cHotSpot], 0 );
        SendMessage( scintilla, SCI_SETCURRENTPOS,
                     ( WPARAM )cQuickText.hotSpotsPos[cQuickText.cHotSpot] +
                     cQuickText.hotSpotsLen[cQuickText.cHotSpot], 0 );
    }
    else
        cQuickText.editing = false;
}

// clear settings and etc.
void clear()
{
    cQuickText.text = "";
    cQuickText.cHotSpot = NEW_HOTSPOT;
    cQuickText.editing = false;
    cQuickText.hotSpotsPos.clear();
    cQuickText.hotSpotsLen.clear();
}

void loadConfig()
{
    tags_replica = tags; // Backup current configuration
    DialogBox( appInstance, MAKEINTRESOURCE( IDD_DLGCONFIG ),
               nppData._nppHandle, ( DLGPROC )DlgConfigProc );
    return;
}

BOOL CALLBACK DlgConfigProc( HWND hwndDlg, UINT message, WPARAM wParam,
                             LPARAM lParam )
{
    switch ( message )
    {
        case WM_INITDIALOG:
        {
            ConfigWin.add = GetDlgItem( hwndDlg, IDADD );
            ConfigWin.del = GetDlgItem( hwndDlg, IDDEL );
            ConfigWin.langCB = GetDlgItem( hwndDlg, IDLANG_CB );
            ConfigWin.tag = GetDlgItem( hwndDlg, IDTAG );
            ConfigWin.tagname = GetDlgItem( hwndDlg, IDTAGNAME );
            ConfigWin.text = GetDlgItem( hwndDlg, IDTEXT );
            ConfigWin.changed = false;

            std::string version;
            version = "<a>";
            version += VER_STRING;
            version += "</a>";
            SetDlgItemTextA( hwndDlg, IDC_STC_VER, version.c_str() );

            SendMessage( GetDlgItem( hwndDlg, IDC_CHK_AIA ), BM_SETCHECK,
                         ( WPARAM )( g_bInsertOnAutoC ? 1 : 0 ), 0 );

            int numberOfLang;
            ULongPtrToInt ( lang_menu.size(), &numberOfLang );
            for ( int i = 0; i < numberOfLang; i++ )
                SendMessageA( ConfigWin.langCB, CB_INSERTSTRING, ( WPARAM ) - 1,
                              ( LPARAM ) lang_menu.at( i ).c_str() );

            // highlight current language in Notepad++
            int langIndex = 0;
            SendMessage( nppData._nppHandle, NPPM_GETCURRENTLANGTYPE, 0,
                         ( LPARAM )&langIndex );
            SendMessage( ConfigWin.langCB, CB_SETCURSEL, langIndex, 0 );

            // ((LBN_SELCHANGE << 16) | ( IDLANG)) <-- message to pass through to IDLANG loop
            DlgConfigProc( hwndDlg, ( UINT ) WM_COMMAND,
                           ( WPARAM )( ( LBN_SELCHANGE << 16 ) | ( IDLANG ) ), 0 );
        }
        break;

        case WM_NOTIFY:
        {
            switch (((LPNMHDR)lParam)->code)
            {
                case NM_CLICK:
                case NM_RETURN:
                {
                    PNMLINK pNMLink = (PNMLINK)lParam;
                    LITEM   item    = pNMLink->item;
                    HWND ver = GetDlgItem( hwndDlg, IDC_STC_VER );

                    if ((((LPNMHDR)lParam)->hwndFrom == ver) && (item.iLink == 0))
                        ShellExecute(hwndDlg, TEXT("open"), TEXT("https://github.com/VinsWorldcom/nppQuickText"), NULL, NULL, SW_SHOWNORMAL);

                    return TRUE;
                }
            }
            break;
        }

        case WM_COMMAND:
            switch ( LOWORD( wParam ) )
            {
                case IDOK:
                {
                    BOOL exiting = true;

                    // if modified, asking to save
                    if ( ConfigWin.changed == true &&
                            MessageBox( hwndDlg, _T( "Do you want to save changes?" ), _T( "Warning" ),
                                        MB_YESNO | MB_ICONWARNING ) )
                    {
                        // add tag/substitution to DB before exiting
                        exiting = DlgConfigProc( hwndDlg, ( UINT ) WM_COMMAND,
                                                 ( WPARAM )( ( BN_CLICKED << 16 ) | ( IDADD ) ), 0 );
                    }

                    if ( exiting == TRUE )
                    {
                        tags = tags_replica; // Save changes
                        tags.WriteFile( tagsFileName.c_str(), lang_menu );
                        EndDialog( hwndDlg, wParam );
                        return TRUE;
                    }

                    return FALSE;
                }

                case IDCANCEL:
                    if ( ConfigWin.changed )
                    {
                        if ( MessageBox( hwndDlg, _T( "Are you sure you want to discard changes?" ),
                                         _T( "Warning" ), MB_YESNO | MB_ICONWARNING ) == IDYES )
                            EndDialog( hwndDlg, wParam );
                    }
                    else
                        EndDialog( hwndDlg, wParam );

                    return TRUE;

                case IDC_CHK_AIA:
                {
                    int check = ( int )::SendMessage( GetDlgItem( hwndDlg, IDC_CHK_AIA ),
                                                      BM_GETCHECK, 0, 0 );

                    if ( check & BST_CHECKED )
                        g_bInsertOnAutoC = true;
                    else
                        g_bInsertOnAutoC = false;

                    return TRUE;
                }

                case IDLANG_CB:

                //+@TonyM: Tag names list for selected language in GUI ("Options..." dialog)
                case IDLANG:
                    switch ( HIWORD( wParam ) )
                    {
                        case LBN_SELCHANGE:
                            int lang;
                            stringstream langss;

                            // Disable controls.
                            SendMessageA( ConfigWin.tagname, WM_SETTEXT, 0, ( LPARAM ) "" );
                            SendMessageA( ConfigWin.text, WM_SETTEXT, 0, ( LPARAM ) "" );
                            ConfigWin.changed = false;
                            SendMessageA( ConfigWin.add, WM_SETTEXT, 0, ( LPARAM ) "Add/Modify" );

                            // Clear the tags list.
                            SendMessage( ConfigWin.tag, LB_RESETCONTENT, 0, 0 );

                            // Get lang
                            lang = ( int ) SendMessage( ConfigWin.langCB, CB_GETCURSEL, 0, 0 );

                            //+@TonyM: to treat number 255 from QuickText.ini as GLOBAL group.
                            if ( lang == ( lang_menu.size() - 1 ) )
                                lang = 255;

                            langss << lang;

                            // Fill the tags list
                            for ( keymap::const_iterator i = tags_replica[langss.str()].begin();
                                    i != tags_replica[langss.str()].end(); i++ )
                                SendMessageA( ConfigWin.tag, LB_ADDSTRING, 0, ( LPARAM ) i->first.c_str() );

                            break;
                    }

                    return TRUE;

                //+@TonyM: Tag (snippet) content for selected tag name in GUI ("Options..." dialog)
                case IDTAG:
                {
                    switch ( HIWORD( wParam ) )
                    {
                        case LBN_SELCHANGE:
                        {
                            int tag, lang;
                            stringstream langss;
                            string quicktext;
                            char tag_s[SZ_TAG + 1];

                            // Enable controls.
                            EnableWindow( ConfigWin.tagname, TRUE );
                            EnableWindow( ConfigWin.add, TRUE );
                            EnableWindow( ConfigWin.del, TRUE );
                            EnableWindow( ConfigWin.text, TRUE );

                            // Fetch the lang and tag.
                            lang = ( int ) SendMessage( ConfigWin.langCB, CB_GETCURSEL, 0, 0 );

                            //+@TonyM: to treat number 255 from QuickText.ini as GLOBAL group.
                            if ( lang == ( lang_menu.size() - 1 ) )
                                lang = 255;

                            tag = ( int ) SendMessage( ConfigWin.tag, LB_GETCURSEL, 0, 0 );

                            // nothing selected.
                            if ( tag == LB_ERR )
                                return TRUE;

                            SendMessageA( ConfigWin.tag, LB_GETTEXT, ( WPARAM ) tag, ( LPARAM ) tag_s );
                            // Strip the text.
                            langss << lang;
                            quicktext = tags_replica[langss.str()][tag_s];
                            stripBreaks( quicktext );

                            // Insert the corresponding tagname and text.
                            SendMessageA( ConfigWin.tagname, WM_SETTEXT, 0, ( LPARAM ) tag_s );
                            SendMessageA( ConfigWin.text, WM_SETTEXT, 0, ( LPARAM ) quicktext.c_str() );
                            ConfigWin.changed = false;
                            SendMessageA( ConfigWin.add, WM_SETTEXT, 0, ( LPARAM ) "Add/Modify" );

                            break;
                        }
                    }
                    return TRUE;
                }

                case IDTEXT:
                {
                    switch ( HIWORD( wParam ) )
                    {
                        case EN_CHANGE:
                            SendMessageA( ConfigWin.add, WM_SETTEXT, 0, ( LPARAM ) "&Modify" );
                            ConfigWin.changed = true;
                            break;
                    }

                    return TRUE;
                }
                
                case IDTAGNAME:
                {
                    switch ( HIWORD( wParam ) )
                    {
                        case EN_CHANGE:
                            SendMessageA( ConfigWin.add, WM_SETTEXT, 0, ( LPARAM ) "&Add" );
                            ConfigWin.changed = true;
                            break;

                        case EN_SETFOCUS:
                            SendMessage( ConfigWin.tag, LB_SETCURSEL , ( WPARAM ) -1, ( LPARAM ) NULL );
                            SendMessageA( ConfigWin.tagname, WM_SETTEXT, 0, ( LPARAM ) "" );
                            SendMessageA( ConfigWin.text, WM_SETTEXT, 0, ( LPARAM ) "" );
                            break;
                    }

                    return TRUE;
                }

                case IDADD:
                {
                    switch ( HIWORD( wParam ) )
                    {
                        case BN_CLICKED:
                            int tag_Listbox_Index, language, newtag_index;
                            char oldtag[SZ_TAG + 1];
                            char newtag[SZ_TAG + 1];
                            DWORD_PTR textLength = SendMessageA( ConfigWin.text, WM_GETTEXTLENGTH, 0,
                                                                 0 ) + 1;
                            string substitutionTxt_s;
                            stringstream lang_ss;
                            char *substitutionTxt_ptr = NULL;

                            // validating substitution text
                            if ( textLength == 1 )
                            {
                                MessageBox( hwndDlg,
                                            _T( "Invalid Substitution Text! Please check all input fields." ),
                                            _T( "Warning" ), MB_OK | MB_ICONWARNING );
                                return FALSE;
                            }

                            // get the old and new tagnames.
                            tag_Listbox_Index = ( int ) SendMessage( ConfigWin.tag, LB_GETCURSEL, 0,
                                                0 );
                            SendMessageA( ConfigWin.tag, LB_GETTEXT, ( WPARAM ) tag_Listbox_Index,
                                          ( LPARAM ) oldtag );
                            SendMessageA( ConfigWin.tagname, WM_GETTEXT, ( WPARAM ) SZ_TAG,
                                          ( LPARAM ) newtag );

                            // validating tag
                            if ( strlen( newtag ) == 0 )
                            {
                                MessageBox( hwndDlg,
                                            _T( "Invalid Tag Name! Please check all input fields." ), _T( "Warning" ),
                                            MB_OK | MB_ICONWARNING );
                                return FALSE;
                            }

                            // dynamically allocating space for substitution text
                            substitutionTxt_ptr = ( char * )malloc( ( sizeof( char ) * ( (
                                    size_t )textLength ) ) );

                            // get language
                            language = ( int ) SendMessage( ConfigWin.langCB, CB_GETCURSEL, 0, 0 );

                            //+@VinsWorldcom: to treat number 255 from QuickText.ini as GLOBAL group.
                            // https://github.com/vinsworldcom/nppQuickText/issues/10
                            if ( language == ( lang_menu.size() - 1 ) )
                                language = 255;

                            // get substitution text
                            SendMessageA( ConfigWin.text, WM_GETTEXT, ( WPARAM ) textLength,
                                          ( LPARAM ) substitutionTxt_ptr );
                            substitutionTxt_s = substitutionTxt_ptr;

                            // garbage collecting
                            free( substitutionTxt_ptr );

                            revStripBreaks( substitutionTxt_s );

                            // Replace the values
                            lang_ss << language;

                            if ( strcmp( oldtag, newtag ) != 0 )
                                tags_replica.DeleteKey( lang_ss.str(), oldtag );

                            tags_replica[lang_ss.str()][newtag] = substitutionTxt_s;

                            // Refresh
                            SendMessage( ConfigWin.tag, LB_RESETCONTENT, 0, 0 );

                            for ( keymap::const_iterator i = tags_replica[lang_ss.str()].begin();
                                    i != tags_replica[lang_ss.str()].end(); i++ )
                                SendMessageA( ConfigWin.tag, LB_ADDSTRING, 0, ( LPARAM ) i->first.c_str() );

                            newtag_index = static_cast<int>( SendMessageA( ConfigWin.tag, LB_FINDSTRING,
                                                             ( WPARAM ) - 1, ( LPARAM ) newtag ) );
                            SendMessage( ConfigWin.tag, LB_SETCURSEL, ( WPARAM ) newtag_index, 0 );

                            SendMessageA( ConfigWin.add, WM_SETTEXT, 0, ( LPARAM ) "Add/Modify" );

                            break;
                    }

                    return TRUE;
                }

                case IDDEL:
                    switch ( HIWORD( wParam ) )
                    {
                        case BN_CLICKED:
                            int selectedTag, lang;
                            char tagname[SZ_TAG + 1];
                            stringstream lang_ss;
                            string msg;

                            // Fetch the tag name index
                            selectedTag = ( int ) SendMessage( ConfigWin.tag, LB_GETCURSEL, 0, 0 );

                            // validating selection
                            if ( selectedTag == LB_ERR )
                            {
                                MessageBox( hwndDlg, _T( "No tag name is selected." ), _T( "Warning" ),
                                            MB_OK | MB_ICONWARNING );
                                return TRUE;
                            }

                            SendMessageA( ConfigWin.tag, LB_GETTEXT, ( WPARAM ) selectedTag,
                                          ( LPARAM ) tagname );

                            // Fetch the lang
                            lang = ( int ) SendMessage( ConfigWin.langCB, CB_GETCURSEL, 0, 0 );

                            //+@VinsWorldcom: to treat number 255 from QuickText.ini as GLOBAL group.
                            // https://github.com/vinsworldcom/nppQuickText/issues/10
                            if ( lang == ( lang_menu.size() - 1 ) )
                                lang = 255;

                            msg = "Are you sure you want to delete the tag ";
                            msg.append( tagname );
                            msg.push_back( '?' );

                            if ( MessageBoxA( hwndDlg, msg.c_str(), "Warning",
                                              MB_YESNO | MB_ICONQUESTION ) == IDYES )
                            {
                                // Delete it
                                lang_ss << lang;
                                tags_replica.DeleteKey( lang_ss.str(), tagname );

                                // Refresh
                                SendMessage( ConfigWin.tag, LB_RESETCONTENT, 0, 0 );

                                for ( keymap::const_iterator i = tags_replica[lang_ss.str()].begin();
                                        i != tags_replica[lang_ss.str()].end(); i++ )
                                    SendMessageA( ConfigWin.tag, LB_ADDSTRING, 0, ( LPARAM ) i->first.c_str() );

                                SendMessageA( ConfigWin.tagname, WM_SETTEXT, 0, ( LPARAM ) "" );
                                SendMessageA( ConfigWin.text, WM_SETTEXT, 0, ( LPARAM ) "" );
                                ConfigWin.changed = false;
                                SendMessageA( ConfigWin.add, WM_SETTEXT, 0, ( LPARAM ) "Add/Modify" );
                            }

                            break;
                    }

                    return TRUE;
            }
    }

    return FALSE;
}
// if tag doesn't exist, a tab should be outputted
bool restoreKeyStroke( int cursorPos, HWND scintilla )
{
    // restoring original selection
    SendMessage( scintilla, SCI_SETEMPTYSELECTION, cursorPos, 0 );

    // Get current shortcut key (no modifiers necessary)
    ShortcutKey sk;
    SendMessage( nppData._nppHandle, NPPM_GETSHORTCUTBYCMDID,
                 ( WPARAM ) funcItems[0]._cmdID, ( LPARAM )&sk );

    if ( ( sk._key == VK_TAB ) &&
          !sk._isCtrl && !sk._isAlt && !sk._isShift )
        SendMessage( scintilla, SCI_TAB, 0, 0 );

    return true;
}
