#ifndef __FIGURE_H__
#define __FIGURE_H__

#include <vector>

namespace Figures {

class Figure {
public:
	Figure( int fig_len ) ;
	~Figure( ) ;

	void Generate( ) ;
	void Rotate( ) ;
	void UnRotate( ) ;

	const std::vector< std::vector<char> >& getCurrFig( ) const ;
	const std::vector< std::vector<char> >& getNextFig( ) const ;

private:
	int len ;
	std::vector< std::vector<char> > currFig ;
	std::vector< std::vector<char> > nextFig ;

	void resetFig( std::vector< std::vector<char> > &fig ) ;
	void generateFig( std::vector< std::vector<char> > &fig ) ;
} ;

}

#endif

