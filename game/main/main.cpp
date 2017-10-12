#if 1
#include "ncurses/ncmainwindow.h"

#include <iostream>
#include <vector>
#include <random>
#include <stdint.h>
#include <cassert>
#include <cstdio>

#include "log/Log.h"

#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <termios.h>
#include <string.h>

void off( struct termios &t ) {
	tcgetattr( STDIN_FILENO, &t ) ;
	t.c_lflag &= ~ICANON ;
	t.c_lflag &= ~ECHO ;
	tcsetattr( STDIN_FILENO, TCSANOW, &t ) ;
}

void on( struct termios &t ) {
	tcgetattr( STDIN_FILENO, &t ) ;
	t.c_lflag |= ICANON ;
	t.c_lflag |= ECHO ;
	tcsetattr( STDIN_FILENO, TCSANOW, &t ) ;
}

int main( void ) {
#if 0
	struct termios t ;
	struct itimerspec period1 ;
	struct itimerspec period2 ;
	struct timespec now ;

	int kbd, tmrd1, tmrd2 ;
	const int maxEpollEvents = 3 ;
	epoll_event events[ maxEpollEvents ] ;
	memset( &events, 0, sizeof( events ) ) ;

	off( t ) ;

	kbd = STDIN_FILENO ;

	clock_gettime( CLOCK_REALTIME, &now ) ;

	tmrd1 = timerfd_create( CLOCK_REALTIME, 0 ) ;
	if( tmrd1 == -1 ) {
		_err << "Error occured with create timerfd" << endl_ ;
		throw ;
	}

	tmrd2 = timerfd_create( CLOCK_REALTIME, 0 ) ;
	if( tmrd2 == -1 ) {
		_err << "Error occured with create timerfd" << endl_ ;
		throw ;
	}

	_wrn << tmrd1 << ", " << tmrd2 << endl_ ;

	period1.it_value.tv_sec  = now.tv_sec + 1 ;
	period1.it_value.tv_nsec = now.tv_nsec ;
	period1.it_interval.tv_sec  = 1 ;
	period1.it_interval.tv_nsec = 0 ; //now.tv_nsec ; //0 ;

	if( timerfd_settime( tmrd1, TFD_TIMER_ABSTIME, &period1, NULL ) == -1 ) {
   		_err << "Error occured with timerfd set time" << endl_ ;
		throw ;
	}

	period2.it_value.tv_sec  = now.tv_sec + 1 ;
	period2.it_value.tv_nsec = now.tv_nsec ;
	period2.it_interval.tv_sec  = 0 ;
	period2.it_interval.tv_nsec = static_cast<long>( 10e7 ) ; //now.tv_nsec ; //0 ;

	if( timerfd_settime( tmrd2, TFD_TIMER_ABSTIME, &period2, NULL ) == -1 ) {
		_err << "Error occured with timerfd set time" << endl_ ;
		throw ;
	}

	int epolld = ::epoll_create( maxEpollEvents ) ;

	epoll_event evkb ;
	evkb.data.fd = kbd ;
	evkb.events = EPOLLIN | EPOLLET ;

	if (::epoll_ctl( epolld, EPOLL_CTL_ADD, kbd, &evkb ) ) {
		_err << "Error occured while added write signal to epoll" << endl_ ;
		throw std::runtime_error("rror occured while added write signal to epoll");
	}

	epoll_event evtmr1 ;
	evtmr1.data.fd = tmrd1 ;
	evtmr1.events = EPOLLIN | EPOLLET ;

	if (::epoll_ctl( epolld, EPOLL_CTL_ADD, tmrd1, &evtmr1 ) ) {
		_err << "Error occured while added write signal to epoll" << endl_ ;
		throw std::runtime_error("rror occured while added write signal to epoll");
	}

//	epoll_event evtmr2 ;
//	evtmr2.data.fd = tmrd2 ;
//	evtmr2.events = EPOLLIN | EPOLLET ;

//	if (::epoll_ctl( epolld, EPOLL_CTL_ADD, tmrd2, &evtmr2 ) ) {
//		_err << "Error occured while added write signal to epoll" << endl_ ;
//		throw std::runtime_error("rror occured while added write signal to epoll");
//	}

	bool quit = 0 ;
	while( !quit ) {
		int nfds = ::epoll_wait( epolld, events, maxEpollEvents,  -1 ) ;
	
		for( auto i = 0; i < nfds; i++ ) {
			if( events[ i ].data.fd == kbd && ( events[ i ].events & EPOLLPRI || events[ i ].events & EPOLLIN ) ){
				char ch[ 10 ] ;
	
				int n = read( 0, &ch, 10 ) ;

				std::cout << "readed only " << n << " bytes" << std::endl ;
				std::cout << "was pressed button  : " ;
				for( auto k = 0; k < n; ++k ) {
					std::cout << ch[ k ]  << " with code " << ( int ) ch[ k ] << std::hex << " " << (int) ch[ k ] << std::dec <<  std::endl ;
				}
				if( n == 1 && ch[ 0 ] == 27) {
					quit = 1 ;
				}

				static const std::string key_up( "\x1b\x5b\x41" ) ;

				std::string str( ch, n ) ;

				if( str == key_up ) {
					_wrn << "KEY_UP PRESSED" << endl_ ;
				}

			} else if( events[ i ].data.fd == tmrd1 && ( events[ i ].events & EPOLLPRI || events[i].events & EPOLLIN ) ){
				static int cnt = 0 ;
				std::cout << "Process timer1 " << cnt++ << std::endl ;
				uint64_t exp ;
				int s = read( tmrd1, &exp, sizeof(uint64_t));

				struct itimerspec suxx ;
				int l = timerfd_gettime( tmrd1, &suxx ) ;
				_inf << suxx.it_value.tv_sec << "," << suxx.it_value.tv_nsec << endl_ ;
				_inf << suxx.it_interval.tv_sec << "," << suxx.it_interval.tv_nsec << endl_ ;

				if( cnt == 10 ) {
					std::cout << "Speed Up to 2x " << std::endl ;

					period1.it_interval.tv_sec  = 0 ;
					period1.it_interval.tv_nsec = 5*10e7 ;
					if( timerfd_settime( tmrd1, TFD_TIMER_ABSTIME, &period1, NULL ) == -1 ) {
						_err << "Error occured with timerfd set time" << endl_ ;
						throw ;
					}
				} else if( cnt == 20 ) {
					
					std::cout << "Speed Up to 4x " << std::endl ;

					period1.it_interval.tv_sec  = 0 ;
					period1.it_interval.tv_nsec = 2.5*10e7 ;
					if( timerfd_settime( tmrd1, TFD_TIMER_ABSTIME, &period1, NULL ) == -1 ) {
						_err << "Error occured with timerfd set time" << endl_ ;
						throw ;
					}
				
				}


			} else if( events[ i ].data.fd == tmrd2 && ( events[ i ].events & EPOLLPRI || events[i].events & EPOLLIN ) ){
				static int cnt = 0 ;
				std::cout << "Process timer2 " << cnt++ << std::endl ;
				uint64_t exp ;
				int s = read( tmrd2, &exp, sizeof(uint64_t));

				struct itimerspec suxx ;
				int l = timerfd_gettime( tmrd1, &suxx ) ;
				_inf << suxx.it_value.tv_sec << "," << suxx.it_value.tv_nsec << endl_ ;
				_inf << suxx.it_interval.tv_sec << "," << suxx.it_interval.tv_nsec << endl_ ;


//				period.it_interval.tv_sec  =  0 ;
//				period.it_interval.tv_nsec = 500000000; //now.tv_nsec ; //0 ;

//				if( timerfd_settime( tmrd, TFD_TIMER_ABSTIME, &period, NULL ) == -1 ) {
//					_err << "Error occured with timerfd set time" << endl_ ;
//					throw ;
//				}

			}
		}
	}


	on( t ) ;

	close( tmrd1 ) ;
	close( tmrd2 ) ;
	close( epolld ) ;


	return 1 ;

#endif

	const int figlen = 4 ;


	// logging in files, for ncurses
	std::ofstream fslog( "log.txt", std::fstream::out ), fserr( "err.txt", std::fstream::out ) ;

	std::streambuf *backuplog = std::cout.rdbuf( ) ;
	std::streambuf *backuperr = std::cerr.rdbuf( ) ;

	std::cout.rdbuf( fslog.rdbuf( ) ) ;
	std::cerr.rdbuf( fserr.rdbuf( ) ) ;

#ifdef GAME_WIN_NCURSES
	NcMainWindow win( figlen ) ;
	win.run( ) ;
#elif defined( GAME_WIN_QT )
	#error "Game with Qt unsupported now"
#endif

	std::cout.rdbuf( backuplog ) ;
	std::cerr.rdbuf( backuperr ) ;

	return 0 ;
}

#else

       #include <sys/timerfd.h>
       #include <time.h>
       #include <unistd.h>
       #include <stdlib.h>
       #include <iostream>
       #include <stdio.h>
       #include <stdint.h>        /* Definition of uint64_t */

       #define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)

       static void
       print_elapsed_time(void)
       {
           static struct timespec start;
           struct timespec curr;
           static int first_call = 1;
           int secs, nsecs;

           if (first_call) {
               first_call = 0;
               if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
                   handle_error("clock_gettime");
           }

           if (clock_gettime(CLOCK_MONOTONIC, &curr) == -1)
               handle_error("clock_gettime");

           secs = curr.tv_sec - start.tv_sec;
           nsecs = curr.tv_nsec - start.tv_nsec;
           if (nsecs < 0) {
               secs--;
               nsecs += 1000000000;
           }
           printf("%d.%03d: ", secs, (nsecs + 500000) / 1000000);
       }

       int
       main(int argc, char *argv[])
       {
           struct itimerspec new_value;
           int max_exp, fd;
           struct timespec now;
           uint64_t exp, tot_exp;
           ssize_t s;

           if ((argc != 2) && (argc != 4)) {
               fprintf(stderr, "%s init-secs [interval-secs max-exp]\n",
                       argv[0]);
               exit(EXIT_FAILURE);
           }

           if (clock_gettime(CLOCK_REALTIME, &now) == -1)
               handle_error("clock_gettime");

           /* Create a CLOCK_REALTIME absolute timer with initial
              expiration and interval as specified in command line */
/*
		   new_value.it_value.tv_sec = now.tv_sec + atoi(argv[1]);
           new_value.it_value.tv_nsec = now.tv_nsec;
           if (argc == 2) {
               new_value.it_interval.tv_sec = 0;
               max_exp = 1;
           } else {
               new_value.it_interval.tv_sec = atoi(argv[2]);
               max_exp = atoi(argv[3]);
           }
           new_value.it_interval.tv_nsec = 0;

           fd = timerfd_create(CLOCK_REALTIME, 0);
           if (fd == -1)
               handle_error("timerfd_create");

           if (timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_value, NULL) == -1)
               handle_error("timerfd_settime");
*/

		   new_value.it_value.tv_sec = now.tv_sec + 1;
		   new_value.it_value.tv_nsec = now.tv_nsec;

		   new_value.it_interval.tv_sec = 1;
		   new_value.it_interval.tv_nsec = 0;

		   max_exp = 10 ;


           struct itimerspec new_value2;

		   new_value2.it_value.tv_sec = now.tv_sec + 1;
		   new_value2.it_value.tv_nsec = now.tv_nsec;

		   new_value2.it_interval.tv_sec = 0;
		   new_value2.it_interval.tv_nsec = 5*10e7;



		   fd = timerfd_create(CLOCK_REALTIME, 0);
		   if (fd == -1)
			   handle_error("timerfd_create");

		   if (timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_value, NULL) == -1)
			   handle_error("timerfd_settime");


		   int fd2 = timerfd_create(CLOCK_REALTIME, 0);
		   if (fd2 == -1)
			   handle_error("timerfd_create");

		   if (timerfd_settime(fd2, TFD_TIMER_ABSTIME, &new_value2, NULL) == -1)
			   handle_error("timerfd_settime");


           print_elapsed_time();
           printf("timer started\n");

           for (tot_exp = 0; tot_exp < max_exp;) {
               //s = read(fd, &exp, sizeof(uint64_t));
			   s = read(fd2, &exp, sizeof(uint64_t));
               if (s != sizeof(uint64_t))
                   handle_error("read");

			   struct itimerspec suxx ;
			   int l = timerfd_gettime( fd2, &suxx ) ;
			   std::cout << suxx.it_value.tv_sec << "," << suxx.it_value.tv_nsec << std::endl ;
			   std::cout << suxx.it_interval.tv_sec << "," << suxx.it_interval.tv_nsec << std::endl ;


               tot_exp += exp;
               print_elapsed_time();
               printf("read: %llu; total=%llu\n",
                       (unsigned long long) exp,
                       (unsigned long long) tot_exp);
           }

           exit(EXIT_SUCCESS);
       }
#endif
