#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

# define MAX_SEQUENCE 100

struct shared_data {
	int fib_sequence[MAX_SEQUENCE];
	int sequence_size;
};

int main() {
	long long fib1 = 0;
	long long fib2 = 1;
	long long fib3 = fib1 + fib2;
	int input;
	int input_orig;
	int fd;
	int res;
	void *addr;
	char *shm_name = "/shm_name";
	struct shared_data data;
	
	printf("Enter the number of a Fibonacci Sequence:\n");

	// Error checking: below zero input
	if(scanf("%d", &input) < 0) {
		printf("%s", "Input Error: Must enter a number larger than 0!");
		exit(0);
	}

	// Error checking it is less than MAX_SEQUENCE
	if(input > MAX_SEQUENCE) {
		printf("%s %d\n", "Input Error: cannot enter number larger than", MAX_SEQUENCE);
		exit(0);
	}

	input_orig = input;

	// fork()
	pid_t pid = fork();
	if (pid == 0) {
		// create the shared memory object
     	fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
     	if (fd == -1) {
     		printf("%s\n", "Error creating shared memory.");
     		exit(0);
     	}

     	/* configure the size of the shared memory object */
     	res = ftruncate(fd, MAX_SEQUENCE);
     	if (res == -1) {
			perror("ftruncate");
			exit(0);
		}

     	/* memory map the shared memory object */
     	addr = mmap(NULL, MAX_SEQUENCE, PROT_WRITE, MAP_SHARED, fd, 0);
     	if (addr == MAP_FAILED) {
			perror("mmap");
			exit(0);
		}



     	printf("Child is producing the Fibonacci Sequence...\n");

     	// Produces squence and saves to shared memory
		data.fib_sequence[0] = 0;
		data.fib_sequence[1] = 1;
		input--;
		for(int i = 2; i < i + input; i++) {
			fib3 = fib1 + fib2;
			data.fib_sequence[i] = fib3;
			fib1 = fib2;
			fib2 = fib3;
			input--;
		}

		// printing sequence from shared memory
		printf("Printing from shared memory: ");
		for(int i = 0; i < input_orig + 1; i++) {
			if (i == input_orig)
				printf("%d\n", data.fib_sequence[i]);
			else
				printf("%d, ", data.fib_sequence[i]);
		}
	}
	else {
		// Parent wait() for child to complete
		wait(NULL);
		printf("\n");
	}

	// Remove shared memory object
	// mmap cleanup
	res = munmap(addr, MAX_SEQUENCE);
	if (res == -1)
	{
		perror("munmap");
		return 40;
	}

	// shm_open cleanup
	fd = shm_unlink(shm_name);
	if (fd == -1)
	{
		perror("unlink");
		return 100;
	}

	return 0;
}