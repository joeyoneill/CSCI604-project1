#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

# define MAX_SEQUENCE 100

typedef struct {
	int fib_sequence[MAX_SEQUENCE];
	int sequence_size;
}shared_data;

int main() {
	long long fib1 = 0;
	long long fib2 = 1;
	long long fib3 = fib1 + fib2;
	int input;

	printf("Enter the number of a Fibonacci Sequence:\n");

	// Error checking: below zero input
	if(scanf("%d", &input) < 0) {
		printf("%s", "Input Error: Must enter a number larger than 0!");
		exit(0);
	}

	// Error checking it is less than MAX_SEQUENCE
	if(input > MAX_SEQUENCE) {
		printf("%s\n", "Input Error: cannot enter number larger than " + MAX_SEQUENCE);
		exit(0);
	}

	// fork()
	pid_t pid = fork();
	if (pid == 0) {
		printf("0 %llu ", fib3);
		while (input > 0) {
			fib3 = fib1 + fib2;
			printf("%llu ", fib3);
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