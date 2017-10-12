#include "figure.h"

#include <random>
#include <stdint.h>
#include <algorithm>

#include <stdio.h>
#include <iostream>
#include <cassert>


using namespace Figures ;

Figure::Figure( int len ) : len( len ) {
	if( len < 1 ) {
		std::cerr << "min len is 2" << std::endl ;
		throw ;
	}

	generateFig( nextFig ) ;
}

Figure::~Figure( ) {
}


void Figure::Generate( ) {
	// random generate
	currFig = nextFig ;
	generateFig( nextFig ) ;
}

void Figure::Rotate( ) { // rotate always currFig
	assert( currFig.size( ) ) ;
	assert( currFig[ 0 ].size( ) ) ;

	std::vector< std::vector<char> > temp = currFig ; // copy

	int old_rows = currFig.size( ) ;
	int old_cols = currFig[ 0 ].size( ) ;

	currFig.resize( old_cols ) ;
	for( auto &col : currFig ) col.resize( old_rows, 0 ) ;

	for( auto row = 0; row < temp.size( ); ++row ) {
		for( auto col = 0; col < temp[ row ].size( ); ++col ) {
			currFig.at( col ).at( row ) = temp.at( ( old_rows - 1 ) - row ).at( col ) ;
		}
	}
}

void Figure::UnRotate( ) {
	assert( currFig.size( ) ) ;
	assert( currFig[ 0 ].size( ) ) ;

	std::vector< std::vector<char> > temp = currFig ; // copy

	int old_rows = currFig.size( ) ;
	int old_cols = currFig[ 0 ].size( ) ;

	currFig.resize( old_cols ) ;
	for( auto &col : currFig ) col.resize( old_rows, 0 ) ;

	for( auto row = 0; row < temp.size( ); ++row ) {
		for( auto col = 0; col < temp[ row ].size( ); ++col ) {
			currFig.at( col ).at( row ) = temp.at( row ).at( ( old_cols - 1 ) - col ) ;
		}
	}
}

void Figure::resetFig( std::vector< std::vector<char> > &fig ) {
	// make empty len * len figure
	fig.clear( ) ;
	std::vector<char> temp( len, 0 ) ;
	fig.resize( len, temp ) ;
}

void Figure::generateFig( std::vector< std::vector<char> > &fig ) {
	
	resetFig( fig ) ;

	auto fig_blocks = len ;

	uint8_t first = 0 ;
	uint8_t last = len - 1 ;

	auto mincol = len - 1 ;
	auto maxcol = 0 ;

	for( auto row = 0; row < len; ++row ) {
		std::random_device rd ;
		std::uniform_int_distribution<int> figdist( 1, fig_blocks ) ;

		uint8_t cnt = figdist( rd ) ; //count block of figure in row

//		if( 0 == row ) 		cnt = 4 ;
//		else if( 1 == row ) cnt = 1 ;
		

		int8_t offmin = first - ( cnt - 1 ) ; if( offmin < 0 ) offmin = 0 ;
		int8_t offmax = last ; if( offmax + cnt > len ) offmax = len - cnt ;

		std::uniform_int_distribution<int> offsetdist( offmin, offmax ) ;

		uint8_t offset =  offsetdist( rd ) ;
		for( auto col = offset; col < offset + cnt; ++col ) fig.at( row ).at( col ) = 1 ;

		if( mincol >= offset ) mincol = offset ;
		if( maxcol <= offset + cnt - 1 ) maxcol = offset + cnt - 1 ;

		first = offset ;
		last  = offset + cnt -  1 ;

		fig_blocks -= cnt ;

		if( !fig_blocks ) {
			// delete all empty rows at end
			fig.erase( fig.begin( ) + row + 1, fig.end( ) ) ;
			break ;
		}
	}

	// delete all empty cols
	for( auto &col : fig  ) {
		// first delete empty cols from maxcol to end
		col.erase( col.begin( ) + maxcol + 1, col.end( ) ) ;

		// second delete empty cols from begin to mincol
		col.erase( col.begin( ), col.begin( ) + mincol ) ;
	}
}

std::vector< std::vector<char> >& Figure::getCurrFig( ) {
	assert( currFig.size( ) ) ;
	assert( currFig[ 0 ].size( ) ) ;

	return currFig ;
}

std::vector< std::vector<char> >& Figure::getNextFig( ) {
	assert( nextFig.size( ) ) ;
	assert( nextFig[ 0 ].size( ) ) ;

	return nextFig ;
}
