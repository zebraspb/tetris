#include "field_utils.h"
#include "box_field.h"

#include "log/Log.h"

#include <assert.h>

using namespace Utils ;

const unsigned int Field_utils::delays[ max_level ] = { 500000, 400000, 320000, 290000, 250000,
														200000, 160000,  130000, 100000,  90000 } ;

		
Field_utils::Field_utils( std::vector< std::vector<char> > &field, int figlen ) 
	: field( field ), fieldOverFlow( false ), level( 1 ), scores( 0 ), figlen( 0 )
{
	scores_mult = std::max( 1,  10 * ( figlen - 4 ) ) ;
	level_mult = scores_mult * 10 ;

	_inf << "scores_mult : " << scores_mult << " level_mult " << level_mult << endl_ ;
}

Field_utils::~Field_utils( ) {

}

bool Field_utils::checkFigureLeft( fig_place &fig_pos, std::vector< std::vector<char> > &fig ) {
	//check for field fill in left shift
	auto begcol = fig_pos.getBeginCol( ) ;
	auto endcol = fig_pos.getEndCol( ) ;
	auto begrow = fig_pos.getBeginRow( ) ;
	auto endrow = fig_pos.getEndRow( ) ;

	//_inf << "begcol " << begcol << " endcol " << endcol <<  " begrow " << begrow <<  " endrow " << endrow << endl_ ;
	if( begrow < 0 ) begrow = 0 ; // after rotate it can'be in begin

	for( auto row = begrow; row < endrow; ++row ) {
		auto firstCol = endcol ;
		for( auto col = endcol - 1; col >= begcol; --col ) {
			if( fig.at( row - begrow ).at( col - begcol ) ) firstCol = col ;
		}

		if( field.at( row ).at( firstCol ) && fig.at( row - begrow ).at( firstCol - begcol ) ) return false ;
	}

	return true ;
}

bool Field_utils::checkFigureRight( fig_place &fig_pos, std::vector< std::vector<char> > &fig ) {
		//check for field fill in right shift
	auto begcol = fig_pos.getBeginCol( ) ;
	auto endcol = fig_pos.getEndCol( ) ;
	auto begrow = fig_pos.getBeginRow( ) ;
	auto endrow = fig_pos.getEndRow( ) ;

//	_inf << "begcol " << begcol << " endcol " << endcol <<  " begrow " << begrow <<  " endrow " << endrow << endl_ ;
	if( begrow < 0 ) begrow = 0 ; // after rotate it can'be in begin

	for( auto row = begrow; row < endrow; ++row ) {
		auto lastCol = begcol ;
		for( auto col = begcol; col < endcol; ++col ) {
			if( fig.at( row - begrow ).at( col - begcol ) ) lastCol = col ;
		}

		if( field.at( row ).at( lastCol ) && fig.at( row - begrow ).at( lastCol - begcol ) ) return false ;
	}
	return true ;
}

bool Field_utils::checkColBorders( fig_place &fig_pos ) {

	assert( field.size( ) ) ;
	assert( field.at( 0 ).size( ) ) ;

	int rows_len = field.size( ) ;
	int cols_len = field.at( 0 ).size( ) ;

	auto fig_right_offset = fig_pos.getEndCol( ) - cols_len ;
	if( fig_right_offset > 0 )       return false ;
	if( fig_pos.getBeginCol( ) < 0 ) return false ;

	return true ;
}

bool Field_utils::checkRowBorder( fig_place &fig_pos ) {
	assert( field.size( ) ) ;
	assert( field.at( 0 ).size( ) ) ;

	int rows_len = field.size( ) ;
	int cols_len = field.at( 0 ).size( ) ;

	auto fig_bottom_offset = fig_pos.getEndRow( ) - rows_len ;
	if( fig_bottom_offset > 0 ) return false ;

	return true ;
}

bool Field_utils::checkFigureBottom( fig_place &fig_pos, std::vector< std::vector<char> > &fig ) {
	auto begcol = fig_pos.getBeginCol( ) ;
	auto endcol = fig_pos.getEndCol  ( ) ;
	auto begrow = fig_pos.getBeginRow( ) ;
	auto endrow = fig_pos.getEndRow  ( ) ;

//	_inf << "begcol " << begcol << " endcol " << endcol << " begrow " << begrow << " endrow " << endrow << endl_ ;

	if( begrow < 0 ) begrow = 0 ;// it can'be in begin of figure fall

	for( auto col = begcol; col < endcol; ++col ) {
		auto lastRow = 0 ;
		for( auto row = begrow; row < endrow; ++row ) {
			if( fig.at( row - begrow ).at( col - begcol ) ) lastRow = row ;
		}
//		std::cout << "For col =  " << col - begcol << " lastRow is " << lastRow << std::endl ;

		if( field.at( lastRow ).at( col ) && fig.at( lastRow - begrow ).at( col - begcol ) ) {
			if( begrow <= 0 ) fieldOverFlow = true ;
			return false ;
		}
		
	}
	return true ;
}

void Field_utils::addScores( int lines ) {
	auto summprev = 0 ;
	for( auto i = 0; i < lines; ++ i ) {
		summprev += i + 1 ;
		scores += summprev ;
	}

	scores *= scores_mult ;

	if( ( scores >= level * level_mult ) && ( level < max_level ) ) {
		++level ;
		level_mult *= 2 ;
	}
}

unsigned int Field_utils::getDelay( ) {
	if( level > max_level ) level = max_level ;
	if( level < 1 ) level = 1 ;
	return delays[ level - 1 ] ;
}

