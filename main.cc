#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <sys/stat.h>

#include "bloomFilter.h"
#include "skipList.h"

#define INPUT "\n./vaccineMonitor -c citizenRecordsFile –b bloomSize\n"

int main (int argc, char *argv[]) {

  std::string str;
  struct stat fileName;
  int bloomSize;

  // ========== Input Validity Check ==========

  if (argc != 5) {
    std::cerr << "Invalid number of arguments.\n";
    std::cout << "Input should be like: " << INPUT;
    exit(-1);
  }

  str.assign(argv[1]);
  if (str.compare("-c") != 0) {
    std::cerr << "Invalid record file identifier.\n";
    std::cout << "Input should be like: " << INPUT;
    exit(-1);
  }

  if ((stat(argv[2], &fileName) != 0)) {
    std::cerr << "Input file not found.\n";
    std::cout << "Input should be like: " << INPUT;
    exit(-1);
  }

  str.assign(argv[3]);
  if (str.compare("-b") != 0) {
    std::cerr << "Invalid bloom filter identifier.\n";
    std::cout << "Input should be like: " << INPUT;
    exit(-1);
  }

  str.assign(argv[4]);
  for (int i = 0; i < str.length(); i++) {
    if (isdigit(str[i]) == false) {
      std::cerr << "Invalid bloom filter size.\n";
      std::cout << "Input should be like: " << INPUT;
      exit(-1);
    }
  }

  if((bloomSize = atoi(argv[4]) <= 0)) {
    std::cerr << "Bloom filter size must be greater than 0.\n";
    std::cout << "Input should be like: " << INPUT;
    exit(-1);
  }

  // ========== Input Validity Check ==========

  std::ifstream inputFile(argv[2]);

  while (getline(inputFile, str)) {
    // Output the text from the file
    std::cout << str << std::endl;
  }

  system("echo The program terminated with normal exit code [0]");
  return 0;
}