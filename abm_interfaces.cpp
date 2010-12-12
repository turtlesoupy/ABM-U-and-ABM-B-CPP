#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include "abm_interfaces.h"
#include "sample.h"

DataList::DataList() : begin(0), step(0) {

}

DataList::DataList(std::ifstream &input,
        unsigned int begin, unsigned int step) : begin(begin), step(step) {
    std::string tmp;
    while(std::getline(input, tmp)) {
        if(tmp != "") {
            double val = atof(tmp.c_str());
            data.push_back(val);
        }
    }
}

void DataList::adjustData(double multiplicationFactor) {
    for(std::vector<double>::iterator it = data.begin(); it != data.end(); it++) {
        *it *= multiplicationFactor;
    }
}

DataList::~DataList() {

}

double DataList::lookup(int wavelength) const {
    int ind = (wavelength - begin) / (double)step;
    std::cout << "INDEX: " << ind << "; LENGTH: " << data.size() << std::endl;
    return data[ind];
}


ABMInterfaces::~ABMInterfaces() {

}

ABMInterfaces::ABMInterfaces(const std::string &dataDirectory)  :
    dataDirectory(dataDirectory)    
{
    readAllData(dataDirectory);
}

InterfaceList ABMInterfaces::buildInterfaces(const Sample &sample, int wavelength) {
   double proteinAbsorptionCoefficient     = sample.proteinConcentration * proteinAbsorption.lookup(wavelength);
   double chlorophyllAbsorptionCoefficient = (sample.chlorophyllAConcentration + sample.chlorophyllBConcentration)  
       * chlorophyllAbsorption.lookup(wavelength);
   double carotenoidAbsorptionCoefficient = sample.carotenoidConcentration * carotenoidAbsorption.lookup(wavelength);
   double celluloseAbsorptionCoefficient = sample.celluloseConcentration * celluloseAbsorption.lookup(wavelength);
   double linginAbsorptionCoefficient = sample.linginConcentration * celluloseAbsorption.lookup(wavelength); //Use cellulose for lingin due to lack of data
   double waterAbsorptionCoefficient = waterAbsorption.lookup(wavelength);

   double mesophyllAbsorption = proteinAbsorptionCoefficient + chlorophyllAbsorptionCoefficient + 
       carotenoidAbsorptionCoefficient + celluloseAbsorptionCoefficient + linginAbsorptionCoefficient +
       waterAbsorptionCoefficient;

   double cuticleRI = cuticleRefractiveIndex.lookup(wavelength);
   double mesophyllRI = mesophyllRefractiveIndex.lookup(wavelength);
   double antidermalRI = antidermalRefractiveIndex.lookup(wavelength);
   double airRI = 1.0;

   double mesophyllThickness = sample.mesophyllFraction * sample.wholeLeafThickness;

   //ABM-U
   InterfaceList interfaces;

   ABMInterface airCuticle;
   airCuticle.name   = "Air<->Adaxial Epidermis";
   airCuticle.nAbove = airRI;
   airCuticle.nBelow = cuticleRI;
   airCuticle.perturbanceDownAbove = sample.cuticleUndulationsAspectRatio;
   airCuticle.perturbanceDownBelow = sample.epidermisCellCapsAspectRatio;
   airCuticle.perturbanceUpBelow = sample.epidermisCellCapsAspectRatio;

   interfaces.push_back(airCuticle);

   ABMInterface epidermisMesophyll;
   epidermisMesophyll.name = "Adaxial Epidermis<->Mesophyll";
   epidermisMesophyll.nAbove = cuticleRI;
   epidermisMesophyll.nBelow = mesophyllRI;
   epidermisMesophyll.perturbanceDownAbove = sample.epidermisCellCapsAspectRatio;
   epidermisMesophyll.perturbanceUpAbove   = sample.epidermisCellCapsAspectRatio;
   epidermisMesophyll.perturbanceDownBelow = sample.spongyCellCapsAspectRatio;
   epidermisMesophyll.perturbanceUpBelow   = sample.spongyCellCapsAspectRatio;
   epidermisMesophyll.thicknessBelow = mesophyllThickness;
   epidermisMesophyll.absorptionBelow = mesophyllAbsorption;
   epidermisMesophyll.splitBelow = 1;

   interfaces.push_back(epidermisMesophyll);

   ABMInterface mesophyllAir;
   mesophyllAir.name = "Mesophylll<->Air";
   mesophyllAir.nAbove = mesophyllRI;
   mesophyllAir.nBelow = airRI;
   mesophyllAir.perturbanceDownAbove = sample.spongyCellCapsAspectRatio;
   mesophyllAir.perturbanceUpAbove   = sample.spongyCellCapsAspectRatio;
   mesophyllAir.perturbanceDownBelow = sample.spongyCellCapsAspectRatio;
   mesophyllAir.perturbanceUpBelow   = sample.spongyCellCapsAspectRatio;
   mesophyllAir.thicknessAbove = mesophyllThickness;
   mesophyllAir.absorptionAbove = mesophyllAbsorption;
   mesophyllAir.splitAbove = 1;

   interfaces.push_back(mesophyllAir);

   ABMInterface airMesophyll;
   airMesophyll.name = "Air<->Mesophyll";
   airMesophyll.nAbove = airRI;
   airMesophyll.nBelow = mesophyllRI;
   airMesophyll.perturbanceDownAbove = sample.spongyCellCapsAspectRatio;
   airMesophyll.perturbanceUpAbove   = sample.spongyCellCapsAspectRatio;
   airMesophyll.perturbanceDownBelow = sample.spongyCellCapsAspectRatio;
   airMesophyll.perturbanceUpBelow   = sample.spongyCellCapsAspectRatio;
   airMesophyll.thicknessBelow = mesophyllThickness;
   airMesophyll.absorptionBelow = mesophyllAbsorption;
   airMesophyll.splitBelow = 2;

   interfaces.push_back(airMesophyll);

   ABMInterface mesophyllEpidermis;
   mesophyllEpidermis.name = "Mesophyll<->Adaxial Epidermis";
   mesophyllEpidermis.nAbove = mesophyllRI;
   mesophyllEpidermis.nBelow = cuticleRI;
   mesophyllEpidermis.perturbanceDownAbove = sample.spongyCellCapsAspectRatio;
   mesophyllEpidermis.perturbanceUpAbove   = sample.spongyCellCapsAspectRatio;
   mesophyllEpidermis.perturbanceDownBelow = sample.epidermisCellCapsAspectRatio;
   mesophyllEpidermis.perturbanceUpBelow   = sample.epidermisCellCapsAspectRatio;
   mesophyllEpidermis.thicknessAbove  = mesophyllThickness;
   mesophyllEpidermis.absorptionAbove = mesophyllAbsorption;
   mesophyllEpidermis.splitAbove = 2;

   interfaces.push_back(mesophyllEpidermis);

   ABMInterface epidermisAir;
   epidermisAir.name = "Adaxial Epidermis <-> Air";
   epidermisAir.nAbove = cuticleRI;
   epidermisAir.nBelow = airRI;
   epidermisAir.perturbanceDownAbove = sample.spongyCellCapsAspectRatio;
   epidermisAir.perturbanceUpAbove   = sample.spongyCellCapsAspectRatio;
   epidermisAir.perturbanceUpBelow   = sample.cuticleUndulationsAspectRatio;

   interfaces.push_back(epidermisAir);

   return interfaces;
}

void ABMInterfaces::readAllData(const std::string &dataDirectory) {
    const std::string carotenoidAbsorptionFilename("caro-PAS-400-2500.txt");
    const std::string celluloseAbsorptionFilename("cellulose400-2500.txt");
    const std::string chlorophyllAbsorptionFilename("chloAB-DFA-400-2500.txt");
    const std::string proteinAbsorptionFilename("protein400-2500.txt");
    const std::string waterAbsorptionFilename("sacwH400-2500.txt");
    const std::string mesophyllRefractiveIndexFilename("rmH400-2500.txt");
    const std::string cuticleRefractiveIndexFilename("rcH400-2500.txt"); 
    const std::string antidermalRefractiveIndexFilename("raH400-2500.txt"); 

    readData(dataDirectory + "/" + carotenoidAbsorptionFilename, carotenoidAbsorption);
    readData(dataDirectory + "/" + celluloseAbsorptionFilename, celluloseAbsorption);
    readData(dataDirectory + "/" + chlorophyllAbsorptionFilename, chlorophyllAbsorption);
    readData(dataDirectory + "/" + proteinAbsorptionFilename, proteinAbsorption);
    readData(dataDirectory + "/" + waterAbsorptionFilename, waterAbsorption);
    readData(dataDirectory + "/" + mesophyllRefractiveIndexFilename, mesophyllRefractiveIndex);
    readData(dataDirectory + "/" + cuticleRefractiveIndexFilename, cuticleRefractiveIndex);
    readData(dataDirectory + "/" + antidermalRefractiveIndexFilename, antidermalRefractiveIndex);

    carotenoidAbsorption.adjustData(1.0 / 10.0);
    celluloseAbsorption.adjustData(1.0 / 10.0);
    chlorophyllAbsorption.adjustData(1.0 / 10.0);
    proteinAbsorption.adjustData(1.0 / 10.0);
}

void ABMInterfaces::readData(std::string filename, DataList &dlist) {
    const double step  = 5;
    const double begin = 400;
    std::ifstream f(filename.c_str());
    if(f.is_open()) {
        std::cout << "Reading in " << filename << std::endl;
        dlist = DataList(f, begin, step);
    }

    f.close();
}
