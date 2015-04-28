/**
 * Author: rodrigo
 * 2015
 */
#pragma once

#include <vector>
#include <memory>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

// Forward declaration
class Descriptor;
typedef shared_ptr<Descriptor> DescriptorPtr;

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
	Descriptor(const Mat &_frame, const int _frameNumber, const DescType &_type, const int param);
	// Copy constructor
	Descriptor(const Descriptor &_other);
	// Assignment operator
	Descriptor &operator=(const Descriptor &_other);

	// Returns a single vector representing the descriptor data
	inline vector<double> getDescriptorVector() const
	{
		return data;
	}
	// Gets the dimension of the descriptor
	inline size_t getDimension() const
	{
		return data.size();
	}
	// Returns number of the frame used to build the descriptor
	inline int getFrame()
	{
		return originFrameNumber;
	}
	inline static const char* ToString(DescType v)
	{
		switch (v)
		{
			case HIST:	return "HIST";
			case OMD:   return "OMD";
			default:	return "[Unknown]";
		}
	}

private:
	int originFrameNumber;
	vector<double> data;

	void Histogram(const Mat &_frame, int histSize);
	void Omd(const Mat &_frame, int N);
	// Normaliza histograma segun el total de pixeles del area
	void normalizeHist(const Mat &_hist, const int _bins, const double _total, vector<double> &_normalizedHist);
};
