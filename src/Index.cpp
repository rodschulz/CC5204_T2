/**
 * Author: rodrigo
 * 2015
 */
#include "Index.h"
#include "Helper.h"
#include "Metric.h"
#include <math.h>
#include <map>

Index::Index(const vector<Descriptor> &_videoDescriptor, const MetricType &_metric)
{
	// Set metric
	switch (_metric)
	{
		case MANHATTAN:
			metric = &Metric::ManhattanMetric;
		break;

		case EUCLIDEAN:
			metric = &Metric::EuclideanMetric;
		break;

		case MAX:
			metric = &Metric::MaxMetric;
		break;
	}

	// Select pivots
	double alpha = 0.2;
	double maxDistance = sqrt(_videoDescriptor.front().getDimension());
	double threshold = alpha * maxDistance;

	size_t firstPivot = (size_t) Helper::getRandomNumber(0, _videoDescriptor.size() - 1);
	map<int, const Descriptor *> pivots;
	pivots[firstPivot] = &_videoDescriptor[firstPivot];

	for (size_t i = 0; i < _videoDescriptor.size(); i++)
	{
		if (i == firstPivot)
			continue;

		bool addPivot = true;
		for (pair<int, const Descriptor *> p : pivots)
		{
			if (metric(*p.second, _videoDescriptor[i]) < threshold)
			{
				addPivot = false;
				break;
			}
		}

		if (addPivot)
			pivots[i] = &_videoDescriptor[i];
	}

	// Generate index
	index.resize(_videoDescriptor.size());
	for (size_t i = 0; i < _videoDescriptor.size(); i++)
	{
		size_t j = 0;
		for (pair<int, const Descriptor *> p : pivots)
		{
			index[i].resize(pivots.size(), 0);
			index[i][j] = metric(_videoDescriptor[i], *p.second);
		}
	}
}

Index::~Index()
{
}
