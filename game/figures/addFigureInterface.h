#ifndef __ADD_FIGURE_INTERFACE_H__
#define __ADD_FIGURE_INTERFACE_H__

#include <vector>

namespace Figures {

	class addFigureInterface {
	public:
		virtual void drowGameField( const std::vector< std::vector<char> > &field ) = 0 ;
		virtual void drowNextFig( const std::vector< std::vector<char> > &nextfig ) = 0 ;
		virtual void drowScores( int scores, int level ) = 0 ;
	};

} 


#endif

