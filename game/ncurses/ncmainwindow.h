#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <ncurses.h>

#include <mutex>
#include <fstream>

#include "utils/box_field.h"
#include "figures/addFigureInterface.h"


namespace Figures {
	class Figure ;
	class GameField ;
}


class NcMainWindow : public Figures::addFigureInterface {
public:
	NcMainWindow ( int figlen ) ;
	~NcMainWindow( ) ;

	void run( ) ;

	void drowGameField( const std::vector< std::vector<char> > &field ) ;
	void drowNextFig( const std::vector< std::vector<char> > &nextfig ) ;
	void drowScores( int scores, int level ) ;


	static inline long tonc( char e ) {
	if( e ) return ACS_CKBOARD ;
	return ' ' ;
}
private:

private:
	static constexpr int maxcol = 80 ;
	static constexpr int maxrow = 35 ;

	struct boxfield {
		int pos_col ;
		int pos_row ;

		int cols_len ;
		int rows_len ;
	};

	int figlen ;

	Figures::GameField *gameField ;

	Utils::box_field game_field_pos ;
	Utils::box_field next_fig_pos ;
	Utils::box_field inf_field_pos ;

    bool quit ;

	std::mutex m ;

	WINDOW *game_win ;
	WINDOW *nextfig_win ;
	WINDOW *inf_win ;
};


#endif


