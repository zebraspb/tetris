#include "ncmainwindow.h"

#include "figures/figure.h"
#include "figures/field.h"

#include "log/Log.h"

#include <ncurses.h>
#include <stdexcept>
#include <string>

#include <iostream>


namespace Errors {
	std::string outofborders( "Figure Out Of Borders" ) ;
}


NcMainWindow::NcMainWindow( int figlen )
	: figlen( figlen )
{

	WINDOW *win = initscr( ) ;
	cbreak( ) ;
	curs_set( 0 ) ;
	noecho( ) ;

	wresize( win, maxrow + 2, maxcol + 2 ) ;

	gameField = new Figures::GameField( this, figlen, maxcol, maxrow ) ;
	
	game_field_pos.rows_len = gameField->getFieldRowsLen( ) ;
	game_field_pos.cols_len = gameField->getFieldColsLen( ) ;
	game_field_pos.pos_col = ( maxcol / 2 ) - ( game_field_pos.cols_len / 2 ) ;
	game_field_pos.pos_row = ( maxrow / 2 ) - ( game_field_pos.rows_len  / 2 ) ;
	game_win = newwin( game_field_pos.rows_len + 2, game_field_pos.cols_len + 2, game_field_pos.pos_row, game_field_pos.pos_col ) ;


	inf_field_pos.cols_len = 20 ;
	inf_field_pos.rows_len = 4 ;
	inf_field_pos.pos_col  = game_field_pos.pos_col + game_field_pos.cols_len + 2 ;
	inf_field_pos.pos_row  = game_field_pos.pos_row ;
	inf_win = newwin( inf_field_pos.rows_len, inf_field_pos.cols_len, inf_field_pos.pos_row, inf_field_pos.pos_col ) ;

	next_fig_pos.cols_len = std::max( figlen + 2, 6 ) ;
	next_fig_pos.rows_len = std::max( figlen + 2, 6 ) ;
	next_fig_pos.pos_col  = game_field_pos.pos_col + game_field_pos.cols_len + 2 ;
	next_fig_pos.pos_row  = inf_field_pos.pos_row + inf_field_pos.rows_len + 1 ;
	nextfig_win = newwin( next_fig_pos.rows_len, next_fig_pos.cols_len, next_fig_pos.pos_row, next_fig_pos.pos_col ) ;

	box( stdscr, 0, 0 ) ;
	box( game_win, 0, 0 ) ;		// 0, 0 gives default characters for the vertical and horizontal lines
	box( inf_win, 0, 0 ) ;
	box( nextfig_win, 0, 0 ) ;
	mvwprintw( nextfig_win, 0, 1, "next") ;

	refresh( ) ;
	wrefresh( game_win ) ;		// Show that box
	wrefresh( inf_win ) ;
	wrefresh( nextfig_win ) ;
}

NcMainWindow::~NcMainWindow( ) {
	std::lock_guard<std::mutex> lock( m ) ;	// wait for run's end before destruct

	delete gameField ;

	if( game_win )    delwin( game_win ) ;
	if( nextfig_win ) delwin( nextfig_win ) ;
	if( inf_win )     delwin( inf_win ) ;

	endwin( ) ;

	_wrn << "Exit..." << endl_ ;
}

void NcMainWindow::run( ) {
	std::lock_guard<std::mutex> lock( m ) ;	// run only once
	gameField->run( ) ; // blocked call until exit


	clear( ) ;
	nocbreak( ) ;

	std::string mesg ("Thank you. Good buy!" );
	mvwprintw( stdscr, maxrow / 2, ( maxcol - mesg.size( ) ) / 2, "%s", mesg.c_str( ) ) ;

	getch( ) ;
}




void NcMainWindow::drowGameField( const std::vector< std::vector<char> > &field ) {
	int rows_len = field.size( ) ;
	int cols_len = field[ 0 ].size( ) ;

	for ( auto c = 0; c < cols_len; ++c ) {
		for ( auto r = 0; r < rows_len; ++r ) {
			auto ch = tonc( field.at( r ).at( c ) ) ;
			mvwaddch( game_win, r + 1, c + 1, ch ) ;
		}
	}
	wrefresh( game_win ) ;
}

void NcMainWindow::drowNextFig( const std::vector< std::vector<char> > &nextfig ) {
	int rows_len = nextfig.size( ) ;
	int cols_len = nextfig[ 0 ].size( ) ;

	for ( auto c = 0; c < next_fig_pos.cols_len - 2; ++c ) {
		for ( auto r = 0; r < next_fig_pos.rows_len - 2; ++r ) {
			mvwaddch( nextfig_win, r + 1, c + 1, tonc( 0 ) ) ;
		}
	}

	for ( auto c = 0; c < cols_len; ++c ) {
		for ( auto r = 0; r < rows_len; ++r ) {
			auto ch = tonc( nextfig.at( r ).at( c ) ) ;
			mvwaddch( nextfig_win, r + 1, c + 1, ch ) ;
		}
	}
	wrefresh( nextfig_win ) ;
	
}

void NcMainWindow::drowScores( int scores, int level ) {
	mvwprintw( inf_win, 1, 1, "level  %02d", level ) ;
	mvwprintw( inf_win, 2, 1, "scores %05d", scores ) ;
	wrefresh( inf_win ) ;		// Show that box
}

