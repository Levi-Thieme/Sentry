#include "./sentry.h"

#include <iostream>
#include <fstream>
#include <mpi.h>
#include "../TextSearch/search.h"
#include <chrono>

int serialDriver(int argc, char** argv);
int parallelDriver(int argc, char** argv);

using namespace std::chrono;

int main(int argc, char** argv) {
	auto start = high_resolution_clock::now();
	parallelDriver(argc, argv);
	auto runtime = duration_cast<milliseconds>(high_resolution_clock::now() - start);
	cout << "Runtime: " << runtime.count() << std::endl;
	return 0;
}

int serialDriver(int argc, char** argv) {
	string filepath = argv[1];
	string pattern = argv[2];
	bool verbose = argc == 4;

	ifstream file;
	file.open(filepath);
	if (!file) {
		cerr << "Unable to open " << filepath;
		MPI_Finalize();
		return -1;
	}
	istream& inputStream = file;
	inputStream.seekg(0, inputStream.end);
	long long length = inputStream.tellg();
	inputStream.seekg(0, inputStream.beg);
	string text(length, ' ');
	inputStream.read(&text[0], length);
	vector<size_t> offsets;
	allOffsetsOfPattern(text, pattern, 0, offsets);
	cout << "Found " << offsets.size() << " matches of " << pattern << std::endl;
	return 0;
}

int parallelDriver(int argc, char** argv) {
	string filepath = argv[1];
	string pattern = argv[2];
	bool verbose = argc == 4;

	MPI_Init(&argc, &argv);
	auto offsets = mpiSearch(filepath, pattern, verbose);
	cout << "Found " << offsets.size() << " matches of " << pattern << std::endl;
	return 0;
}