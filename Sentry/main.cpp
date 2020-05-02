#include "./sentry.h"

#include <iostream>
#include <fstream>
#include <mpi.h>
#include "../TextSearch/search.h"
#include <chrono>

int getArguments(int argc, char** argv, string& filepath, string& pattern, uint64_t& maxReadSize);
int serialDriver(int argc, char** argv);
int parallelDriver(int argc, char** argv);

using namespace std::chrono;

int main(int argc, char** argv) {
	auto start = high_resolution_clock::now();
	int status = 0;

	if (strcmp(argv[1], "mpi") == 0) {
		std::cout << "Starting parallel search..." << std::endl;
		status = parallelDriver(argc, argv);
	}
	else if (strcmp(argv[1], "serial") == 0) {
		std::cout << "Starting serial search..." << std::endl;
		status = serialDriver(argc, argv);
	}
	else
		std::cout << "Please specify 'mpi' or 'serial' as the first argument followed by input filepath and pattern." << std::endl;

	if (status != 0) {
		std::cout << "An error ocurred. Quitting...";
		return status;
	}
	auto runtime = duration_cast<milliseconds>(high_resolution_clock::now() - start);
	std::cout << "Runtime: " << runtime.count() / 1000.00 << " seconds" << std::endl;
	return status;
}

int serialDriver(int argc, char** argv) {
	string filepath, pattern;
	uint64_t maxReadSize = 1024 * 64;
	int status = getArguments(argc, argv, filepath, pattern, maxReadSize);
	if (status != 0)
		return status;

	auto offsetsOfPattern = serialSearch(filepath, pattern, maxReadSize);
	
	std::cout << "Found " << offsetsOfPattern.size() << " matches of " << pattern << std::endl;
	return 0;
}

int parallelDriver(int argc, char** argv) {
	string filepath, pattern;
	uint64_t maxReadSize = 1024 * 64;
	int status = getArguments(argc, argv, filepath, pattern, maxReadSize);
	if (status != 0)
		return status;

	MPI_Init(&argc, &argv);
	auto offsets = mpiSearch(filepath, pattern, maxReadSize);
	std::cout << "Found " << offsets.size() << " matches of " << pattern << std::endl;
	return 0;
}

int getArguments(int argc, char** argv, string& filepath, string& pattern, uint64_t& maxReadSize) {
	if (argc != 4 && argc != 5) {
		std::cout << "Invalid number of arguments. Please provide the following arguments: input_filepath, pattern, max_read_size(kilobytes, default is 64kb)" << std::endl;
		return -1;
	}
	filepath = argv[2];
	pattern = argv[3];
	if (argc == 5) {
		uint64_t parsedMaxReadSize = strtoull(argv[4], NULL, 10);
		if (parsedMaxReadSize != ULONG_MAX && errno != ERANGE) {
			maxReadSize = 1024 * parsedMaxReadSize;
		}
	}
	return 0;
}