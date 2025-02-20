// file: customer.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <semaphore.h>

int rand_int(int n)
{
	// Generate random integer number between [1, n]
	int r = rand();
	return (r % n) + 1;
}

int main(int argc, char **argv)
{
	printf("Starting customer\n");

	// TODO: OS -- OPEN NAMED SEMAPHORE HERE
	// Open the named semaphore "callcenter"
	sem_t *sem = sem_open("callcenter", 0);
	if (sem == SEM_FAILED)
	{
		perror("sem_open failed");
		exit(EXIT_FAILURE);
	}
	//

	while (1)
	{
		// Customer will sleep between 1-3 seconds before placing the next phone call
		int sleep_time = rand_int(3);
		printf("Do something else for %d minutes\n\n", sleep_time);
		sleep(sleep_time);
		printf("Next customer calls the call center, press ten buttons, and listens to silly music.\n");
		time_t t0 = time(NULL);
		// Wait for an agent

		// TODO: OS -- LOCK SEMAPHORE HERE
		// Wait for an available agent
		if (sem_wait(sem) == -1)
		{
			perror("sem_wait failed");
			exit(EXIT_FAILURE);
		}
		//

		time_t t = time(NULL) - t0;
		// An agent accepts the call, using it for 3-5 seconds.
		int call_time = rand_int(3) + 2;
		printf("After waiting for %ld minutes, an agent accepts the call.  Talk for %d minutes.\n", t, call_time);
		sleep(call_time);
		// Customer hangs up the phone

		// TODO: OS -- UNLOCK SEMAPHORE HERE
		// Hang up: release the agent
		if (sem_post(sem) == -1)
		{
			perror("sem_post failed");
			exit(EXIT_FAILURE);
		}
		//

		printf("Customer ends the call.\n");
	}
}
