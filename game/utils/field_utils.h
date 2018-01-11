#ifndef __FIELD_UTILS_H__
#define __FIELD_UTILS_H__

#include <vector>


namespace Utils {
	
	class fig_place ;


	class Field_utils {
	public:
		Field_utils( std::vector< std::vector<char> > &field, int figlen ) ;
		~Field_utils( ) ;

		bool checkFigureLeft  ( const fig_place &fig_pos, const std::vector< std::vector<char> > &fig ) const ;
		bool checkFigureRight ( const fig_place &fig_pos, const std::vector< std::vector<char> > &fig ) const ;
		bool checkFigureBottom( const fig_place &fig_pos, const std::vector< std::vector<char> > &fig ) const ;

		bool checkFieldOverFlow( ) const { return fieldOverFlow ; }

		bool checkColBorders  ( const fig_place &fig_pos ) const ;
		bool checkRowBorder   ( const fig_place &fig_pos ) const ;

		void addScores( int lines ) ;

		unsigned int getDelay( ) const ;
		int getlevel( ) const { return level ; }
		int getscores( ) const { return scores ; }


	private:
		static constexpr int max_level = 10 ;
		static const unsigned int delays[ max_level ] ;

		std::vector< std::vector<char> > &field ;

		mutable bool fieldOverFlow ;

		int figlen ;
		int scores_mult ;
		mutable int level ;
		int level_mult ;
		int scores ;

	};


}

#endif

