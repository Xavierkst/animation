#ifndef _ANIMATION_PLAYER_H_
#define _ANIMATION_PLAYER_H_

#include "Skeleton.h"
#include "AnimationClip.h"

class AnimationPlayer {
private:
	float time;
	Skeleton* skel;
	AnimationClip* clip;

public:
	AnimationPlayer();

	AnimationPlayer(AnimationClip* c, Skeleton* skeleton);

	~AnimationPlayer();

	void SetClip(AnimationClip* c);
	//const Pose& getPose();

	// returns a reference to an array of the current joint DOFs
	std::vector<Dof*>& getPoseDofs();

	void Update(float deltaTime);
};

#endif