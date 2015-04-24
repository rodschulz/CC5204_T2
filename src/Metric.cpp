/**
 * Author: rodrigo
 * 2015
 */
#include "Metric.h"
#include <vector>
#include <math.h>

using namespace std;

Metric::Metric()
{
	type = EUCLIDEAN;
	metric = &Metric::EuclideanMetric;
}

Metric::~Metric()
{
}

double Metric::ManhattanMetric(const Descriptor &_desc1, const Descriptor &_desc2)
{
	vector<double> d1 = _desc1.getDescriptorVector();
	vector<double> d2 = _desc2.getDescriptorVector();

	double result = 0;
	size_t length = d1.size();
	for (size_t i = 0; i < length; i++)
	{
		result += fabs(d1[i] - d2[i]);
	}

	return result;
}

double Metric::EuclideanMetric(const Descriptor &_desc1, const Descriptor &_desc2)
{
	vector<double> d1 = _desc1.getDescriptorVector();
	vector<double> d2 = _desc2.getDescriptorVector();

	double result = 0;
	size_t length = d1.size();
	for (size_t i = 0; i < length; i++)
	{
		double a = d1[i];
		double b = d2[i];
		result += ((a - b) * (a - b));
	}

	return sqrt(result);
}

double Metric::MaxMetric(const Descriptor &_desc1, const Descriptor &_desc2)
{
	vector<double> d1 = _desc1.getDescriptorVector();
	vector<double> d2 = _desc2.getDescriptorVector();

	double result = 0;
	size_t length = d1.size();
	for (size_t i = 0; i < length; i++)
	{
		double delta = fabs(d1[i] - d2[i]);
		if (result < delta)
			result = delta;
	}

	return result;
}

Metric *Metric::getInstance()
{
	static Metric* instance = new Metric();
	return instance;
}

void Metric::setMetricType(const MetricType &_type)
{
	getInstance()->type = _type;
	switch (_type)
	{
		case MANHATTAN:
			getInstance()->metric = &Metric::ManhattanMetric;
		break;

		default:
		case EUCLIDEAN:
			getInstance()->metric = &Metric::EuclideanMetric;
		break;

		case MAX:
			getInstance()->metric = &Metric::MaxMetric;
		break;
	}
}

functionPointer Metric::getMetric()
{
	return getInstance()->metric;
}
