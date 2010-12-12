#ifndef __ABM_INTERFACES_H
#define __ABM_INTERFACES_H
#include <vector>
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
    int splitAbove;
    int splitBelow;
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
        splitAbove(0),
        splitBelow(0),
        name("Unnamed Interface")
    {

    }
};

typedef std::vector<ABMInterface> InterfaceList;

class ABMInterfaces {
    public:
        ABMInterfaces(const std::string &dataDirectory);
        ~ABMInterfaces();

        InterfaceList buildInterfaces(const Sample &sample, int wavelength);

    private:
        void readAllData(const std::string &dataDirectory);
        void readData(std::string filename, DataList &dlist);

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
