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

DESCRIPTION

    QuickText is a Notepad++ plugin for quick text substitution, including 
    multi field inputs. It's similar to Tab Triggers in TextMate.

BUILD

    I compiled with MS Visual Studio Community 2017 and this seems to work 
    OK.
    
    For 32-bit:
      [x86 Native Tools Command Prompt for VS 2017]
      C:\> set Configuration=Release
      C:\> set Platform=x86
      C:\> msbuild
    
    For 64-bit:
      [x64 Native Tools Command Prompt for VS 2017]
      C:\> set Configuration=Release
      C:\> set Platform=x64
      C:\> msbuild

INSTALLATION

    Copy the:
    
    32-bit:
        ./bin/QuickText.dll
       
    64-bit:
        ./bin64/QuickText.dll

    to the Notepad++ plugins folder:
      - In N++ <7.6, directly in the plugins/ folder
      - In N++ >=7.6, in a directory called QuickText in the plugins/ folder
        (plugins/QuickText/)
    
    NOTE:  The original version put config files in weird places and with 
           no updated since the Notepad++ 7.6 new plugins architecture, it's 
           gotten worse.  This version corrects this.  You'll need to put 
           the following files in their respective directory locations:
           
           QuickText.conf.ini = NPP_INSTALL_DIR/plugins/Config
           QuickText.ini      = NPP_INSTALL_DIR/plugins/QuickText

USAGE

    Use the key shortcut to use QuickText tags from within the current 
    Notepad++ document.
    
CUSTOMIZATION

    To make you're own tags:
        - First make sure the tag's Language Section already exists. This is done by
        creating a new section with the code corresponding to the Language.
        (See LANGUAGE CODES).
        - Then, for the key of the tag, use only lower/upper case and numbers.
        - Special chars:
            - $ hotspots
            - \$ for writing actual '$'
            - \n break line.

    Or just use the Options GUI. (v0.2) :)

EXAMPLE

 *** (8 is the Language Code for HTML)

[8]  
link=<a href="$">$</a>

LANGUAGE CODES

0   TEXT
1   PHP
2   C
3   CPP
4   CS
5   OBJC
6   JAVA
7   RC
8   HTML
9   XML
10  MAKEFILE
11  PASCAL
12  BATCH
13  INI
14  ASCII
15  USER
16  ASP
17  SQL
18  VB
19  JS
20  CSS
21  PERL
22  PYTHON
23  LUA
24  TEX
25  FORTRAN
26  SH
27  FLASH
28  NSIS
29  TCL
30  LISP
31  SCHEME
32  ASM
33  DIFF
34  PROPS
35  PS
36  RUBY
37  SMALLTALK
38  VHDL
39  KIX
40  AU3
41  CAML
42  ADA
43  VERILOG
44  MATLAB
45  HASKELL
46  INNO
47  SEARCH_RESULT
48  CMAKE
49  YAML
50  EXTERNAL
51  GLOBAL
