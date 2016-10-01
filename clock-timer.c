/* This is the source code for the final assignment
 * of subject: Embedded and Real-time Systems.
 * Subject is part of the 8th semester of EECE class in AUTH.
 * 
 * We are tasked with developing a bare-bones real-time task
 * capable of counting time and also displaying the current local time
 * in a readable format using the prex Operating System (or any other RTOS).
 * 
 * Author : Kopsacheilis Charalampos, 7909
 * Date   : 20 / 9 / 2016
 */

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/prex.h>
#include <sys/signal.h>
#include <errno.h>


#define SLEEP_INTERVAL 150 /* 150 msec */

/* We must now try to implement a kbhit() function in prex
 * in order to create a pseudo-asynchronous connection.
 */
 int tcgetattr(int, struct termios *);
 int tcsetattr(int, int, const struct termios *);
 void changemode(int);
 u_long get_time(void);
 void printTime(void);


 long extra_hours, extra_minutes;
 time_t last_sreset;
 
 /* Some flags to make switching between modes easier. */
  char SEC_ZERO, INITIAL_START, REGULAR, STOP;
 

/* main() is the function manifestation of our programm.
 * both the clock and/or timer functions are called inside
 * the main() if certain keys are pressed
 */
int main(void){
		time_t Unix_time;
		time_t SW_start_time, elapsed_time, last_elapsed_time;
		device_t ttydev;
		int hours,minutes,seconds;
		char input;
		
		
		/* Setting variables to 0 */
		Unix_time = 0;
		SW_start_time = 0;
		elapsed_time = 0;
		last_elapsed_time = 0;
		extra_hours = 0;
		extra_minutes = 0;
		SEC_ZERO = 0;
		last_sreset = 0;
		INITIAL_START = 0;
		REGULAR = 0;
		STOP = 0;
		
		
		/* Putting the tty into "raw" mode */
		changemode(1);
		
		/* The time() loop */
		TIME:
		printf("\r                                                   ");
		while(1){
			/* Checking the tty input buffer for any input */
			device_open("tty", 0 , &ttydev);
			device_ioctl(ttydev, TIOCINQ, &input);
			device_close(ttydev);
			
			/* Checking input and following the appropiate actions */
			if (input > 0){
				input = getchar();
				if (input == 't'){
					goto STOPWATCH;
				}
				if (input == 'h')
					extra_hours += 3600;
				if (input == 'm')
					extra_minutes += 60;
				if (input == 'z')
					SEC_ZERO = 1;
			}
			printTime();
			
			/* Sleep for a while */
			/* So the CPU isn't used all the time */
			timer_sleep(SLEEP_INTERVAL, NULL);
		}
		
		
		INITIAL_SW:
		/* STOPWATCH label for the goto statement. */
		if (INITIAL_START)
			SW_start_time = get_time();
		STOPWATCH:
		printf("\r                                                   ");
		while(1){
			
			/* Waiting for input */
			device_open("tty", 0 , &ttydev);
			device_ioctl(ttydev, TIOCINQ, &input);
			device_close(ttydev);
			
			if (input > 0){
				input = getchar();
				
				/* Toggle */
				if (input == 't')
					goto TIME;
					
				/* First time starting the StopWatch */
				if ((input == 's') && (!INITIAL_START)){
					INITIAL_START = 1;
					goto INITIAL_SW;
				}
				
				/* Stopping the SW at current display */
				if ((input == 's') && (!STOP)){
						STOP = 1;
						last_elapsed_time = elapsed_time;
				}
				/* Re - starting the SW. */
				else if ((input == 's') && (STOP)) {
					STOP = 0;
					goto INITIAL_SW;
				}
				
				/* Pausing the SW at current display */
				if ((input == 'p') && (!STOP)){
					STOP = 1;
					last_elapsed_time = elapsed_time;
				}
				else if ((input == 'p') && (STOP)){
					STOP = 0;
				}
				
				/* Reset SW */
				if (input == 'r'){
					STOP = 0;
					goto INITIAL_SW;
				}
				
				/* Quit the programm */
				if (input == 'q')
					break;
			}	
			
			if (INITIAL_START && (!STOP))
				elapsed_time = get_time() - SW_start_time;
			else
				elapsed_time = last_elapsed_time;
			seconds = elapsed_time % 60;
			minutes = (elapsed_time / 60 ) % 60;
			hours = (elapsed_time / 3600) % 24;
			
			
			/* Print time. */
			printf("\r%02d:%02d:%02d",hours,minutes,seconds);
			
			/* Sleep for a while */
			/* So that CPU isn't used all the time */
			timer_sleep(SLEEP_INTERVAL, NULL);
		}
		
		
		/* Undoing changemode(1) */
		changemode(0);
		
		printf("\nGood job!\n");
		printf("input = %c\n", input);
		
		return 0;
}

/* Prints the local time in HH:MM:SS format */
void printTime(void){
	int hours,minutes,seconds;
	time_t Unix_time = get_time();
	time_t zero_time;
	
	if (SEC_ZERO){
		last_sreset = get_time();
		SEC_ZERO = 0;
	}
	hours = ((Unix_time + extra_hours + extra_minutes - (last_sreset % 60))/ 3600) % 24;
	minutes = ((Unix_time + extra_hours + extra_minutes - (last_sreset % 60))/ 60) % 60;
	seconds = ((Unix_time + extra_hours + extra_minutes) - (last_sreset % 60)) % 60;
	printf("\rCurrent time : %02d:%02d:%02d",hours,minutes,seconds);
}

/* The changemode() function is copied from
 * www.cboard.cprogramming.com/c-programming/63166-kbhit-linux.html
 * as suggested by the user: Thantos.
 * tcgetattr and tcsetattr are modified a little bit by me
 * to be user is Real - time task mode.
 */

int
tcgetattr(fd,t)
	struct termios *t;
{
	device_t tty_dev;
	int ret_value;
	
	device_open("tty", 0 , &tty_dev);
	ret_value = device_ioctl(tty_dev, TIOCGETA, t);
	device_close(tty_dev);
	
	return ret_value;
	
	/* return (ioctl(fd, TIOCGETA, t)); */
}

int tcsetattr(fd, opt, t)
	int fd, opt;
	const struct termios *t;
{
	struct termios localterm;
	int ret_value;
	device_t tty_dev;
	
	if (opt & TCSASOFT){
		localterm = *t;
		localterm.c_cflag |= CIGNORE;
		t = &localterm;
	}
	switch (opt & ~TCSASOFT){
		case TCSANOW:
			{
				device_open("tty", 2 , &tty_dev);
				ret_value = device_ioctl(tty_dev, TIOCSETA, t);
				device_close(tty_dev);
				
				return ret_value;
			}
			
		case TCSADRAIN:
			{
				device_open("tty", 2 , &tty_dev);
				ret_value = device_ioctl(tty_dev, TIOCSETAW, t);
				device_close(tty_dev);
				
				return ret_value;
			}
			
		case TCSAFLUSH:
			{
				device_open("tty", 2 , &tty_dev);
				ret_value = device_ioctl(tty_dev, TIOCSETAF, t);
				device_close(tty_dev);
				
				return ret_value;	
			}
		default:
			errno = EINVAL;
			return (-1);	
	}
}

/* Changes the tty "mode" */
/* if dir == 1    ---> "raw" */
/* if dir == 0    ---> "normal" */
void changemode(int dir){
		static struct termios oldt, newt;
		
		if (dir == 1){
			tcgetattr(STDIN_FILENO, &oldt);
			newt = oldt;
			newt.c_lflag &= ~(ICANON | ECHO);
			tcsetattr(STDIN_FILENO, TCSANOW, &newt);
		}
		else{
			tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
		}
}

/* Our build for the get_time() function
 * ALL credit goes to svart.
 * May the force be with you brother.
 */
 u_long get_time(void){
		device_t rtc_dev;
		u_long sec;
		
		device_open("rtc", 0 , &rtc_dev);
		device_ioctl(rtc_dev, RTCIOC_GET_TIME, &sec);
		device_close(rtc_dev);
		
		return sec;
} 
