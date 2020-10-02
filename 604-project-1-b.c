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

typedef struct {
	int fib_sequence[MAX_SEQUENCE];
	int sequence_size;
}shared_data;

int main() {
	long long fib1 = 0;
	long long fib2 = 1;
	long long fib3 = fib1 + fib2;
	int input;
	int fd;
	int res;
	void *addr;
	char *shm_name = "/shm_name";
	
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
		printf("0 %llu ", fib3);
		input--;

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

	// mmap cleanup
	res = munmap(addr, MAX_SEQUENCE);
	if (res == -1)
	{
		perror("munmap");
		exit(0);
	}

	// shm_open cleanup
	fd = shm_unlink(shm_name);
	if (fd == -1)
	{
		perror("unlink");
		exit(0);
	}

	return 0;
}