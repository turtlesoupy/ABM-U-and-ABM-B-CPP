#ifndef __ABM_INTERFACES_H
#define __ABM_INTERFACES_H
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

class Sample;

class DataList {
    public:
        DataList();
        DataList(std::ifstream &input, unsigned int begin, unsigned int step);
        ~DataList();

        void adjustData(double multiplicationFactor);
        double lookup(int wavelength) const;

    private:
        unsigned int begin;
        unsigned int step;
        std::vector<double> data;
};

class ABMInterface {
    public:
    double nAbove;
    double nBelow;
    double perturbanceDownAbove;
    double perturbanceDownBelow;
    double perturbanceUpAbove;
    double perturbanceUpBelow;
    double absorptionAbove;
    double absorptionBelow;
    double thicknessAbove;
    double thicknessBelow;
    std::string name;

    ABMInterface()  :
        nAbove(1.0),
        nBelow(1.0),
        perturbanceDownAbove(1.0/0.0),
        perturbanceDownBelow(1.0/0.0),
        perturbanceUpAbove(1.0/0.0),
        perturbanceUpBelow(1.0/0.0),
        absorptionAbove(0.0),
        absorptionBelow(0.0),
        thicknessAbove(0.0),
        thicknessBelow(0.0),
        name("Unnamed Interface")
    {

    }
    friend std::ostream &operator<<(std::ostream&, const ABMInterface&);
};

class InterfaceList {
    public:
        virtual void prepareForSample() = 0;
        ABMInterface getInterface(int index) const {
            return interfaces[index];
        }
        size_t size() const {
            return interfaces.size();
        }
    protected:
        std::vector<ABMInterface> interfaces;
};


class ABMInterfaceListBuilder {
    public:
        ABMInterfaceListBuilder() {
        }
        ABMInterfaceListBuilder(const std::string &dataDirectory);
        virtual InterfaceList *buildInterfaces(const Sample &sample, int wavelength);

    protected:
        virtual void readAllData(const std::string &dataDirectory);
        virtual void readData(std::string filename, DataList &dlist);
        virtual InterfaceList *createInterfaceList(const Sample &sample, double airRI, 
                double cuticleRI, double mesophyllRI, double antidermalRI,
                double mesophyllAbsorption, double mesophyllThickness);

        std::string dataDirectory;
        DataList carotenoidAbsorption;
        DataList celluloseAbsorption;
        DataList chlorophyllAbsorption;
        DataList proteinAbsorption;
        DataList waterAbsorption;
        DataList mesophyllRefractiveIndex;
        DataList cuticleRefractiveIndex;
        DataList antidermalRefractiveIndex;
};

#endif
