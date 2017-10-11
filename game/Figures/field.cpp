#include "field.h"
#include "Log/Log.h"

#include <stdexcept>
#include <iostream>
#include <unistd.h>

#include <cassert>

using namespace Figures ;

GameField::GameField( addFigureInterface *addfig, int figure_len, int maxcols, int maxrows )
	: figure( figure_len ), addfig( addfig ), utils( field )
{
	if( !addfig ) {
		_err << "addfig interface is empty, exit with error" << endl_ ;
		throw ;
	}

	if( maxcols <= figure_len ) {
		_err << "Can't create field, with figure_len " << figure_len << " and maxcols " << maxcols << endl_ ;
		throw ;
	}

	if( maxrows <= figure_len * 4 ) {
		_err << "Can't create field, with figure_len " << figure_len << " and maxrows " << maxrows << endl_;
		throw ;
	}

	cols_len = figure_len * 2 + figure_len / 2 + figure_len / 3 ;
	if( cols_len > maxcols ) cols_len = maxcols ;

	rows_len = maxrows - 10 - figure_len / 2 ;

	std::vector<char> temp( cols_len, 0 ) ;
	field.resize( rows_len, temp ) ;

//	for( auto r = rows_len - 3; r < rows_len; ++r ) {
//		for(auto c = 0; c < cols_len; ++c ) {
//			field[r][c] = 1 ;
//		}
//	}

	generateNewFig( ) ;
}

GameField::~GameField( ) {

}

void GameField::Start( ) {
	if( !activeThread ) {
		activeThread = true ;
		t1 = new std::thread ( &GameField::figureFallThread, std::ref( *this ) ) ;

		addfig->drowScores( utils.getscores( ), utils.getlevel( ) ) ;
		addfig->drowNextFig( figure.getNextFig( ) ) ;
	}
}

void GameField::Stop( ) {
	if( activeThread ) {
		activeThread = false ;
		t1->join( ) ;
		delete t1 ;
	}
}

void GameField::figureFallThread( ) {
	while( 1 ) {
		usleep( utils.getDelay( ) ) ;
		if( !activeThread ) break ;
		ProcessAction( Figures::Action_down ) ;
	}
}

void GameField::generateNewFig( ) {

	_inf << "generateNewFig" << endl_ ;

	figure.Generate( ) ;

	std::vector< std::vector<char> > &fig = figure.getCurrFig( ) ;

	int figrows = fig.size( ) ; // fig checked in getCurrFig
	int figcols = fig.at( 0 ).size( ) ;

	fig_pos.setf( cols_len / 2, -1, figcols, figrows ) ;
	fig_pos_old.clear( ) ;
}

std::vector< std::vector<char> > &GameField::getField( ) {
	assert( field.size( ) ) ;
	assert( field.at( 0 ).size( ) ) ;

	return field ;
}

void GameField::ProcessAction( Actions action ) {
	std::lock_guard<std::mutex> lock( m ) ;	// thread safety for figureFallThread and main thread run( )

	switch( action ) {
		case Actions::Action_down:
		{
			_inf << "ActionDown" << endl_ ;
			processDown( ) ;
		}
		break;

		case Actions::Action_left:
		{
			_inf << "Action_left" << endl_ ;
			processLeft( ) ;
		}
		break ;

		case Actions::Action_right:
		{
			_inf << "Action_right" << endl_ ;
			processRight( ) ;
		}
		break ;

		case Actions::Action_rotate:
		{
			_inf << "Action_rotate" << endl_ ;
			processRotate( ) ;
		}
		break ;

		case Actions::Action_unrotate:
		{
			_inf << "Action_unrotate" << endl_ ;
//			figure.UnRotate( ) ;
//			std::vector< std::vector<char> > &fig = figure.getCurrFig( ) ;
//			fig_pos.rotate( fig.at( 0 ).size( ), fig.size( ) ) ;
		}
		break ;

		case Actions::Action_empty:
		default:
		{
			return ;
		}
		break ;
	}
	// drow figure after ProcessAction
	addfig->drowGameField( field ) ;
}

void GameField::processFilledRows( ) {
	auto cleared_lines = 0 ;
	for( auto r = field.begin( ); r < field.end( ); ) {
		bool filled_row = true ;
		for( auto c = r->begin( ); c < r->end( ); ++c ) if( ! ( *c ) ) filled_row = false ; 

		if( filled_row ) {
			++cleared_lines ;
			std::vector<char> temp( cols_len, 0 ) ;

			r = field.erase( r ) ;
			field.insert( field.begin( ), temp ) ;
			addfig->drowGameField( field ) ;
			usleep( 100000 ) ;
		} else {
			++r ;
		}
	}
	
	utils.addScores( cleared_lines ) ;
	addfig->drowScores( utils.getscores( ), utils.getlevel( ) ) ;
}


void GameField::processRotate( ) {
	clearFigure( ) ;

	figure.Rotate( ) ;
	std::vector< std::vector<char> > &fig = figure.getCurrFig( ) ;
	fig_pos.rotate( fig.at( 0 ).size( ), fig.size( ) ) ;

	if( !utils.checkColBorders( fig_pos ) || 
		!utils.checkFigureLeft( fig_pos, fig ) || 
		!utils.checkFigureRight( fig_pos, fig ) || 
		!utils.checkFigureBottom( fig_pos, fig ) ) 
	{
		figure.UnRotate( ) ;
		std::vector< std::vector<char> > &fig = figure.getCurrFig( ) ;
		fig_pos.rotate( fig.at( 0 ).size( ), fig.size( ) ) ;
	}

	addFigure( ) ;
}




void GameField::processLeft( ) {
	clearFigure( ) ;

	auto &fig = figure.getCurrFig( ) ;

	fig_pos.shiftLeft( ) ;
	if( !utils.checkColBorders( fig_pos ) )           fig_pos.shiftRight( ) ;
	else if( !utils.checkFigureLeft( fig_pos, fig ) ) fig_pos.shiftRight( ) ;
	
	addFigure( ) ;
}

void GameField::processRight( ) {
	clearFigure( ) ;

	auto &fig = figure.getCurrFig( ) ;

	fig_pos.shiftRight( ) ;
	if( !utils.checkColBorders( fig_pos ) )            fig_pos.shiftLeft( ) ;
	else if( !utils.checkFigureRight( fig_pos, fig ) ) fig_pos.shiftLeft( ) ;


	addFigure( ) ;
}

void GameField::processDown( ) {
	clearFigure( ) ;
	fig_pos.down( ) ;

	auto &fig = figure.getCurrFig( ) ;
	if( !utils.checkRowBorder( fig_pos ) || !utils.checkFigureBottom( fig_pos, fig ) ) {
		fig_pos.up( ) ;
		addFigure( ) ;

		if( utils.checkFieldOverFlow( ) ) {
			addfig->overflow( ) ;
			return ;
		}

		generateNewFig( ) ;
		processFilledRows( ) ;

		addfig->drowNextFig( figure.getNextFig( ) ) ;
	}
	addFigure( ) ;
}


void GameField::clearFigure( ) {
	if( !fig_pos_old.empty( ) ) { // delete old figure

		_inf << "delete old begincol " << fig_pos_old.getBeginCol( )  << " endcol " << fig_pos_old.getEndCol( ) << 
			    " beginrow " << fig_pos_old.getBeginRow( ) << " endrow " << fig_pos_old.getEndRow( ) << endl_ ;

		auto &fig = figure.getCurrFig( ) ;

		for ( auto c = fig_pos_old.getBeginCol( ); c < fig_pos_old.getEndCol( ); ++c ) {
			for ( auto r = fig_pos_old.getBeginRow( ); r < fig_pos_old.getEndRow( ); ++r ) {
				if( r >= 0 && r < rows_len && c >= 0 && c < cols_len ) {
					if( fig.at( r - fig_pos_old.getBeginRow( ) ).at( c - fig_pos_old.getBeginCol( ) ) ) 
						field.at( r ).at( c ) = 0 ;
				}
			}
		}

		_inf << "clear figure done ..." << endl_ ;
	}
}

void GameField::addFigure( ) {

	fig_pos_old = fig_pos ;

	_inf << "draw new begincol " << fig_pos.getBeginCol( )  << " endcol " << fig_pos.getEndCol( ) << 
		    " beginrow " << fig_pos.getBeginRow( ) << " endrow " << fig_pos.getEndRow( ) << endl_ ;

	auto &fig = figure.getCurrFig( ) ;

	for ( auto c = fig_pos.getBeginCol( ); c < fig_pos.getEndCol( ); ++c ) {
		for ( auto r = fig_pos.getBeginRow( ); r < fig_pos.getEndRow( ); ++r ) {
			//mvwaddch( game_win, r, c, fig.at( r - beginrow ).at( c - begincol ) ) ;
			if( r >= 0 && r < rows_len && c >= 0 && c < cols_len ) {
				if( fig.at( r - fig_pos.getBeginRow( ) ).at( c - fig_pos.getBeginCol( ) ) ) 
					field.at( r ).at( c ) = fig.at( r - fig_pos.getBeginRow( ) ).at( c - fig_pos.getBeginCol( ) ) ;
			}
		}
	}
}

