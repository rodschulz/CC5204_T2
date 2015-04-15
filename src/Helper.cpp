/**
 * Author: rodrigo
 * 2015
 */
#include "Helper.h"
#include <fstream>

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
	while (std::getline(file, line))
	{
		// Skip comments
		if (line.size() == 0 || line.at(0) == '#')
			continue;

		// The first line is target's location
		break;
	}

	return line;
}

vector<string> Helper::getQueryLocations(const string &_inputFile)
{
	ifstream file(_inputFile.c_str());
	vector <string> locations;

	bool firstLine = true;
	string line;
	while (std::getline(file, line))
	{
		// Skip comments
		if (line.at(0) == '#')
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