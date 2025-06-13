#ifndef ANIMATION_H
#define ANIMATION_H

typedef struct clearAnim {
    int x, y;
    float timer;
    struct clearAnim* next;
} clearAnim;

extern clearAnim* animFront;
extern clearAnim* animRear;

void EnqueueAnimation(int x, int y);
void UpdateAndDrawAnimations(float deltaTime);
bool IsAnyAnimationActive();
void ClearAnimationQueue();

#endif