#ifndef _ANIMATION_CLIP_H_
#define _ANIMATION_CLIP_H_

#include "Channel.h"
#include "Pose.h"
#include "Dof.h"

class AnimationClip {
private:
	std::vector<Channel*> channels;
	float t_start, t_end;

public: 

	AnimationClip();

	~AnimationClip(); 

	float getStartTime(); 

	float getEndTime();

	bool Load(const char* fileName);
	//void Evaluate(float time, Pose& p);
	
	// our make shift eval function: takes in a time and 
	void Evaluate(float time, std::vector<Dof*>& dofArray);
};


#endif