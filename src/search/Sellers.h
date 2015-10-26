#ifndef SELLERS_H
#define SELLERS_H

#include <vector>
#include "ApproximateSearchStrategy.h"
#include "Occurrence.h"

class Sellers : public ApproximateSearchStrategy {
public:
	Sellers(int editDistance);
	virtual std::vector<Occurrence> search(std::string pattern, char* inputFile);
	virtual ~Sellers();
};

#endif
