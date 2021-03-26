#include "Dof.h"
Dof::Dof() {
	setMinMax(-100000, 100000);
	value = .0f;
}
Dof::Dof(float minVal, float maxVal)
{
	//if (minVal == .0f) 
	setMinMax(minVal, maxVal);
	value = .0f;
}

void Dof::setValue(float val)
{
	// check if value exceeds min or max
	// set to either ends
	if (val < minDeg) value = minDeg;
	else if (val > maxDeg) value = maxDeg;
	else value = val;
	//value = val;
}

float Dof::getValue()
{
	return value;
}

float& Dof::getDOFValue()
{
	return value;
}

void Dof::setMinMax(float min, float max)
{
	minDeg = min;
	maxDeg = max;
}
