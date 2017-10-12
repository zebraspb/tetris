#ifndef __FIELD_UTILS_H__
#define __FIELD_UTILS_H__

#include <vector>


namespace Utils {
	
	class fig_place ;


	class Field_utils {
	public:
		Field_utils( std::vector< std::vector<char> > &field, int figlen ) ;
		~Field_utils( ) ;

		bool checkFigureLeft  ( fig_place &fig_pos, std::vector< std::vector<char> > &fig ) ;
		bool checkFigureRight ( fig_place &fig_pos, std::vector< std::vector<char> > &fig ) ;
		bool checkFigureBottom( fig_place &fig_pos, std::vector< std::vector<char> > &fig ) ;

		bool checkFieldOverFlow( ) { return fieldOverFlow ; }

		bool checkColBorders  ( fig_place &fig_pos ) ;
		bool checkRowBorder   ( fig_place &fig_pos ) ;

		void addScores( int lines ) ;

		unsigned int getDelay( ) ;
		int getlevel( ) { return level ; }
		int getscores( ) { return scores ; }


	private:
		static constexpr int max_level = 10 ;
		static const unsigned int delays[ max_level ] ;

		std::vector< std::vector<char> > &field ;

		bool fieldOverFlow ;

		int figlen ;
		int scores_mult ;
		int level ;
		int level_mult ;
		int scores ;

	};


}

#endif

