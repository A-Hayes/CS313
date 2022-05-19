#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	for (int i = 0; i < 3; i++) {
    printf("i: %d - ",i);
		fork();
		printf ("PID: %d\n", getpid());
		wait(0);
	}
  printf("EndPid: %d\n", getpid());
	return 0;
}