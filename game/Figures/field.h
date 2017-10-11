#ifndef __FIG_CNTRL_H__
#define __FIG_CNTRL_H__

#include "figure.h"
#include "box_field.h"
#include "field_utils.h"
#include "addFigureInterface.h"

#include <vector>
#include <thread>
#include <mutex>


namespace Figures {

enum Actions {
	Action_rotate,
	Action_left,
	Action_right,
	Action_down,
	Action_unrotate,


	Action_empty,
	Action_last
} ;

class GameField {
public:
	GameField( addFigureInterface *addfig, int figure_len, int maxcols, int maxrows ) ;	// maxcols and maxrows from ncurses!
	~GameField( ) ;

	void Start( ) ;
	void Stop ( ) ;

	void ProcessAction( Actions action ) ;

	std::vector< std::vector<char> > &getField( ) ;	// all game field with figure

private:
	void generateNewFig( ) ;

	void processFilledRows( ) ;

	void processRotate( ) ;
	void processLeft( ) ;
	void processRight( ) ;
	void processDown( ) ;

	void addFigure( ) ;
	void clearFigure( ) ;

	void figureFallThread( ) ;

private:
	int cols_len ;
	int rows_len ;

	Field_utils utils ;

	Figure figure ;

	box_field box_pos ;
	box_field box_pos_old ;

	fig_place fig_pos ;
	fig_place fig_pos_old ;


	addFigureInterface *addfig ;

	std::mutex m ;
	std::thread *t1 ;
	bool activeThread ;

	std::vector< std::vector<char> > field ;
};


}
#endif

