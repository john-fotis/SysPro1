#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <sys/stat.h>

#include "../header/BloomFilter.hpp"
#include "../header/Country.hpp"
#include "../header/Date.hpp"
#include "../header/HashTable.hpp"
#include "../header/List.hpp"
#include "../header/Person.hpp"
#include "../header/Record.hpp"
#include "../header/SkipList.hpp"
#include "../header/StringLibrary.hpp"
#include "../header/Virus.hpp"
#include "../header/VirusCountryEntry.hpp"

#define CITIZEN_REGISTRY_SIZE 1000
#define VIRUS_COUNTRY_ENTRIES 100
#define INPUT "\n./vaccineMonitor -c citizenRecordsFile –b bloomSize\n"
#define ARGUMENTS_NUMBER "Invalid number of arguments\n"

typedef std::string string;

// Variables used as temporary buffer to store a record's arguments
struct recordInfo {
  string idStr, firstName, lastName, countryName,
  ageStr, virusName, status, dateVaccinated;
};

// Upon constructing this object we set up the virus object to have the desired
// bloomSize. Since all objects of this struct are meant to be used as buffer
// variables to initialize all application objects, every virus that will be
// created, will be a copy of this object at its initial size and bitArray
// values, with the difference to its name value. Thus, every different virus
// differs only by name, and has the same size and initial bitArray status as
// all the others. Symmetrically, every person is copied by this Person object
// we just change this objects desired info before doing so. Same goes for each 
// country, record and vCountryEntry objects that will be created.
struct recordObject {
  Virus virus;
  Person person;
  Country country;
  Record record;
  VirusCountryEntry vCountryEntry;
  Date date1, date2;  
  // Constructor to set up the desired bloom size for every virus bloom filter
  // Note that we will NOT insert any citizen in this virus filter!!
  recordObject(unsigned int bloomSize) : virus(bloomSize) {}
};

// Basic data structrures that implement the database of the app
struct appDataBase {
  // A table that hold all the citizen information
  HashTable<Person> citizenRegistry;
  // A list with all known viruses
  List<Virus> virusList;
  // A list with all known countries
  List<Country> countryList;
  // A table with entries that associate virus statistics for every country
  HashTable<VirusCountryEntry> entriesTable;
  appDataBase(unsigned int pTableSize, unsigned int entryTableSize)
  : citizenRegistry(pTableSize), entriesTable(entryTableSize) {}
};

// Main menu options
enum Option {
  exitProgram = 0, statusBloom = 1,
  statusList = 2, popStatus = 3,
  popStatusAge = 4, insertRecord = 5,
  vaccinateNow = 6, listNonVaccinated = 7, menu = 8
};

// Returns -1 on invalid option or number of selected option in enumeration
int getOptions (string input) {
  if (!input.compare("/exit")) return exitProgram;
  if (!input.compare("/vaccineStatusBloom")) return statusBloom;
  if (!input.compare("/vaccineStatus")) return statusList;
  if (!input.compare("/populationStatus")) return popStatus;
  if (!input.compare("/popStatusByAge")) return popStatusAge;
  if (!input.compare("/insertCitizenRecord")) return insertRecord;
  if (!input.compare("/vaccinateNow")) return vaccinateNow;
  if (!input.compare("/list-nonVaccinated-Persons")) return listNonVaccinated;
  if (!input.compare("/menu")) return menu;
  return -1;
}

void printOptions() {
  std::cout << "\nOptions:\n" \
  "=========================================\n" \
  "/vaccineStatusBloom citizenID virusName\n" \
  "/vaccineStatus citizenID virusName\n" \
  "/vaccineStatus citizenID\n" \
  "/populationStatus [country] virusName date1 date2\n" \
  "/popStatusByAge [country] virusName date1 date2\n" \
  "/insertCitizenRecord citizenID firstName lastName country age virusName YES/NO [date]\n" \
  "/vaccinateNow citizenID firstName lastName country age virusName\n" \
  "/list-nonVaccinated-Persons virusName\n" \
  "/exit\n" \
  "=========================================\n";
}

// Reads and validates a record line while also splitting it.
// It stores the splitted results into given string variables
static bool testRecord(string &line, recordInfo &recInfo, recordObject &obj, appDataBase &db) {
  List<string> args;
  Date date;
  // Push the arguments in the argument list
  splitLine(line, args);
  // Check if the record had enough information
  if (args.getSize() != 7 && args.getSize() != 8) return false;

  // Retrieve the arguments from the argument list
  recInfo.idStr.assign(*args.getNode(0));
  recInfo.firstName.assign(*args.getNode(1));
  recInfo.lastName.assign(*args.getNode(2));
  recInfo.countryName.assign(*args.getNode(3));
  recInfo.ageStr.assign(*args.getNode(4));
  recInfo.virusName.assign(*args.getNode(5));
  recInfo.status.assign(*args.getNode(6));

  // A negative ID is not valid
  if (myStoi(recInfo.idStr) < 0) return false;
  // Check for non alpha characters in firstName
  if (containsNonAlpha(recInfo.firstName)) return false;
  // Check for non alpha characters in lastName
  if (containsNonAlpha(recInfo.lastName)) return false;
  // Check for non alpha characters in countryName
  if (containsNonAlpha(recInfo.countryName)) return false;
  // A negative age is not valid
  if (myStoi(recInfo.ageStr) < 0) return false;
  // The status will either be YES or NO
  if (recInfo.status.compare("YES") && recInfo.status.compare("NO")) return false;
  // If there's no date after YES, the record is faulty
  if (!recInfo.status.compare("YES") && args.getSize() != 8) return false;
  // If there's a date after NO, the record is faulty
  if (!recInfo.status.compare("NO") && args.getSize() != 7) return false;
  // Read the date for status == YES or set it to invalid for NO
  if (!recInfo.status.compare("YES")) recInfo.dateVaccinated.assign(*args.getNode(7));
  else recInfo.dateVaccinated.clear();
  // Set the object Date properly for further validation
  date.set(recInfo.dateVaccinated);
  // Validate dates ONLY for records with vaccination status = YES
  // because for NO we have set the ID's to be 0-0-0 by default
  // which would cause every NO record to fail this test
  if (!date.valid() && !recInfo.status.compare("YES")) return false;

  // ================== Person Identification ==================
  obj.country.setName(recInfo.countryName);
  Country *countryPtr = db.countryList.search(obj.country);
  if (countryPtr) {
    // If that's NOT the first record from this country, check out this person's info
    obj.person.set(myStoi(recInfo.idStr), recInfo.firstName,
      recInfo.lastName, countryPtr, myStoi(recInfo.ageStr));
    Person *personPtr = db.citizenRegistry.search(recInfo.idStr, obj.person);
    if (personPtr) // If this ID is already in the registry, validate all the rest person's info
      if (!personPtr->isIdentical(obj.person)) return false;
  }

  // If we made it till here, the record is valid...
  return true;
}

// Returns true on successfull import and false if the record was a duplicate
bool insertNewrecord(recordInfo &recInfo, recordObject &obj, appDataBase &db) {

  // ========== Duplicates check - Start ==========

  // Set up the new record and virus information
  obj.record.set(myStoi(recInfo.idStr), recInfo.dateVaccinated);
  obj.virus.setName(recInfo.virusName);

  // Look for the appropriate virus
  Virus *virusPtr = db.virusList.search(obj.virus);
  if (!virusPtr) { // If that's the first time we see this virus, insert it as new
    db.virusList.insertAscending(obj.virus);
    // Search again for it, as it should be in the list now
    virusPtr = db.virusList.search(obj.virus);
    // Initialize this virus filter by copying the virus prototype (required for bloomSize)
    virusPtr->initializeBloom(obj.virus);
  } else { // In this case we need to check if this is a duplicate record
    // If the person already has a record in EITHER of the two
    // skip lists, or the bloom filter, then this is a duplicate record
    if (virusPtr->checkBloom(recInfo.idStr))
      // Attempt to save some time by asking the filter first and not the skip list
      if (virusPtr->searchVaccinatedList(obj.record)) return false;
    if (virusPtr->searchNonVaccinatedList(myStoi(recInfo.idStr))) return false;
  }

  // =========== Duplicates check - End ===========

  // =========== Insert Record Info Start ===========

  // Since the record passed the duplication check we can now insert it
  if (!recInfo.status.compare("YES")) {
    // For positive records we insert both, in the bloom filter
    // and the vaccinated skip list of the current virus
    virusPtr->insertBloom(recInfo.idStr);
    virusPtr->insertVaccinatedList(obj.record);
  } else {
    // Insert in non-vaccinated skip-list.
    // We're not seting up a record this time,
    // as there's no point to store a date.
    // So we just pass the citizen's ID
    virusPtr->insertNonVaccinatedList(myStoi(recInfo.idStr));
  }

  // ======= Store country person and virus information =======

  // Look for the appropriate country
  obj.country.setName(recInfo.countryName);
  Country *countryPtr = db.countryList.search(obj.country);
  if (!countryPtr) {
    // If that's the first citizen, create the new country
    db.countryList.insertAscending(obj.country);
    // Search again for it, as it should be in the list now
    countryPtr = db.countryList.search(obj.country);
  }

  // Set up the person object
  obj.person.set(myStoi(recInfo.idStr), recInfo.firstName,
    recInfo.lastName, countryPtr, myStoi(recInfo.ageStr));
  Person *personPtr = db.citizenRegistry.search(recInfo.idStr, obj.person);
  // Insert the person information in the registry only if he isn't already in
  if (!personPtr) db.citizenRegistry.insert(recInfo.idStr, obj.person);

  // Insert the new virus-country entry if its NOT already stored
  VirusCountryEntry * entryPtr;
  // Set up the new entry
  obj.vCountryEntry.set(virusPtr, countryPtr);
  // The key of each entry is virusName + countryName
  entryPtr = db.entriesTable.search(recInfo.virusName+recInfo.countryName, obj.vCountryEntry);
  if (!entryPtr) {
    // Insert it in the table
    db.entriesTable.insert(recInfo.virusName + recInfo.countryName, obj.vCountryEntry);
    // And search for it again, as it must be in now
    entryPtr = db.entriesTable.search(recInfo.virusName + recInfo.countryName, obj.vCountryEntry);
  }
  // Finally, having ensured entryPtr != NULL we can make the appropriate accounting
  // for the vaccination statistics in the corresponding virus-country table
  entryPtr->registerPerson(myStoi(recInfo.ageStr), recInfo.status);
  
  // ============= Insert Record Info End =============

  // If we made it till here, the record was imported successfully...
  return true;
}

// Print format for popStatusByAge without dates
void printAllStatisticsByAge(VirusCountryEntry *entryPtr) {
  if (!entryPtr) return;

  float result1, result2, result3, result4;
  entryPtr->getTotal_0_20() ?
  result1=(100*(float)entryPtr->vac_0_20()/entryPtr->getTotal_0_20()) : result1=0;
  entryPtr->getTotal_20_40() ?
  result2=(100*(float)entryPtr->vac_20_40()/entryPtr->getTotal_20_40()) : result2=0;
  entryPtr->getTotal_40_60() ?
  result3=(100*(float)entryPtr->vac_40_60()/entryPtr->getTotal_40_60()) : result3=0;
  entryPtr->getTotal_60_plus() ?
  result4=(100*(float)entryPtr->vac_60_plus()/entryPtr->getTotal_60_plus()) : result4=0;

  std::cout << std::endl << entryPtr->getCountry().getName()
  << " " << entryPtr->getVirus().getName() << std::endl
  << std::setw(7) << "0-20: " << std::setw(5) << entryPtr->vac_0_20()
  << "  " << std::fixed << std::setprecision(2) << result1 << "%\n"
  << std::setw(7) << "20-40: " << std::setw(5) << entryPtr->vac_20_40()
  << "  " << std::fixed << std::setprecision(2) << result2 << "%\n"
  << std::setw(7) << "40-60: " << std::setw(5) << entryPtr->vac_40_60()
  << "  " << std::fixed << std::setprecision(2) << result3 << "%\n"
  << std::setw(7) << "60+: " << std::setw(5) << entryPtr->vac_60_plus()
  << "  " << std::fixed << std::setprecision(2) << result4 << "%\n";
}

// Print format for popStatusByAge with dates
void printByAgeWithDates(VirusCountryEntry *entryPtr,
  unsigned int age_0_20, unsigned int age_20_40, unsigned int age_40_60,
  unsigned int age_60_plus, Date date1, Date date2) {
  if (!entryPtr) return;

  float result1, result2, result3, result4;
  entryPtr->getTotal_0_20() ?
  result1=(100*(float)age_0_20/entryPtr->getTotal_0_20()) : result1=0;
  entryPtr->getTotal_20_40() ?
  result2=(100*(float)age_20_40/entryPtr->getTotal_20_40()) : result2=0;
  entryPtr->getTotal_40_60() ?
  result3=(100*(float)age_40_60/entryPtr->getTotal_40_60()) : result3=0;
  entryPtr->getTotal_60_plus() ?
  result4=(100*(float)age_60_plus/entryPtr->getTotal_60_plus()) : result4=0;

  std::cout << std::endl << entryPtr->getCountry().getName()
  << " " << entryPtr->getVirus().getName() << " BETWEEN "
  << date1 << " AND " << date2 << std::endl
  << std::setw(7) << "0-20: " << std::setw(5) << age_0_20
  << "  " << std::fixed << std::setprecision(2) << result1 << "%\n"
  << std::setw(7) << "20-40: " << std::setw(5) << age_20_40
  << "  " << std::fixed << std::setprecision(2) << result2 << "%\n"
  << std::setw(7) << "40-60: " << std::setw(5) << age_40_60
  << "  " << std::fixed << std::setprecision(2) << result3 << "%\n"
  << std::setw(7) << "60+: " << std::setw(5) << age_60_plus
  << "  " << std::fixed << std::setprecision(2) << result4 << "%\n";
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
  if (line.compare("-c")) {
    std::cerr << "Invalid record file identifier.\n";
    std::cout << "Input should be like: " << INPUT;
    exit(-1);
  }

  if ((stat(argv[2], &fileName))) {
    std::cerr << "Input file not found.\n";
    std::cout << "Input should be like: " << INPUT;
    exit(-1);
  }

  line.assign(argv[3]);
  if (line.compare("-b")) {
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
  // Used for input lines as an argument stack implemented by a list
  List<string> args;
  // Keeps all necessary info for a valid record
  recordInfo recInfo;
  // Contains all the required objects that are used as buffer to insert
  // the info of a record into the appropriate data structure
  // It's initialised with the desired bloomSize
  recordObject obj(myStoi(argv[4]));
  // Contains all the data structrures that implement the app's database for the queries
  appDataBase db(CITIZEN_REGISTRY_SIZE, VIRUS_COUNTRY_ENTRIES);
  bool recordOk = false, inserted = true;
  unsigned inconsistentRecs = 0, duplicateRecs = 0, totalRecords = 0;
  // ========== Variables ==========

  /* Validate & import the input records into the application structure */
  while (std::getline(inputFile, line)) {
    totalRecords++;
    // Split the current line in variables and do basic validation
    recordOk = testRecord(line, recInfo, obj, db);
    if (recordOk) {
      // Then insert the valid info into the appropriate structures of the app
      inserted = insertNewrecord(recInfo, obj,  db);
      if (!inserted) {
        std::cerr << "DUPLICATE RECORD: " << line << std::endl;
        duplicateRecs++;
      }
    } else {
      inconsistentRecs++;
      std::cerr << "ERROR IN RECORD: " << line << std::endl;
      // Dump this record
      continue;
    }
  }
  /* Validate & import the input records into the application structure */

  inputFile.close();
  std::cout << "\nDone reading '" << argv[2] << "'\n"
  << "Excluded " << inconsistentRecs+duplicateRecs
  << "/" << totalRecords << " records.\n"
  << "Inconsistent records: " << std::setw(4) << inconsistentRecs << std::endl
  << "Duplicate records:" << std::setw(8) << duplicateRecs << std::endl;


  // Current time!
  std::time_t t = std::time(0);
  std::tm *time = std::localtime(&t);

  // Pointers to handle all the structures and objects
  Virus *virusPtr;
  Country *countryPtr;
  Person *personPtr;
  Record *recordPtr;
  int *idPtr;
  VirusCountryEntry *entryPtr;

  // Variables for menu options
  string command;
  int option = -1;

  // These will count the vaccinated persons within certain dates.
  // When dates are NOT given we retrieve the all-time information
  // from the correspondent Virus-Country Entry in the database.
  unsigned int age_0_20 = 0;
  unsigned int age_20_40 = 0;
  unsigned int age_40_60 = 0;
  unsigned int age_60_plus = 0;
  unsigned int totalVaccinated = 0;

  // ========== Main application menu - Queries ==========
  do {

    option = -1;
    while(option == -1) {
      std::cout << "\nType /menu to display the menu or /exit to exit the application.\n\n";
      line.clear();
      getline(std::cin, line);
      splitLine(line, args);
      // Caution for empty input!
      if (args.getSize()) command = args.getFirst();
      else command.assign("\n");
      args.popFirst();
      option = getOptions(command);
    }

    switch (option) {

      case 1: // Vaccine status bloom filter

        if (args.getSize() != 2) {
          std::cerr << ARGUMENTS_NUMBER << std::endl;
          break;
        }
        recInfo.idStr.assign(*args.getNode(0));
        recInfo.virusName.assign(*args.getNode(1));
        obj.virus.setName(recInfo.virusName);
        virusPtr = db.virusList.search(obj.virus);
        if (virusPtr) {
          virusPtr->checkBloom(recInfo.idStr)? std::cout << "\nMAYBE\n" : std::cout << "\nNO\n";
          break;
        } else std::cerr << "\nNO SUCH VIRUS FOUND" << std::endl;
        break;

      case 2: // Vaccine status skip list & vaccine status for all viruses

        if (args.getSize() == 2) { // Case: /vaccineStatus citizenID virusName
          recInfo.idStr.assign(*args.getNode(0));
          recInfo.virusName.assign(*args.getNode(1));
          obj.virus.setName(recInfo.virusName);
          virusPtr = db.virusList.search(obj.virus);

          if (virusPtr) {
            obj.record.setID(myStoi(recInfo.idStr));
            recordPtr = virusPtr->searchVaccinatedList(obj.record);
            recordPtr? std::cout << "\nVACCINATED ON "
            << recordPtr->getDate() << std::endl : std::cout << "\nNOT VACCINATED\n";
          } else std::cerr << "\nNO SUCH VIRUS FOUND\n" << std::endl;
        }
        else if (args.getSize() == 1) { // Case: /vaccineStatus citizenID
          obj.record.setID(myStoi(args.getFirst()));
          // Loop through all the known viruses
          for (int i = 0; i < db.virusList.getSize(); i++) {
            virusPtr = db.virusList.getNode(i);
            obj.virus.setName(virusPtr->getName());
            std::cout << std::setw(11) << virusPtr->getName() << std::setw(5);
            recordPtr = virusPtr->searchVaccinatedList(obj.record);
            if (!recordPtr) std::cout << " NO\n";
            else std::cout << " YES " << std::setw(10) << recordPtr->getDate() << std::endl;
          } std::cout << std::endl;
        }
        else std::cerr << ARGUMENTS_NUMBER << std::endl;
        break;

      case 3: // Population status
        
        switch (args.getSize()) {

          case 1: // Case: /populationStatus virusName

            obj.virus.setName(*args.getNode(0));
            virusPtr = db.virusList.search(obj.virus);

            if (virusPtr) {
              // Loop through all the known Countries
              for (int i = 0; i < db.countryList.getSize(); i++) {
                countryPtr = db.countryList.getNode(i);
                // Find the right virus-country entry in the hashtable
                // in order to retrieve the statistics.
                // Set up the correct entry parameters
                obj.vCountryEntry.set(virusPtr, countryPtr);
                entryPtr = db.entriesTable.search(
                  virusPtr->getName() + countryPtr->getName(), obj.vCountryEntry
                );
                if (entryPtr) {
                  // Print the results for the country
                  std::cout << std::setw(11) << countryPtr->getName() << " "
                    << std::setw(5) << entryPtr->getTotalVaccinated() << " ";
                  if (entryPtr->getTotalRegistered())
                    std::cout << std::fixed << std::setprecision(2)
                    << (100*(float)entryPtr->getTotalVaccinated()\
                    /entryPtr->getTotalRegistered()) << "%\n";
                  else std::cout << "0.00%\n";
                }
              }
            } else std::cerr << "\nNO SUCH VIRUS FOUND\n";
            break;

          case 2: // Case: /populationStatus country virusName

            obj.country.setName(*args.getNode(0));
            obj.virus.setName(*args.getNode(1));
            // Find the right virus-country entry in the hashtable
            // in order to retrieve the statistics
            countryPtr = db.countryList.search(obj.country);
            virusPtr = db.virusList.search(obj.virus);

            if (countryPtr && virusPtr) {
              // Set up the correct entry parameters
              obj.vCountryEntry.set(virusPtr, countryPtr);
              entryPtr = db.entriesTable.search(
                virusPtr->getName() + countryPtr->getName(), obj.vCountryEntry
              );
              if (entryPtr) {
                // Print the results for the country
                std::cout << countryPtr->getName() << " "
                  << std::setw(5) << entryPtr->getTotalVaccinated() << " ";
                if (entryPtr->getTotalRegistered())
                  std::cout << std::fixed << std::setprecision(2)
                  << (100*(float)entryPtr->getTotalVaccinated()\
                  /entryPtr->getTotalRegistered()) << "%\n";
                else std::cout << "0.00%\n";
              }
            }
            else if (!countryPtr && !virusPtr) std::cerr << "\nINVALID COUNTRY AND VIRUS NAME\n"; 
            else if (!countryPtr) std::cerr << "\nNO SUCH COUNTRY FOUND\n";
            else std::cerr << "\nNO SUCH VIRUS FOUND\n";
            break;

          case 3: // Case: /populationStatus virusName date1 date 2

            obj.virus.setName(*args.getNode(0));
            obj.date1.set(*args.getNode(1));
            obj.date2.set(*args.getNode(2));
            virusPtr = db.virusList.search(obj.virus);

            if (virusPtr && obj.date1.valid() && obj.date2.valid() && (obj.date1<=obj.date2)) {
              // Loop through all the known Countries
              for (int i = 0; i < db.countryList.getSize(); i++) {
                countryPtr = db.countryList.getNode(i);
                // Reset the vaccinated citizens counter for this country
                totalVaccinated = 0;
                // We locate the corresponting virus-country entry in order
                // to get the total vaccinated persons per age-category
                obj.vCountryEntry.set(virusPtr, countryPtr);
                entryPtr = db.entriesTable.search(
                  virusPtr->getName() + countryPtr->getName(), obj.vCountryEntry
                );
                // We will check every positive record of this virus
                // and country and make sure its within the given dates
                for (int recNum = 0; recNum < virusPtr->getVaccinatedListSize(); recNum++) {
                  recordPtr = virusPtr->getPositiveRecordNumber(recNum);
                  // Validate the record's dates
                  if (recordPtr->getDate()>=obj.date1 && recordPtr->getDate()<=obj.date2) {
                    // Validate this belongs to the country we're looking for
                    recInfo.idStr.assign(toString(recordPtr->ID()));
                    obj.person.set(myStoi(recInfo.idStr), recInfo.firstName,
                      recInfo.lastName, countryPtr, myStoi(recInfo.ageStr));
                    // Find the persons info through the citizen registry
                    personPtr = db.citizenRegistry.search(recInfo.idStr, obj.person);
                    // If the country is the same as the one we're checking
                    if (!personPtr->getCountry().getName().compare(countryPtr->getName()))
                      totalVaccinated++;
                  }
                }
                if (entryPtr) {
                  // Print the results for the country
                  std::cout << countryPtr->getName() << " ";
                  if (entryPtr->getTotalRegistered())
                    std::cout << std::setw(5) << totalVaccinated << " "
                    << std::fixed << std::setprecision(2)
                    << (100*(float)totalVaccinated/entryPtr->getTotalRegistered()) << "%\n";
                  else std::cout << "0.00%\n";
                }
              }
            }
            else if (!virusPtr) std::cerr << "\nNO SUCH VIRUS FOUND\n";
            else std::cerr << "\nINVALID DATES GIVEN\n";
            break;

          case 4: // Case: /populationStatus country virusName date1 date2

            obj.country.setName(*args.getNode(0));
            obj.virus.setName(*args.getNode(1));
            obj.date1.set(*args.getNode(2));
            obj.date2.set(*args.getNode(3));
            // Locate the virus and country in the database
            countryPtr = db.countryList.search(obj.country);
            virusPtr = db.virusList.search(obj.virus);
            // Reset the vaccinated citizens counter for this country
            totalVaccinated = 0;

            if (virusPtr && countryPtr && obj.date1.valid() && obj.date2.valid() && (obj.date1<=obj.date2)) {
              // We locate the corresponting virus-country entry in order
              // to get the total vaccinated persons per age-category
              obj.vCountryEntry.set(virusPtr, countryPtr);
              entryPtr = db.entriesTable.search(
                virusPtr->getName() + countryPtr->getName(), obj.vCountryEntry
              );
              // We will check every positive record of this virus
              // and country and make sure its within the given dates
              for (int recNum = 0; recNum < virusPtr->getVaccinatedListSize(); recNum++) {
                recordPtr = virusPtr->getPositiveRecordNumber(recNum);
                // Validate the record's dates
                if (recordPtr->getDate()>=obj.date1 && recordPtr->getDate()<=obj.date2) {
                  // Validate this belongs to the country we're looking for
                  recInfo.idStr.assign(toString(recordPtr->ID()));
                  obj.person.set(myStoi(recInfo.idStr), recInfo.firstName,
                    recInfo.lastName, countryPtr, myStoi(recInfo.ageStr));
                  // Find the persons info through the citizen registry
                  personPtr = db.citizenRegistry.search(recInfo.idStr, obj.person);
                  // If the country is the same as the one we're checking
                  if (!personPtr->getCountry().getName().compare(countryPtr->getName()))
                    totalVaccinated++;
                }
              }
              if (entryPtr) {
                // Print the results for the country
                std::cout << countryPtr->getName() << " ";
                if (entryPtr->getTotalRegistered())
                  std::cout << std::setw(5) << totalVaccinated << " "
                  << std::fixed << std::setprecision(2)
                  << (100*(float)totalVaccinated/entryPtr->getTotalRegistered()) << "%\n";
                else std::cout << "0.00%\n";
              }
            }
            else if (!virusPtr && !countryPtr) std::cerr << "\nINVALID VIRUS AND COUNTRY NAME\n";
            else if (!virusPtr) std::cerr << "\nNO SUCH VIRUS FOUND\n";
            else if (!countryPtr) std::cerr << "\nNO SUCH COUNTRY FOUND\n";
            else std::cerr << "\nINVALID DATES GIVEN\n";
            break;
          default:
            std::cerr << ARGUMENTS_NUMBER << std::endl;
            break;
        }

        break; // populationStatus - END

      case 4: // Population status by age

        switch (args.getSize()) {

          case 1: // Case: /popStatusByAge virusName

            obj.virus.setName(*args.getNode(0));
            virusPtr = db.virusList.search(obj.virus);
            if (virusPtr) {
              // Loop through all the known Countries
              for (int i = 0; i < db.countryList.getSize(); i++) {
                countryPtr = db.countryList.getNode(i);
                // Find the right virus-country entry in the hashtable
                // in order to retrieve the statistics.
                // Set up the correct entry parameters
                obj.vCountryEntry.set(virusPtr, countryPtr);
                entryPtr = db.entriesTable.search(
                  virusPtr->getName() + countryPtr->getName(), obj.vCountryEntry
                );
                printAllStatisticsByAge(entryPtr);
              }
            } else std::cerr << "\nNO SUCH VIRUS FOUND\n";
            break;

          case 2: // Case: /popStatusByAge country virusName

            obj.country.setName(*args.getNode(0));
            obj.virus.setName(*args.getNode(1));
            countryPtr = db.countryList.search(obj.country);
            virusPtr = db.virusList.search(obj.virus);
            // Find the right virus-country entry in the hashtable
            // in order to retrieve the statistics
            if (countryPtr && virusPtr) {
              // Set up the correct entry parameters
              obj.vCountryEntry.set(virusPtr, countryPtr);
              entryPtr = db.entriesTable.search(
                virusPtr->getName() + countryPtr->getName(), obj.vCountryEntry
              );
              printAllStatisticsByAge(entryPtr);
            }
            else if (!countryPtr && !virusPtr) std::cerr << "\nINVALID COUNTRY AND VIRUS NAME\n"; 
            else if (!countryPtr) std::cerr << "\nNO SUCH COUNTRY FOUND\n";
            else std::cerr << "\nNO SUCH VIRUS FOUND\n";
            break;

          case 3: // Case: /popStatusByAge virusName date1 date 2

            obj.virus.setName(*args.getNode(0));
            obj.date1.set(*args.getNode(1));
            obj.date2.set(*args.getNode(2));
            virusPtr = db.virusList.search(obj.virus);

            if (virusPtr && obj.date1.valid() && obj.date2.valid() && (obj.date1<=obj.date2)) {
              // Loop through all the known Countries
              for (int i = 0; i < db.countryList.getSize(); i++) {
                countryPtr = db.countryList.getNode(i);
                // We locate the corresponting virus-country entry in order
                // to get the total vaccinated persons per age-category
                obj.vCountryEntry.set(virusPtr, countryPtr);
                entryPtr = db.entriesTable.search(
                  virusPtr->getName() + countryPtr->getName(), obj.vCountryEntry
                );
                // Reset the vaccinated citizens counters for this country
                age_0_20 = age_20_40 = age_40_60 = age_60_plus = 0;
                // We will check every positive record of this virus and make
                // sure its within the given dates and belongs to this country
                for (int recNum = 0; recNum < virusPtr->getVaccinatedListSize(); recNum++) {
                  recordPtr = virusPtr->getPositiveRecordNumber(recNum);
                  // Validate the record's dates
                  if (recordPtr->getDate()>=obj.date1 && recordPtr->getDate()<=obj.date2) {
                    // Validate this belongs to the country we're looking for
                    recInfo.idStr.assign(toString(recordPtr->ID()));
                    obj.person.set(myStoi(recInfo.idStr), recInfo.firstName,
                      recInfo.lastName, countryPtr, myStoi(recInfo.ageStr));
                    // Find the persons info through the citizen registry
                    personPtr = db.citizenRegistry.search(recInfo.idStr, obj.person);
                    // If the country is the same as the one we're checking
                    if (!personPtr->getCountry().getName().compare(countryPtr->getName())) {
                      // Find out in which age category he belongs
                      if (personPtr->getAge() <= 20) age_0_20++;
                      else if (personPtr->getAge() <= 40) age_20_40++;
                      else if (personPtr->getAge() <= 60) age_60_plus++;
                      else age_60_plus++;
                    }
                  }
                }
                // Print out the results
                printByAgeWithDates(entryPtr, age_0_20, age_20_40,
                  age_40_60, age_60_plus, obj.date1, obj.date2);
              }
            }
            else if (!virusPtr) std::cerr << "\nNO SUCH VIRUS FOUND\n";
            else std::cerr << "\nINVALID DATES GIVEN\n";
            break;

          case 4: // Case: /popStatusByAge country virusName date1 date2

            obj.country.setName(*args.getNode(0));
            obj.virus.setName(*args.getNode(1));
            obj.date1.set(*args.getNode(2));
            obj.date2.set(*args.getNode(3));
            // Locate the virus and country in the database
            countryPtr = db.countryList.search(obj.country);
            virusPtr = db.virusList.search(obj.virus);

            if (virusPtr && countryPtr && obj.date1.valid() && obj.date2.valid() && (obj.date1<=obj.date2)) {
              // We locate the corresponting virus-country entry in order
              // to get the total vaccinated persons per age-category
              obj.vCountryEntry.set(virusPtr, countryPtr);
              entryPtr = db.entriesTable.search(
                virusPtr->getName() + countryPtr->getName(), obj.vCountryEntry
              );
              // Reset the vaccinated citizens counters for this country
              age_0_20 = age_20_40 = age_40_60 = age_60_plus = 0;
              // We will check every positive record of this virus and make
              // sure its within the given dates and belongs to this country
              for (int recNum = 0; recNum < virusPtr->getVaccinatedListSize(); recNum++) {
                recordPtr = virusPtr->getPositiveRecordNumber(recNum);
                // Validate the record's dates
                if (recordPtr->getDate()>=obj.date1 && recordPtr->getDate()<=obj.date2) {
                  // Validate this belongs to the country we're looking for
                  recInfo.idStr.assign(toString(recordPtr->ID()));
                  obj.person.set(myStoi(recInfo.idStr), recInfo.firstName,
                    recInfo.lastName, countryPtr, myStoi(recInfo.ageStr));
                  // Find the persons info through the citizen registry
                  personPtr = db.citizenRegistry.search(recInfo.idStr, obj.person);
                  // If the country is the same as the one we're checking
                  if (!personPtr->getCountry().getName().compare(countryPtr->getName())) {
                    // Find out in which age category he belongs
                    if (personPtr->getAge() <= 20) age_0_20++;
                    else if (personPtr->getAge() <= 40) age_20_40++;
                    else if (personPtr->getAge() <= 60) age_60_plus++;
                    else age_60_plus++;
                  }
                }
              }
              // Print out the results
              printByAgeWithDates(entryPtr, age_0_20, age_20_40,
                age_40_60, age_60_plus, obj.date1, obj.date2);
            }
            else if (!virusPtr && !countryPtr) std::cerr << "\nINVALID VIRUS AND COUNTRY NAME\n";
            else if (!virusPtr) std::cerr << "\nNO SUCH VIRUS FOUND\n";
            else if (!countryPtr) std::cerr << "\nNO SUCH COUNTRY FOUND\n";
            else std::cerr << "\nINVALID DATES GIVEN\n";
            break;
          default:
            std::cerr << ARGUMENTS_NUMBER << std::endl;
            break;
        }

        break; // popStatusByAge - END

      case 5: // Insert citizen record

        if (args.getSize() != 7 && args.getSize() != 8) {
          std::cerr << ARGUMENTS_NUMBER << std::endl;
          break;
        }

        // If the record has vaccination status YES and the date is missing
        if (!args.getNode(6)->compare("YES") && args.getSize()==7) {
          // Set up today's date
          obj.date1.set();
          obj.date1.get(recInfo.dateVaccinated);
          // And push it as the last argument in the arguments list
          args.insertLast(recInfo.dateVaccinated);
        }

        line.clear();
        // Reassemble the input line ignoring the command in order
        // to have only the record arguments in the string line
        while(!args.empty()) {
          line.append(args.getFirst());
          line.append(" ");
          args.popFirst();
        }

        // Validate the constructed record
        recordOk = testRecord(line, recInfo, obj, db);
        if (recordOk) {
          inserted = insertNewrecord(recInfo, obj, db);
          if (inserted) std::cout << "\nSUCCESSFULLY INSERTED RECORD: " << line << std::endl;
          else {
            // The record was considered as duplicate, so look out for the reason
            // obj.virus holds the name of the virus the user gave
            virusPtr = db.virusList.search(obj.virus);
            if (virusPtr) {
              // recInfo.idStr holds the ID the user gave
              obj.record.setID(myStoi(recInfo.idStr));
              recordPtr = virusPtr->searchVaccinatedList(obj.record);
              // If the record was found in vaccinated skip list
              if (recordPtr) std::cerr << "\nERROR: CITIZEN " << recordPtr->ID() 
                << " ALREADY VACCINATED ON " << recordPtr->getDate() << std::endl;

              idPtr = virusPtr->searchNonVaccinatedList(myStoi(recInfo.idStr));
              // If the record was found in non vaccinated skip list
              if (idPtr) {
                // Since it was rejected before there was a previous NO record
                // we need to remove the old record and try ton insert the new one again
                virusPtr->removeNonVaccinatedList(myStoi(recInfo.idStr));
                inserted = insertNewrecord(recInfo, obj, db);
                if (inserted)
                  std::cout << "\nSUCCESSFULLY INSERTED RECORD: " << line
                  << "\nPREVIOUS RECORD: " << obj.person << " "
                  << recInfo.virusName << " NO WAS DELETED\n";
              }
            }
          }
        } else std::cerr << "\nINVALID OR DUPLICATE RECORD: " << line << std::endl;
        break;

      case 6: // Vaccinate now

        if (args.getSize() != 6) {
          std::cerr << ARGUMENTS_NUMBER << std::endl;
          break;
        }

        line.clear();
        // Reassemble the input line ignoring the command in order
        // to have only the record arguments in the string line
        while (!args.empty()) {
          line.append(args.getFirst());
          line.append(" ");
          args.popFirst();
        }
        // Add 'YES' and today's date to the new record
        line.append("YES ");
        line.append(toString(time->tm_mday));
        line.append("-");
        line.append(toString(time->tm_mon + 1));
        line.append("-");
        line.append(toString(time->tm_year + 1900));

        // Validate the constructed record
        recordOk = testRecord(line, recInfo, obj, db);
        if (recordOk) {
          inserted = insertNewrecord(recInfo, obj, db);
          if (inserted) 
            std::cout << "\nSUCCESSFULLY VACCINATED CITIZEN "
            << recInfo.idStr << " FOR " << recInfo.virusName
            << " ON " << recInfo.dateVaccinated << std::endl;
          else {
            // The record was considered as duplicate, so look out for the reason
            // obj.virus holds the name of the virus the user gave
            virusPtr = db.virusList.search(obj.virus);
            if (virusPtr) {
              // recInfo.idStr holds the ID the user gave
              obj.record.setID(myStoi(recInfo.idStr));
              recordPtr = virusPtr->searchVaccinatedList(obj.record);
              // If the record was found in vaccinated skip list
              if (recordPtr) std::cerr << "\nERROR: CITIZEN " << recordPtr->ID() 
                << " ALREADY VACCINATED ON " << recordPtr->getDate() << std::endl;
              idPtr = virusPtr->searchNonVaccinatedList(myStoi(recInfo.idStr));
              // If the record was found in non vaccinated skip list
              if (idPtr) {
                // Remove it from the list
                virusPtr->removeNonVaccinatedList(myStoi(recInfo.idStr));
                // And try to insert it again.
                inserted = insertNewrecord(recInfo, obj, db);
                if (inserted)
                  std::cout << "\nSUCCESSFULLY VACCINATED CITIZEN "
                  << recInfo.idStr << " FOR " << recInfo.virusName << " ON "
                  << recInfo.dateVaccinated << std::endl;
              }
            }
          }
        } else std::cerr << "\nINVALID RECORD: " << line << std::endl;
        break;

      case 7: // Non vaccinated persons skip list

        if (args.getSize() != 1) {
          std::cerr << ARGUMENTS_NUMBER << std::endl;
          break;
        }

        obj.virus.setName(*args.getNode(0));
        virusPtr = db.virusList.search(obj.virus);

        if (virusPtr) {
          // Loop through the non vaccinated skip list of the desired virus
          for (int i = 0; i < virusPtr->getNonVaccinatedListSize(); i++) {
            // Find each person's ID to retrieve his rest data from the citizen registry
            idPtr = virusPtr->getNegativeRecordNumber(i);
            recInfo.idStr = toString(*idPtr);
            // Set up the person object to search - We don't care if the rest info
            // is not valid, as the citizen registry table locates persons only by ID
            obj.person.set(myStoi(recInfo.idStr), recInfo.firstName,
              recInfo.lastName, countryPtr, myStoi(recInfo.ageStr));
            personPtr = db.citizenRegistry.search(recInfo.idStr, obj.person);
            // The pointer now points to the correct person in the registry
            std::cout << personPtr->ID() << " " << personPtr->getFirstName() << " "
              << personPtr->getLastName() << " " << personPtr->getCountry() << " "
              << personPtr->getAge() << std::endl;
          }
        } else std::cerr << "\nNO SUCH VIRUS FOUND\n";
        break;

      case 8: // Display menu options
        printOptions();
        break;

      default:
        break;
    }
    
  } while (option);

  std::cout << "\nExiting application now...\n\n";
  system("echo Exit code: $?");
  return 0;

}