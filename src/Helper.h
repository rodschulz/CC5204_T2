/**
 * Author: rodrigo
 * 2015
 */
#pragma once

#include <vector>
#include <string>
#include "Descriptor.h"
#include "Metric.h"

using namespace std;

class Helper
{
public:
	/** Location of the video to be searched according to the input file */
	static string getTargetLocation(const string &_inputFile);
	/** Location of the video to be searched according to the input file */
	static vector<string> getQueryLocations(const string &_inputFile);
	/** Returns a random number between the given limits */
	static int getRandomNumber(const int _min, const int _max);
	/** Find the nearest frame in the given set of frames */
	static void findNearestFrame(const Descriptor &_targetFrame, const vector<Descriptor> &_queryFrames, const MetricType &_metric, vector<Descriptor> &_output);

private:
	Helper();
	~Helper();

	static bool comparePairs(const pair<double, Descriptor*> &_p1, const pair<double, Descriptor*> &_p2);
};
