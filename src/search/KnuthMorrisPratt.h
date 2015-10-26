#ifndef KMP_H
#define KMP_H

#include <vector>

#include "ExactSearchStrategy.h"
#include "Occurrence.h"
#include "../input/FileReader.h"

class KnuthMorrisPratt : public ExactSearchStrategy {
public:
	KnuthMorrisPratt();
	virtual ~KnuthMorrisPratt();
	virtual std::vector<Occurrence> search(std::string pattern, char* inputFile);
private:
	int* initNext(std::string pattern);
	void readFromFile(FileReader &fr, int patternLength, char* buffer, int &bufSize);
	char getTextAt(int index);

	char* currentBuffer;
	int currentBufferSize;
	char* nextBuffer;
	int nextBufferSize;
};

#endif
