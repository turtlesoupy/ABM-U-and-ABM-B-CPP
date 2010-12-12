#include <vector>
#include <iostream>
#include <cstdio>
#include <string>

#include "abm_interfaces.h"
#include "run_abm.h"
#include "sample_parser.h"
#include "sample.h"

void usage() { 
    fprintf(stderr, "Usage: ./abmu <sample.json> <data_dir>\n");
}

int main(int argc, char *argv[]) {
    Sample sample;
    FILE *sampleFile;
    int retcode = 0;

    if(argc < 3) {
        usage();     
        return 2;
    }

    sampleFile = fopen(argv[1], "r");
    if(sampleFile == NULL) {
        fprintf(stderr, "Error while opening '%s'", argv[1]);
        return 2;
    }

    if(parseSampleFromFile(&sample, sampleFile)) {
        ABMInterfaces interfaceBuilder(argv[2]);
        InterfaceList interfaces = interfaceBuilder.buildInterfaces(sample, 400);
        runABM(1000, 0.0, 172.0, interfaces);
    } else {
        fprintf(stderr, "Error while parsing sample json\n");
        retcode = 1;
    }


    fclose(sampleFile);


    return retcode;
}
