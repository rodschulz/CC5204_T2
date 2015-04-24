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
}

Descriptor::Descriptor(cv::Mat &_frame, const DescType &_type)
{
	switch (_type)
	{
		case HIST:
			vec = Histogram(_frame);
		break;

		case OMD:
			vec = Omd(_frame);
		break;
	}
}

Descriptor::Descriptor(const Descriptor &_other)
{
	vec = _other.vec;
}

Descriptor::~Descriptor()
{
}

Descriptor &Descriptor::operator=(const Descriptor &_other)
{
	if (this != &_other)
	{
		vec = _other.vec;
	}

	return *this;
}

vector<double> Descriptor::Histogram(Mat &_grayFrame)
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
	int histSize = 128;
	//the upper boundary is exclusive
	float range[] =
	{ 0, 256 };
	const float *histRange =
	{ range };
	bool uniform = true;
	bool accumulate = false;
	cv::Mat hist1, hist2, hist3, hist4;

	// Calculate histogram for each area
	calcHist(&area1, 1, 0, cv::Mat(), hist1, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&area2, 1, 0, cv::Mat(), hist2, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&area3, 1, 0, cv::Mat(), hist3, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&area4, 1, 0, cv::Mat(), hist4, 1, &histSize, &histRange, uniform, accumulate);

	// Normalize each histogram and copy to vector
	double totalPxs = areaHeight * areaWidth;
	vector<double> normHist1 = Descriptor::normalizeHist(hist1, histSize, totalPxs);
	vector<double> normHist2 = Descriptor::normalizeHist(hist2, histSize, totalPxs);
	vector<double> normHist3 = Descriptor::normalizeHist(hist3, histSize, totalPxs);
	vector<double> normHist4 = Descriptor::normalizeHist(hist4, histSize, totalPxs);

	vector<double> whole;
	whole.reserve(normHist1.size() + normHist2.size() + normHist3.size() + normHist4.size());
	whole.insert(whole.end(), normHist1.begin(), normHist1.end());
	whole.insert(whole.end(), normHist2.begin(), normHist2.end());
	whole.insert(whole.end(), normHist3.begin(), normHist3.end());
	whole.insert(whole.end(), normHist4.begin(), normHist4.end());

	return whole;
}

/* Normaliza histograma segun el total de pixeles del area */
vector<double> Descriptor::normalizeHist(vector<double> hist, int bins, double total)
{
	for (int i = 0; i < bins; i++)
	{
		hist[i] = hist[i] / total;
	}
	return hist;
}

vector<double> Descriptor::Omd(Mat &_grayFrame)
{
	cv::Size s = _grayFrame.Mat::size();
	int frameWidth = s.width;
	int frameHeight = s.height;

	/**
	 * |  1 |  2 |  3 | ... | 9  | 10 |
	 * | 11 | 12 | 13 | ... ...  | 20 |
	 * ...
	 * | 91 | 92 | 93 | ... ...  | 100|
	 */
	//TODO dejar numero de divisiones como parametro
	int N = 10;
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

	vector<double> omd(meanIntensities.size());
	for (size_t i = 0; i < meanIntensities.size(); i++)
	{
		int pos = find(sortedIntensities.begin(), sortedIntensities.end(), meanIntensities[i]) - sortedIntensities.begin();
		omd[i] = pos;
	}

	return omd;
}
