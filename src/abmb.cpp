#include <cmath>
#include <cstdio>
#include <ctime>
#include <vector>
#include <algorithm>
#include <queue>
#include <pthread.h>

#include "abmb_interfaces.h"
#include "run_abm.h"
#include "sample_parser.h"
#include "sample.h"
#include "stdlib.h"
#include "unistd.h"

extern "C" {
    #include "mt19937ar.h"
}


void usage() { 
    fprintf(stderr, "Usage: ./abmb [options] <sample_file.json> <output_file.csv>\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\t-n <int>\tNumber of samples\n");
    fprintf(stderr, "\t-a <float>\tAzimuthal angle (degrees)\n");
    fprintf(stderr, "\t-p <float>\tPolar angle (degrees)\n");
    fprintf(stderr, "\t-s <int>\tWavelength step (nanometers)\n");
    fprintf(stderr, "\t-w <int>\tWavelength start (nanometers)\n");
    fprintf(stderr, "\t-e <int>\tWavelength end (nanometers)\n");
    fprintf(stderr, "\t-d <path>\tData directory\n");
    fprintf(stderr, "\t-t <int>\tNumber of threads\n");
    fprintf(stderr, "\t-q\tDisable sieve and detour effects\n");
    fprintf(stderr, "\n");
}


struct WorkTask {
    bool disableSieve;
    int wavelength; 
    int numSamples;
    double polarAngle;
    double azimuthalAngle;
    ABMBInterfaceListBuilder  *builder;
    Sample *sample;
};

struct WorkResult {
    int wavelength;
    ReflectPair pair;
};

bool resultSort (WorkResult i,WorkResult j) { return (i.wavelength<j.wavelength); }


std::vector<WorkResult> modelResults;
std::queue<WorkTask>  workTasks;
pthread_mutex_t workMutex;
pthread_mutex_t resultsMutex;

bool workEmpty() {
    return workTasks.empty();
}

void *threadWork(void *arg) {
    WorkTask task;
    while(true) {
        pthread_mutex_lock(&workMutex);
        if(workTasks.empty()) {
            pthread_mutex_unlock(&workMutex);
            break;
        } else {
            task = workTasks.front();
            workTasks.pop();
            pthread_mutex_unlock(&workMutex);
        }

        WorkResult result;
        result.wavelength = task.wavelength;
        InterfaceList *interfaces = task.builder->buildInterfaces(*task.sample, task.wavelength);
        result.pair = runABM(task.numSamples, task.azimuthalAngle, task.polarAngle, task.disableSieve, *interfaces);
        delete interfaces;

        const ReflectPair &rt = result.pair;
        fprintf(stderr, "Wavelength %d\t r:%f, t:%f, a:%f\n", task.wavelength, rt.first, rt.second, 1-(rt.first + rt.second));
        pthread_mutex_lock(&resultsMutex);
        modelResults.push_back(result);
        pthread_mutex_unlock(&resultsMutex);
    }
    pthread_exit((void*) 0);
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
    char *datadir = (char *)"data";
    int wavelengthStart = 400;
    int wavelengthEnd   = 2500;
    int c;
    int step = 5;
    int numThreads = 4;
    bool disableSieve = false;

    while((c = getopt(argc, argv, "n:a:p:w:s:e:d:t:q")) != -1) {
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
            case 't':
                numThreads = atoi(optarg);
                break;
            case 'v':
                disableSieve = true;
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


    pthread_t *workThreads = new pthread_t[numThreads];
    pthread_attr_t attr;
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    if(parseSampleFromFile(&sample, sampleFile)) {
        fprintf(outputFile, "wavelength, reflectance, transmittance, absorptance\n");

        fprintf(stderr, "Running simulation (%d samples, wavelengths %dnm-%dnm)...\n",
                numSamples, wavelengthStart, wavelengthEnd);

        //Populate work queue
        ABMBInterfaceListBuilder interfaceBuilder(datadir);
        for(int w = wavelengthStart; w <= wavelengthEnd; w+= step) {
            WorkTask task;
            task.wavelength = w;
            task.builder = &interfaceBuilder;
            task.sample = &sample;
            task.numSamples = numSamples;
            task.azimuthalAngle = azimuthalAngle;
            task.polarAngle = polarAngle;
            task.disableSieve = disableSieve;
            workTasks.push(task);
        }

        //Init threads
        pthread_attr_init(&attr);
        pthread_mutex_init(&workMutex, NULL);
        pthread_mutex_init(&resultsMutex, NULL);
        for(int i = 0; i <numThreads; i++) {
            pthread_create(&workThreads[i], &attr, threadWork, (void *)i);
        }
        pthread_attr_destroy(&attr);
        //Wait on threads
        for(int i = 0; i < numThreads; i++) {
            void *status;
            pthread_join(workThreads[i], &status);
        }
        pthread_mutex_destroy(&workMutex);
        pthread_mutex_destroy(&resultsMutex);


        std::sort(modelResults.begin(), modelResults.end(), resultSort);

        //Spit results
        for(std::vector<WorkResult>::iterator result = modelResults.begin();
                result != modelResults.end(); result++) {
            int w = result->wavelength;
            ReflectPair rt = result->pair;
            fprintf(outputFile, "%d,%f,%f,%f\n", w, rt.first, rt.second, 1-(rt.first+rt.second));
            fflush(outputFile);
        }
    } else {
        fprintf(stderr, "Error while parsing sample json\n");
        retcode = 1;
    }


    delete []workThreads;
    fclose(outputFile);
    fclose(sampleFile);


    pthread_exit((void *)retcode);
    //return retcode;
}
