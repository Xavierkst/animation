#ifndef _KEYFRAME_H_
#define _KEYFRAME_H_

#include "../core.h"
#include <string>

class Keyframe {
private:
	float time; 
	float value;
	float tanIn, tanOut;
	std::string ruleIn, ruleOut;
	float a, b, c, d;

public:

	Keyframe();

	float getTime();
	void setTime(float t);

	float getValue();
	void setValue(float val);

	float getTanIn();
	float getTanOut();

	void setTanIn(float in);
	void setTanOut(float tanOut);

	std::string getRuleIn();
	std::string getRuleout();

	void setRuleIn(std::string in);
	void setRuleOut(std::string rule);

	float getA();
	float getB();
	float getC(); 
	float getD();

	void setA(float A);
	void setB(float B);
	void setC(float C); 
	void setD(float D);
};
#endif 