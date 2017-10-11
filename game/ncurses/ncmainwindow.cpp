#include "ncmainwindow.h"

#include "Figures/figure.h"
#include "Figures/field.h"

#include "Log/Log.h"

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
	keypad( stdscr, true ) ;
	noecho( );

	wresize( win, maxrow + 2, maxcol + 2 ) ;

	box( stdscr, 0, 0 ) ;
//	getmaxyx( stdscr, maxrow, maxcol ) ;	// macro

	_inf << "maxrow : " << maxrow << " maxcol " << maxcol << endl_ ;

	gameField = new Figures::GameField( this, figlen, maxcol, maxrow ) ;
	
	std::vector< std::vector<char> > &field =  gameField->getField( ) ;

	game_field_pos.rows_len = field.size( ) ;
	game_field_pos.cols_len = field[ 0 ].size( ) ;
	game_field_pos.pos_col = ( maxcol / 2 ) - ( game_field_pos.cols_len / 2 ) ;
	game_field_pos.pos_row = ( maxrow / 2 ) - ( game_field_pos.rows_len  / 2 ) ;
	game_win = newwin( game_field_pos.rows_len + 2, game_field_pos.cols_len + 2, game_field_pos.pos_row, game_field_pos.pos_col ) ;


	inf_field_pos.cols_len = 20 ;
	inf_field_pos.rows_len = 4 ;
	inf_field_pos.pos_col  = game_field_pos.pos_col + game_field_pos.cols_len + 2 ;
	inf_field_pos.pos_row  = game_field_pos.pos_row ;
	inf_win = newwin( inf_field_pos.rows_len, inf_field_pos.cols_len, inf_field_pos.pos_row, inf_field_pos.pos_col ) ;

	next_fig_pos.cols_len = figlen + 2 ;
	next_fig_pos.rows_len = figlen + 2 ;
	next_fig_pos.pos_col  = game_field_pos.pos_col + game_field_pos.cols_len + 2 ;
	next_fig_pos.pos_row  = inf_field_pos.pos_row + inf_field_pos.rows_len + 1 ;
	nextfig_win = newwin( next_fig_pos.rows_len, next_fig_pos.cols_len, next_fig_pos.pos_row, next_fig_pos.pos_col ) ;


	box( game_win, 0, 0 ) ;		// 0, 0 gives default characters for the vertical and horizontal lines
	box( inf_win, 0, 0 ) ;
	box( nextfig_win, 0, 0 ) ;
	mvwprintw( nextfig_win, 0, 1, "next") ;

	refresh( ) ;
	wrefresh( game_win ) ;		// Show that box
	wrefresh( inf_win ) ;
	wrefresh( nextfig_win ) ;

//	getch( ) ;					// press any key
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


	gameField->Start( ) ;
	halfdelay( 1 ) ;

	quit = false;
	while ( !quit ) {
		int ch = getch( ) ;

		switch ( ch ) {
			case ERR:
				//printw("\rPlease, press any key...");
				//figrow ++ ;
				//clear( ) ;
				//printFigNc( figrow , figcol, figlen, figure.getCurrFig( ) ) ;
			break;

			case 27:
			case 'q':
			case 'Q':
			case KEY_F(2):
				_wrn << "Exit key is pressed!" << endl_ ;
				quit = true;
				continue ;
			break;

			case KEY_UP:
				gameField->ProcessAction( Figures::Action_rotate ) ;
			break ;

			case KEY_RIGHT:
				gameField->ProcessAction( Figures::Action_right ) ;
			break ;

			case KEY_LEFT:
				gameField->ProcessAction( Figures::Action_left ) ;
			break ;

			case KEY_DOWN:
				_inf << "KEY_DOWN" << endl_ ;
				gameField->ProcessAction( Figures::Action_down ) ;
				//gameField->ProcessAction( Figures::Action_unrotate ) ;
			break ;

			default:
				//  
			break;
		}
	}

	gameField->Stop( ) ;

	clear( ) ;
	nocbreak( ) ;

	std::string mesg ("Thank you. Good buy!" );
	mvwprintw( stdscr, maxrow / 2, ( maxcol - mesg.size( ) ) / 2, "%s", mesg.c_str( ) ) ;

	getch( ) ;
}




void NcMainWindow::drowGameField( std::vector< std::vector<char> > &field ) {
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

void NcMainWindow::drowNextFig( std::vector< std::vector<char> > &nextfig ) {
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

void NcMainWindow::overflow( ) {
	quit = true ;
}


void NcMainWindow::drowScores( int scores, int level ) {
	mvwprintw( inf_win, 1, 1, "level  %02d", level ) ;
	mvwprintw( inf_win, 2, 1, "scores %05d", scores ) ;
	wrefresh( inf_win ) ;		// Show that box
}
