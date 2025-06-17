#ifndef ANIMATION_H
#define ANIMATION_H

#include "boolean.h"

typedef struct clearAnim {
    int x, y;
    float timer;
    struct clearAnim* next;
} clearAnim;

extern clearAnim* animFront;
extern clearAnim* animRear;

void EnqueueAnimation(int x, int y);
void UpdateAndDrawAnimations(float deltaTime);
boolean IsAnyAnimationActive();
void ClearAnimationQueue();

#endif