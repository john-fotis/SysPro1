# ========== Run "make help" to see the available commands ==========
CPP	= g++
FLAGS	= -g -c -Wall -std=c++0x
HEADER	= BloomFilter.hpp SkipList.hpp List.hpp HashTable.hpp Person.hpp Record.hpp Date.hpp StringLibrary.hpp hashFunctions.hpp Virus.hpp Country.hpp VirusCountryEntry.hpp
SOURCE	= main.cpp BloomFilter.cpp SkipList.cpp List.cpp HashTable.cpp Person.cpp Record.cpp Virus.cpp Country.cpp VirusCountryEntry.cpp
OBJS	= main.o BloomFilter.o SkipList.o List.o HashTable.o Person.o Record.o Virus.o Country.o VirusCountryEntry.o
SCRIPTS = testFile.sh
LDLIBS	=
TARGET	= vaccineMonitor
INFILE	= citizenRecordsFile # Application input File
RUNSIZE	= 1000 # Bloomfilter size in bytes

$(TARGET): $(OBJS)
	$(CPP) -g $(OBJS) -o $@ $(LDLIBS)

main: main.cpp
	$(CPP) $(FLAGS) main.cpp

BloomFilter: BloomFilter.cpp
	$(CPP) $(FLAGS) BloomFilter.cpp

SkipList: SkipList.cpp
	$(CPP) $(FLAGS) SkipList.cpp

List: List.cpp
	$(CPP) $(FLAGS) List.cpp

HashTable: HashTable.cpp
	$(CPP) $(FLAGS) HashTable.cpp

Person: Person.cpp
	$(CPP) $(FLAGS) Person.cpp

Record: Record.cpp
	$(CPP) $(FLAGS) Record.cpp

Virus: Virus.cpp
	$(CPP) $(FLAGS) Virus.cpp

Country: Country.cpp
	$(CPP) $(FLAGS) Country.cpp

VirusCountryEntry: VirusCountryEntry.cpp
	$(CPP) $(FLAGS) VirusCountryEntry.cpp

clean:
	rm -f *.o $(OBJS) $(TARGET)

count:
	wc -l -w $(SOURCE) $(HEADER) $(SCRIPTS)

run:
	./$(TARGET) -c $(INFILE) -b $(RUNSIZE)

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