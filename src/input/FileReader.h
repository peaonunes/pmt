#ifndef FILEREADER_H
#define FILEREADER_H

#include <fstream>
#include <string>

class FileReader {
public:
	FileReader(char *filePath, int maxBytes=1048576);
	~FileReader();
	char* read(char* buffer, int bytes);
	char* read();
	std::string readAsString();
	std::string readAsString(int bytes);
	bool hasContent();
	std::string readLine();

	int lineCount;
	int currentReadingPosition;
	int bufferSize;
	int maxBytes;
private:
	std::ifstream* fileStream;
	char* buffer;
};

#endif
