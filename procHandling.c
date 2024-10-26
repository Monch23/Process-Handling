#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <sys/mman.h>

int *sigUsr1;

void delay(const char *str) {
	const char *ptr = str;

	while (*ptr) {
		usleep(35000);
		putchar(*ptr);
		fflush(stdout);
		++ptr;
	}

	return;
}

void Alarm(int signum) {
	printf("\n*** SIGUSR1 signal current count <%d> *** \n\n", *sigUsr1);
	return;
}

void sigusr1(int signum) {
	(*sigUsr1)++;
	return;
}

int main() {
	char apply[10];
	sigUsr1 = (int*)mmap(NULL, 1024, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	pid_t pid = fork();
	
	if (pid == 0) { // child
		signal(SIGUSR1, sigusr1);
		pause();
		while(1);
	} else if (pid > 0) { // parent
		while (1) {
			usleep(65000);

			signal(SIGALRM, Alarm);
			raise(SIGALRM);
			delay("Do you want to kill a child (y/n) ? \n");
			scanf("%s", apply);

			if (!strcmp(apply, "y")) {
				kill(pid, SIGKILL);
				delay("Child process terminated \n");
				exit(0);
			} else if (!strcmp(apply, "n")) {
				int cycleCnt = 0;
				delay("How many time send signal SIGUSR1 ? \n");
				scanf("%d", &cycleCnt);

				for (int i = 0; i < cycleCnt; ++i) {
					kill(pid, SIGUSR1);
					usleep(50000);
				}
			} else {
				puts("<--- Input valid answer ---> \n");
			}
		}
	}

	return 0;
}
