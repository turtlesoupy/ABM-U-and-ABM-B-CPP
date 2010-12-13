#ifndef __RUN_ABM_H
#define __RUN_ABM_H

#include <utility>

typedef std::pair<double, double> ReflectPair;

ReflectPair runABM(int nSamples, double azimuthalAngle, double polarAngle, InterfaceList &interfaceList);

#endif
