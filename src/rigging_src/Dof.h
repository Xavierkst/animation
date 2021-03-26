#ifndef _DOF_H_
#define _DOF_H_

class Dof {
private:
	float value; // defaulted
	float maxDeg; // defaulted
	float minDeg; // defaulted

public: 
	// dof constructor
	Dof();
	Dof(float minVal, float maxVal);

	// sets dof to val passed in, clamps it by default to 
	// either end if it exceeds or goes below one of them
	void setValue(float val);

	float getValue();

	float& getDOFValue(); 

	// sound bounds for freedom degree to be clamped
	void setMinMax(float min, float max);
};
#endif