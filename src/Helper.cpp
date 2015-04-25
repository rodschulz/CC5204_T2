/**
 * Author: rodrigo
 * 2015
 */
#include "Helper.h"
#include <fstream>
#include <iostream>

Helper::Helper()
{
}

Helper::~Helper()
{
}

string Helper::getTargetLocation(const string &_inputFile)
{
	ifstream file(_inputFile.c_str());
	string line;

	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			// Skip comments
			if (line.size() == 0 || line.at(0) == '#')
				continue;

			// The first line is target's location
			break;
		}
	}
	else
	{
		cout << "Can't open input file\n";
	}

	return line;
}

vector<string> Helper::getQueryLocations(const string &_inputFile)
{
	ifstream file(_inputFile.c_str());
	vector<string> locations;

	bool firstLine = true;
	string line;
	while (std::getline(file, line))
	{
		// Skip comments
		if (line.size() == 0 || line.at(0) == '#')
			continue;

		// Skip first line
		if (firstLine)
		{
			firstLine = false;
			continue;
		}

		locations.push_back(line);
	}

	return locations;
}

int Helper::getRandomNumber(const int _min, const int _max)
{
	srand(rand());
	srand(rand());
	int number = _min + (rand() % (int) (_max - _min + 1));
	return number;
}

void Helper::findNearestFrame(const Descriptor &_targetFrame, const vector<Descriptor> &_queryFrames, const MetricType &_metric, vector<Match> &_output)
{
	Metric::setMetricType(_metric);

	vector<Match> data;
	for (Descriptor d : _queryFrames)
	{
		double distance = Metric::getMetric()(d, _targetFrame);
		data.push_back(Match(distance, d, _targetFrame));
	}

	sort(data.begin(), data.end(), &Helper::compare);

	int n = 3;
	_output.clear();
//	copy(data.begin(), data.begin() + n, _output.begin());
	for (int i = 0; i < n; i++)
		_output.push_back(data[i]);
}

bool Helper::compare(const Match &_m1, const Match &_m2)
{
	return _m1.distance < _m2.distance;
}
