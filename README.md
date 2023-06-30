# QuickText

    QuickText - Quick editing tags for Notepad++
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

    Original Link:    http://sourceforge.net/projects/quicktext/
    Original Author:  Joao Moreno <alph.pt@gmail.com>, Jing Teng <jingbeta@gmail.com>
    Original Version: 0.0.2.1

Based on Version: 0.0.2.2

    Link:         https://sourceforge.net/p/quicktext/patches/4/
    Author:       TonyM

## Description

QuickText is a Notepad++ plugin for quick text substitution, including
multi field inputs. It's similar to Tab Triggers in TextMate.

## Build

I compiled with MS Visual Studio Community 2017 and this seems to work OK.

For 32-bit:

    [x86 Native Tools Command Prompt for VS 2017]

```
    C:\> set Configuration=Release
    C:\> set Platform=x86
    C:\> msbuild
```

For 64-bit:

    [x64 Native Tools Command Prompt for VS 2017]

```
    C:\> set Configuration=Release
    C:\> set Platform=x64
    C:\> msbuild
```

## Installation

QuickText 0.2.6.1 and newer requires Notepad++ 8.4.6 or newer.

Copy the:
+ 32-bit: ./Release/Win32/QuickText.dll
+ 64-bit: ./Release/x64/QuickText.dll

to the Notepad++ plugins folder:
+ In N++ <7.6, directly in the plugins/ folder
+ In N++ >=7.6, in a directory called QuickText in the plugins/ folder (plugins/QuickText/)

**NOTE:**  The original version put config files in weird places and with
           no updates since the Notepad++ 7.6 new plugins architecture, it's
           gotten worse.  This version corrects this.  You'll need to put
           the following files (or your existing versions) in their 
           respective directory locations:

    Config/QuickText.conf.default.ini = NPP_INSTALL_DIR/plugins/Config/QuickText.conf.ini
    Config/QuickText.default.ini      = NPP_INSTALL_DIR/plugins/Config/QuickText.ini

## Usage

Assign a shortcut key to the Plugins => QuickText => Replace Snip menu to 
use QuickText snips from within the current Notepad++ document.

If the "Replace Snip" hotkey is used and there is no text before it or
no valid expansion for the text before it, the "Replace Snip" hotkey
character is inserted.  For example, using `Tab` as the key with
no valid expansion just inserts a tab (as if `Tab` key press).

Once a snip is inserted, you can use the "Replace Snip" hotkey again and 
again to navigate through the hotspots in the inserted snippet.

**NOTE:**  `Tab` can now be used again as the "Replace Snip" hotkey.
           There is no default.

## Customization

Settings:

Use to configure Snips for languages.
+ **Use Scintilla Autocomplete**: uses Scintilla autocomplete while typing to 
  suggest snips for the current language.  Without this enabled, you need to 
  trigger the "Replace Snip" hotkey to get the autocomplete list or just type 
  a snip from memory.
+ **Auto Insert Autocomplete**: when a snip is completed from a Scintilla 
  autocompletion, expand the snip.  Without this enabled, you need to trigger 
  the "Replace Snip" hotkey again to expand the snip.
+ **Convert Tabs (N++ setting)**: convert any `Tab` characters in the 
  "Substitution Text" to spaces based on Notepad++ settings for the current 
  language.  If Notepad++ does not replace tabs for the current language, this 
  setting respects that.  If Notepad++ does replace tabs with a given number 
  of spaces, that number of spaces is used in the replacement.
+ **Use Fixed Font**: use a fixed-width font (Courier New) in the 
  "Substitution Text" text box.
+ **Use N++ Colors**: use Notepad++ background and "Default Style" text colors 
  in the "Snips" and "Substitution Text" list and edit boxes.
+ **Confirm before close**: use message box prompts to confirm actions before 
  closing the Settings dialog with the "OK" and "Cancel" buttons if potential 
  changes.

To make you're own snips:

+ Snips file (QuickText.ini) *MUST* use "Windows (CR LF)" line endings.
+ First make sure the snip's Language Section already exists. This is 
  done by creating a new section with the code corresponding to the 
  Language.  See LANGUAGE CODES.
+ Then, for the key of the snip, use only lower/upper case and numbers.
+ Special chars:
  + `$` hotspots
  + `\$` for literal '$'
  + `\n` for line break
  + `\\n` for literal '\n'

When using the Settings "Substitution Text" GUI:
  + `Ctrl+Tab` for literal tab
  + `$` hotspots
  + `\$` for literal '$'
  + `Enter` for line break
  + `\n` for literal '\n'

## Example

8 is the Language Code for HTML

    [8]
    link=<a href="$">$</a>

---

## Language Codes

```
0    Normal text
1    PHP
2    C
3    C++
4    C#
5    Objective-C
6    Java
7    RC
8    HTML
9    XML
10   Makefile
11   Pascal
12   Batch
13   ini
14   NFO
15   (User Defined Language Type)
16   ASP
17   SQL
18   Visual Basic
19   JavaScript  (Do not use for JavaScript, use 58 instead)
20   CSS
21   Perl
22   Python
23   Lua
24   TeX
25   Fortran free form
26   Shell
27   ActionScript
28   NSIS
29   TCL
30   Lisp
31   Scheme
32   Assembly
33   Diff
34   Properties file
35   PostScript
36   Ruby
37   Smalltalk
38   VHDL
39   KiXtart
40   AutoIt
41   CAML
42   Ada
43   Verilog
44   MATLAB
45   Haskell
46   Inno Setup
47   Internal Search
48   CMake
49   YAML
50   COBOL
51   Gui4Cli
52   D
53   PowerShell
54   R
55   JSP
56   CoffeeScript
57   json
58   JavaScript
59   Fortran fixed form
60   BaanC
61   S-Record
62   Intel HEX
63   Tektronix extended HEX
64   Swift
65   ASN.1
66   AviSynth
67   BlitzBasic
68   PureBasic
69   FreeBasic
70   Csound
71   Erlang
72   ESCRIPT
73   Forth
74   LaTeX
75   MMIXAL
76   Nim
77   Nncrontab
78   OScript
79   REBOL
80   registry
81   Rust
82   Spice
83   txt2tags
84   Visual Prolog
85   TypeScript
86   json5
87   mssql
88   GDScript
89   Hollywood
90   External

255  GLOBAL
```
