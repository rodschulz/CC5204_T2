/**
 * Author: rodrigo
 * 2015
 */
#include "Descriptor.h"

Descriptor::Descriptor()
{
}

Descriptor::Descriptor(const vector<double> &_vec)
{
	vec = _vec;
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
