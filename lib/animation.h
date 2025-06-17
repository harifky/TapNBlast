#ifndef ANIMATION_H
#define ANIMATION_H

#include "boolean.h"

// Struktur Animasi Clear
typedef struct clearAnim {
    int x, y;
    float timer;
    struct clearAnim* next;
} clearAnim;

// Struktur Animasi Place
typedef struct placeAnim {
    int x, y;
    int blockType;
    float timer;
    struct placeAnim* next;
} placeAnim;

// Fungsi Animasi Clear
void EnqueueClearAnimation(int x, int y);
void UpdateAndDrawClearAnimations(float deltaTime);

// Fungsi Animasi Place
void EnqueuePlaceAnimation(int x, int y, int blockType);
void UpdateAndDrawPlaceAnimations(float deltaTime);

// Fungsi Umum
void UpdateAndDrawAnimations(float deltaTime);
boolean IsAnyAnimationActive();
boolean IsClearAnimationActive();
boolean IsPlaceAnimationActive();
void ClearAnimationQueue();
    
// Fungsi Easing
float EaseOutBounce(float t);
float EaseOutQuart(float t);
float EaseInQuart(float t);

#endif // ANIMATION_H