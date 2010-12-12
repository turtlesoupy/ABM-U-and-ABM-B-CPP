#ifndef __ABM_SAMPLE_PARSER_H
#define __ABM_SAMPLE_PARSER_H
#include <stdio.h>

struct Sample;
int parseSampleFromFile(struct Sample *sample, FILE *inputFile);
#endif
