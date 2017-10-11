
#include "ncurses/ncmainwindow.h"

#include <iostream>
#include <vector>
#include <random>
#include <stdint.h>
#include <cassert>

#include "Log/Log.h"

int main( void ) {
	const int figlen = 4 ;


	// logging in files, for ncurses
	std::ofstream fslog( "log.txt", std::fstream::out ), fserr( "err.txt", std::fstream::out ) ;

	std::streambuf *backuplog = std::cout.rdbuf( ) ;
	std::streambuf *backuperr = std::cerr.rdbuf( ) ;

	std::cout.rdbuf( fslog.rdbuf( ) ) ;
	std::cerr.rdbuf( fserr.rdbuf( ) ) ;

#ifdef GAME_WIN_NCURSES
	NcMainWindow win( figlen ) ;
	win.run( ) ;
#elif defined( GAME_WIN_QT )
	#error "Game with Qt unsupported now"
#endif

	std::cout.rdbuf( backuplog ) ;
	std::cerr.rdbuf( backuperr ) ;

	return 0 ;
}


