GCC=g++ -std=c++17 -O2 -Wall

random:
	$(GCC) -D RANDOM main.cpp agent.cpp -o exec.agent

clean:
	rm -rf exec.agent
	rm -rf log.*