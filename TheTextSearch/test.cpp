#include "pch.h"
#include "../TextSearch/search.cpp"
#include <string.h>
#include <vector>

using namespace std;
TEST(offsetOfPattern, GivenATextContainingPattern_ReturnsTheOffsetOfPattern) {
    char text[] = "hi bye hi";
    char pattern[] = "hi";
    
    size_t actualOffset = offsetOfPattern(text, pattern, 0);

    EXPECT_EQ(0, actualOffset);
    actualOffset = offsetOfPattern(text, pattern, ++actualOffset);
    EXPECT_EQ(7, actualOffset);
}

TEST(allOffsetsOfPattern, GivenATextContainingPattern_ReturnsAllUniqueOffsetsOfPattern) {
    char text[] = "hi bye hi";
    char pattern[] = "hi";
    vector<size_t> expectedOffsets;
    expectedOffsets.push_back(0);
    expectedOffsets.push_back(7);

    vector<size_t> actualOffsets;
    allOffsetsOfPattern(text, pattern, 0, actualOffsets);

    EXPECT_EQ(expectedOffsets, actualOffsets);
}

TEST(findMatches, GivenATextContainingPattern_ReturnsAllUniqueOffsetsOfPattern) {
    char text[] = "hi bye hi";
    char pattern[] = "hi";
    set<int> expected{ 0, 7 };
    set<int> actual;

    findMatches(text, pattern, actual, 0);

    EXPECT_EQ(expected, actual);
}

TEST(findMatches, GivenARelativeOffset_ReturnsRelativeOffsetsOfPattern) {
    char text[] = "hi bye hi";
    char pattern[] = "hi";
    int relativeOffset = 2;
    set<int> expected{ 2, 9 };
    set<int> actual;

    findMatches(text, pattern, actual, relativeOffset);

    EXPECT_EQ(expected, actual);
}

TEST(getOffsetBefore, GivenANegativeOffset_ReturnsZero) {
    int actual = getOffsetBefore(-1, 0);

    EXPECT_EQ(0, actual);
}

TEST(getOffsetBefore, GivenAnoffsetOfZero_ReturnsZero) {
    int offset = 0;
    int count = 4;

    int actual = getOffsetBefore(offset, count);

    EXPECT_EQ(0, actual);
}

TEST(getOffsetBefore, GivenAValidOffsetAndCount_ReturnsOffsetMinusHalfOfCount) {
    int offset = 2;
    int count = 4;

    int actual = getOffsetBefore(offset, count);

    EXPECT_EQ(0, actual);
}

TEST(getOffsetBefore, GivenAnOffsetAndOddCount_ReturnsOffsetMinusHalfOfCountPlusOne) {
    int offset = 4;
    int count = 5;

    int actual = getOffsetBefore(offset, count);

    EXPECT_EQ(1, actual);
}

TEST(getOffsetBefore, GivenACountMoreThanTwiceAsLargeAsOffset_ReturnsZero) {
    int offset = 4;
    int count = offset * 2 + 1;

    int actual = getOffsetBefore(offset, count);

    EXPECT_EQ(0, actual);
}

TEST(getTextAroundOffset, GivenANegativeOffset_FillsBufferWithCount) {
    string text = "0123456789";
    stringstream sstream(text);
    istream& istream = sstream;
    int offset = -1;
    int count = 5;
    string expected = text.substr(0, count);

    string actual = getTextAroundOffset(istream, offset, count);

    EXPECT_EQ(expected, actual);
}

TEST(getTextAroundOffset, GivenAnOffsetLargerThanInput_ReturnsAnEmptyString) {
    string text = "0123456789";
    stringstream sstream(text);
    istream& istream = sstream;
    string expected = "";

    string actual = getTextAroundOffset(istream, text.size() + 1, text.size());

    EXPECT_EQ(expected, actual);
}

TEST(getTextAroundOffset, GivenAValidOffsetAndCount_ReturnsAStringCenteredAroundOffsetAndWithALengthOfCount) {
    string text = "hi bill how are you?";
    stringstream sstream(text);
    istream& istream = sstream;
    string expected = "bill";
    int offset = text.find("ll", 0);

    string actual = getTextAroundOffset(istream, offset, expected.length());

    EXPECT_EQ(expected, actual);
}

TEST(toString, ReturnsAStringStartingAtOffsetWithLengthCount) {
    string text = "the the the";
    stringstream sstream(text);
    istream& stream = sstream;
    int count = 6;
    string expected = "the th";

    string actual = toString(stream, count);

    EXPECT_EQ(expected, actual);
}

TEST(toString, GivenANonPositiveCountReturnsAnEmptyString) {
    string text = "the the the";
    stringstream sstream(text);
    istream& stream = sstream;
    stream.seekg(0, stream.beg);
    int count = -1;
    string expected = "";

    string actual = toString(stream, count);

    EXPECT_EQ(expected, actual);
}

TEST(remainingChars, ReturnsTheRemainingCountOfCharacters) {
    string text = "123456789";
    stringstream sstream(text);
    istream& stream = sstream;
    stream.seekg(1, stream.beg);
    int expected = text.length() - 1;

    int actual = remainingChars(stream);

    EXPECT_EQ(expected, actual);
}

TEST(searchAroundOffset, GivenATextContainingPattern_AndAnOffsetInsidePattern_ReturnsTheOffsetOfPattern) {
    string text = "the the the";
    stringstream sstream(text);
    istream& istream = sstream;
    int offset = 5;
    string pattern = "the";
    size_t expectedOffset = text.find("the", 4);
    
    size_t actualOffset = searchAroundOffset(istream, offset, pattern);

    EXPECT_EQ(expectedOffset, actualOffset);
}

TEST(searchAroundOffset, GivenATextNotContainingPattern_ReturnsStringNpos) {
    string text = "0000000000000";
    stringstream sstream(text);
    istream& istream = sstream;
    int offset = 5;
    string pattern = "the";

    size_t actualOffset = searchAroundOffset(istream, offset, pattern);

    EXPECT_EQ(string::npos, actualOffset);
}

TEST(searchAroundAllOffsets, GivenTextContainingPatternAtOffset_AndOffsetsToSearchAround_ReturnsAllOffsetsOfPattern) {
    string text = "the     the the";
    stringstream sstream(text);
    istream& istream = sstream;
    vector<int> offsetsToSearchAround{0, 5, 10 };
    string pattern = "the";
    vector<size_t> expectedPatternOffsets{ 0, 8};

    vector<size_t> actualPatternOffsets = searchAroundAllOffsets(istream, offsetsToSearchAround, pattern);

    EXPECT_EQ(expectedPatternOffsets, actualPatternOffsets);
}

TEST(searchAroundOffset, GivenATextContainingPattern_ReturnsTheOffsetOfPattern) {
    string text = "the     the the";
    stringstream sstream(text);
    istream& istream = sstream;
    int offset = 10;
    string pattern = "the";
    string substr = text.substr(offset, pattern.size());
    size_t actualOffset = searchAroundOffset(istream, offset, pattern);

    EXPECT_EQ(8, (int)actualOffset);
}