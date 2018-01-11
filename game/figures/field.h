#ifndef __FIG_CNTRL_H__
#define __FIG_CNTRL_H__

#include "figure.h"
#include "utils/box_field.h"
#include "utils/field_utils.h"
#include "utils/common_utils.h"
#include "addFigureInterface.h"

#include <vector>
#include <thread>
#include <mutex>


namespace Figures {



class GameField {
public:
	GameField( addFigureInterface *addfig, int figure_len, int maxcols, int maxrows ) ;	// maxcols and maxrows from ncurses!
	~GameField( ) ;

	void run( ) ;

	int getFieldColsLen( ) const { return cols_len ; }
	int getFieldRowsLen( ) const { return rows_len ; }

private:
	void generateNewFig( ) ;
	bool processAction( Utils::Actions action ) ;


	void processFilledRows( ) ;

	void processRotate( ) ;
	void processLeft( ) ;
	void processRight( ) ;
	bool processDown( ) ;

	void addFigure( ) ;
	void clearFigure( ) ;

private:
	const int cols_len ;
	const int rows_len ;

	Utils::Field_utils utils ;

	Figure figure ;

	Utils::fig_place fig_pos ;
	Utils::fig_place fig_pos_old ;

	addFigureInterface *addfig ;
	std::vector< std::vector<char> > field ;
};


}
#endif

