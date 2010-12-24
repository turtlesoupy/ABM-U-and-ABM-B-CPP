#include <utility>
#include <cmath>
#include <ctime>
#include <iostream>

#include "abm_interfaces.h"
#include "run_abm.h"
#include "vector.h"

extern "C" {
    #include "mt19937ar.h"
}

#define RANDOM_FUNCTION genrand_real2

double freePathLength(const vec3 &vector, const vec3 &normal, double cosI, double absorptionCoefficient, bool disableSieve) {
    if(disableSieve) {
        return -(1/absorptionCoefficient) * log(RANDOM_FUNCTION()) * cosI;
    } else {
        return -(1/absorptionCoefficient) * log(RANDOM_FUNCTION()) * cos(cosI);
    }
}

vec3 reflect(const vec3 &vector, const vec3 &normal, double cosI) {
    return vector - normal * 2 * (-cosI);
}

vec3 refract(const vec3 &vector, const vec3 &normal, double cosI, double n1, double n2) {
    const double n = n1 /n2;
    return  vector * n + normal*(n*cosI - sqrt(1 - n*n*(1-cosI*cosI)));
}

double fresnellCoefficient(const vec3 &vector, const vec3 &normal, double cosI, double n1, double n2) {
    const double sinISquared = 1 - cosI*cosI;
    const double n = (n1/n2);
    const double nSquared = n*n;
    const double rootTerm = 1 - nSquared*sinISquared;
    if(rootTerm < 0) {
        return 1.0;
    } 
        
    const double rootedTerm = sqrt(rootTerm);
    double rootRS = ((n1*cosI - n2*rootedTerm) / (n1*cosI + n2*rootedTerm));
    double rootRP = ((n1*rootedTerm - n2*cosI) / (n1*rootedTerm + n2*cosI));

    return (rootRS * rootRS + rootRP * rootRP) / 2;
}

vec3 perpendicular(const vec3 &vector) {
    if( fabs(vector.x) < fabs(vector.y) && fabs(vector.x) < fabs(vector.z)) {
        return vec3(0,-vector.z, vector.y);
    } else if (fabs(vector.y) <  fabs(vector.z)) {
        return vec3(vector.z, 0, -vector.x);
    } else {
        return vec3(-vector.y, vector.x, 0);
    } 
}

vec3 brakkeScattering(const vec3 &vector, double delta) {
    vec3 perp = perpendicular(vector);
    perp.Normalize();
    const vec3 &w = vector;
    const vec3 &u = perp;
    const vec3 v = w.Cross(u);
    const double exponent = 1/(delta+1);

    vec3 perturbed = -vector;
    while(perturbed.z *  vector.z < 0) {
        double polar = acos(pow(RANDOM_FUNCTION(), exponent));
        double azimuthal = 2*M_PI*RANDOM_FUNCTION();

        double sp = sin(polar);
        double sa = sin(azimuthal);
        double cp = cos(polar);
        double ca = cos(azimuthal);

        perturbed = u * (sp*ca) + (v*sp*sa) + (w*cp);
    }

    return perturbed;
}

ReflectPair runABM(int nSamples, double azimuthalAngle, 
        double polarAngle, bool disableSieve, InterfaceList &interfaceList) {
    int startState;
    int reflectedState;
    int transmittedState;
    int absorbedState = -2;
    int numReflected   = 0;
    int numTransmitted = 0;
    int numAbsorbed    = 0;

    double sp = sin(polarAngle);
    vec3 startingPosition(
        cos(azimuthalAngle)*sp,
        sin(azimuthalAngle)*sp,
        cos(polarAngle)
    );

    startingPosition.z *= -1; /* Leaf interfaces are listed adaxial-first, 
                                 orientation is generally assumed with respect to abaxial */

    if(startingPosition.z < 0) {
        startState = 0;
        reflectedState = -1;
        transmittedState = interfaceList.size();
    } else {
        startState = interfaceList.size() - 1;
        reflectedState = startState + 1;
        transmittedState = -1;
    }

    for(int i = 0; i < nSamples; i++) {
        vec3 direction(startingPosition);
        int state = startState;
        interfaceList.prepareForSample();

        while(state != reflectedState && state != transmittedState && state != absorbedState) {
            const ABMInterface &interface = interfaceList.getInterface(state);

            double n1;
            double n2;
            double perturbanceReflect;
            double perturbanceRefract;
            int reflectState;
            int refractState;
            double thickness;
            double absorption;
            vec3 normal(0,0,0);

            if(direction.z < 0) {
                normal.z = 1.0;
                n1 = interface.nAbove;
                n2 = interface.nBelow;
                perturbanceReflect = interface.perturbanceDownAbove;
                perturbanceRefract = interface.perturbanceDownBelow;
                refractState = state + 1;
                reflectState = state - 1;
                thickness    = interface.thicknessAbove;
                absorption   = interface.absorptionAbove;
            } else {
                normal.z = -1.0;
                n1 = interface.nBelow;
                n2 = interface.nAbove;
                perturbanceReflect = interface.perturbanceUpBelow;
                perturbanceRefract = interface.perturbanceUpAbove;
                reflectState = state + 1;
                refractState = state - 1;
                thickness  = interface.thicknessBelow;
                absorption = interface.absorptionBelow;
            }

            double normalAngle = -direction.Dot(normal);

            if(thickness > 0 && freePathLength(direction, normal, normalAngle, absorption, disableSieve) < thickness) {
                state = absorbedState;
                break;
            } else {
                if(RANDOM_FUNCTION() < fresnellCoefficient(direction, normal, normalAngle, n1, n2)) {
                    state = reflectState;
                    direction = reflect(direction, normal, normalAngle);
                    if(perturbanceReflect != INFINITY) {
                        direction = brakkeScattering(direction, perturbanceReflect);
                    }
                } else {
                    state = refractState;
                    direction = refract(direction, normal, normalAngle, n1, n2);
                    if(perturbanceReflect != INFINITY) {
                        direction = brakkeScattering(direction, perturbanceRefract);
                    }
                }
            }
        }

        if(state == reflectedState) {
            numReflected++;
        } else if(state == transmittedState) {
            numTransmitted++;
        } else {
            numAbsorbed++;
        }
    }
    return ReflectPair((double)numReflected / nSamples, (double)numTransmitted / nSamples);
}
