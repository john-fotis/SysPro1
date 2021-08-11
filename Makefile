# ========== Run "make help" to see the available commands ==========
CPP	= g++
FLAGS	= -g -c -Wall -std=c++0x
HEADER	= $(HDR)BloomFilter.hpp $(HDR)SkipList.hpp $(HDR)List.hpp $(HDR)HashTable.hpp $(HDR)Person.hpp $(HDR)Record.hpp $(HDR)Date.hpp $(HDR)StringLibrary.hpp $(HDR)hashFunctions.hpp $(HDR)Virus.hpp $(HDR)Country.hpp $(HDR)VirusCountryEntry.hpp
SOURCE	= $(SRC)main.cpp $(SRC)BloomFilter.cpp $(SRC)SkipList.cpp $(SRC)List.cpp $(SRC)HashTable.cpp $(SRC)Person.cpp $(SRC)Record.cpp $(SRC)Virus.cpp $(SRC)Country.cpp $(SRC)VirusCountryEntry.cpp
OBJS	= $(SRC)main.o $(SRC)BloomFilter.o $(SRC)SkipList.o $(SRC)List.o $(SRC)HashTable.o $(SRC)Person.o $(SRC)Record.o $(SRC)Virus.o $(SRC)Country.o $(SRC)VirusCountryEntry.o
SCRIPTS = testFile.sh
LDLIBS	=
TARGET	= vaccineMonitor
HDR = ./header/
SRC = ./source/

# Run parameters
VFILE	= virusesFile
# script viruses file
CFILE	= countriesFile
# script vountries file
NUMRECS	= 10000
# number of records to be produced by script
DUPFLAG	= 0
# 0 for non duplicates, non-zero for duplicates
INFILE	= citizenRecordsFile
# Application input File
RUNSIZE	= 1000
# Bloomfilter size in bytes

$(TARGET): $(OBJS)
	$(CPP) -g $(OBJS) -o $@ $(LDLIBS)
	
%.o: %.cpp
	$(CPP) $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

count:
	wc -l -w $(SOURCE) $(HEADER) $(SCRIPTS)

run:
	./$(TARGET) -c $(INFILE) -b $(RUNSIZE)

scriptRun:
	./$(SCRIPTS) $(VFILE) $(CFILE) $(NUMRECS) $(DUPFLAG)

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --show-reachable=yes --track-origins=yes ./$(TARGET) -c $(INFILE) -b $(RUNSIZE)

help:
	@echo Options:
	@printf "make%20s -- build the application\n"
	@printf "make $(TARGET)%5s -- build $(TARGET)\n"
	@printf "make clean%14s -- delete application and objective files\n"
	@printf "make count%14s -- project line and words accounting\n"
	@printf "make run%16s -- run $(TARGET)\n"
	@printf "make valgrind%11s -- run $(TARGET) with valgrind enabled\n"
	@printf "make help%15s -- view this help message\n"