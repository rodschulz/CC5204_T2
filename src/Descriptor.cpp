/**
 * Author: rodrigo
 * 2015
 */
#include "Descriptor.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

Descriptor::Descriptor()
{
	originFrameNumber = -1;
	data.clear();
}

//Param: # of bins if HIST, # of areas (param x param) if OMD
Descriptor::Descriptor(const Mat &_frame, const int _frameNumber, const DescType &_type, const int param)
{
	switch (_type)
	{
		case HIST:
			Histogram(_frame, param);
		break;

		case OMD:
			Omd(_frame, param);
		break;
	}

	originFrameNumber = _frameNumber;
}

Descriptor::Descriptor(const Descriptor &_other)
{
	data = _other.data;
	originFrameNumber = _other.originFrameNumber;
}

Descriptor::~Descriptor()
{
}

Descriptor &Descriptor::operator=(const Descriptor &_other)
{
	if (this != &_other)
	{
		data = _other.data;
		originFrameNumber = _other.originFrameNumber;
	}

	return *this;
}

void Descriptor::Histogram(const Mat &_grayFrame, int histSize)
{
	cv::Size s = _grayFrame.Mat::size();
	int frameWidth = s.width;
	int frameHeight = s.height;

	/**
	 * | 1 | 2 |
	 * | 3 | 4 |
	 */
	int areaWidth = frameWidth / 2;
	int areaHeight = frameHeight / 2;
	cv::Mat area1, area2, area3, area4;
	area1 = _grayFrame(cv::Rect(0, 0, areaWidth, areaHeight));
	area2 = _grayFrame(cv::Rect(areaWidth, 0, areaWidth, areaHeight));
	area3 = _grayFrame(cv::Rect(0, areaHeight, areaWidth, areaHeight));
	area4 = _grayFrame(cv::Rect(areaWidth, areaHeight, areaWidth, areaHeight));

	//Histogram
	//int histSize = 128;
	//the upper boundary is exclusive
	float range[] =
	{ 0, 256 };
	const float *histRange =
	{ range };
	bool uniform = true;
	bool accumulate = false;
	Mat hist1, hist2, hist3, hist4;

	// Calculate histogram for each area
	calcHist(&area1, 1, 0, Mat(), hist1, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&area2, 1, 0, Mat(), hist2, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&area3, 1, 0, Mat(), hist3, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&area4, 1, 0, Mat(), hist4, 1, &histSize, &histRange, uniform, accumulate);

	// Normalize each histogram and copy to vector
	double totalPxs = areaHeight * areaWidth;
	normalizeHist(hist1, histSize, totalPxs, data);
	normalizeHist(hist2, histSize, totalPxs, data);
	normalizeHist(hist3, histSize, totalPxs, data);
	normalizeHist(hist4, histSize, totalPxs, data);
}

void Descriptor::normalizeHist(const Mat &_hist, const int _bins, const double _total, vector<double> &_normalizedHist)
{
	for (int i = 0; i < _bins; i++)
		_normalizedHist.push_back(_hist.at<float>(i, 0) / _total);
}

void Descriptor::Omd(const Mat &_grayFrame, int N)
{
	cv::Size s = _grayFrame.Mat::size();
	int frameWidth = s.width;
	int frameHeight = s.height;

	/**
	 * |  1  |  2  |  3  | ... ...  |   N  |
	 * | N+1 | N+2 | N+3 | ... ...  |  2N  |
	 * ...
	 * | ... | ... | ... | ... ...  | N x N|
	 */
	int areaWidth = frameWidth / N;
	int areaHeight = frameHeight / N;

	// Generate each area, calculate average (mean) and add to intensities vector
	vector<double> meanIntensities;
	int onCol = 0;
	int onRow = 0;
	for (int i = 0; i < N * N; i++)
	{
		cv::Mat currArea;
		currArea = _grayFrame(cv::Rect(onCol * areaWidth, onRow * areaHeight, areaWidth, areaHeight));
		meanIntensities.push_back(cv::mean(currArea)[0]);

		if (onCol == N - 1)
		{
			onCol = 0;
			onRow++;
		}
		else
		{
			onCol++;
		}
	}

	// Calculate the position on the intensity scale of the image, for each area
	vector<double> sortedIntensities(meanIntensities.size());
	copy(meanIntensities.begin(), meanIntensities.end(), sortedIntensities.begin());
	sort(sortedIntensities.begin(), sortedIntensities.end());

	data.clear();
	data.resize(meanIntensities.size());
	for (size_t i = 0; i < meanIntensities.size(); i++)
	{
		int pos = find(sortedIntensities.begin(), sortedIntensities.end(), meanIntensities[i]) - sortedIntensities.begin();
		data[i] = pos;
	}
}
