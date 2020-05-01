#include "pch.h"
#include "framework.h"
#include "search.h"
#include <istream>
#include <iostream>

size_t offsetOfPattern(string text, string pattern, size_t offset) {
	return text.find(pattern, offset);
}

void allOffsetsOfPattern(string text, string pattern, size_t offset, vector<size_t>& offsets) {
	offset = offsetOfPattern(text, pattern, offset);
	while (offset != string::npos) {
		offsets.push_back(offset);
		offset = offsetOfPattern(text, pattern, ++offset);
	}
}

int getOffsetBefore(int offset, int count) {
	int offsetBefore =  offset - (count + 1) / 2;
	return offsetBefore >= 0 ? offsetBefore : 0;
}

string getTextAroundOffset(istream& input, int offset, int count) {
	input.seekg(0, input.beg);
	if (offset > remainingChars(input))
		return "";
	int start = getOffsetBefore(offset, count);
	input.seekg(start, input.beg);
	string text = toString(input, count);
	return text;
}

size_t searchAroundOffset(istream& input, int offset, string pattern) {
	uint64_t count = pattern.length() * 2;
	string text = getTextAroundOffset(input, offset, count);
	size_t patternOffset = text.find(pattern);
	if (patternOffset != string::npos && patternOffset != 0)
		patternOffset = offset - pattern.length() + (int)patternOffset;
	return patternOffset;
}

vector<size_t> searchAroundAllOffsets(istream& input, vector<int>& offsets, string pattern) {
	vector<size_t> patternOffsets;
	for (int offset : offsets) {
		size_t offsetOfPattern = searchAroundOffset(input, offset, pattern);
		if (offsetOfPattern != string::npos)
			patternOffsets.push_back(offsetOfPattern);
	}
	return patternOffsets;
}

string toString(istream& stream, int count) {
	if (count <= 0)
		return "";
	string text(count, ' ');
	stream.read(&text[0], count);
	return text;
}

long long remainingChars(istream& stream) {
	return stream.rdbuf()->in_avail();
}