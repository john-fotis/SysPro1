CPP			= g++
FLAGS		= -g -c -Wall
HEADER	= bloomFilter.h skipList.h
SOURCE	= main.cc
OBJS		= main.o bloomFilter.o skipList.o
LDLIBS	=
TARGET	= vaccineMonitor

$(TARGET): $(OBJS)
	$(CPP) -g $(OBJS) -o $@ $(LDLIBS)

main: main.cc
	$(CPP) $(FLAGS) main.cc

bloomFilter: bloomFilter.cc
	$(CPP) $(FLAGS) bloomFilter.cc

skipList: skipList.cc
	$(CPP) $(FLAGS) skipList.cc

clean:
	rm -f $(OBJS) $(TARGET)

count:
	wc $(SOURCE) $(HEADER)