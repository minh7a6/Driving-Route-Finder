# Kaden Dreger and Rutvik Patel
# 1528632 and 1530012
# CMPUT 275 Winter 2019
# Assignment 2 Part 1: Driving Route Finder

#  This makefile allows the user to link, compile and clean the files
#  necessary to run dijkstra.cpp and server.cpp.
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
CC = g++ # compiler of choice
OBJS = dijkstra.o server.o digraph.o serialport.o # object files generated
CFLAGS = -c -Wall -std=c++11 # compile flags
LFLAGS = -static
EXC = server # executable name

# Builds the project, generating the server executable
$(EXC): $(OBJS)
	$(CC) $(OBJS) -o server $(LFLAGS)

serialport.o: serialport.h serialport.cpp
	$(CC) serialport.cpp -o serialport.o $(CFLAGS)

# compiles digraph.cpp into its respective .o file
digraph.o: digraph.cpp digraph.h
	$(CC) digraph.cpp -o digraph.o $(CFLAGS)

# compiles dijkstra.cpp into its respective .o file
dijkstra.o: dijkstra.cpp dijkstra.h heap.h
	$(CC) dijkstra.cpp -o dijkstra.o $(CFLAGS)

# compiles server.cpp into its respective .o file
server.o: server.cpp digraph.h wdigraph.h serialport.h
	$(CC) server.cpp -o server.o $(CFLAGS)

# removes the .o and executable files from the directory
clean:
	@rm -f $(OBJS) $(EXC) test

# printing out the makefile target objectives
help:
	@echo 'make server    : builds the project and generates the server executable'
	@echo 'make serialport.o : compiles serialport.cpp into its respective object (.o) file'
	@echo 'make digraph.o : compiles digraph.cpp into its respective object (.o) file'
	@echo 'make dijkstra.o: compiles dijkstra.cpp into its respective object (.o) file'
	@echo 'make server.o  : compiles server.cpp into its respective object (.o) file'
	@echo 'make clean     : removes the .o and executable files from the directory'
	@echo 'make help      : prints out information about the targets of the make file'