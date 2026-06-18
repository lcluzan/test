valgrind --leak-check=full --track-fds=all --show-leak-kinds=all --trace-children=yes ./webserv

make re && strace -e trace=socket,fcntl,bind,listen,poll ./webserv
