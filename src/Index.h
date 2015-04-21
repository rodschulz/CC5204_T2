/**
 * Author: rodrigo
 * 2015
 */
#pragma once

#include <vector>
#include "Descriptor.h"

using namespace std;

// Types of metrics available
typedef enum MetricType
{
	MANHATTAN, EUCLIDEAN, MAX
} MetricType;

// Definition of an element of the index
typedef pair<double, const Descriptor *> Element;

class Index
{
public:
	// Builds an index for the given array of descriptors
	Index(const vector<Descriptor> &_descriptors, const MetricType &_type);
	~Index();

	// Returns the index of the nearest neighbor, according to the indices the descriptor array used to build the index
	int findNearestNeighbor(const Descriptor &_descriptor);

private:
	// Sets the metric to be used by the index
	void setMetric(const MetricType &_type);
	// Builds a new index
	void buildIndex(const vector<Descriptor> &_descriptors, const double &_alpha);

	// Metric used to build the index
	double (*metric)(const Descriptor &, const Descriptor &);
	// Index data
	vector<vector<Element>> index;
	vector<const Descriptor *> pivots;
};
