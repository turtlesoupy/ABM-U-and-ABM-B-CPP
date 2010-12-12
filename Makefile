CC = gcc
CPP = g++
CFLAGS = -Wall -g
OBJECTS = abmu.o sample_parser.o abm_interfaces.o run_abm.o
LIBS = -lyajl

.cpp.o:
	$(CPP) $(CFLAGS) -c $<

abmu: $(OBJECTS)
	$(CPP) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)

clean:
	rm -f *.o abmu
