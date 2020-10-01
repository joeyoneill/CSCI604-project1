#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main() {
	int fib1 = 0;
	int fib2 = 1;
	int fib3 = fib1 + fib2;
	int input;

	printf("Enter the number of a Fibonacci Sequence:\n");

	// Error checking: below zero input
	if(scanf("%d", &input) < 0) {
		printf("%s", "Input Error: Must enter a number larger than 0!");
		exit(0);
	}

	// fork()
	pid_t pid = fork();
	if (pid == 0) {
		printf("0 %d ", fib3);
		while (input > 0) {
			fib3 = fib1 + fib2;
			printf("%d ", fib3);
			fib1 = fib2;
			fib2 = fib3;
			input--;
		}
	}
	else {
		// Parent wait() for child to complete
		wait(NULL);
		printf("\n");
	}
	return 0;
}