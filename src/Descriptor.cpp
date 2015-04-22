/**
 * Author: rodrigo
 * 2015
 */
#include "Descriptor.h"

Descriptor::Descriptor()
{
}

Descriptor::Descriptor(const vector<double> &_h1, const vector<double> &_h2, const vector<double> &_h3, const vector<double> &_h4)
{
	hist1 = _h1;
	hist2 = _h2;
	hist3 = _h3;
	hist4 = _h4;

	whole.clear();
	whole.reserve(hist1.size() + hist2.size() + hist3.size() + hist4.size());
	whole.insert(whole.end(), hist1.begin(), hist1.end());
	whole.insert(whole.end(), hist2.begin(), hist2.end());
	whole.insert(whole.end(), hist3.begin(), hist3.end());
	whole.insert(whole.end(), hist4.begin(), hist4.end());
}

Descriptor::Descriptor(const Descriptor &_other)
{
	hist1 = _other.hist1;
	hist2 = _other.hist2;
	hist3 = _other.hist3;
	hist4 = _other.hist4;
	whole = _other.whole;
}

Descriptor::~Descriptor()
{
}

Descriptor &Descriptor::operator=(const Descriptor &_other)
{
	if (this != &_other)
	{
		hist1 = _other.hist1;
		hist2 = _other.hist2;
		hist3 = _other.hist3;
		hist4 = _other.hist4;
		whole = _other.whole;
	}

	return *this;
}
