#include <fstream>
#include <iostream>
#include <string.h>

#include "FileReader.h"

using namespace std;

FileReader::FileReader(char* filePath, int maxBytes)
		: lineCount(1),
			currentReadingPosition(0),
			bufferSize(0),
			maxBytes(maxBytes) {
	this->fileStream = new ifstream(filePath);
	this->buffer = new char[maxBytes];
}

FileReader::~FileReader() {
	this->fileStream->close();

	delete this->fileStream;
	delete this->buffer;
}

char* FileReader::read(char* buffer, int bytes) {
	int lastBufferSize = this->bufferSize;

	this->fileStream->get(buffer, bytes);
	this->bufferSize = this->fileStream->gcount();

	if (!this->bufferSize) {
		this->fileStream->clear();
		this->fileStream->getline(buffer, bytes); //\r\n\0

		this->bufferSize = 0;
		this->currentReadingPosition = 0;
		++this->lineCount;
	} else {
		this->currentReadingPosition += lastBufferSize;
	}

	return this->buffer;
}

char* FileReader::read() {
	return this->read(this->buffer, this->maxBytes);
}

string FileReader::readAsString() {
	return string(this->read(this->buffer, this->maxBytes));
}

string FileReader::readAsString(int bytes) {
	return string(this->read(this->buffer, bytes));
}

string FileReader::readLine() {
	string str;

	getline(*this->fileStream, str);
	++this->lineCount;
	this->currentReadingPosition = 0;
	this->bufferSize = str.length();

	return str;
}

bool FileReader::hasContent() {
	return this->fileStream->good();
}
