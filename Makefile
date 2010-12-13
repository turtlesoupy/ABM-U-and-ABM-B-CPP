CC = gcc
CPP = g++
CFLAGS = -Wall -O2
OBJECTS = sample_parser.o abm_interfaces.o run_abm.o mt19937ar.o
ABMUOBJECTS = $(OBJECTS) abmu.o abmu_interfaces.o
ABMBOBJECTS = $(OBJECTS) abmb.o abmb_interfaces.o
LIBS = -lyajl

all: abmu abmb

abmu: $(ABMUOBJECTS)
	$(CPP) $(LDFLAGS) -o $@ $(ABMUOBJECTS) $(LIBS)

abmb: $(ABMBOBJECTS) abmb.o abmb_interfaces.o
	$(CPP) $(LDFLAGS) -o $@ $(ABMBOBJECTS) $(LIBS)

%.o : %.cpp
	$(CPP) $(CFLAGS) -c $<

%.o : %.c
	$(CPP) $(CFLAGS) -c $<

clean:
	rm -f *.o abmu abmb
