#include "box_field.h"
#include "log/Log.h"

using namespace Utils ;


fig_place::fig_place( ) {
	clear( ) ;
}

fig_place::fig_place( const box_field &box ) {
	setf( box.pos_col, box.pos_row, box.cols_len, box.rows_len ) ;
}


fig_place::fig_place( unsigned int col, unsigned int row, unsigned int cols_len, unsigned int rows_len ) {
	setf( col, row, cols_len, rows_len ) ;
}

fig_place::fig_place( const fig_place &fig ) {
	beg_col = fig.beg_col ;
	end_col = fig.end_col ;
			  
	beg_row = fig.beg_row ;
	end_row = fig.end_row ;
}

void fig_place::setf( unsigned int col, unsigned int row, unsigned int cols_len, unsigned int rows_len ) {
	beg_col = col - cols_len / 2 ;
	end_col = col + cols_len / 2  + ( cols_len & 1 ) ;

	beg_row = row - rows_len / 2 ;
	end_row = row + rows_len / 2 + ( rows_len & 1 ) ;

//	_inf << "col " << col << " row " << row << " cols_len " << cols_len << " rows_len " << rows_len << endl_ ;
//	_inf << "beg_col " << beg_col << " end_col " << end_col << " beg_row " << beg_row << " end_row " << end_row << endl_ ;
}

void fig_place::shiftRight( ) {
	beg_col += 1 ;
	end_col += 1 ;
}

void fig_place::shiftLeft( ) {
	beg_col -= 1 ;
	end_col -= 1 ;
}

void fig_place::down( ) {
	beg_row += 1 ;
	end_row += 1 ;
}

void fig_place::up( ) {
	beg_row -= 1 ;
	end_row -= 1 ;
}


void fig_place::rotate( unsigned int cols_len, unsigned int row_len ) { // figures sizes after rotate
	
	int centerC = ( beg_col + end_col ) / 2 ;
	int centerR = ( beg_row + end_row ) / 2 ;

	setf( centerC, centerR, cols_len, row_len ) ;
}

void fig_place::clear( ) {
	beg_col = end_col = beg_row = end_row = -1 ;
}
