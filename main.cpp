#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sys/stat.h>

#include "StringLibrary.hpp"
#include "List.hpp"
#include "HashTable.hpp"
#include "BloomFilter.hpp"
#include "SkipList.hpp"
#include "Date.hpp"
#include "Person.hpp"
#include "Virus.hpp"
#include "Country.hpp"
#include "Record.hpp"

#define INPUT "\n./vaccineMonitor -c citizenRecordsFile –b bloomSize\n"

typedef std::string string;

struct recordInfo {
  string idStr, firstName, lastName, countryName,
  ageStr, virusName, status, dateVaccinated;
};

struct recordObject {
  Person person;
  Virus virus;
  Country country;
  Record record;
};

struct appDataBase {
  HashTable<Person> citizenRegistry;
  List<Virus> virusList;
  List<Country> countryList;
  SkipList<Record> vaccinatedSkipList;
  SkipList<int> nonVaccinatedSkipList;
};

// Main menu options
enum Option {
  exitProgram = 0, statusBloom = 1,
  statusList = 2, popStatus = 3,
  popStatusAge = 4, insertRecord = 5,
  vaccinateNow = 6, listNonVaccinated = 7
};

// Returns -1 on invalid option or number of selected option in enumeration
int getOptions (string input) {
  if (!input.compare("/exit")) return exitProgram;
  // if (!input.compare("/vaccineStatusBloom")) return statusBloom;
  // if (!input.compare("/vaccineStatus")) return statusList;
  // if (!input.compare("/populationStatus")) return popStatus;
  // if (!input.compare("/popStatusByAge")) return popStatusAge;
  // if (!input.compare("/insertCitizenRecord")) return insertRecord;
  // if (!input.compare("/vaccinateNow")) return vaccinateNow;
  // if (!input.compare("/list-nonVaccinated-Persons")) return listNonVaccinated;
  if (!input.compare("/1")) return statusBloom;
  if (!input.compare("/2")) return statusList;
  if (!input.compare("/3")) return popStatus;
  if (!input.compare("/4")) return popStatusAge;
  if (!input.compare("/5")) return insertRecord;
  if (!input.compare("/6")) return vaccinateNow;
  if (!input.compare("/7")) return listNonVaccinated;
  return -1;
}

// Reads and validates a record line while also splitting it.
// It stores the splitted results into given string variables
static bool testRecord(string &line, recordInfo &recInfo) {
  List<string> args;
  Date date;
  // Push the arguments in the argument list
  splitLine(line, args);
  // Check if the record had enough information
  if (args.getSize() < 7) return false;

  // Retrieve the arguments from the argument list
  recInfo.idStr = args.getFirst();
  args.popFirst();
  recInfo.firstName = args.getFirst();
  args.popFirst();
  recInfo.lastName = args.getFirst();
  args.popFirst();
  recInfo.countryName = args.getFirst();
  args.popFirst();
  recInfo.ageStr = args.getFirst();
  args.popFirst();
  recInfo.virusName = args.getFirst();
  args.popFirst();
  recInfo.status = args.getFirst();
  args.popFirst();

  // A negative age is not invalid
  if (myStoi(recInfo.ageStr) < 0) return false;
  // If there's no date after YES, the record is faulty
  if (!recInfo.status.compare("YES") && args.empty()) return false;
  // If there's a date after NO, the record is faulty
  if (!recInfo.status.compare("NO") && !args.empty()) return false;
  // Read the date for status == YES or set it to invalid for NO
  if (!recInfo.status.compare("YES")) recInfo.dateVaccinated = args.getFirst();
  else recInfo.dateVaccinated.clear();
  // Set the object Date properly for further validation
  date.set(recInfo.dateVaccinated);
  // Validate dates ONLY for records with vaccination status = YES
  if (!date.valid() && !recInfo.status.compare("YES")) return false;

  // If we made it till here, the record is valid...
  return true;
}

void insertNewrecord(recordInfo &recInfo, recordObject &obj, appDataBase &app) {

  // ===== Store virus information =====
  if (!recInfo.status.compare("YES")) {
    // Insert in BloomFilter
    obj.virus.setName(recInfo.virusName);
    Virus *virusPtr = app.virusList.search(obj.virus);
    if (!virusPtr) {
      // If that's the first time we see this virus, insert it as new
      app.virusList.insertAscending(obj.virus);
      // Search again for it, as it should be in the list now
      virusPtr = app.virusList.search(obj.virus);
    } else virusPtr->insertBloom(recInfo.idStr);

    // Set up a new record
    obj.record.set(myStoi(recInfo.idStr), recInfo.dateVaccinated);
    // and insert it in the vaccinated skip-list
    app.vaccinatedSkipList.insert(obj.record);
  } else {
    // Insert in non-vaccinated skip-list.
    // We're not seting up a record this time,
    // as there's no point to store a date.
    // So we just pass the citizen's ID
    app.nonVaccinatedSkipList.insert(myStoi(recInfo.idStr));
  }

  // ===== Store country and person information =====
  obj.country.setName(recInfo.countryName);
  Country *countryPtr = app.countryList.search(obj.country);
  if (!countryPtr) {
    // If that's the first citizen, create the new country
    app.countryList.insertAscending(obj.country);
    // Search again for it, as it should be in the list now
    countryPtr = app.countryList.search(obj.country);
  }

  obj.person.set(myStoi(recInfo.idStr), recInfo.firstName,
    recInfo.lastName, countryPtr, myStoi(recInfo.ageStr));
  // Avoid duplicate persons in citizen Registry and country population
  if (!app.citizenRegistry.search(myStoi(recInfo.idStr), obj.person)) {
    app.citizenRegistry.insert(myStoi(recInfo.idStr), obj.person);
    countryPtr->addPerson();
  }
}

int main (int argc, char *argv[]) {

  // ========== Input Validation Check Start ==========

  string line;
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

  // =========== Input Validation Check End ===========

  std::ifstream inputFile(argv[2]);
  if (!inputFile.is_open()) {
    std::cerr << "Couldn't open " << argv[2] << ".\n";
    exit(-1);
  }

  // ========== Variables ==========
  int bloomSize = myStoi(argv[4]);
  BloomFilter bloom(bloomSize, HASH_FUNCTIONS_NUMBER);
  // Used for input lines as an argument stack implemented by a list
  List<string> args;
  // Keeps all necessary info for a valid record
  recordInfo recInfo;
  // Contains all the required objects that are used as buffer to insert
  // the info of a record into the appropriate data structure
  recordObject obj;
  // Contains all the data structrures that implement the app's database for the queries
  appDataBase app;
  bool recordOk = false;
  // ========== Variables ==========

  /* Validate & import the input records into the application structure */
  while (std::getline(inputFile, line)) {

    // Split the current line in variables and do basic validation
    recordOk = testRecord(line, recInfo);
    if (recordOk) {
      // Then insert the valid info into the appropriate structures of the app
      insertNewrecord(recInfo, obj,  app);
    } else {
      std::cerr << "Error in record: " << line << std::endl;
      continue;
    }
  }

  /* Validate & import the input records into the application structure */

  inputFile.close();
  std::cout << "Done reading '" << argv[2] << "'\n";

  // string command;
  // int option = -1;

  // do {
  //   option = -1;
  //   while(option == -1) {
  //     line.clear();
  //     getline(std::cin, line);
  //     splitLine(line, args);
  //     command = args.getFirst();
  //     args.popFirst();
  //     option = getOptions(command);
  //   }

  //   switch (option) {
  //     case 1:
  //       std::cout << "I am option 1\n";
  //       break;
  //     case 2:
  //       std::cout << "I am option 2\n";
  //       break;
  //     case 3:
  //       std::cout << "I am option 3\n";
  //       break;
  //     case 4:
  //       std::cout << "I am option 4\n";
  //       break;
  //     case 5:
  //       std::cout << "I am option 5\n";
  //       break;
  //     case 6:
  //       std::cout << "I am option 6\n";
  //       break;
  //     case 7:
  //       std::cout << "I am option 7\n";
  //       break;
  //     default:
  //       break;
  //   }

  // } while (option);


  // app.countryList.print();
  // app.virusList.print();
  // app.citizenRegistry.print();

  std::cout << "Exiting application now...\n";
  system("echo");
  system("echo Exit code: $?");
  return 0;

}