#include <iostream>
#include <semaphore.h>
#include <cstring>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

class shared_array {
private:
    std::string name;
    std::size_t size;
    int* data;
    sem_t* semaphore;

    void initialize_shared_memory() {
        int fd = shm_open(name.c_str(), O_CREAT | O_RDWR, 0666);
        if (fd < 0) {
            std::cerr << "Can't open shared memory" << std::endl;
            exit(1);    
        }
        if (ftruncate(fd, size * sizeof(int)) < 0) {
            std::cerr << "Can't resize the file" << std::endl;
            close(fd);
            exit(1);
        }
        void* mapped_memory = mmap(nullptr, size * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        close(fd);

        if (mapped_memory == MAP_FAILED) {
            std::cerr << "Can't map shared memory" << std::endl;
            exit(1);
        }

        data = static_cast<int*>(mapped_memory);
    }

    void initialize_semaphore() {
        std::string sem_name = "/" + name + "_sem";
        semaphore = sem_open(sem_name.c_str(), O_CREAT, 0666, 1);
        if (semaphore == SEM_FAILED) {
            std::cerr << "Failed to open semaphore" << std::endl;
            exit(1);
        }
    }

public:
    shared_array(std::string name, std::size_t size) : name(name), size(size), data(nullptr), semaphore(nullptr) {
        if (size > 1000000) {
            std::cerr << "Array has invalid size" << std::endl;
            exit(1);
        }
        initialize_shared_memory();
        initialize_semaphore();
    }

    ~shared_array() {
        munmap(data, size * sizeof(int));
        std::string sem_name = "/" + name + "_sem";
        sem_close(semaphore);
        sem_unlink(sem_name.c_str());
    }

    int& operator[](std::size_t index) {
        if (index >= size) {
            std::cerr << "Index is out of range" << std::endl;
            exit(1);
        }
        return data[index];
    }

    void lock() {
        sem_wait(semaphore);
    }

    void unlock() {
        sem_post(semaphore);
    }
};

void program1() {
    shared_array array("shared_array", 25);
    while (true) {
        array.lock();
        for (std::size_t i = 0; i < 25; i++) {
            array[i] = i + 5;
            std::cout << "Program1: Writing " << array[i] << " at index " << i << std::endl;
        }
        array.unlock();
        sleep(2);
    }
}

void program2() {
    shared_array array("shared_array", 25);
    while (true) {
        array.lock();
        for (std::size_t i = 0; i < 25; i++) {
            std::cout << "Program2: Reading " << array[i] << " at index " << i << std::endl;
        }
        array.unlock();
        sleep(2);
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Invalid number of arguments" << std::endl;
        return 1;
    }
    int program_num = std::stoi(argv[1]);
    if (program_num == 1) {
        program1();
    } else if (program_num == 2) {
        program2();
    } else {
        std::cerr << "Invalid argument(It should be 1 or 2)" << std::endl;
	return 1;
    }

    return 0;
}

