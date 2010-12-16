CC=gcc
CXX = g++
CFLAGS=-I./include -Wall -O2 -pthread
CXXFLAGS=$(CFLAGS)
OBJECTS = src/sample_parser.o src/abm_interfaces.o src/run_abm.o src/mt19937ar.o
ABMU_OBJECTS = $(OBJECTS) src/abmu.o src/abmu_interfaces.o
ABMB_OBJECTS = $(OBJECTS) src/abmb.o src/abmb_interfaces.o
LIBS = -lyajl -lpthread

all: abmu abmb

abmu: $(ABMU_OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $(ABMU_OBJECTS) $(LIBS)

abmb: $(ABMB_OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $(ABMB_OBJECTS) $(LIBS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o abmu abmb
