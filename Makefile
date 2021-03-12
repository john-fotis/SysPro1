CPP	= g++
FLAGS	= -g -c -Wall -std=c++0x
HEADER	= BloomFilter.hpp SkipList.hpp List.hpp HashTable.hpp Person.hpp Record.hpp Date.hpp StringLibrary.hpp hashFunctions.hpp Virus.hpp Country.hpp
SOURCE	= main.cpp BloomFilter.cpp SkipList.cpp List.cpp HashTable.cpp Person.cpp Record.cpp Virus.cpp Country.cpp
OBJS	= main.o BloomFilter.o SkipList.o List.o HashTable.o Person.o Record.o Virus.o Country.o
LDLIBS	=
TARGET	= vaccineMonitor

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

Virus: Virus.cpp
	$(CPP) $(FLAGS) Virus.cpp

Country: Country.cpp
	$(CPP) $(FLAGS) Country.cpp

Record: Record.cpp
	$(CPP) $(FLAGS) Record.cpp

clean:
	rm -f *.o $(OBJS) $(TARGET)

count:
	wc -l -w $(SOURCE) $(HEADER)

run:
	./$(TARGET) -c citizenRecordsFile -b 1000

valgrind:
	valgrind -s --leak-check=full --track-origins=yes ./$(TARGET) -c citizenRecordsFile -b 1000

help:
	@echo Options:
	@printf "make%20s -- build the application\n"
	@printf "make $(TARGET)%5s -- build $(TARGET)\n"
	@printf "make clean%14s -- delete application and objective files\n"
	@printf "make count%14s -- project line and words accounting\n"
	@printf "make run%16s -- run $(TARGET)\n"
	@printf "make valgrind%11s -- run $(TARGET) with valgrind enabled\n"
	@printf "make help%15s -- view this help message\n"