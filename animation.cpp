#include "animation.hpp"

Animation::Animation(std::vector<int> frames, float speed) : frames(frames), playing(false), speed(speed), delay(0.0f), currentFrameIndex(0)
{
};

void Animation::play()
{
  playing = true;
};

int Animation::update(float dt)
{
  if(playing)
    {
      delay += dt;
      if(delay >= speed)
	{
	  currentFrameIndex++;
	  delay = 0.0f;
	}
    }
  
  if(currentFrameIndex == frames.size() - 1)
    {
      playing = false;
    }
  
  return frames[currentFrameIndex];
  
}
