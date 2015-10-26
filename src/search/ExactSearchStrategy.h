#ifndef EXACTSS_H
#define EXACTSS_H

#include <vector>

#include "Occurrence.h"

class ExactSearchStrategy {
public:
	virtual ~ExactSearchStrategy();
	virtual std::vector<Occurrence> search(std::string pattern, char* inputFile) = 0;
};

#endif
