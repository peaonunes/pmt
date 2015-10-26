#include <iostream>
#include <vector>

#include "Sellers.h"
#include "../input/FileReader.h"

using namespace std;

Sellers::Sellers(int editDistance)
	: ApproximateSearchStrategy(editDistance) { }

vector<Occurrence> Sellers::search(string pattern, char *inputFile) {
	FileReader fr(inputFile);
	vector<Occurrence> result;
	char* buffer;
	int columnLen = pattern.size() + 1;
	int* previousColumn = new int[columnLen];
	int* currentColumn = new int[columnLen];
	bool isNewLine = true;
	int bufferIndex = 0;

	while (fr.hasContent()) {
		if (isNewLine) {
			for (int i = 0; i < columnLen; i++) {
				previousColumn[i] = i;
			}
			isNewLine = false;
		}

		buffer = fr.read();

		if (!fr.bufferSize) {
			isNewLine = true;
		} else {
			bufferIndex = 0;

			while (buffer[bufferIndex] != '\0') {
				currentColumn[0] = 0;

				for (int i = 1; i < columnLen; i++) {
					currentColumn[i] = min(
						min(previousColumn[i] + 1, previousColumn[i - 1] + this->phi(pattern[i - 1], buffer[bufferIndex])),
						currentColumn[i - 1] + 1
					);
				}

				if (currentColumn[columnLen - 1] <= this->editDistance) {
					result.push_back(Occurrence(fr.lineCount, fr.currentReadingPosition + bufferIndex, currentColumn[columnLen - 1]));
				}

				//swap columns
				int* aux = currentColumn;
				currentColumn = previousColumn;
				previousColumn = aux;
				bufferIndex++;
			}
		}
	}

	delete previousColumn;
	delete currentColumn;

	return result;
}

Sellers::~Sellers() {

}
