#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "abm_interfaces.h"
#include "abmb_interfaces.h"
#include "sample.h"

ABMBInterfaceList::ABMBInterfaceList(const Sample &sample, double airRI, double cuticleRI, double mesophyllRI, 
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
   epidermisMesophyll.perturbanceDownBelow = sample.palisadeCellCapsAspectRatio;
   epidermisMesophyll.perturbanceUpBelow   = sample.palisadeCellCapsAspectRatio;
   epidermisMesophyll.thicknessBelow = mesophyllThickness;
   epidermisMesophyll.absorptionBelow = mesophyllAbsorption;

   interfaces.push_back(epidermisMesophyll);

   ABMInterface mesophyllAir;
   mesophyllAir.name = "Mesophylll<->Air";
   mesophyllAir.nAbove = mesophyllRI;
   mesophyllAir.nBelow = airRI;
   mesophyllAir.perturbanceDownAbove = sample.palisadeCellCapsAspectRatio;
   mesophyllAir.perturbanceUpAbove   = sample.palisadeCellCapsAspectRatio;
   mesophyllAir.perturbanceDownBelow = sample.spongyCellCapsAspectRatio;
   mesophyllAir.perturbanceUpBelow   = sample.spongyCellCapsAspectRatio;
   mesophyllAir.thicknessAbove = mesophyllThickness;
   mesophyllAir.absorptionAbove = mesophyllAbsorption;

   interfaces.push_back(mesophyllAir);

   ABMInterface airAntidermalWall;
   airAntidermalWall.name = "Air<->Antidermal Wall";
   airAntidermalWall.nAbove = airRI;
   airAntidermalWall.nBelow = antidermalRI;

   interfaces.push_back(airAntidermalWall);

   ABMInterface antidermalWallCuticle;
   antidermalWallCuticle.name = "Antidermal Wall<->Cuticle";
   antidermalWallCuticle.nAbove = antidermalRI;
   antidermalWallCuticle.nBelow = cuticleRI;
   antidermalWallCuticle.perturbanceDownBelow = sample.epidermisCellCapsAspectRatio;
   antidermalWallCuticle.perturbanceUpBelow   = sample.epidermisCellCapsAspectRatio;

   interfaces.push_back(antidermalWallCuticle);

   ABMInterface cuticleAir;
   cuticleAir.name = "Cuticle <-> Air";
   cuticleAir.nAbove = cuticleRI;
   cuticleAir.nBelow = airRI;
   cuticleAir.perturbanceDownAbove = sample.epidermisCellCapsAspectRatio;
   cuticleAir.perturbanceUpAbove   = sample.epidermisCellCapsAspectRatio;
   cuticleAir.perturbanceUpBelow   = sample.cuticleUndulationsAspectRatio;

   interfaces.push_back(cuticleAir);
}

void ABMBInterfaceList::prepareForSample() {
}

ABMBInterfaceListBuilder::ABMBInterfaceListBuilder(const std::string &dataDirectory) :
    ABMInterfaceListBuilder(dataDirectory)
{
}

InterfaceList *ABMBInterfaceListBuilder::createInterfaceList(const Sample &sample, double airRI,
                        double cuticleRI, double mesophyllRI, double antidermalRI,
                        double mesophyllAbsorption, double mesophyllThickness) {

    return new ABMBInterfaceList(sample, airRI, cuticleRI, mesophyllRI, antidermalRI, 
            mesophyllAbsorption, mesophyllThickness);
}

