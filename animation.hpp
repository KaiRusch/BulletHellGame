#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>

class Animation
{
public:

  std::vector<int> frames;
  bool playing;
  float speed;
  float delay;
  int currentFrameIndex;

  Animation(std::vector<int> frames, float speed);
  
  void play();  
  int update(float dt);
};

#endif //ANIMATION_H
