#ifndef OCCURRENCE_H
#define OCCURRENCE_H

#include <string>

struct Occurrence {
	int lineNumber;
	int position;
	int error;
	Occurrence(int lineNumber, int position, int error=0);
};

struct OccurrenceMultiplePatterns {
    int lineNumber;
    int position;
    std::string value;
    OccurrenceMultiplePatterns(int lineNumber, int position, std::string value);
};

#endif
