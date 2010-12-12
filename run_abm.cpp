#include <utility>

#include "abm_interfaces.h"
#include "run_abm.h"

ReflectPair runABM(int nSamples, double azimuthalAngle, 
        double polarAngle, const InterfaceList &interfaceList) {

    double reflectance   = 1.0/0.0;
    double transmittance = 1.0/0.0;


    return ReflectPair(reflectance, transmittance);

}
