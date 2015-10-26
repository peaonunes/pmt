#include <string.h>
#include <algorithm>
#include <iostream>
#include <stdio.h>

#include "BoyerMoore.h"

#define NUMBER_OF_CHARS 256

using namespace std;

BoyerMoore::BoyerMoore()
	: currentBuffer(0),
    	currentBufferSize(0),
    	nextBuffer(0),
    	nextBufferSize(0) { }

BoyerMoore::~BoyerMoore() {
}

void BoyerMoore::borders(string pattern, int *borderArray) {
	int i;
	borderArray[0] = -1;
	for (i = 1; i <= pattern.size(); i++)
		borderArray[i] = 0;
	i = 1;
	int j = 0;
	while (i < pattern.size()) {
		while((i + j) < pattern.size() && pattern[j] == pattern[i + j]) {
		cout << "ti" << endl;
			j += 1;
			borderArray[i + j]=j;
		}
		i += (j-borderArray[j]);
		j = std::max(0, borderArray[j]);
	}
}

void BoyerMoore::goodSuffixHeuristic(string pattern, int *gs) {
	int patternLength = pattern.size();
	int Pi [patternLength+1];
	BoyerMoore::borders(pattern, Pi);
	int PiR [patternLength+1];
	int i;
	PiR[0] = Pi[patternLength];
	gs[0] = patternLength+1;
	for (i = 1; i < patternLength+1; i++) {
		PiR[i] = Pi[patternLength-i];
		gs[i] = patternLength-Pi[patternLength];
	}
	int j;
	for (i = 1; i < patternLength; i++) {
		j = patternLength-1-PiR[i];
		if (gs[j] > i-PiR[i])
			gs[j] = i-PiR[i];
	}
}

void BoyerMoore::badCharacterHeuristic(string pattern, int *bc) {
	int i;

	for (i = 0; i < NUMBER_OF_CHARS; i++) {
		bc[i] = -1;
	}

	for (i = 0; i < pattern.size(); i++) {
		bc[(int) pattern[i]] = i;
	}
}

void BoyerMoore::readFromFile(FileReader &fr, int patternLength, char* buffer, int &bufSize) {
    if (patternLength >= fr.maxBytes) {
        fr.read(buffer, patternLength);
    } else {
        strcpy(buffer, fr.read());
    }

    bufSize = fr.bufferSize;
}

char BoyerMoore::getTextAt(int index) {
    if (index >= this->currentBufferSize) {
        return this->nextBuffer[index - this->currentBufferSize];
    } else {
        return this->currentBuffer[index];
    }
}

vector<Occurrence> BoyerMoore::search(string pattern, char *inputFile) {
	vector<Occurrence> result;
	FileReader fr(inputFile);
	int bufferLength; //TODO
	int patternLength = pattern.size();

	int badCharArray[NUMBER_OF_CHARS];
	badCharacterHeuristic(pattern, badCharArray);
	int goodSuffixArray[patternLength + 1];
	goodSuffixHeuristic(pattern, goodSuffixArray);

	int cursor = 0;
	int matches = 0;
	int readingPosition = 0;
	int lineCount = 0;

	if (patternLength >= fr.maxBytes) {
		this->currentBuffer = new char[patternLength + 1]; // tamanho mínimo de buffer
		this->nextBuffer = new char[patternLength + 1];
		bufferLength = patternLength;
	} else {
		this->currentBuffer = new char[fr.maxBytes]; // tamanho mínimo de buffer
		this->nextBuffer = new char[fr.maxBytes];
		bufferLength = fr.maxBytes;
	}

	while (fr.hasContent()) {
		++lineCount;
		cursor = 0;
		matches = 0;
		readingPosition = 0;

		readFromFile(fr, bufferLength, this->currentBuffer, this->currentBufferSize);
		if (this->currentBufferSize) {
			readFromFile(fr, bufferLength, this->nextBuffer, this->nextBufferSize);
		} else {
			this->nextBuffer[0] = '\0';
			this->nextBufferSize = 0;
		}

		while (this->nextBufferSize || cursor + patternLength <= this->currentBufferSize + this->nextBufferSize) {
			int j = patternLength-1;
			while(j >= 0 && this->getTextAt(cursor + j)==pattern[j]) {
				j -= 1;
			}
			if (j < 0) {
				result.push_back(Occurrence(lineCount, readingPosition + cursor));
				cursor += goodSuffixArray[0];
			} else {
				cursor += std::max(j - badCharArray[(int)this->getTextAt(cursor + j)], goodSuffixArray[j]);
			}

			if (this->nextBufferSize && cursor >= this->currentBufferSize) {
				char* aux = this->currentBuffer;
				this->currentBuffer = this->nextBuffer;
				this->nextBuffer = aux;
				readingPosition = fr.currentReadingPosition;

				readFromFile(fr, bufferLength, this->nextBuffer, this->nextBufferSize);

				cursor -= this->currentBufferSize;
			}
		}
	}

	delete this->currentBuffer;
	delete this->nextBuffer;

	return result;
}
