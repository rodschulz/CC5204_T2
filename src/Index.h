/**
 * Author: rodrigo
 * 2015
 */
#pragma once

#include <vector>
#include "Descriptor.h"

using namespace std;

typedef enum MetricType
{
	MANHATTAN, EUCLIDEAN, MAX
} MetricType;

class Index
{
public:
	Index(const vector<Descriptor> &_descriptors, const MetricType &_metric);
	~Index();

private:
	double (*metric)(const Descriptor &, const Descriptor &);
	vector<vector<double>> index;
};
