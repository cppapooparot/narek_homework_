#include <iostream>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <cstdlib>

#define N 5

void random_sleep()
{
	usleep((rand() % 401 + 100) * 1000);
}

int main()
{
	int shm_fd = shm_open("/philosopher_sticks", O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, N * sizeof(sem_t));
	sem_t *sticks = static_cast<sem_t*>(mmap(nullptr, N * sizeof(sem_t), PROT_READ | PROT_WRITE,
			       	MAP_SHARED, shm_fd, 0));

	for (int i = 0; i < N; ++i)
	{
		sem_init(&sticks[i], 1, 1);
	}

	for (int i = 0; i < N; ++i)
	{
		if (fork() == 0)
		{
			srand(getpid());
			int left = i;
			int right = (i + 1) % N;

			while (true)
			{
				std::cout << "Philosopher " << i << " is thinking...\n";
				random_sleep();
				sem_wait(&sticks[left]);
				sem_wait(&sticks[right]);

				std::cout << "Philosopher " << i << " is eating...\n";
				random_sleep();

				sem_post(&sticks[left]);
				sem_post(&sticks[right]);

				std::cout << "Philosopher " << i <<
				       	" put down the sticks and started thinking again...\n";
			}
			exit(0);
		}
	}
	for (int i = 0; i < N; ++i)
	{
		wait(nullptr);
	}

	for (int i = 0; i < N; ++i)
	{
		sem_destroy(&sticks[i]);
	}
	munmap(sticks, N * sizeof(sem_t));
	shm_unlink("/philosopher_sticks");

	return 0;
}

