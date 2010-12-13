CC = gcc
CPP = g++
CFLAGS = -Wall -O2
OBJECTS = abmu.o sample_parser.o abm_interfaces.o run_abm.o mt19937ar.o
LIBS = -lyajl

.cpp.o:
	$(CPP) $(CFLAGS) -c $<

.c.o:
	$(CPP) $(CFLAGS) -c $<

abmu: $(OBJECTS)
	$(CPP) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)

clean:
	rm -f *.o abmu
