#include "common_utils.h"
#include "log/Log.h"

#include <unistd.h>
#include <string.h>
#include <sys/timerfd.h>

using namespace Utils ;

common_utils::common_utils( ) 
	: tmr_fd( -1 )
	, kbd_fd( -1 )
	, epollfd( -1 )

{
	memset( &events, 0, sizeof( events ) ) ;

	delayus = 500000 ; // 1 sec by default
	change_delay = false ;
}

common_utils::~common_utils( ) {

}

void common_utils::simpleReadKbd_start( ) {
	tcgetattr( STDIN_FILENO, &t ) ;
	t.c_lflag &= ~ICANON ;
	t.c_lflag &= ~ECHO ;
	tcsetattr( STDIN_FILENO, TCSANOW, &t ) ;
}


void common_utils::simpleReadKbd_stop ( ) {
	tcgetattr( STDIN_FILENO, &t ) ;
	t.c_lflag |= ICANON ;
	t.c_lflag |= ECHO ;
	tcsetattr( STDIN_FILENO, TCSANOW, &t ) ;
}

void common_utils::startGameControl( ) {
	struct timespec now ;
	clock_gettime( CLOCK_REALTIME, &now ) ;
	
	kbd_fd = STDIN_FILENO ;
	tmr_fd = timerfd_create( CLOCK_REALTIME, 0 ) ;
	if( tmr_fd == -1 ) {
		_err << "Error occured with create timerfd" << endl_ ;
		throw ;
	}

	const long delay = static_cast<long>( delayus * 10e2 ) ; // mcs to 10ns

	period.it_value.tv_sec  = now.tv_sec + 1 ;
	period.it_value.tv_nsec = now.tv_nsec ;
	period.it_interval.tv_sec  = 0 ;
	period.it_interval.tv_nsec = delay ; 

	if( timerfd_settime( tmr_fd, TFD_TIMER_ABSTIME, &period, NULL ) == -1 ) {
		_err << "Error occured with timerfd set time" << endl_ ;
		throw ;
	}

	epollfd = ::epoll_create( maxEpollEvents ) ;

	epoll_event evkb ;
	evkb.data.fd = kbd_fd ;
	evkb.events = EPOLLIN | EPOLLET ;

	if (::epoll_ctl( epollfd, EPOLL_CTL_ADD, kbd_fd, &evkb ) ) {
		_err << "Error occured while added write signal to epoll" << endl_ ;
		throw ;
	}

	epoll_event evtmr ;
	evtmr.data.fd = tmr_fd;
	evtmr.events = EPOLLIN | EPOLLET ;

	if (::epoll_ctl( epollfd, EPOLL_CTL_ADD, tmr_fd, &evtmr ) ) {
		_err << "Error occured while added write signal to epoll" << endl_ ;
		throw ;
	}

	simpleReadKbd_start( ) ;
}

void common_utils::stopGameControl( ) {
	::close( tmr_fd  ) ;
	::close( epollfd ) ;

	simpleReadKbd_stop( ) ;
}

Actions common_utils::getAction( ) {
	Actions ret ;

	int nfds = ::epoll_wait( epollfd, events, maxEpollEvents,  -1 ) ;

	_inf << "epoll exit, nfds " << nfds << endl_ ;
	for( auto i = 0; i < nfds; i++ ) {
		if( events[ i ].data.fd == kbd_fd && ( events[ i ].events & EPOLLPRI || events[ i ].events & EPOLLIN ) ){
			char ch[ 10 ] ;
			int len = read( kbd_fd, &ch, 10 ) ;

			return parseReadedKey( ch, len ) ;
		} else if( events[ i ].data.fd == tmr_fd && ( events[ i ].events & EPOLLPRI || events[i].events & EPOLLIN ) ){
			uint64_t exp ;
			int s = read( tmr_fd, &exp, sizeof(uint64_t));
			if( change_delay ) {
				const long delay = static_cast<long>( delayus * 10e2 ) ; // mcs to 10ns
				struct timespec now ;

				_err << "Set New Delay " << delayus << " mcs " << endl_ ;

				clock_gettime( CLOCK_REALTIME, &now ) ;
				now.tv_nsec += delay ;
				if( now.tv_nsec >= 10e8 ) {
					now.tv_nsec -= 10e8 ;
					now.tv_sec += 1 ;
				}

				period.it_value = now ;
				period.it_interval.tv_sec  = 0 ;
				period.it_interval.tv_nsec = delay ; 

				if( timerfd_settime( tmr_fd, TFD_TIMER_ABSTIME, &period, NULL ) == -1 ) {
					_err << "Error occured with timerfd set time" << endl_ ;
					throw ;
				}
				change_delay = false ;
			}
			return Actions::Action_down ;
		}
	}
}


namespace {
	static const std::string key_up   ( "\x1b\x5b\x41" ) ;
	static const std::string key_down ( "\x1b\x5b\x42" ) ;
	static const std::string key_right( "\x1b\x5b\x43" ) ;
	static const std::string key_left ( "\x1b\x5b\x44" ) ;

	static const std::string key_esc ( "\x1b" ) ;
	static const std::string key_q ( "q" ) ;
	static const std::string key_Q ( "Q" ) ;
	static const std::string key_F2 ( "\x1b\x4f\x51" ) ;
}


Actions common_utils::parseReadedKey( char *ch, int len ) {
	
	std::string str( ch, len ) ;

	if( str == key_up )         return Utils::Action_rotate ;
	else if( str == key_down )  return Utils::Action_down ;
	else if( str == key_left )  return Utils::Action_left ;
	else if( str == key_right ) return Utils::Action_right ;
	
	else if( str == key_esc ||
			 str == key_q ||
			 str == key_Q || 
			 str == key_F2 )    return Utils::Action_exit ;

	return Utils::Action_empty ;
}


void common_utils::setGameDelay( unsigned long delayus ) {
	if( this->delayus != delayus ) {
		this->delayus = delayus ;
		change_delay = true ;
	}
}

