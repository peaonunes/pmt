#include <string.h>
#include <algorithm>
#include <iostream>

#include "KnuthMorrisPratt.h"

using namespace std;

KnuthMorrisPratt::KnuthMorrisPratt()
	: currentBuffer(0),
		currentBufferSize(0),
		nextBuffer(0),
		nextBufferSize(0) { }

KnuthMorrisPratt::~KnuthMorrisPratt() {
}

int* KnuthMorrisPratt::initNext(string pattern) {
	int patternLength = pattern.size();
	int* result = new int[patternLength + 1];
	int cursor;
	int matchesSoFar = 0;

	for (int i = 0; i < patternLength + 1; ++i) {
		result[i] = -1;
	}

	if (patternLength == 1) {
		result[1] = 0;
	} else if (pattern[0] != pattern [1]) {
		result[1] = 0;
	}

	cursor = 1;

	while (cursor < patternLength) {
		while ((cursor + matchesSoFar) < patternLength && pattern[cursor + matchesSoFar] == pattern[matchesSoFar]) {
			++matchesSoFar;

			if (result[cursor + matchesSoFar] == -1) { // só atualizamos campos que nunca foram setados
				if (cursor + matchesSoFar == patternLength ||
						pattern[cursor + matchesSoFar] != pattern[matchesSoFar]) { // se for borda estrita...
					result[cursor + matchesSoFar] = matchesSoFar;
				} else {
					result[cursor + matchesSoFar] = result[matchesSoFar];
				}
			}
		}

		cursor += matchesSoFar - result[matchesSoFar];
		matchesSoFar = std::max(0, result[matchesSoFar]);
	}

	return result;
}

void KnuthMorrisPratt::readFromFile(FileReader &fr, int patternLength, char* buffer, int &bufSize) {
	if (patternLength >= fr.maxBytes) {
		fr.read(buffer, patternLength);
	} else {
		strcpy(buffer, fr.read());
	}

	bufSize = fr.bufferSize;
}

char KnuthMorrisPratt::getTextAt(int index) {
	if (index >= this->currentBufferSize) {
		return this->nextBuffer[index - this->currentBufferSize];
	} else {
		return this->currentBuffer[index];
	}
}

vector<Occurrence> KnuthMorrisPratt::search(string pattern, char *inputFile) {
	vector<Occurrence> result;
	FileReader fr(inputFile);
	int patternLength = pattern.size();
	int bufferLength;
	int* strictBorderTable = this->initNext(pattern);
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
			while (matches < patternLength &&
							this->getTextAt(cursor + matches) == pattern[matches]) {
				++matches;
			}

			if (matches == patternLength) {
				result.push_back(Occurrence(lineCount, readingPosition + cursor));
			}

			cursor += matches - strictBorderTable[matches];
			matches = std::max(0, strictBorderTable[matches]);

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

	delete strictBorderTable;
	delete this->currentBuffer;
	delete this->nextBuffer;

	return result;
}
