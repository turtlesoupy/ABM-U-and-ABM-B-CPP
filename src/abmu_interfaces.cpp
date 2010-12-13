#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "abm_interfaces.h"
#include "abmu_interfaces.h"
#include "sample.h"
#include "mt19937ar.h"

#define RANDOM_FUNCTION genrand_real2

ABMUInterfaceList::ABMUInterfaceList(const Sample &sample, double airRI, double cuticleRI, double mesophyllRI, 
                   double antidermalRI, double mesophyllAbsorption, double mesophyllThickness) :
    airRI(airRI),
    cuticleRI(cuticleRI),
    mesophyllRI(mesophyllRI),
    antidermalRI(antidermalRI),
    mesophyllAbsorption(mesophyllAbsorption),
    mesophyllThickness(mesophyllThickness)
{
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

   interfaces.push_back(mesophyllEpidermis);

   ABMInterface epidermisAir;
   epidermisAir.name = "Adaxial Epidermis <-> Air";
   epidermisAir.nAbove = cuticleRI;
   epidermisAir.nBelow = airRI;
   epidermisAir.perturbanceDownAbove = sample.spongyCellCapsAspectRatio;
   epidermisAir.perturbanceUpAbove   = sample.spongyCellCapsAspectRatio;
   epidermisAir.perturbanceUpBelow   = sample.cuticleUndulationsAspectRatio;

   interfaces.push_back(epidermisAir);
}

void ABMUInterfaceList::prepareForSample() {
    double p = RANDOM_FUNCTION();
    interfaces[1].thicknessBelow =  p    * mesophyllThickness;
    interfaces[2].thicknessAbove =  p    * mesophyllThickness;
    interfaces[3].thicknessBelow = (1-p) * mesophyllThickness;
    interfaces[4].thicknessAbove = (1-p) * mesophyllThickness;
}

ABMUInterfaceListBuilder::ABMUInterfaceListBuilder(const std::string &dataDirectory) :
    ABMInterfaceListBuilder(dataDirectory)
{
}

InterfaceList *ABMUInterfaceListBuilder::createInterfaceList(const Sample &sample, double airRI,
                        double cuticleRI, double mesophyllRI, double antidermalRI,
                        double mesophyllAbsorption, double mesophyllThickness) {

    return new ABMUInterfaceList(sample, airRI, cuticleRI, mesophyllRI, antidermalRI, 
            mesophyllAbsorption, mesophyllThickness);
}

