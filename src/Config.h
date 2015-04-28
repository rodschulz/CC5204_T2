/**
 * Author: rodrigo
 * 2015
 */
#pragma once

#include <string>
#include "Descriptor.h"
#include "Metric.h"

using namespace std;

class Config
{
public:
	~Config();

	// Returns the instance of the singleton
	static Config *getInstance()
	{
		static Config *instance = new Config();
		return instance;
	}
	// Loads the configuration file
	static void load(const string &_filename);

	static DescType getDescriptorType();
	static MetricType getMetricType();
	static double getMinVideoLength();
	static int getQuerySkippedFrames();
	static int getTargetFrameRate();
	static int getMaxJump();
	static int getDescriptorParam();

private:
	Config();
	// Parses the given value according to the given key
	static void parse(const string _key, const string _value);

	DescType descriptorType;
	MetricType metricType;
	double minVideoLength;
	double maxJump;
	int querySkippedFrames;
	int targetFrameRate;
	int param;
};

