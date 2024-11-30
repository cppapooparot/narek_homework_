all: demo

demo: parallel_scheduler.cpp demo.cpp
	g++ -o demo parallel_scheduler.cpp demo.cpp -pthread

clean:
	rm -f demo
