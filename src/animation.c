#include "../lib/animation.h"
#include "../lib/global.h"
#include "raylib.h"
#include <stdlib.h>

clearAnim* animFront = NULL;
clearAnim* animRear = NULL;

void EnqueueAnimation(int x, int y) {
    clearAnim* newNode = (clearAnim*)malloc(sizeof(clearAnim));
    newNode->x = x;
    newNode->y = y;
    newNode->timer = 0;
    newNode->next = NULL;

    if (animRear == NULL) {
        animFront = animRear = newNode;
    } else {
        animRear->next = newNode;
        animRear = newNode;
    }
}

void UpdateAndDrawAnimations(float deltaTime) {
    clearAnim* curr = animFront;
    clearAnim* prev = NULL;

    while (curr != NULL) {
        curr->timer += deltaTime;

        // Gambar dengan efek alpha
        float alpha = 1.0f - (curr->timer / 0.5f);
        Color color = blockColors[grid[curr->y][curr->x] - 1];
        color.a = (unsigned char)(255 * alpha);
        int dx = gridOriginX + curr->x * TILE_SIZE;
        int dy = gridOriginY + curr->y * TILE_SIZE;
        DrawRectangle(dx + 2, dy + 2, TILE_SIZE - 4, TILE_SIZE - 4, color);

        if (curr->timer > 0.5f) {
            grid[curr->y][curr->x] = 0;

            // Remove node
            if (prev == NULL) {
                clearAnim* temp = curr;
                animFront = curr->next;
                if (animFront == NULL) animRear = NULL;
                curr = curr->next;
                free(temp);
            } else {
                prev->next = curr->next;
                if (curr == animRear) animRear = prev;
                free(curr);
                curr = prev->next;
            }
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

boolean IsAnyAnimationActive() {
    return animFront != NULL;
}

void ClearAnimationQueue() {
    while (animFront != NULL) {
        clearAnim* temp = animFront;
        animFront = animFront->next;
        free(temp);
    }
    animRear = NULL;
}
