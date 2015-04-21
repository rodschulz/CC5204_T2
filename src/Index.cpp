/**
 * Author: rodrigo
 * 2015
 */
#include "Index.h"
#include "Helper.h"
#include "Metric.h"
#include <math.h>
#include <map>

Index::Index(const vector<Descriptor> &_videoDescriptor, const MetricType &_type)
{
	// Set metric
	setMetric(_type);
	// Build the index
	buildIndex(_videoDescriptor, 0.42);
}

Index::~Index()
{
}

int Index::findNearestNeighbor(const Descriptor &_descriptor)
{
}

void Index::setMetric(const MetricType &_type)
{
	switch (_type)
	{
		case MANHATTAN:
			metric = &Metric::ManhattanMetric;
		break;

		default:
		case EUCLIDEAN:
			metric = &Metric::EuclideanMetric;
		break;

		case MAX:
			metric = &Metric::MaxMetric;
		break;
	}
}

void Index::buildIndex(const vector<Descriptor> &_descriptors, const double &_alpha)
{
	index.clear();
	pivots.clear();

	// Select pivots
	double maxDistance = sqrt(_descriptors.front().getDimension());
	double threshold = _alpha * maxDistance;

	size_t firstPivot = (size_t) Helper::getRandomNumber(0, _descriptors.size() - 1);
	pivots.push_back(&_descriptors[firstPivot]);

	// TODO improve efficiency by using distances already calculated to select pivots
	for (size_t i = 0; i < _descriptors.size(); i++)
	{
		if (i == firstPivot)
			continue;

		bool addPivot = true;
		for (const Descriptor * p : pivots)
		{
			double distance = metric(*p, _descriptors[i]);
			if (distance < threshold)
			{
				addPivot = false;
				break;
			}
		}

		if (addPivot)
			pivots.push_back(&_descriptors[i]);
	}

	// Generate index
	index.resize(_descriptors.size());
	for (size_t i = 0; i < _descriptors.size(); i++)
	{
		size_t j = 0;
		for (const Descriptor * p : pivots)
		{
			index[i].resize(pivots.size());
			index[i][j] = make_pair(metric(_descriptors[i], *p), p);
		}
	}
}
