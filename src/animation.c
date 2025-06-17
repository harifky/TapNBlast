#include "../lib/animation.h"
#include "../lib/global.h"
#include "raylib.h"
#include <stdlib.h>
#include <math.h>

// Kontribusi: Faiz, Fariz

clearAnim* clearAnimFront = NULL;
clearAnim* clearAnimRear = NULL;

placeAnim* placeAnimFront = NULL;
placeAnim* placeAnimRear = NULL;

void EnqueueClearAnimation(int x, int y) {
    clearAnim* newNode = (clearAnim*)malloc(sizeof(clearAnim));
    if (newNode == NULL) return;
    
    newNode->x = x;
    newNode->y = y;
    newNode->timer = 0.0f;
    newNode->next = NULL;

    if (clearAnimRear == NULL) {
        clearAnimFront = clearAnimRear = newNode;
    } else {
        clearAnimRear->next = newNode;
        clearAnimRear = newNode;
    }
}

void EnqueuePlaceAnimation(int x, int y, int blockType) {
    placeAnim* newNode = (placeAnim*)malloc(sizeof(placeAnim));
    if (newNode == NULL) return;
    
    newNode->x = x;
    newNode->y = y;
    newNode->blockType = blockType;
    newNode->timer = 0.0f;
    newNode->next = NULL;

    if (placeAnimRear == NULL) {
        placeAnimFront = placeAnimRear = newNode;
    } else {
        placeAnimRear->next = newNode;
        placeAnimRear = newNode;
    }
}

float EaseOutBounce(float t) {
    if (t < 1.0f / 2.75f) {
        return 7.5625f * t * t;
    } else if (t < 2.0f / 2.75f) {
        t -= 1.5f / 2.75f;
        return 7.5625f * t * t + 0.75f;
    } else if (t < 2.5f / 2.75f) {
        t -= 2.25f / 2.75f;
        return 7.5625f * t * t + 0.9375f;
    } else {
        t -= 2.625f / 2.75f;
        return 7.5625f * t * t + 0.984375f;
    }
}

float EaseOutQuart(float t) {
    return 1.0f - powf(1.0f - t, 4.0f);
}

float EaseInQuart(float t) {
    return t * t * t * t;
}

void UpdateAndDrawClearAnimations(float deltaTime) {
    clearAnim* curr = clearAnimFront;
    clearAnim* prev = NULL;

    while (curr != NULL) {
        curr->timer += deltaTime;
        
        const float CLEAR_DURATION = 0.6f;
        float progress = curr->timer / CLEAR_DURATION;
        
        if (progress <= 1.0f) {
            float alpha = 1.0f - EaseInQuart(progress);
            float scale = 1.0f + (progress * 0.3f);
            
            Color color = blockColors[grid[curr->y][curr->x] - 1];
            color.a = (unsigned char)(255 * alpha);
            
            int dx = gridOriginX + curr->x * TILE_SIZE;
            int dy = gridOriginY + curr->y * TILE_SIZE;
            
            int scaledSize = (int)((TILE_SIZE - 4) * scale);
            int offsetX = (TILE_SIZE - scaledSize) / 2;
            int offsetY = (TILE_SIZE - scaledSize) / 2;
            
            DrawRectangle(dx + 2 + offsetX, dy + 2 + offsetY, scaledSize, scaledSize, color);
            
            if (alpha > 0.3f) {
                Color glowColor = color;
                glowColor.a = (unsigned char)(50 * alpha);
                DrawRectangle(dx, dy, TILE_SIZE, TILE_SIZE, glowColor);
            }
        }

        if (curr->timer >= CLEAR_DURATION) {
            grid[curr->y][curr->x] = 0;

            if (prev == NULL) {
                clearAnim* temp = curr;
                clearAnimFront = curr->next;
                if (clearAnimFront == NULL) clearAnimRear = NULL;
                curr = curr->next;
                free(temp);
            } else {
                prev->next = curr->next;
                if (curr == clearAnimRear) clearAnimRear = prev;
                clearAnim* temp = curr;
                curr = prev->next;
                free(temp);
            }
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void UpdateAndDrawPlaceAnimations(float deltaTime) {
    placeAnim* curr = placeAnimFront;
    placeAnim* prev = NULL;

    while (curr != NULL) {
        curr->timer += deltaTime;
        
        const float PLACE_DURATION = 0.4f;
        float progress = curr->timer / PLACE_DURATION;
        
        if (progress <= 1.0f) {
            float scale = EaseOutBounce(progress);
            float alpha = EaseOutQuart(progress);
            
            Color color = blockColors[curr->blockType - 1];
            color.a = (unsigned char)(255 * alpha);
            
            int dx = gridOriginX + curr->x * TILE_SIZE;
            int dy = gridOriginY + curr->y * TILE_SIZE;
            
            int scaledSize = (int)((TILE_SIZE - 4) * scale);
            int offsetX = (TILE_SIZE - scaledSize) / 2;
            int offsetY = (TILE_SIZE - scaledSize) / 2;
            
            DrawRectangle(dx + 2 + offsetX, dy + 2 + offsetY, scaledSize, scaledSize, color);
            
            if (progress < 0.3f) {
                Color highlightColor = WHITE;
                highlightColor.a = (unsigned char)(100 * (1.0f - progress / 0.3f));
                DrawRectangle(dx + 2 + offsetX, dy + 2 + offsetY, scaledSize, scaledSize, highlightColor);
            }
        }

        if (curr->timer >= PLACE_DURATION) {
            grid[curr->y][curr->x] = curr->blockType;

            if (prev == NULL) {
                placeAnim* temp = curr;
                placeAnimFront = curr->next;
                if (placeAnimFront == NULL) placeAnimRear = NULL;
                curr = curr->next;
                free(temp);
            } else {
                prev->next = curr->next;
                if (curr == placeAnimRear) placeAnimRear = prev;
                placeAnim* temp = curr;
                curr = prev->next;
                free(temp);
            }
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void UpdateAndDrawAnimations(float deltaTime) {
    UpdateAndDrawClearAnimations(deltaTime);
    UpdateAndDrawPlaceAnimations(deltaTime);
}

boolean IsAnyAnimationActive() {
    return (clearAnimFront != NULL) || (placeAnimFront != NULL);
}

boolean IsClearAnimationActive() {
    return clearAnimFront != NULL;
}

boolean IsPlaceAnimationActive() {
    return placeAnimFront != NULL;
}

void ClearAnimationQueue() {
    while (clearAnimFront != NULL) {
        clearAnim* temp = clearAnimFront;
        clearAnimFront = clearAnimFront->next;
        free(temp);
    }
    clearAnimRear = NULL;
    
    while (placeAnimFront != NULL) {
        placeAnim* temp = placeAnimFront;
        placeAnimFront = placeAnimFront->next;
        free(temp);
    }
    placeAnimRear = NULL;
}