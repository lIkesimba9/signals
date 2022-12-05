#ifndef DEBUG_H
#define DEBUG_H

#include <cstdio>
#include <iostream>

#define PROGRESS_OUT(pos)	\
	std::cout << "\r" << (pos) << "%: ";	\
	std::cout << std::string((pos), '|');				\
	std::cout.flush();

#endif // DEBUG_H