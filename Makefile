CC = g++

a.out: list_folder_contents.cpp commands.h
	$(CC) list_folder_contents.cpp -Wall -Wextra -std=c++17 -lncurses
