#ifndef __COMMON_UTILS_H__
#define __COMMON_UTILS_H__

#include <sys/epoll.h>
#include <termios.h>
#include <time.h>

namespace Utils {

	enum Actions {
		Action_rotate,
		Action_left,
		Action_right,
		Action_down,
		Action_unrotate,
	
		Action_exit,
		Action_empty,
		Action_last
	} ;

	class common_utils {
	public:
		 common_utils( ) ;
		~common_utils( ) ;

		void startGameControl( ) ;
		void stopGameControl  ( ) ;

		Actions getAction( ) ;

	private:
		Actions parseReadedKey( char *ch, int len ) ;
		void simpleReadKbd_start( ) ;
		void simpleReadKbd_stop ( ) ;

	private:
		static constexpr int maxEpollEvents = 2 ;
		epoll_event events[ maxEpollEvents ] ;

		struct termios t ;

		struct itimerspec period ;

		int epollfd ;
		int kbd_fd ;
		int tmr_fd ;
	};
}


#endif

