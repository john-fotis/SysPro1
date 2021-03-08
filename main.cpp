#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <cstdlib>
#include <sys/stat.h>

#include "bloomFilter.hpp"
#include "skipList.hpp"
#include "List.hpp"
#include "HashTable.hpp"
#include "Date.hpp"
#include "Record.hpp"
#include "string.hpp"

#define INPUT "\n./vaccineMonitor -c citizenRecordsFile –b bloomSize\n"

int main (int argc, char *argv[]) {

  // ========== Input Validity Check Start ==========

  std::string line;
  struct stat fileName;

  if (argc != 5) {
    std::cerr << "Invalid number of arguments.\n";
    std::cout << "Input should be like: " << INPUT;
    exit(-1);
  }

  line.assign(argv[1]);
  if (line.compare("-c") != 0) {
    std::cerr << "Invalid record file identifier.\n";
    std::cout << "Input should be like: " << INPUT;
    exit(-1);
  }

  if ((stat(argv[2], &fileName) != 0)) {
    std::cerr << "Input file not found.\n";
    std::cout << "Input should be like: " << INPUT;
    exit(-1);
  }

  line.assign(argv[3]);
  if (line.compare("-b") != 0) {
    std::cerr << "Invalid bloom filter identifier.\n";
    std::cout << "Input should be like: " << INPUT;
    exit(-1);
  }

  line.assign(argv[4]);
  for (int i = 0; i < line.length(); i++) {
    if (isdigit(line[i]) == false) {
      std::cerr << "Invalid bloom filter size.\n";
      std::cout << "Input should be like: " << INPUT;
      exit(-1);
    }
  }

  if ((myStoi(argv[4]) <= 0)) {
    std::cerr << "Bloom filter size must be greater than 0.\n";
    std::cout << "Input should be like: " << INPUT;
    exit(-1);
  }

  // ========== Input Validity Check End ==========

  // Validate & import the records into the hash table - Start

  std::ifstream inputFile(argv[2]);
  int bloomSize = myStoi(argv[4]);
  unsigned int id = 100;
  unsigned int age = 10;
  std::string idStr, firstName, lastName, country, ageStr, virusName, status, dateVaccinated;
  bool inconsistent = false;
  Record record;
  HashTable<Record> table;

  int i = 0, j = 0, len = 0;
  while (std::getline(inputFile, line)) {
    // Split the current line in variables and do basic validation
    i = j = len = 0;
    for (i = 0; line[i] != ' '; i++) len++;
    idStr = line.substr(0, len);
    len = 0; j = i + 1;
    for (i = i+1; line[i] != ' '; i++) len++;
    firstName = line.substr(j, len);
    len = 0; j = i + 1;
    for (i = i+1; line[i] != ' '; i++) len++;
    lastName = line.substr(j, len);
    len = 0; j = i + 1;
    for (i = i+1; line[i] != ' '; i++) len++;
    country = line.substr(j, len);
    len = 0; j = i + 1;
    for (i = i + 1; line[i] != ' '; i++) len++;
    ageStr = line.substr(j, len);
    len = 0; j = i + 1;
    for (i = i + 1; line[i] != ' '; i++) len++;
    virusName = line.substr(j, len);
    len = 0; j = i + 1;

    if (!(line.substr(j,2)).compare("NO"))
      status.assign("NO");
    else if (!(line.substr(j, 3)).compare("YES"))
      status.assign("YES");
    else
      std::cout << "NO" << std::endl;
    i = i + status.length();

    // Validation of status & number of words in record
    if (status.compare("YES") && status.compare("NO")) inconsistent = true;
    // If there's no date after YES, the record is faulty
    if (!status.compare("YES") && line[i+1] == '\0') inconsistent = true;
    // If there's a date after NO, the record is faulty
    if (!status.compare("NO") && line[i+1] != '\0') inconsistent = true;

    if(inconsistent) {
      std::cerr << "Inconsistent record: " << line << std::endl;
      continue;
    } else {
      j = i + 2;
      if (!status.compare("YES")) {
        for (i = i + 1; line[i] != '\0'; i++) len++;
        dateVaccinated = line.substr(j, len);
      } else dateVaccinated.assign("0-0-0");
      id = myStoi(idStr);
      age = myStoi(ageStr);
      record.set(id, firstName, lastName, country, age, virusName, status, dateVaccinated);
      table.insert(record.ID(), record);
    }
  }

  // Validate & import the records into the hash table - End

  table.print();

  system("echo");
  system("echo Exit code: $?");
  return 0;
}