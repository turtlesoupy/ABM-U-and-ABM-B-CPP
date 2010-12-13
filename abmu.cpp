#include <cmath>
#include <cstdio>
#include <ctime>
#include <vector>
#include <string>
#include <iostream>

#include "abm_interfaces.h"
#include "run_abm.h"
#include "sample_parser.h"
#include "sample.h"
#include "mt19937ar.h"
#include "stdlib.h"
#include "unistd.h"

void usage() { 
    fprintf(stderr, "Usage: ./abmu [options] <sample_file.json> <output_file.csv>\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\t-n <int>\tNumber of samples\n");
    fprintf(stderr, "\t-a <float>\tAzimuthal angle (degrees)\n");
    fprintf(stderr, "\t-p <float>\tPolar angle (degrees)\n");
    fprintf(stderr, "\t-s <int>\tWavelength step (nanometers)\n");
    fprintf(stderr, "\t-w <int>\tWavelength start (nanometers)\n");
    fprintf(stderr, "\t-e <int>\tWavelength end (nanometers)\n");
    fprintf(stderr, "\t-d <path>\tData directory\n");
    fprintf(stderr, "\n");
}

int main(int argc, char *argv[]) {
    Sample sample;
    FILE *sampleFile;
    FILE *outputFile;
    int retcode = 0;
    init_genrand(time(NULL)); // Seed twister
    int numSamples = 100000;
    double azimuthalAngle = 0.0;
    double polarAngle = 8.0 * M_PI / 180;
    char *datadir = (char *)"../data/all";
    int wavelengthStart = 400;
    int wavelengthEnd   = 2500;
    int c;
    int step = 5;

    while((c = getopt(argc, argv, "n:a:p:w:s:e:d:")) != -1) {
        switch(c) {
            case 'n':
                numSamples = atoi(optarg);
                break;
            case 'a':
                azimuthalAngle = atof(optarg) * M_PI / 180;
                break;
            case 'p':
                polarAngle = atof(optarg) * M_PI / 180;
                break;
            case 's':
                step = atoi(optarg);
                break;
            case 'w':
                wavelengthStart = atoi(optarg);
                break;
            case 'e':
                wavelengthEnd = atoi(optarg);
                break;
            case 'd':
                datadir = optarg;
                break;
            case '?':
                break;
            default:
                fprintf(stderr, "Getopt returned error\n");
                return 2;
        }
    }

    if(argc - optind != 2) {
        fprintf(stderr, "Both sample file and output file are required\n");
        usage();
        return 2;
    }


    char *sampleFilename = argv[optind];
    char *outputFilename = argv[optind+1];

    sampleFile = fopen(sampleFilename, "r");
    if(sampleFile == NULL) {
        fprintf(stderr, "Error while opening '%s'", sampleFilename);
        return 1;
    }

    outputFile = fopen(outputFilename, "w");
    if(outputFile == NULL) {
        fprintf(stderr, "Error while opening output '%s'", outputFilename);
        fclose(sampleFile);
        return 1;
    }


    if(parseSampleFromFile(&sample, sampleFile)) {
        fprintf(outputFile, "wavelength, reflectance, transmittance, absorptance\n");

        fprintf(stderr, "Running simulation (%d samples, wavelengths %dnm-%dnm)...\n",
                numSamples, wavelengthStart, wavelengthEnd);

        /*
        for(int i = 0; i < interfaces->size(); i++) {
            std::cout << interfaces->getInterface(i);
            std::cout << std::endl;
        }
        */

        ABMInterfaceListBuilder interfaceBuilder(datadir);
        for(int w = wavelengthStart; w <= wavelengthEnd; w+= step) {
            fprintf(stderr, "Wavelength %d\t", w);
            fflush(stderr);
            InterfaceList *interfaces = interfaceBuilder.buildInterfaces(sample, w);
            ReflectPair rt = runABM(numSamples, azimuthalAngle, polarAngle, *interfaces);
            delete interfaces;

            fprintf(stderr, " r:%f, t:%f, a:%f\n", rt.first, rt.second, 1-(rt.first + rt.second));
            fprintf(outputFile, "%d,%f,%f,%f\n", w, rt.first, rt.second, 1-(rt.first+rt.second));
            fflush(outputFile);
        }
    } else {
        fprintf(stderr, "Error while parsing sample json\n");
        retcode = 1;
    }


    fclose(outputFile);
    fclose(sampleFile);


    return retcode;
}
