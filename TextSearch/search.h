#pragma once
#include <string>
#include <vector>
#include <set>

using namespace std;

size_t offsetOfPattern(string text, string pattern, size_t offset);

void allOffsetsOfPattern(string text, string pattern, size_t offset, vector<size_t>& offsets);

void findMatches(string text, string pattern, set<int>& matches, int relativeOffset);

int getOffsetBefore(int offset, int count);

string getTextAroundOffset(istream& input, int offset, int count);

size_t searchAroundOffset(istream& input, int offset, string pattern);

vector<size_t> searchAroundAllOffsets(istream& input, vector<int>& offsets, string pattern);

string toString(istream& stream, int count);

long long remainingChars(istream& stream);