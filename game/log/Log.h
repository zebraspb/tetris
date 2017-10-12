#ifndef __LOG_H__
#define __LOG_H__

#include <iostream>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define _func_context_ __FILE__ "(" TOSTRING(__LINE__) "), " << __FUNCTION__

	#define clr_null      "\033[m"
	#define clr_black     "\033[30m"
	#define clr_red       "\033[31m"
	#define clr_green     "\033[32m"
	#define clr_brown     "\033[33m"
	#define clr_blue      "\033[34m"
	#define clr_magenta   "\033[35m"
	#define clr_cyan      "\033[36m"
	#define clr_ltgray    "\033[37m"
	#define clr_dkgray    "\033[1;30m"
	#define clr_ltred     "\033[1;31m"
	#define clr_ltgreen   "\033[1;32m"
	#define clr_yellow    "\033[1;33m"
	#define clr_ltblue    "\033[1;34m"
	#define clr_purple    "\033[1;35m"
	#define clr_skyblue   "\033[1;36m"
	#define clr_white     "\033[1;37m"

	#define bgclr_black   "\033[40m"
	#define bgclr_red     "\033[41m"
	#define bgclr_green   "\033[42m"
	#define bgclr_brown   "\033[43m"
	#define bgclr_blue    "\033[44m"
	#define bgclr_magenta "\033[45m"
	#define bgclr_cyan    "\033[46m"
	#define bgclr_ltgray  "\033[47m"
	#define bgclr_dkgray  "\033[1;40m"
	#define bgclr_ltred   "\033[1;41m"
	#define bgclr_ltgreen "\033[1;42m"
	#define bgclr_yellow  "\033[1;43m"
	#define bgclr_ltblue  "\033[1;44m"
	#define bgclr_purple  "\033[1;45m"
	#define bgclr_skyblue "\033[1;46m"
	#define bgclr_white   "\033[1;47m"

	#define out_red( x )     clr_red     x clr_null
	#define out_green( x )   clr_green   x clr_null
	#define out_brown( x )   clr_brown   x clr_null
	#define out_blue( x )    clr_blue    x clr_null
	#define out_magenta( x ) clr_magenta x clr_null
	#define out_cyan( x )    clr_cyan    x clr_null
	#define out_ltgray( x )  clr_ltgray  x clr_null
	#define out_dkgray( x )  clr_dkgray  x clr_null
	#define out_ltred( x )   clr_ltred   x clr_null
	#define out_ltgreen( x ) clr_ltgreen x clr_null
	#define out_yellow( x )  clr_yellow  x clr_null
	#define out_ltblue( x )  clr_ltblue  x clr_null
	#define out_purple( x )  clr_purple  x clr_null
	#define out_skyblue( x ) clr_skyblue x clr_null
	#define out_white( x )   clr_white   x clr_null

	#define _wrn std::cerr << clr_purple << "[w] " << _func_context_ << ": "
	#define _wrn2 std::cerr << clr_yellow << "[w] " << _func_context_ << ": "
	#define _err std::cerr << clr_red << "[!] " << _func_context_ << ": "
	#define _inf std::cout << clr_white  << "[i] " << _func_context_ << ": "
	#define endl_ clr_null << std::endl


#endif

