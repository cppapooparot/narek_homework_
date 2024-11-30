#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <string>

void do_command(char** argv) {
    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Error: Failed to run fork()" << std::endl;
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        execvp(argv[0], argv);
        std::cerr << "Error: Failed to complete execvp" << std::endl;
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            std::cout << "Command ended with the code " << WEXITSTATUS(status) << std::endl;
        } else {
            std::cerr << "Error: Command ended not normally" << std::endl;
        }
    }
}

std::vector<std::string> split_input(const std::string& input) {
    std::vector<std::string> commands;
    std::string current;
    for (size_t i = 0; i < input.size(); i++) {
        if (input[i] == ';' || (input[i] == '&' && i + 1 < input.size() && input[i + 1] == '&') || 
            (input[i] == '|' && i + 1 < input.size() && input[i + 1] == '|')) {
            if (!current.empty()) {
                commands.push_back(current);
                current.clear();
            }
            if (input[i] == ';') {
                commands.push_back(";");
            } else if (input[i] == '&') {
                commands.push_back("&&");
            } else if (input[i] == '|') {
                commands.push_back("||");
            }
            i += (input[i] == '&' || input[i] == '|') ? 1 : 0; 
        } else {
            current += input[i];
        }
    }
    if (!current.empty()) {
        commands.push_back(current);
    }
    return commands;
}

void execute_commands(const std::vector<std::string>& commands) {
    for (const auto& command : commands) {
        std::istringstream iss(command);
        std::vector<std::string> args;
        std::string arg;
        while (iss >> arg) {
            args.push_back(arg);
        }
        std::vector<char*> argv;
        for (auto& s : args) {
            argv.push_back(&s[0]);
        }
        argv.push_back(nullptr);
        do_command(argv.data());
    }
}

int main() {
    std::string input;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "exit") {
            break;
        }

        std::vector<std::string> commands = split_input(input);
        execute_commands(commands);
    }
    return 0;
}

