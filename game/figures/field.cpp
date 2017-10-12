#include "field.h"
#include "log/Log.h"
#include "utils/common_utils.h"

#include <stdexcept>
#include <iostream>
#include <unistd.h>

#include <cassert>

using namespace Figures ;
using namespace Utils ;

GameField::GameField( addFigureInterface *addfig, int figure_len, int maxcols, int maxrows )
	: figure( figure_len ), addfig( addfig ), utils( field, figure_len )
	, rows_len( maxrows - 10 - figure_len / 2 )
	, cols_len( figure_len * 2 + figure_len / 2 + figure_len / 3 )
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

void GameField::run( ) {

	Utils::common_utils cmnut ;
	cmnut.startGameControl( ) ;

	addfig->drowGameField( field ) ;
	addfig->drowNextFig( figure.getNextFig( ) ) ;
	addfig->drowScores( utils.getscores( ), utils.getlevel( ) ) ;
    
	while( processAction( cmnut.getAction( ) ) ) {
		cmnut.setGameDelay( utils.getDelay( ) ) ;
	}

	cmnut.stopGameControl( ) ;
}

void GameField::generateNewFig( ) {

	_inf << "generateNewFig" << endl_ ;

	figure.Generate( ) ;

	std::vector< std::vector<char> > &fig = figure.getCurrFig( ) ;

	int figrows = fig.size( ) ; // fig checked in getCurrFig
	int figcols = fig.at( 0 ).size( ) ;

	fig_pos.setf( cols_len / 2, -1, figcols, figrows ) ;
	fig_pos_old.clear( ) ;

	addFigure( ) ;
}

bool GameField::processAction( Utils::Actions action ) {
	bool ret = true ;
	switch( action ) {
		case Utils::Actions::Action_down:
		{
			_inf << "ActionDown" << endl_ ;
			ret = processDown( ) ;
		}
		break;

		case Utils::Actions::Action_left:
		{
			_inf << "Action_left" << endl_ ;
			processLeft( ) ;
		}
		break ;

		case Utils::Actions::Action_right:
		{
			_inf << "Action_right" << endl_ ;
			processRight( ) ;
		}
		break ;

		case Utils::Actions::Action_rotate:
		{
			_inf << "Action_rotate" << endl_ ;
			processRotate( ) ;
		}
		break ;

		case Utils::Actions::Action_unrotate:
		{
			_inf << "Action_unrotate" << endl_ ;
//			figure.UnRotate( ) ;
//			std::vector< std::vector<char> > &fig = figure.getCurrFig( ) ;
//			fig_pos.rotate( fig.at( 0 ).size( ), fig.size( ) ) ;
		}
		break ;

		case Utils::Action_exit:
			return false ;
		break;

		case Actions::Action_empty:
		default:
		{
			return true ;
		}
		break ;
	}
	// drow figure after ProcessAction
	addfig->drowGameField( field ) ;
	return true ;
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

bool GameField::processDown( ) {
	clearFigure( ) ;
	fig_pos.down( ) ;

	auto &fig = figure.getCurrFig( ) ;
	if( !utils.checkRowBorder( fig_pos ) || !utils.checkFigureBottom( fig_pos, fig ) ) {
		fig_pos.up( ) ;
		addFigure( ) ;

		if( utils.checkFieldOverFlow( ) ) {
			return false ;
		}

		processFilledRows( ) ;

		generateNewFig( ) ;
		addfig->drowNextFig( figure.getNextFig( ) ) ;
	}
	addFigure( ) ;

	return true ;
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


