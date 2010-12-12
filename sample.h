#ifndef __ABM_SAMPLE_H
#define __ABM_SAMPLE_H

typedef struct Sample {
    double wholeLeafThickness;
    double cuticleUndulationsAspectRatio;
    double epidermisCellCapsAspectRatio;
    double spongyCellCapsAspectRatio;
    double palisadeCellCapsAspectRatio;
    double proteinConcentration;
    double celluloseConcentration;
    double linginConcentration;
    double chlorophyllAConcentration;
    double chlorophyllBConcentration;
    double carotenoidConcentration;
    double mesophyllFraction;
    unsigned int bifacial;
} Sample;

#endif 
