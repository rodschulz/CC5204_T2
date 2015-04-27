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

void Helper::findNearestFrames(const DescriptorPtr &_targetFrame, const vector<DescriptorPtr> &_queryFrames, const MetricType &_metric, vector<Match> &_output)
{
	Metric::setMetricType(_metric);

	vector<Match> data;
	for (DescriptorPtr descriptor : _queryFrames)
	{
		double distance = Metric::getMetric()(*descriptor, *_targetFrame);
		data.push_back(Match(distance, descriptor, _targetFrame));
	}

	sort(data.begin(), data.end(), &Helper::compare);

	int n = 3;
	_output.clear();
	for (int i = 0; i < n; i++)
		_output.push_back(data[i]);
}

bool Helper::compare(const Match &_m1, const Match &_m2)
{
	return _m1.distance < _m2.distance;
}

int Helper::getSkipFrames(const int _samplingRate, VideoCapture &_capture)
{
	double fps = _capture.get(CV_CAP_PROP_FPS);
	int skipFrames = ceil(fps / _samplingRate) - 1;
	skipFrames = skipFrames == 0 ? 1 : skipFrames;

	return skipFrames;
}

double Helper::getMedian(const vector<double> &_data)
{
	bool isOdd = _data.size() % 2 == 1;

	double median;
	if (isOdd)
	{
		int middle = _data.size() / 2;
		median = _data[middle];
	}
	else
	{
		int middle = _data.size() / 2;
		median = (_data[middle - 1] + _data[middle]) / 2;
	}

	return median;
}

void Helper::printToFile(const string &_outputFile, const vector<double> &_data)
{
	ofstream out;
	out.open(_outputFile.c_str(), fstream::out);

	for (double d : _data)
		out << d << "\n";

	out.close();
}

void Helper::clearOutliers(vector<MatchArrayPtr> &_matches)
{
	string prevQuery = _matches[0]->getMinDistanceQuery();
	string thisQuery = _matches[1]->getMinDistanceQuery();
	string nextQuery;
	for (size_t i = 2; i < _matches.size() - 1; i++)
	{
		nextQuery = _matches[i]->getMinDistanceQuery();
		if (prevQuery.compare(nextQuery) == 0 && thisQuery.compare(prevQuery) != 0)
		{
			double potencialMinDist = _matches[i - 1]->getQueryMinDistance(prevQuery).distance;
			double currentMinDist = _matches[i - 1]->getMinDistance();

			if (potencialMinDist <= currentMinDist * 2)
				_matches[i - 1]->swapMinDistanceQuery(prevQuery);
		}

		prevQuery = thisQuery;
		thisQuery = nextQuery;
	}
}
