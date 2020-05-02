#include "sentry.h"
#include <algorithm>
#include "../TextSearch/search.cpp"
#include <string>
#include <fstream>
#include <iostream>
#include <set>

std::set<int> mpiSearch(string filepath, string pattern, bool verbose) {
	int isInitialized = 0;
	MPI_Initialized(&isInitialized);
	if (!isInitialized)
		return set<int>();
	int currentProcess, processCount;
	MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
	MPI_Comm_size(MPI_COMM_WORLD, &processCount);
	MPI_Comm_rank(MPI_COMM_WORLD, &currentProcess);
	const uint64_t MinRead = 129536;
	const uint64_t MinSend = 8096;

	uint64_t sendCount = 0;
	bool eof = true;
	long long charsRead = 0;
	long long filesize = 0;
	ifstream ifstream;
	if (currentProcess == ROOT_ID) {
		ifstream.open(filepath, ifstream::in);
		ifstream.seekg(0, ifstream.end);
		filesize = ifstream.tellg();
		ifstream.seekg(charsRead, ifstream.beg);
		std::cout << "Filesize: " << filesize << std::endl;
	}

	vector<size_t> currentProcessMatches;

	do {
		char* textToSend = NULL;
		int* counts = new int[processCount];
		int* displacements = new int[processCount];
		int receiveCount = 0;
		int displacement = 0;
		MPI_Request endOfFileRequest;

		if (currentProcess == ROOT_ID)
		{
			sendCount = getReadCount(filesize - charsRead, MinRead);
			textToSend = new char[sendCount];
			ifstream.read(textToSend, sendCount);
			charsRead += sendCount;
			eof = charsRead >= filesize;
			getSendCountsAndDisplacements(sendCount, processCount, counts, displacements);
			//Add an overlap of pattern.size() to each count, so that patterns are not missed
			offsetAllBy(counts, 0, processCount - 1, pattern.size());
		}
		//Scatter the receive counts to each processor respectively.
		MPI_Scatter(counts, 1, MPI_INT, &receiveCount, 1, MPI_INT, ROOT_ID, MPI_COMM_WORLD);
		//Scatter the displacements to each processor respectively.
		MPI_Scatter(displacements, 1, MPI_INT, &displacement, 1, MPI_INT, ROOT_ID, MPI_COMM_WORLD);

		//Scatter the text to each process according to the counts and displacements
		char* receiveBuffer = new char[receiveCount];

		MPI_Scatterv(textToSend, counts, displacements, MPI_CHAR, receiveBuffer, receiveCount, MPI_CHAR, ROOT_ID, MPI_COMM_WORLD);
		
		vector<uint64_t> currentProcessSubstringMatches;
		allOffsetsOfPattern(string(receiveBuffer), pattern, 0, currentProcessSubstringMatches);
		incrementAllBy(currentProcessSubstringMatches, displacement);
		currentProcessMatches.insert(currentProcessMatches.end(), currentProcessSubstringMatches.begin(), currentProcessSubstringMatches.end());

		delete[] textToSend;
		delete[] counts;
		delete[] displacements;
		delete[] receiveBuffer;
		
		MPI_Ibcast(&eof, 1, MPI_INT, ROOT_ID, MPI_COMM_WORLD, &endOfFileRequest);
		MPI_Status eofReqStatus;
		MPI_Wait(&endOfFileRequest, &eofReqStatus);

	} while (!eof);

	if (ifstream.is_open())
		ifstream.close();

	//Gather the count of offsets from each process, and use it to create the displacements needed for MPI_IGatherv
	int patternCount = currentProcessMatches.size();
	int* offsetCounts = NULL;
	if (currentProcess == ROOT_ID) {
		offsetCounts = new int[processCount];
		for (int i = 0; i < processCount; i++)
			offsetCounts[i] = 0;
	}

	//Gather the offset counts from each process and store in counts
	MPI_Request gatherCountsRequest;
	MPI_Gather(
		&patternCount,
		1,
		MPI_INT,
		offsetCounts,
		1,
		MPI_INT,
		ROOT_ID,
		MPI_COMM_WORLD
	);

	
	if (currentProcess == ROOT_ID) {
		for (int i = 0; i < processCount; i++)
			std::cout << offsetCounts[i] << " ";
		std::cout << std::endl;
	}

	int* displacements = NULL;
	int* gatheredOffsets = NULL;
	int allOffsetsLength = 0;

	if (currentProcess == ROOT_ID) {
		auto displacementsVector = getDisplacements(offsetCounts, processCount);
		allOffsetsLength = displacementsVector.back() + offsetCounts[processCount - 1];
		gatheredOffsets = new int[allOffsetsLength];
		displacements = new int[displacementsVector.size()];
		std::copy(displacementsVector.begin(), displacementsVector.end(), displacements);
	}

	int * offsetsToSend = new int[patternCount];
	std::copy(currentProcessMatches.begin(), currentProcessMatches.end(), offsetsToSend);

	MPI_Gatherv(
		offsetsToSend,
		patternCount,
		MPI_INT,
		gatheredOffsets,
		offsetCounts,
		displacements,
		MPI_INT,
		ROOT_ID,
		MPI_COMM_WORLD
	);


	MPI_Finalize();
	killAllButRoot(currentProcess);

	set<int> uniqueOffsetsOfPattern;
	delete[] displacements;
	delete[] offsetCounts;
	for (int i = 0; i < allOffsetsLength; i++) {
		uniqueOffsetsOfPattern.insert(gatheredOffsets[i]);
	}
	delete[] gatheredOffsets;

	return uniqueOffsetsOfPattern;
}

int getReadCount(int length, int minimum) {
	return length < minimum ? length : minimum;
}

void getSendCountsAndDisplacements(int dataSize, int proccessCount, int* counts, int* displacements) {
	int chunkSize = dataSize / proccessCount;
	int offset = 0;
	for (int i = 0; i < proccessCount; i++) {
		counts[i] = chunkSize;
		displacements[i] = offset;
		offset += chunkSize;
	}
	int remainder = dataSize % chunkSize;
	counts[proccessCount - 1] += remainder;
}

void killAllButRoot(int currentProcessId) {
	if (currentProcessId != ROOT_ID)
		exit(0);
}

vector<int> getDisplacements(int counts[], int length) {
	vector<int> displacements;
	int offset = 0;
	displacements.push_back(0);
	for (int i = 0; i < length - 1; i++) {
		offset += counts[i];
		displacements.push_back(offset);
	}
	return displacements;
}

vector<uint64_t> getDisplacements(uint64_t counts[], uint64_t length) {
	vector<uint64_t> displacements;
	int offset = 0;
	displacements.push_back(0);
	for (int i = 0; i < length - 1; i++) {
		offset += counts[i];
		displacements.push_back(offset);
	}
	return displacements;
}

void offsetAllBy(int* counts, int start, int length, int offset) {
	for (int i = start; i < length; i++)
		counts[i] += offset;
}

void incrementAllBy(vector<uint64_t>& counts, uint32_t amount) {
	for (int i = 0; i < counts.size(); i++)
		counts[i] += amount;
}