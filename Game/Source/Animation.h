#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "SDL/include/SDL_rect.h"
#define MAX_FRAMES 128

class Animation
{
public:
	float speedx = 1.0f;
	SDL_Rect frames[MAX_FRAMES];
	bool loop = true;
	// Allows the animation to keep going back and forth
	int totalFrames = 0;

private:
	float currentFrame = 0.0f;
	int loopCount = 0;

public:

	void PushBack(const SDL_Rect& rect)
	{
		frames[totalFrames++] = rect;
	}

	void Reset()
	{
		currentFrame = 0.0f;
	}
	
	bool HasFinished()
	{
		return !loop && loopCount > 0;
	}

	void Update()
	{
		currentFrame += speedx;
		if (currentFrame >= totalFrames)
		{
			currentFrame = (loop) ? 0.0f : totalFrames - 1;
			++loopCount;
		}
	}

	const SDL_Rect& GetCurrentFrame() const
	{
		int actualFrame = currentFrame;
		actualFrame = totalFrames - currentFrame;
		return frames[actualFrame];


	}
};

#endif