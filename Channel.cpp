#include "Channel.h"

std::vector<Keyframe>& Channel::getKeyframes()
{
	return keyframes;
}


float Channel::Evaluate(float time)
{
	// given a time, and cubic coeffs a,b,c,d in ea. keyframe
	// we want to evaluate a DOF value
	// NotE: time is normalized when passed into cubic eqn

	// "channel should always return some reasonable value regdless of 
	// what time t was passed in"
	if (keyframes.size() == 1) {
		// if there is 1 key, it should ret. value of that key
		return keyframes[0].getValue();
	}

	// Check if time is less than 1st k, or greater than last k
	// calculate DOF using extrapolation methods -- pass in time and extrap
	// method
	if (time < keyframes[0].getTime() || time > keyframes[keyframes.size() - 1].getTime()) {
		// call helper fn to calculate DOF, pass in extrap method and time
		return calculateExtrap(extrapIn, time); // to be completed
	}
	// Check if time is equal to any of the keyframe values
	// return its corresp DOF value
	for (int i = 0; i < keyframes.size(); i++) {
		if (time == keyframes[i].getTime()) {
			return keyframes[i].getValue();
		}
	}
	// If we reached this stage, the time falls between one of them. Calculate DOF
	// using: (we know that there's also more than just 1 key
	// find the span that time lies in, use a,b,c,d values
	// for the keyframe on its "left"
	float x = 0.0f;
	for (int j = 0; j < keyframes.size(); j++) {
		if (time > keyframes[j].getTime()) {
			continue;
		}
		else { 
			// time is less than current keyframe, so we look at the one before it
			// Use cubic equation here: 		
			Keyframe currKey = keyframes[j - 1];
			Keyframe nextKey = keyframes[j];
			// you need to normalize time before using it to find the DOF Val: 
			float u = ( (time - currKey.getTime()) / (nextKey.getTime() - currKey.getTime()) ); 

			// set up cubic equation 
			x = currKey.getD() + u * (currKey.getC() + u * (currKey.getB() + u * currKey.getA()));
			return x;
		}
	}
	// if nothing found returns 0?
	return 0.0f;
}

void Channel::setNumber(int num) {
	number = num;
}

void Channel::computeTangents()
{
	// if theres only 1 keyframe, set both tang. to 0, return
	if (keyframes.size() == 1) {
		keyframes[0].setTanIn(.0f);
		keyframes[0].setTanOut(.0f);
		return;
	}
	// if there are 2 keyframes, apply linear by default	
	else if (keyframes.size() == 2) {
		//float tangent = ( (keyframes[1].getValue() - keyframes[0].getValue()) / (keyframes[1].getTime() - keyframes[0].getTime()) );
		float tangent = calculateTangent(keyframes[1], keyframes[0]);
		keyframes[0].setTanOut(tangent);
		keyframes[1].setTanIn(tangent);
		return;
	}

	// from here, there are 3 or more keyframes, look for keywords
	// compute first and last using linear, and the middle ones using 
	// whatever rule was given
	for (int i = 1; i < keyframes.size() - 1; i++) {
		// check what the rule is and calculate accordingly:
		
		// we'll assume that tangent ruleIn and ruleOut are the same, hence only checking
		// ruleIn-- might not be the case always
		if (keyframes[i].getRuleIn().compare("flat") == 0) {
			keyframes[i].setTanIn(.0f);
			keyframes[i].setTanOut(.0f);
		}
		
		else if (keyframes[i].getRuleIn().compare("linear") == 0) {
			//float tangent = ( (keyframes[i+1].getValue() - keyframes[i].getValue()) / (keyframes[i+1].getTime() - keyframes[i].getTime()) );
			float tangent = calculateTangent(keyframes[i+1], keyframes[i]);
			keyframes[i].setTanOut(tangent);
			keyframes[i+1].setTanIn(tangent);
		}
		else if (keyframes[i].getRuleIn().compare("smooth") == 0) {
			//float tangent = ( (keyframes[i+1].getValue() - keyframes[i-1].getValue()) / (keyframes[i+1].getTime() - keyframes[i-1].getTime()) );
			float tangent = calculateTangent(keyframes[i+1], keyframes[i-1]);
			keyframes[i].setTanIn(tangent);
			keyframes[i].setTanOut(tangent);
		}
		// what about step tangent rule? didn't account for that -- lec6 slide 35
	}

	// compute first and last keyframe tangents using linear:

	//float tangent1 = ( (keyframes[1].getValue() - keyframes[0].getValue()) / (keyframes[1].getTime() - keyframes[0].getTime()) );
	float tangent1 = calculateTangent(keyframes[1], keyframes[0]);
	//float tangent2 = ( (keyframes[keyframes.size()-1].getValue() - keyframes[keyframes.size()-2].getValue()) / (keyframes[keyframes.size()-1].getTime() - keyframes[keyframes.size()-2].getTime()) );
	float tangent2 = calculateTangent(keyframes[keyframes.size() - 1], keyframes[keyframes.size() - 2]);

	keyframes[0].setTanOut(tangent1);
	keyframes[keyframes.size() - 1].setTanIn(tangent2);

	return;
}

float Channel::calculateExtrap(std::string method, float time)
{
	// check if time is less than first, or greater
	// than last keyframe
	float t_0 = keyframes[0].getTime();
	float t_end = keyframes[keyframes.size() - 1].getTime();
	float t_width = t_end - t_0;
	int scale = 0;
	float vert_dist = keyframes[keyframes.size() - 1].getValue() - keyframes[0].getValue();

	if (time < t_0) {
		// check the method
		if (method.compare("constant") == 0) {
			return keyframes[0].getValue();
		}
		else if (method.compare("linear") == 0) {
			return keyframes[0].getTanOut(); 
		}
		else if (method.compare("cycle") == 0) {
			// add to time until it reaches within t_0 and t_e
			while (time < t_0) {
				time += t_width;
				scale--;
			}
		
			// finding the correct keyframe
			// now time is within t_0 and t_e
			// apply cubic equation	
			float x = 0.0f;

			for (int j = 0; j < keyframes.size(); j++) {
				if (time > keyframes[j].getTime()) {
					continue;
				}
				else { 
					// time is less than current keyframe, so we look at the one before it
					// Use cubic equation here: 		
					Keyframe currKey = keyframes[j - 1];
					Keyframe nextKey = keyframes[j];
					// you need to normalize time before using it to find the DOF Val: 
					float u = ( (time - currKey.getTime()) / (nextKey.getTime() - currKey.getTime()) ); 

					// set up cubic equation 
					x = currKey.getD() + u * (currKey.getC() + u * (currKey.getB() + u * currKey.getA()));
					return x;
				}
			}
		}
		else if (method.compare("cycle_offset")) {
			// the same except you add to x the scaled value of the vertical dist 
			// add to time until it reaches within t_0 and t_e
			while (time < t_0) {
				time += t_width;
				scale--;
			}
		
			// finding the correct keyframe
			// now time is within t_0 and t_e
			// apply cubic equation	
			float x = 0.0f;

			for (int j = 0; j < keyframes.size(); j++) {
				if (time > keyframes[j].getTime()) {
					continue;
				}
				else { 
					// time is less than current keyframe, so we look at the one before it
					// Use cubic equation here: 		
					Keyframe currKey = keyframes[j - 1];
					Keyframe nextKey = keyframes[j];
					// you need to normalize time before using it to find the DOF Val: 
					float u = ( (time - currKey.getTime()) / (nextKey.getTime() - currKey.getTime()) ); 

					// set up cubic equation 
					x = currKey.getD() + u * (currKey.getC() + u * (currKey.getB() + u * currKey.getA()));
					x += scale * vert_dist;
					return x;
				}
			}
		}
		else if (method.compare("bounce") == 0) {
			// add to time until it reaches within t_0 and t_e
			while (time < t_0) {
				time += t_width;
				scale--;
			}
			
			float x = 0.0f;

			for (int j = 0; j < keyframes.size(); j++) {
				if (time > keyframes[j].getTime()) {
					continue;
				}
				else { 
					// time is less than current keyframe, so we look at the one before it
					// Use cubic equation here: 		
					Keyframe currKey = keyframes[j - 1];
					Keyframe nextKey = keyframes[j];

					// you need to normalize time before using it to find the DOF Val: 
					// check if scale is odd or even and find "u" accordingly
					float u = .0f;
					if (abs(scale % 2) == 1) {
						u = ( (nextKey.getTime() - time) / (nextKey.getTime() - currKey.getTime()) ); 
					} 
					else {
						u = ( (time - currKey.getTime()) / (nextKey.getTime() - currKey.getTime()) ); 
					}

					// set up cubic equation 
					x = currKey.getD() + u * (currKey.getC() + u * (currKey.getB() + u * currKey.getA()));
					return x;
				}
			}
		}
	}
	// time > keyframes[keyframes.size() -1]	
	else {
		if (method.compare("constant") == 0) {
			return keyframes[keyframes.size() - 1].getValue();
		}
		else if (method.compare("linear") == 0) {
			return keyframes[keyframes.size() - 1].getTanIn();
		}
		else if (method.compare("cycle") == 0) {
			// add to time until it reaches within t_0 and t_e
			while (time > t_end) {
				time -= t_width;
				scale++;
			}
		
			// finding the correct keyframe
			// now time is within t_0 and t_e
			// apply cubic equation	
			float x = 0.0f;

			for (int j = 0; j < keyframes.size(); j++) {
				if (time > keyframes[j].getTime()) {
					continue;
				}
				else { 
					// time is less than current keyframe, so we look at the one before it
					// Use cubic equation here: 		
					Keyframe currKey = keyframes[j - 1];
					Keyframe nextKey = keyframes[j];
					// you need to normalize time before using it to find the DOF Val: 
					float u = ( (time - currKey.getTime()) / (nextKey.getTime() - currKey.getTime()) ); 

					// set up cubic equation 
					x = currKey.getD() + u * (currKey.getC() + u * (currKey.getB() + u * currKey.getA()));
					return x;
				}
			}
		}
		else if (method.compare("cycle_offset") == 0) {
			// the same except you add to x the scaled value of the vertical dist 
			// add to time until it reaches within t_0 and t_e
			while (time > t_end) {
				time -= t_width;
				scale++;
			}
		
			// finding the correct keyframe
			// now time is within t_0 and t_e
			// apply cubic equation	
			float x = 0.0f;

			for (int j = 0; j < keyframes.size(); j++) {
				if (time > keyframes[j].getTime()) {
					continue;
				}
				else { 
					// time is less than current keyframe, so we look at the one before it
					// Use cubic equation here: 		
					Keyframe currKey = keyframes[j - 1];
					Keyframe nextKey = keyframes[j];
					// you need to normalize time before using it to find the DOF Val: 
					float u = ( (time - currKey.getTime()) / (nextKey.getTime() - currKey.getTime()) ); 

					// set up cubic equation 
					x = currKey.getD() + u * (currKey.getC() + u * (currKey.getB() + u * currKey.getA()));
					x += scale * vert_dist;
					//std::cout << x << std::endl;
					return x;
				}
			}

		}
		else if (method.compare("bounce") == 0) {
			// add to time until it reaches within t_0 and t_e
			while (time > t_end) {
				time -= t_width;
				scale++;
			}
			
			float x = 0.0f;

			for (int j = 0; j < keyframes.size(); j++) {
				if (time > keyframes[j].getTime()) {
					continue;
				}
				else { 
					// time is less than current keyframe, so we look at the one before it
					// Use cubic equation here: 		
					Keyframe currKey = keyframes[j - 1];
					Keyframe nextKey = keyframes[j];

					// you need to normalize time before using it to find the DOF Val: 
					// check if scale is odd or even and find "u" accordingly
					float u = .0f;
					if (scale % 2 == 1) {
						u = ( (nextKey.getTime() - time) / (nextKey.getTime() - currKey.getTime()) ); 
					} 
					else {
						u = ( (time - currKey.getTime()) / (nextKey.getTime() - currKey.getTime()) ); 
					}

					// set up cubic equation 
					x = currKey.getD() + u * (currKey.getC() + u * (currKey.getB() + u * currKey.getA()));
					return x;
				}
			}

		}
	}
	return .0f;
}

void Channel::precomputeCoeffs()
{
	// NOTE: this is for t between 0 and 1 only
	// for the 1st keyframe all the way until 2nd to last keyframe
	glm::mat4 C(glm::vec4(2.0f, -3.0f, 0.0f, 1.0f), glm::vec4(-2.0f, 3.0f, .0f, .0f), glm::vec4(1.0f, -2.0f, 1.0f, .0f), glm::vec4(1.0f, -1.0f, .0f, .0f));
	for (int i = 0; i < keyframes.size() - 1; i++) {
		glm::vec4 cubicVec(keyframes[i].getValue(), keyframes[i + 1].getValue(), keyframes[i].getTanOut(), keyframes[i].getTanIn());
		glm::vec4 result(C * cubicVec);
		keyframes[i].setA(result.x);
		keyframes[i].setB(result.y);
		keyframes[i].setC(result.z);
		keyframes[i].setD(result.w);
	}
	// last keyframe is not set, since we'll use the second-to-last keyframe
	// if we're inbetween 2nd last and last key span
}

float Channel::calculateTangent(Keyframe& k1, Keyframe& k2) {
	return ( (k1.getValue() - k2.getValue()) / (k1.getTime() - k2.getTime()) );
}

bool Channel::Load(Tokenizer& token)
{
	number = token.GetInt();
	token.FindToken("extrapolate"); 

	char extIn[256], extOut[256];
	token.GetToken(extIn);
	token.GetToken(extOut);
	extrapIn = extIn;
	extrapOut = extOut;

	token.FindToken("keys");
	int numKeys = token.GetInt();
	token.FindToken("{");
	for (int i = 0; i < numKeys; i++) {
		char tempIn[256], tempOut[256];

		keyframes.push_back(Keyframe());
		float time = token.GetFloat();
		float val = token.GetFloat();
		token.GetToken(tempIn);
		token.GetToken(tempOut);

		keyframes[i].setTime(time);
		keyframes[i].setValue(val);
		keyframes[i].setRuleIn(tempIn);
		keyframes[i].setRuleOut(tempOut);
	}

	// all keyframes are parsed, now to calculate the tangents
	// loop thru each keyframe and calculate
	// computeTangents() does this for us
	computeTangents();
	// also precompute cubic coeffs for each
	precomputeCoeffs();

	return true;
}
