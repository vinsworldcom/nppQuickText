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

#ifndef QUICKTEXT_H
    #define QUICKTEXT_H
    #include "QuickText.h"
#endif // QUICKTEXT_H

#include <intsafe.h>

#define NEW_HOTSPOT -1
#define SZ_LANG 32
#define SZ_TAG 32
#define SZ_TEXT 512

// *** Plugin specific variables
const TCHAR PLUGIN_NAME[] = _T( "QuickText" ); // Nome do plugin
//+@TonyM: nbFunc = 2 -> nbFunc = 5;
const int nbFunc = 5; // number of functions
const std::string
LANGUAGES ( "TXT,PHP,C,CPP,CS,OBJC,JAVA,RC,HTML,XML,MAKEFILE,PASCAL,BATCH,INI,NFO,USER,ASP,SQL,VB,JS,CSS,PERL,PYTHON,LUA,TEX,FORTRAN,BASH,FLASH,NSIS,TCL,LISP,SCHEME,ASM,DIFF,PROPS,PS,RUBY,SMALLTALK,VHDL,KIX,AU3,CAML,ADA,VERILOG,MATLAB,HASKELL,INNO,SEARCHRESULT,CMAKE,YAML,EXTERNAL,GLOBAL" );

NppData nppData; // handles
FuncItem funcItems[nbFunc];

//+@TonyM: added some characters (._-). more characters I've added, more errors occure.
std::string allowedChars =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890._-";
HFONT verdanaFont = CreateFont ( 15, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                 ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                 DEFAULT_PITCH | FF_SWISS, _T( "Verdana" ) );
//+@TonyM: string lang_menu[] -> vector<string> lang_menu(256) - for dynamic loading from configuration file.
vector<string> lang_menu( 256 );
//string lang_menu[] =  {"TXT", "PHP ", "C", "CPP", "CS", "OBJC", "JAVA", "RC","HTML", "XML", "MAKEFILE", "PASCAL", "BATCH", "INI", "NFO", "USER",\
//                      "ASP", "SQL", "VB", "JS", "CSS", "PERL", "PYTHON", "LUA","TEX", "FORTRAN", "BASH", "FLASH", "NSIS", "TCL", "LISP", "SCHEME",\
//                      "ASM", "DIFF", "PROPS", "PS", "RUBY", "SMALLTALK", "VHDL", "KIX", "AU3","CAML", "ADA", "VERILOG", "MATLAB", "HASKELL",\
//                      "INNO", "SEARCHRESULT","CMAKE", "YAML","EXTERNAL","GLOBAL"};

// workaround for language listed in QuickText to match index of LangType
//+@TonyM: Unused - GLOBAL group has number "255" now.
int IDM_LANG_GLOBAL = 51; // sizeof(LangType) + 1


// *** Main
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  reasonForCall,
                       LPVOID /* lpReserved */ )
{
    switch ( reasonForCall )
    {
        case DLL_PROCESS_ATTACH:
        {
            // ConfigFile path
            TCHAR temp[256];
            GetModuleFileName( ( HMODULE )hModule, temp, sizeof( temp ) );
            tagsFileName = temp;
            unsigned int pos;
            pos = static_cast<unsigned int>( tagsFileName.rfind( _T( "\\" ) ) );
            tagsFileName.erase( pos );
            pos = static_cast<unsigned int>( tagsFileName.rfind( _T( "\\" ) ) );
            tagsFileName.erase( pos );

            tagsFileName_Old = tagsFileName + _T( "\\QuickText.ini" );
            config_dir = tagsFileName + _T( "\\plugins\\Config" );
            tagsFileName.append( _T( "\\plugins\\Config\\QuickText.ini" ) );

            // funcItems setting
            funcItems[0]._pFunc = QuickText;
            lstrcpy( funcItems[0]._itemName, _T( "Replace Tag" ) );
            funcItems[0]._init2Check = false;
            funcItems[0]._pShKey = new ShortcutKey;
            funcItems[0]._pShKey->_isAlt = false;
            funcItems[0]._pShKey->_isCtrl = true;
            funcItems[0]._pShKey->_isShift = false;
            funcItems[0]._pShKey->_key = VK_RETURN;

            funcItems[1]._pFunc = loadConfig;
            lstrcpy( funcItems[1]._itemName, _T( "Options..." ) );
            funcItems[1]._init2Check = false;
            funcItems[1]._pShKey = NULL;

            //+@TonyM: Added 3 new plugin menu commands
            funcItems[2]._pFunc = refreshINIMap;
            lstrcpy( funcItems[2]._itemName, _T( "Refresh Configuration" ) );
            funcItems[2]._init2Check = false;
            funcItems[2]._pShKey = new ShortcutKey;
            funcItems[2]._pShKey->_isAlt = false;
            funcItems[2]._pShKey->_isCtrl = true;
            funcItems[2]._pShKey->_isShift = true;
            funcItems[2]._pShKey->_key = VK_F5;

            funcItems[3]._pFunc = openTagsFile;
            lstrcpy( funcItems[3]._itemName, _T( "Open Tags File" ) );
            funcItems[3]._init2Check = false;

            funcItems[4]._pFunc = openConfigFile;
            lstrcpy( funcItems[4]._itemName, _T( "Open Config File" ) );
            funcItems[4]._init2Check = false;

            Config.indenting = true;

            cQuickText.editing = false;
            //+@TonyM: refreshINIMap() -> _refreshINIFiles() - to skip displaying
            //+@TonyM: MessageBox with confirmation of INI files reload on the start of Notepad++.
            _refreshINIFiles();

            appInstance = ( HINSTANCE ) hModule;
        }
        break;

        case DLL_PROCESS_DETACH:
            delete funcItems[0]._pShKey;
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;
    }

    return TRUE;
}

// Notepad++ - handles
extern "C" __declspec( dllexport ) void setInfo( NppData notpadPlusData )
{
    nppData = notpadPlusData;
}

// Notepad++ - nome do plugin
extern "C" __declspec( dllexport ) const TCHAR *getName()
{
    return PLUGIN_NAME;
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

            break;
        }

        // check if outside a hotspot
        case SCN_UPDATEUI:
        {
            int currpos;
            currpos = static_cast<int>( SendMessage( getCurrentHScintilla(),
                                        SCI_GETCURRENTPOS, 0, 0 ) );
            bool r = true;

            for ( unsigned int i = 0; i < cQuickText.hotSpotsPos.size(); i++ )
                if ( currpos >= cQuickText.hotSpotsPos[i]
                        && currpos <= cQuickText.hotSpotsPos[i] + cQuickText.hotSpotsLen[i] )
                    r = false;

            if ( r )
                clear();
        }
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
HWND &getCurrentHScintilla()
{
    int currentEdit;
    SendMessage( nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0,
                 ( LPARAM )&currentEdit );
    return ( currentEdit == 0 ) ? nppData._scintillaMainHandle :
           nppData._scintillaSecondHandle;
}


// Checks if a string is a valid QuickText key
bool isValidKey( const char *key )
{
    string skey = key;

    if ( skey.find_first_not_of( allowedChars ) != static_cast<unsigned>(string::npos) )
        return false;

    return true;
}


//+@TonyM: Used without wrapper (refreshINIMap()) to skip displaying
//+@TonyM: MessageBox with confirmation of INI files reload on the start of Notepad++.
void _refreshINIFiles()
{
    tags.Clear();
    tags.ReadFile( tagsFileName.c_str(), tagsFileName_Old.c_str() );

    std::string ini_file_path       = QTString::ws2s( config_dir ) +
                                      "\\QuickText.conf.ini";
    std::string ini_file_section    = "general";

    //+@TonyM: Reads allowedChars value from config file on each config refresh
    std::string ini_allowedChars = CIniFile::GetValue( "allowedChars",
                                   ini_file_section, ini_file_path );

    if ( !ini_allowedChars.empty() )
        allowedChars = ini_allowedChars;

    ;

    //+@TonyM: Reads lang_menu value from config file on each config refresh
    std::string ini_lang_menu = CIniFile::GetValue( "lang_menu",
                                ini_file_section, ini_file_path );

    if ( ini_lang_menu.empty() && lang_menu.empty() )
        lang_menu = QTString::vexplode( ",", LANGUAGES, false );
    else if ( !ini_lang_menu.empty() )
    {
        lang_menu = QTString::vexplode( ",", ini_lang_menu, true );
        lang_menu.push_back( "GLOBAL" );
    };

    //+#DEBUG
    /*
    stringstream ss;
    ss << lang_menu.at(1).size();
    //+@TonyM: message box for config file testing (INI File parsing library)
    std::wstring stemp = QTString::s2ws(ss.str()); // Temporary buffer is required
    LPCWSTR result = stemp.c_str();
    MessageBox(nppData._nppHandle, result, _T("QuickText"), MB_OK | MB_ICONINFORMATION);
    */
    //-#DEBUG
}


// Clears and loads again the INI file
void refreshINIMap()
{
    _refreshINIFiles();
    MessageBox( nppData._nppHandle,
                _T( "QuickText.ini and QuickText.conf.ini files reloaded!" ),
                _T( "QuickText" ), MB_OK | MB_ICONINFORMATION );
}

void openConfigFile()
{
    basic_string<TCHAR> file_path = config_dir + _T( "\\QuickText.conf.ini" );
    SendMessage( nppData._nppHandle, NPPM_DOOPEN, 0,
                 ( LPARAM )file_path.c_str() );
}

void openTagsFile()
{
    basic_string<TCHAR> file_path = config_dir + _T( "\\QuickText.ini" );
    SendMessage( nppData._nppHandle, NPPM_DOOPEN, 0,
                 ( LPARAM )file_path.c_str() );
}

// Strip all the line breaks
void stripBreaks( string &str, cstring &indent = "" )
{
    // Get line break chars
    //+@TonyM: incorrect newline detection
    //-@TonyM: HWND scintilla = getCurrentHScintilla();
    //-@TonyM: int mode = static_cast<int>(SendMessage(scintilla, SCI_GETEOLMODE, 0, 0));
    char newline[3] = "\r\n";
    //+@TonyM: incorrect newline detection
    /*-@TonyM:
    switch (mode)
    {
        case SC_EOL_CRLF:
            strcpy(newline, "\r\n");
            break;
        case SC_EOL_CR:
            strcpy(newline, "\r");
            break;
        case SC_EOL_LF:
            strcpy(newline, "\n");
            break;
    }*/

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

    while ( ( i = static_cast<unsigned>( str.find( "\r\n" ) ) ) != static_cast<unsigned>(str.npos) )
    {
        str.erase( i, 2 );
        str.insert( i, "\\n" );
    }
}

// set  cQuickText.text to substitution texts
// setup hotspots hopping
void decodeStr( cstring &str, int start, string &indent )
{
    cQuickText.text = str;
    stripBreaks( cQuickText.text, indent );

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

// QuickTxt function
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

    // text selected, restore default key behavior
    //+@TonyM: Not needed if not using the tab shortcut key.
    //+@TonyM: Explanation: If text is selected and we press the tab key, the tab space shoud appear,
    //+@TonyM: but if we set another shortcut combo, then it behaves like a tab, which is annoying with placeholders.
    /*-@TonyM:
    if(textSelectionEnd != textSelectionStart)
    {
        // restoring tab functionality
        if (funcItems[0]._pShKey->_key == VK_TAB)
            SendMessage(scintilla,SCI_TAB,0,0);

        return;
    }
    */
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

    // validate the key
    //+@TonyM: to popup autocompletion list without first letter
    /*-@TonyM
    if (strlen(tag) == 0 && !cQuickText.editing)
    {
        // if key is invalid
        restoreKeyStroke(curPos,scintilla);
        return;
    }
    */

//  if (!isValidKey(tag) && !cQuickText.editing) {
//      MessageBox(nppData._nppHandle, _T("Only alphanumerical characters."), _T("QuickText"), MB_OK | MB_ICONINFORMATION);
//      return;
//  }

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
        {
            //MessageBox(nppData._nppHandle, _T("current tag"), _T("QuickText"), MB_OK | MB_ICONINFORMATION);
            decodeStr( tags[sLangType][tag], startPos, indent );
        }
        else if ( tagInGlobalLang )
        {
            //MessageBox(nppData._nppHandle, _T("global tag"), _T("QuickText"), MB_OK | MB_ICONINFORMATION);
            decodeStr( tags[sLangTypeGlobal][tag], startPos, indent );
        }

        // replace it in scintilla
        SendMessage( scintilla, SCI_REPLACESEL, 0,
                     ( LPARAM )cQuickText.text.c_str() );

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
        SendMessage( scintilla, SCI_AUTOCSETSEPARATOR, WPARAM( '\n' ), 0 );
        SendMessage( scintilla, SCI_AUTOCSETIGNORECASE, true, 0 );

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

            if ( ( index + 1 ) != tagListEnd )
                tagList_ss << '\n';
        }

        string newList = tagList_ss.str();

        // need to build custom ListBox

        //+@TonyM: (WPARAM) 1 -> (WPARAM) strlen(tag)
        SendMessage( scintilla, SCI_AUTOCSHOW, ( WPARAM ) strlen( tag ),
                     ( LPARAM )newList.c_str() );
        //int curSEl =  static_cast<int>(SendMessage(scintilla, SCI_AUTOCGETCURRENT, 0, 0));
    }
    else
        restoreKeyStroke( curPos, scintilla );

    // restore default settings
    SendMessage( scintilla, SCI_SETCHARSDEFAULT, 0, 0 );
}

// hopping through hotspots
void jump( HWND &scintilla )
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
                             LPARAM /* lParam */ )
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

            // LangType contains 51 languages as of now.
            // dynamic way to grab language names?

            //int numberOfLang = sizeof(lang_menu)/sizeof(string);
            int numberOfLang;
            ULongPtrToInt ( lang_menu.size(), &numberOfLang );

            // adding language to QuickText window
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
                        tags.WriteFile( tagsFileName.c_str() );
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
                            //lang = (int) SendMessage(ConfigWin.lang, LB_GETCURSEL, 0, 0);
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
                            //lang = (int) SendMessage(ConfigWin.lang, LB_GETCURSEL, 0, 0);
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
                case IDTAGNAME:
                {
                    switch ( HIWORD( wParam ) )
                    {
                        case EN_CHANGE:
                            SendMessageA( ConfigWin.add, WM_SETTEXT, 0, ( LPARAM ) "Add/Modify [M]" );
                            ConfigWin.changed = true;
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
                            //language = (int) SendMessage(ConfigWin.lang, LB_GETCURSEL,0,0);
                            language = ( int ) SendMessage( ConfigWin.langCB, CB_GETCURSEL, 0, 0 );

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
                            //lang = (int) SendMessage(ConfigWin.lang, LB_GETCURSEL, 0, 0);
                            lang = ( int ) SendMessage( ConfigWin.langCB, CB_GETCURSEL, 0, 0 );

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
bool restoreKeyStroke( int cursorPos, HWND &scintilla )
{
    if ( funcItems[0]._pShKey->_isAlt == false &&
            funcItems[0]._pShKey->_isCtrl == false &&
            funcItems[0]._pShKey->_isShift == false &&
            funcItems[0]._pShKey->_key == VK_TAB
       )
    {
        // restoring original selection
        SendMessage( scintilla, SCI_SETCURRENTPOS, cursorPos, 0 );
        SendMessage( scintilla, SCI_SETSELECTIONSTART, cursorPos, ( LPARAM )true );
        SendMessage( scintilla, SCI_SETSELECTIONEND, cursorPos, ( LPARAM )true );

        //+@TonyM: No tab, because I'll use ctr+enter (add key shortcut detection)
        //+@TonyM: Apparently there is: funcItems[0]._pShKey->_key == VK_TAB
        //+@TonyM: But doesn't work correctly
        //-@TonyM: SendMessage(scintilla,SCI_TAB,0,0);
        return true;
    }

    return false;
}

void SmartEdit()
{
    return;
}