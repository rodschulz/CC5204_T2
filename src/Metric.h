/**
 * Author: rodrigo
 * 2015
 */
#pragma once

#include "Descriptor.h"

class Metric
{
public:
	static double ManhattanMetric(const Descriptor &_desc1, const Descriptor &_desc2);
	static double EuclideanMetric(const Descriptor &_desc1, const Descriptor &_desc2);
	static double MaxMetric(const Descriptor &_desc1, const Descriptor &_desc2);

private:
	Metric();
	~Metric();
};
