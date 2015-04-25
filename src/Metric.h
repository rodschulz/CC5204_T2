/**
 * Author: rodrigo
 * 2015
 */
#pragma once

#include "Descriptor.h"

// Types of metrics available
typedef enum MetricType
{
	MANHATTAN, EUCLIDEAN, MAX
} MetricType;

typedef double (*functionPointer)(const Descriptor &_desc1, const Descriptor &_desc2);

class Metric
{
public:
	// Evaluate 2 descriptors using the Manhattan metric
	static double ManhattanMetric(const Descriptor &_desc1, const Descriptor &_desc2);
	// Evaluate 2 descriptors using the Euclidean metric
	static double EuclideanMetric(const Descriptor &_desc1, const Descriptor &_desc2);
	// Evaluate 2 descriptors using the Max metric
	static double MaxMetric(const Descriptor &_desc1, const Descriptor &_desc2);

	// Returns the current instance of the singleton
	static Metric *getInstance();
	// Sets the type of metric to be used
	static void setMetricType(const MetricType &_type);
	// Returns the metric associated to the currently set type
	static functionPointer getMetric();

private:
	Metric();
	~Metric();

	MetricType type;
	functionPointer metric;
};
