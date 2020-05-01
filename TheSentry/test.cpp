#include "pch.h"
#include "../Sentry/sentry.cpp"
#include <mpi.h>

void EXPECT_ARRAY_EQ(int length, int* arr1, int* arr2) {
    for (int i = 0; i < length; i++)
        EXPECT_EQ(arr1[i], arr2[i]);
}

TEST(getSendCountsAndDisplacements, GivenDataSizeAndProcessorCount_ReturnsDataCountsAndDisplacementsForEachProcess) {
    int expectedCounts[2] = { 5, 6 };
    int expectedDisplacements[2] = { 0, 5 };
    int actualCounts[2], actualDisplacements[2];

    getSendCountsAndDisplacements(11, 2, actualCounts, actualDisplacements);

    EXPECT_ARRAY_EQ(2, expectedCounts, actualCounts);
    EXPECT_ARRAY_EQ(2, expectedDisplacements, actualDisplacements);
}

TEST(getSendCountsAndDisplacements, GivenDataSizeNotEvenlyDividableByProcessCount_AllocatesTheRemainderToTheLastProcess) {
    int expectedCounts[3] = { 13, 13, 14 };
    int expectedDisplacements[3] = { 0, 13, 26 };
    int actualCounts[3], actualDisplacements[3];

    getSendCountsAndDisplacements(40, 3, actualCounts, actualDisplacements);

    EXPECT_ARRAY_EQ(3, expectedCounts, actualCounts);
    EXPECT_ARRAY_EQ(3, expectedDisplacements, actualDisplacements);
}

TEST(getDisplacements, GivenCounts_ReturnsDisplacements) {
    int counts[] = { 3, 5, 0, 2 };
    vector<int> expectedDisplacements{ 0, 3, 8, 8 };

    vector<int> actualDisplacements = getDisplacements(counts, 4);

    EXPECT_EQ(expectedDisplacements, actualDisplacements);
}

TEST(getReadCount, GivenALengthLessThanMinimum_ReturnsLength) {
    int length = 0;
    int minimum = 1;

    int actual = getReadCount(length, minimum);

    EXPECT_EQ(length, actual);
}

TEST(getReadCount, GivenALengthEqualToMinimum_ReturnsMinimum) {
    int length = 1;
    int minimum = length;

    int actual = getReadCount(length, minimum);

    EXPECT_EQ(minimum, actual);
}

TEST(getReadCount, GivenALengthGreaterThanMinimum_ReturnsMinimum) {
    int length = 2;
    int minimum = 1;

    int actual = getReadCount(length, minimum);

    EXPECT_EQ(minimum, actual);
}