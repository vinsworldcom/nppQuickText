------------------------------------------------------------------------------------------------------------------------
	DESCRIPTION
========================================================================================================================
I've downloaded source code (rev. 5) of QuickText from SF SVN server. It seems, that Joăo M. (or pwner4once) made some
changes since v0.2.1 and correct some errors described in
this (http://sourceforge.net/projects/notepad-plus/forums/forum/482781/topic/3659788) topic, but unfortunately make
some more bugs.

Most of them are related to using default shortcut key – "Tab", he made many hacks in code, to make “Tab” usable as
QuickText default shortcut key and as "standard Tab".
I really don't understand why is he doing this – other shortcut keys (like earlier default ctrl + ENTER)
are probably suitable for most users.

Coming to the point, I'd like to say that I made my build and want to share it with you.

------------------------------------------------------------------------------------------------------------------------
	FEATURES
========================================================================================================================
	All changes in custom build are mentioned in ./CHANGELOG.txt
	Main additional features are:
		+ Working snippets (tags) auto-completion
		+ Fixed Hot-spots
		+ Configurable "allowed characters" in snippets (tags) names (allowedChars parameter in QuickText.conf.ini).
	
------------------------------------------------------------------------------------------------------------------------
	FOR ALL OF YOU, THAT WANT TO SIMULATE ZEN-CODING USING THIS PLUGIN
========================================================================================================================
	This plugin use simple replace, there is no parsing rules and probably never intended to have any parser
	(but who knows).
	Better choice for Zen-Coding is Zen-Coding plugin :p,
	which you can download here:
		http://code.google.com/p/zen-coding/downloads/list
		
------------------------------------------------------------------------------------------------------------------------
	FILES
========================================================================================================================
		- QuickText.dll
		+ Config
			- QuickText.default.ini			- default tags file witch some sample snippets
			- QuickText.conf.default.ini	- default configuration file
		+ Doc
			+ TonyM
				- README.txt 				- readme file for my customized build (this file)
				- CHANGELOG.txt				- changelog for my customized build
			- README						- original readme file.
			- CHANGELOG						- original changelog file.
			
------------------------------------------------------------------------------------------------------------------------
	INSTALLATION
========================================================================================================================
	Just copy all unpacked binary package into Notepad++'s plugin directory.
	Replace (or merge if you have already modified this files):
		QuickText.conf.default.ini -> QuickText.conf.ini
		QuickText.default.ini -> QuickText.ini
------------------------------------------------------------------------------------------------------------------------		
	USAGE
========================================================================================================================
	Use shortcut key combo (ctrl + ENTER) (or other defined in shortcut mapper) to use replace QuickText tags (snippets).

------------------------------------------------------------------------------------------------------------------------
	LANGUAGE CODES (used in QuickText.ini)
========================================================================================================================
0	TEXT
1	PHP
2	C
3	CPP
4	CS
5	OBJC
6	JAVA
7	RC
8	HTML
9	XML
10	MAKEFILE
11	PASCAL
12	BATCH
13	INI
14	ASCII
15	USER
16	ASP
17	SQL
18	VB
19	JS
20	CSS
21	PERL
22	PYTHON
23	LUA
24	TEX
25	FORTRAN
26	SH
27	FLASH
28	NSIS
29	TCL
30	LISP
31	SCHEME
32	ASM
33	DIFF
34	PROPS
35	PS
36	RUBY
37	SMALLTALK
38	VHDL
39	KIX
40	AU3
41	CAML
42	ADA
43	VERILOG
44	MATLAB
45	HASKELL
46	INNO
47	SEARCH_RESULT
48	CMAKE
49	YAML
50	EXTERNAL

255	GLOBAL