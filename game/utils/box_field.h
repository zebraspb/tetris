#ifndef __BOX_FIELD_H__
#define __BOX_FIELD_H__

namespace Utils {

struct box_field {
	int pos_col ;
	int pos_row ;

	int cols_len ;
	int rows_len ;
} ;


class fig_place {
public:
	fig_place( ) ;
	fig_place( const box_field &box ) ;
	fig_place( unsigned int col, unsigned int row, unsigned int cols_len, unsigned int rows_len ) ;
	fig_place( const fig_place &fig ) ;

	void clear( ) ;

	bool empty( ) {
		if( beg_col == -1 && end_col == -1 && beg_row == -1 && end_row == -1 ) return true ;
		return false ;
	}

	void setf( unsigned int col, unsigned int row, unsigned int cols_len, unsigned int rows_len ) ;

	void shiftRight( ) ;
	void shiftLeft( ) ;
	void down( ) ;
	void up( ) ;


	void rotate( unsigned int cols_len, unsigned int row_len ) ; // figures sizes after rotate

	int getBeginCol( ) const { return beg_col ; }
	int getEndCol  ( ) const { return end_col ; }
	int getBeginRow( ) const { return beg_row ; }
	int getEndRow  ( ) const { return end_row ; }

private:
	int beg_col ;
	int beg_row ;

	int end_col ;
	int end_row ;

};
}

#endif

