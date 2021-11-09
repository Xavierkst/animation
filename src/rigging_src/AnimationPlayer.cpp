#include "AnimationPlayer.h"

AnimationPlayer::AnimationPlayer()
{
	time = .0f;
	skel = nullptr;
	clip = nullptr;
}

AnimationPlayer::AnimationPlayer(AnimationClip* c, Skeleton* skeleton)
{
	time = 0.0f;
	clip = c;
	skel = skeleton;
}

AnimationPlayer::~AnimationPlayer()
{
}

void AnimationPlayer::SetClip(AnimationClip* c)
{
	clip = c;
}

std::vector<Dof*>& AnimationPlayer::getPoseDofs()
{
	return skel->getDofArray();
}

void AnimationPlayer::Update(float deltaTime)
{
	time += deltaTime;
	clip->Evaluate(time, this->getPoseDofs());
	
	// if time is greater than the end of clip time, reset it 
	if (time > clip->getEndTime()) {
		time = 0.01f;
	}
	//skel->getRoot()->setOffset(glm::vec3(skel->getDofArray()[0], skel->getDofArray()[1], skel->getDofArray()[2]));
}

