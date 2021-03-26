#include "AnimationClip.h"

AnimationClip::AnimationClip()
{
    t_start = .0f;
    t_end = .0f;
}

AnimationClip::~AnimationClip()
{
    for (int i = 0; i < channels.size(); i++) {
        delete channels[i];
    }
}

bool AnimationClip::Load(const char* fileName)
{
    Tokenizer token;
    token.Open(fileName);
    token.FindToken("animation");
    token.FindToken("range");
    t_start = token.GetFloat();
    t_end = token.GetFloat(); 

    token.FindToken("numchannels");
    int numChannels = token.GetInt();

    // handle the first 3 channels for translations 
    // before passing it into recursive channel calls
    //channels.push_back(new Channel());
   
    // loop thru all channels in a for loop
    for (int i = 0; i < numChannels; i++) {
        token.FindToken("channel"); 
        channels.push_back(new Channel());
        channels[i]->Load(token);
    }

    token.Close();
    return true;
}

void AnimationClip::Evaluate(float time, std::vector<Dof*>& dofArray)
{
    // animation clip will call channel->evalute and update
    // each channel's DOFs accordingly for a given time
    for (int i = 0; i < dofArray.size(); i++) {
        dofArray[i]->setValue(channels[i]->Evaluate(time));
    }
}

float AnimationClip::getStartTime()
{
    return t_start;
}

float AnimationClip::getEndTime()
{
    return t_end;
}
