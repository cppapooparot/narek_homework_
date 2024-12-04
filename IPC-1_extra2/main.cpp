#include <iostream>
#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if(pid < 0) {
        std::cerr << "Failed to create child process" << std::endl;
        return 1;
    }
    else if(pid == 0) {
        std::cout << "Child process is trying to kill the parent process" << std::endl;
        if(kill(getppid(), SIGKILL) == -1) {
            std::cerr << "Failed to kill parent process" << std::endl;
        }
        else {
            std::cout << "Child process sent SIGKILL signal to parent" << std::endl;
        }
        return 0;
    }
    else {
        std::cout << "Parent process with ID: " << getpid() << " is waiting for child" << std::endl;
        int status;
        waitpid(pid, &status, 0);

        if(WIFSIGNALED(status)) {
            std::cout << "Child process terminated because of signal: " << WTERMSIG(status) << std::endl;
        }
        else {
            std::cout << "Child process terminated normally" << std::endl;
        }
    }
    return 0;
}

