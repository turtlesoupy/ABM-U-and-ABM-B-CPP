#ifndef __ABMU_INTERFACES_H
#define __ABMU_INTERFACES_H

#include "abm_interfaces.h"

class ABMBInterfaceList : public InterfaceList {
    public:
        ABMBInterfaceList(const Sample &sample, double airRI, double cuticleRI, double mesophyllRI, double
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

class ABMBInterfaceListBuilder : public ABMInterfaceListBuilder {
    public:
        ABMBInterfaceListBuilder(const std::string &dataDirectory);
    protected:
        virtual InterfaceList *createInterfaceList(const Sample &sample, double airRI, 
                double cuticleRI, double mesophyllRI, double antidermalRI,
                double mesophyllAbsorption, double mesophyllThickness);
};

#endif
