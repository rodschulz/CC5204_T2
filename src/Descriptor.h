/**
 * Author: rodrigo
 * 2015
 */
#pragma once

#include <vector>

using namespace std;

class Descriptor
{
public:
	Descriptor();
	~Descriptor();
	Descriptor(vector<double>, vector<double>, vector<double>, vector<double>);
	vector <double> hist1;
	vector <double> hist2;
	vector <double> hist3;
	vector <double> hist4;
};