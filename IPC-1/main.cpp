#include <iostream>
#include <unistd.h>
#include <csignal>
#include <cstdlib>

const char* get_username() {
    const char* username = getenv("USER");
    if (username) {
        return username;
    }
    return "unknown";
}

void signal_handler(int sig, siginfo_t* info, void* context) {
    if(sig == SIGUSR1) {
        ucontext_t* cntxt = static_cast<ucontext_t*>(context);

        auto rip = cntxt->uc_mcontext.gregs[REG_RIP];
        auto rax = cntxt->uc_mcontext.gregs[REG_RAX];
        auto rbx = cntxt->uc_mcontext.gregs[REG_RBX];

        const char* username = get_username();

        std::cout << "Received a SIGUSR1 signal from process " << info->si_pid
                  << " executed by " << info->si_uid << " " << username << std::endl;
        std::cout << "State of the context: RIP = " << static_cast<long long>(rip)
                  << " RAX = " << static_cast<long long>(rax) << " RBX = "
                  << static_cast<long long>(rbx) << std::endl;
    }
}

int main() {
    pid_t pid = getpid();
    std::cout << "Process PID: " << pid << std::endl;

    struct sigaction sa{};

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = signal_handler;

    if (sigaction(SIGUSR1, &sa, nullptr) == -1) {
        std::cerr << "sigaction" << std::endl;
        return 1;
    }

    while (true) {
        std::cout << "Waiting for SIGUSR1" << std::endl;
        sleep(10);
    }

    return 0;
}

