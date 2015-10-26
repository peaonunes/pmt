#ifndef BOYERMOORE_H
#define BOYERMOORE_H

#include <vector>

#include "ExactSearchStrategy.h"
#include "Occurrence.h"
#include "../input/FileReader.h"

class BoyerMoore : public ExactSearchStrategy {
public:
	BoyerMoore();
	virtual ~BoyerMoore();
	virtual std::vector<Occurrence> search(std::string pattern, char* inputFile);
private:
	void borders(std::string pattern, int *borderArray);
	void goodSuffixHeuristic(std::string pattern, int *gs);
	void badCharacterHeuristic(std::string pattern, int *bc);
	void readFromFile(FileReader &fr, int patternLength, char* buffer, int &bufSize);
	char getTextAt(int index);

	char* currentBuffer;
	int currentBufferSize;
	char* nextBuffer;
	int nextBufferSize;
};

#endif
