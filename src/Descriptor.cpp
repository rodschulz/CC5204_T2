/**
 * Author: rodrigo
 * 2015
 */
#include "Descriptor.h"

Descriptor::Descriptor()
{
}

Descriptor::Descriptor(vector<double> h1, vector<double> h2, vector<double> h3, vector<double> h4){
    hist1 = h1;
    hist2 = h2;
    hist3 = h3;
    hist4 = h4;
}

Descriptor::~Descriptor()
{
}