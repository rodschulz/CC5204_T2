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
	~Descriptor();
	Descriptor();
	Descriptor(const vector<double> &_h1, const vector<double> &_h2, const vector<double> &_h3, const vector<double> &_h4);

	inline vector<double> getDescriptorVector() const
	{
		return whole;
	}
	inline size_t getDimension() const
	{
		return whole.size();
	}

private:
	vector<double> hist1;
	vector<double> hist2;
	vector<double> hist3;
	vector<double> hist4;
	vector<double> whole;
};
