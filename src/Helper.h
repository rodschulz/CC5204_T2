/**
 * Author: rodrigo
 * 2015
 */
#pragma once

#include <vector>
#include <string>

using namespace std;

class Helper
{
public:
	/** Location of the video to be searched according to the input file */
	static string getTargetLocation(const string &_inputFile);
	/** Location of the video to be searched according to the input file */
	static vector<string> getQueryLocations(const string &_inputFile);
	
private:
	Helper();
	~Helper();
};