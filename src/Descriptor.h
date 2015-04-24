/**
 * Author: rodrigo
 * 2015
 */
#pragma once

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

// Types of descriptors
typedef enum DescType
{
	HIST, OMD // Oh My Dad!
} DescType;

class Descriptor
{
public:
	~Descriptor();
	// Empty constructor
	Descriptor();
	// Constructor
	Descriptor(Mat &_frame, const DescType &_type);
	// Copy constructor
	Descriptor(const Descriptor &_other);
	// Assignment operator
	Descriptor &operator=(const Descriptor &_other);

	// Returns a single vector representing the descriptor data
	inline vector<double> getDescriptorVector() const
	{
		return vec;
	}
	// Gets the dimension of the descriptor
	inline size_t getDimension() const
	{
		return vec.size();
	}

private:
	vector<double> vec;
	vector<double> Histogram(Mat &_frame);
	vector<double> Omd(Mat &_frame);
	vector<double> normalizeHist(vector<double> hist, int bins, double total);
};
