#ifndef APPROXSS_H
#define APPROXSS_H

#include <vector>
#include "Occurrence.h"

class ApproximateSearchStrategy {
public:
	ApproximateSearchStrategy(int editDistance);
	virtual ~ApproximateSearchStrategy();
	int editDistance;
	virtual std::vector<Occurrence> search(std::string pattern, char* inputFile) = 0;
	int phi(char patternChar, char textChar);
};

#endif
