#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cmath>

bool isPrime(int n) {
    if (n < 2)
        return false;
    for (int i = 2; i * i <= n; ++i) {
        if (n % i == 0)
            return false;
    }
    return true;
}

int find_Mth_prime(int m) {
    int count = 0;
    int num = 1;

    while (count < m) {
        num++;
        if (isPrime(num))
            count++;
    }
    return num;
}

int main() {
    int pipe1[2]; // parent to child
    int pipe2[2]; // child to parent
    
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        std::cerr << "Failed to create pipe" << std::endl;
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Failed to create child process" << std::endl;
        return 1;
    }

    if (pid == 0) {  // child process
        close(pipe1[1]);
        close(pipe2[0]);

        while (true) {
            int m;
            int bytes_read = read(pipe1[0], &m, sizeof(m));
            if (bytes_read == 0)
                break;

            std::cout << "[Child] starts to calculate " << m << "-th prime number..." << std::endl;
            int result = find_Mth_prime(m);
            write(pipe2[1], &result, sizeof(result));
        }

        close(pipe1[0]);
        close(pipe2[1]);
    }
    else { // parent process
        int m;
        std::cout << "[Parent] Please enter the number: ";
        std::cin >> m;

        std::cout << "[Parent] Sending " << m << " to the child process..." << std::endl;
        write(pipe1[1], &m, sizeof(m));  // send number to child

        int res;
        read(pipe2[0], &res, sizeof(res));  // read result from child
        std::cout << "[Parent] Received the " << m << "-th prime: " << res << std::endl;

        close(pipe1[1]);
        close(pipe2[0]);

        wait(nullptr);  // wait for child to finish
    }

    return 0;
}

