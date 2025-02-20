// file: callcenter.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>

int main(int argc, char **argv)
{
	int num_agents = 2;
	if (argc > 1)
		num_agents = atoi(argv[1]);
	printf("Starting a call center with %d agents.\n", num_agents);

	// TODO: OS -- CRAETE NAMED SEMAPHORE HERE
	// Create named semaphore "callcenter" with initial value = num_agents
	sem_t *sem = sem_open("callcenter", O_CREAT, 0644, num_agents);
	if (sem == SEM_FAILED)
	{
		perror("sem_open failed");
		exit(EXIT_FAILURE);
	}
	//

	int semval;
	while (1)
	{

		// TODO: OS -- PLACE CURRENT VALUE OF SEMAPHORE IN 'semval' HERE
		// Get the current value of the semaphore
		if (sem_getvalue(sem, &semval) == -1)
		{
			perror("sem_getvalue failed");
		}
		//

		printf("There are %d agents available now.\n", semval);
		sleep(3);
	}
}
