#include "Occurrence.h"

Occurrence::Occurrence(int lineNumber, int position, int error)
	: lineNumber(lineNumber),
		position(position),
		error(error)
	{ }

OccurrenceMultiplePatterns::OccurrenceMultiplePatterns(int lineNumber, int position,std::string value)
    : lineNumber(lineNumber),
    position(position),
    value(value)
{ }
