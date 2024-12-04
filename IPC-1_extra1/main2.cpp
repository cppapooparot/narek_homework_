#include <iostream>
#include <csignal>
#include <unistd.h>

int usr2_count = 0;

void handle_usr2(int sig) {
	if(sig == SIGUSR2) {
		usr2_count++;
	}
}	

void handle_sigint(int sig) {
	if(sig == SIGINT) {
		std::cout << "Got " << usr2_count << " signals from SIGUSR2" << std::endl;
		exit(0);
	}
}

int main() {
	signal(SIGUSR2, handle_usr2);
	signal(SIGINT, handle_sigint);

	while(true) {
		std::cout <<"I'm still alive" << std::endl;
		sleep(2);
	}

	return 0;

}
