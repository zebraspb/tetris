#include "ncurses/ncmainwindow.h"
#include "log/Log.h"

#include <iostream>
#include <sstream>
#include <getopt.h>

namespace {

void usage( ) {
    std::cout << "help:" << std::endl << 
				 "-h, --help : print this screen" << std::endl
			     << "-s, --size <figurelen>, figure len is from 1 to 10" << std::endl ;
}

const char* opts = "hs:" ;

const struct option longOpts[ ] = {
    { "help", no_argument, 0, opts[ 0 ] },
	{ "size", required_argument, 0, opts[ 1 ] },
    { 0, 0, 0 }
} ;

}

int main( int argc, char *argv[] ) {
	int figlen = 4 ;


	 while (true) {
        int index = 0 ;
        int c = getopt_long( argc, argv, opts, longOpts, &index ) ;

        if (c == -1) // end of options
            break ;

        if( c == opts[ 0 ] ) { // -h
            usage( ) ;
			return 1;
        } else if( c == opts[ 1 ] ) { // -s
			std::stringstream( optarg ) >> figlen ;
			if( figlen < 1 ) {
				_err << "size must be a number better 0" << endl_ ;
				return -1 ;
			} else if( figlen > 10 ) {
				_err << "size must be not greater than 10" << endl_ ;
				return -1 ;
			}
        }
    }

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


