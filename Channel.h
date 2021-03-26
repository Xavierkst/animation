#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include "Keyframe.h"
#include "Tokenizer.h"
#include <iostream>
class Channel {
private:
	std::vector<Keyframe> keyframes;
	std::string extrapIn, extrapOut;
	int number; 

public:
	//Channel();
	//~Channel();
	void setNumber(int num);
	std::vector<Keyframe>& getKeyframes();

	// return the new DOF value at the corresponding time
	// could be simple, could require cubic curve, or extrap, depending
	// the time value passed in (time is normalized when passed into cubic eqn)
	float Evaluate(float time); 

	// supposed to help you calculate the tangent rules
	void computeTangents();
	
	float calculateExtrap(std::string method, float time);

	// Computes a, b, c, d -- cubic coeffs
	void precomputeCoeffs();

	bool Load(Tokenizer& token);

	float calculateTangent(Keyframe& k1, Keyframe& k2);
};

#endif