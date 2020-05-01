#pragma once
#include <vector>
#include <mpi.h>
#include <string>
#include <set>

using namespace std;

const int ROOT_ID = 0;
const int SEARCH_RESULTS = 0;

std::set<int> mpiSearch(string filepath, string pattern, bool verbose);

void getSendCountsAndDisplacements(int dataSize, int proccessCount, int * counts, int * displacements);

void killAllButRoot(int currentProcessId);

vector<int> getDisplacements(int counts[], int length);

vector<uint64_t> getDisplacements(uint64_t counts[], uint64_t length);


int getReadCount(int length, int minimum);