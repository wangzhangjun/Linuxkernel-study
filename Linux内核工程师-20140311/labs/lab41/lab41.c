#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
void sigroutine(int signo) {
	switch (signo) {
		case SIGALRM:
			printf("Catch a signal -- SIGALRM\n");
			break;
		case SIGVTALRM:
			printf("Catch a signal -- SIGVTALRM\n");
			break;
		case SIGPROF:
			printf("Catch a signal -- SIGPROF\n");
			break;
	}
}

int main() {
	struct itimerval value,ovalue,value2,value3;
	printf("process id is %d \n",getpid());
	signal(SIGALRM, sigroutine);
	signal(SIGVTALRM, sigroutine);
	signal(SIGPROF, sigroutine);
	value.it_value.tv_sec = 1;
	value.it_value.tv_usec = 0;
	value.it_interval.tv_sec = 2;
	value.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &value, &ovalue);

	value2.it_value.tv_sec = 1;
	value2.it_value.tv_usec = 0;
	value2.it_interval.tv_sec = 5;
	value2.it_interval.tv_usec = 0;
	setitimer(ITIMER_VIRTUAL, &value2, &ovalue);

	
	value3.it_value.tv_sec = 1;
	value3.it_value.tv_usec = 0;
	value3.it_interval.tv_sec = 10;
	value3.it_interval.tv_usec = 0;
	setitimer(ITIMER_PROF, &value3, &ovalue);

	for (;;) ;
}
