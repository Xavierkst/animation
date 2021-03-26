#include "Keyframe.h"

Keyframe::Keyframe()
{
	time = .0f;
	value = .0f;
	tanIn = .0f;
	tanOut = .0f;
	a = b = c = d = .0f;
}

float Keyframe::getTime()
{
	return time;
}

void Keyframe::setTime(float t)
{
	time = t;
}

float Keyframe::getValue()
{
	return value;
}

void Keyframe::setValue(float val)
{
	value = val;
}

float Keyframe::getTanIn()
{
	return tanIn;
}

float Keyframe::getTanOut()
{
	return tanOut;
}

void Keyframe::setTanIn(float in)
{
	tanIn = in;
}

void Keyframe::setTanOut(float out)
{
	tanOut = out;
}

std::string Keyframe::getRuleIn()
{
	return ruleIn;
}

std::string Keyframe::getRuleout()
{
	return ruleOut;
}

void Keyframe::setRuleIn(std::string in)
{
	ruleIn = in;
}

void Keyframe::setRuleOut(std::string rule)
{
	ruleOut = rule;
}

float Keyframe::getA() {
	return a;
}

float Keyframe::getB() {
	return b;
}

float Keyframe::getC() {
	return c;
}
float Keyframe::getD() {
	return d;
}

void Keyframe::setA(float A)
{
	a = A;
}

void Keyframe::setB(float B)
{
	b = B;
}

void Keyframe::setC(float C)
{
	c = C;
}

void Keyframe::setD(float D)
{
	d = D;
}
