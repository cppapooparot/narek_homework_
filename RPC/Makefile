CXX = g++

SRCS = server.cpp client.cpp

OBJS = server.o client.o

EXEC = server client

all: $(EXEC)

server: server.o
	$(CXX) -o server server.o

client: client.o
	$(CXX) -o client client.o

server.o: server.cpp
	$(CXX) -c server.cpp

client.o: client.cpp
	$(CXX) -c client.cpp

clean:
	rm -f $(OBJS) $(EXEC)

fclean: clean
	rm -f $(EXEC)
