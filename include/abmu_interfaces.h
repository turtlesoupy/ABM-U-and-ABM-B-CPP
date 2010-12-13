#ifndef __ABMU_INTERFACES_H
#define __ABMU_INTERFACES_H

#include "abm_interfaces.h"

class ABMUInterfaceList : public InterfaceList {
    public:
        ABMUInterfaceList(const Sample &sample, double airRI, double cuticleRI, double mesophyllRI, double
                antidermalRI, double mesophyllAbsorption, double mesophyllThickness);
        virtual void prepareForSample();

    private:
        double airRI;
        double cuticleRI;
        double mesophyllRI;
        double antidermalRI;
        double mesophyllAbsorption;
        double mesophyllThickness;
};

class ABMUInterfaceListBuilder : public ABMInterfaceListBuilder {
    public:
        ABMUInterfaceListBuilder(const std::string &dataDirectory);
    protected:
        virtual InterfaceList *createInterfaceList(const Sample &sample, double airRI, 
                double cuticleRI, double mesophyllRI, double antidermalRI,
                double mesophyllAbsorption, double mesophyllThickness);
};

#endif
