/**
 * Author: rodrigo
 * 2015
 */
#include "Index.h"
#include "Helper.h"
#include "Metric.h"
#include <math.h>
#include <map>
#include <limits>
#include <iostream>

Index::Index(const vector<Descriptor> &_videoDescriptor, const MetricType &_type)
{
	// Set metric
	setMetric(_type);
	// Build the index
	buildIndex(_videoDescriptor, 0.4);
}

Index::~Index()
{
}

pair<int, const Descriptor *> Index::findNearestNeighbor(const Descriptor &_query)
{
	pair<int, const Descriptor *> nearestNeigbor(-1, NULL);
	double r = numeric_limits<double>::max();
	vector<double> distanceQP;

	// Find nearest pivot and set the initial search radius
	for (pair<int, const Descriptor *> pivot : pivots)
	{
		double d = metric(_query, *pivot.second);
		if (r > d)
		{
			r = d;
			nearestNeigbor = pivot;
		}

		// Store this distance for future calculations
		distanceQP.push_back(d);
	}

	// Perform the nearest neighbor search
	int checked = 0;
	for (size_t row = 0; row < index.size(); row++)
	{
		bool excluded = false;
		for (size_t col = 0; col < index[row].second.size(); col++)
		{
			if (fabs(distanceQP[col] - index[row].second[col]) > r)
			{
				excluded = true;
				break;
			}
		}

		if (!excluded)
		{
			double distance = metric(_query, *index[row].first);
			checked++;

			// Check if this is the current nearest neighbor
			if (distance < r)
			{
				r = distance;
				nearestNeigbor = make_pair(row, index[row].first);
			}
		}
	}

	cout << "Checked " << checked << " points before find the NN\n";

	return nearestNeigbor;
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
	pivots.push_back(make_pair(firstPivot, &_descriptors[firstPivot]));

	// TODO improve efficiency by using distances already calculated to select pivots
	for (size_t i = 0; i < _descriptors.size(); i++)
	{
		if (i == firstPivot)
			continue;

		bool addPivot = true;
		for (pair<int, const Descriptor *> p : pivots)
		{
			double distance = metric(*p.second, _descriptors[i]);
			if (distance < threshold)
			{
				addPivot = false;
				break;
			}
		}

		if (addPivot)
			pivots.push_back(make_pair(i, &_descriptors[i]));
	}

	cout << "Number of pivots selected: " << pivots.size() << "\n";

	// Generate index
	index.resize(_descriptors.size());
	for (size_t i = 0; i < _descriptors.size(); i++)
	{
		index[i] = make_pair(&_descriptors[i], vector<double>(pivots.size(), numeric_limits<double>::max()));

		for (size_t j = 0; j < pivots.size(); j++)
			index[i].second[j] = metric(_descriptors[i], *pivots[j].second);
	}
}
