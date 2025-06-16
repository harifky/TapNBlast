#ifndef ANIMATION_H
#define ANIMATION_H

#include "boolean.h"

// Struktur untuk animasi clear
typedef struct clearAnim {
    int x, y;
    float timer;
    struct clearAnim* next;
} clearAnim;

// Struktur untuk animasi place
typedef struct placeAnim {
    int x, y;
    int blockType;
    float timer;
    struct placeAnim* next;
} placeAnim;

// Fungsi untuk animasi clear
void EnqueueClearAnimation(int x, int y);
void UpdateAndDrawClearAnimations(float deltaTime);

// Fungsi untuk animasi place
void EnqueuePlaceAnimation(int x, int y, int blockType);
void UpdateAndDrawPlaceAnimations(float deltaTime);

// Fungsi umum
void UpdateAndDrawAnimations(float deltaTime);
boolean IsAnyAnimationActive();
boolean IsClearAnimationActive();
boolean IsPlaceAnimationActive();
void ClearAnimationQueue();
    
// Fungsi easing
float EaseOutBounce(float t);
float EaseOutQuart(float t);
float EaseInQuart(float t);

#endif // ANIMATION_H