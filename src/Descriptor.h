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
	// Empty constructor
	Descriptor();
	// Constructor
	Descriptor(const vector<double> &_v);
	// Copy constructor
	Descriptor(const Descriptor &_other);
	// Assignment operator
	Descriptor &operator=(const Descriptor &_other);

	// Returns a single vector representing the descriptor data
	inline vector<double> getDescriptorVector() const
	{
		return vec;
	}
	// Gets the dimension of the descriptor
	inline size_t getDimension() const
	{
		return vec.size();
	}

private:
	vector<double> vec;
};
