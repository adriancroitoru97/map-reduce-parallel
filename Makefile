build:
	g++ main.cpp my_pthread_utils.cpp -o tema1 -lpthread -Wall -Werror

clean:
	rm -rf tema1
