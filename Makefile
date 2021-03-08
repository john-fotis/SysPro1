CPP	= g++
FLAGS	= -g -c -Wall -std=c++11
HEADER	= bloomFilter.hpp skipList.hpp List.hpp HashTable.hpp Record.hpp Date.hpp string.hpp hashFunctions.hpp
SOURCE	= main.cpp bloomFilter.cpp skipList.cpp List.cpp HashTable.cpp Record.cpp
OBJS	= main.o bloomFilter.o skipList.o List.o HashTable.o Record.o
LDLIBS	=
TARGET	= vaccineMonitor

$(TARGET): $(OBJS)
	$(CPP) -g $(OBJS) -o $@ $(LDLIBS)

main: main.cpp
	$(CPP) $(FLAGS) main.cpp

bloomFilter: bloomFilter.cpp
	$(CPP) $(FLAGS) bloomFilter.cpp

skipList: skipList.cpp
	$(CPP) $(FLAGS) skipList.cpp

List: List.cpp
	$(CPP) $(FLAGS) List.cpp

HashTable: HashTable.cpp
	$(CPP) $(FLAGS) HashTable.cpp

Record: Record.cpp
	$(CPP) $(FLAGS) Record.cpp

clean:
	rm -f $(OBJS) $(TARGET)

count:
	wc $(SOURCE) $(HEADER)

run:
	./vaccineMonitor -c citizenRecordsFile -b 10

valgrind:
	valgrind -s --leak-check=full --track-origins=yes ./vaccineMonitor -c citizenRecordsFile -b 10

help:
	@echo Options:
	@echo make
	@echo make $(TARGET)
	@echo make clean
	@echo make count
	@echo make run
	@echo make valgrind
	@echo make help