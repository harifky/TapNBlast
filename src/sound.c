#include "../lib/sound.h"

Sound clickSound;
Sound explosionSound;
Sound gameOverSound;
Sound placeBlockSound;

Music menuMusic;
Music gameMusic;


void InitSounds(){
    clickSound = LoadSound("assets/buttonfx.wav");
    explosionSound = LoadSound("assets/boom.mp3");
    gameOverSound = LoadSound("assets/gameOver.mp3");
    placeBlockSound = LoadSound("assets/placeBlock.wav");
    menuMusic = LoadMusicStream("assets/backsoundMenu.mp3");
    gameMusic = LoadMusicStream("assets/backsoundGame.mp3");

    SetSoundVolume(clickSound, 1.0f);
    SetSoundVolume(explosionSound, 0.8f);
    SetSoundVolume(gameOverSound, 1.0f);
    SetSoundVolume(placeBlockSound, 0.9f);
    SetSoundVolume(placeBlockSound, 0.9f);

    SetMusicVolume(menuMusic, 1.0f);
    SetMusicVolume(gameMusic, 1.0f);
}

void PlayClickSound()        { PlaySound(clickSound); }
void PlayExplosionSound()    { PlaySound(explosionSound); }
void PlayGameOverSound()     { PlaySound(gameOverSound); }
void PlayPlaceBlockSound()   { PlaySound(placeBlockSound); }


void PlayBacksoundMenu() {
    // StopBacksound();
    // PlayMusicStream(menuMusic);

    if (!IsMusicStreamPlaying(menuMusic)) {   // ← cegah restart
        StopBacksound();
        PlayMusicStream(menuMusic);      // setel volume di sini
    }
}

void PlayBacksoundGame() {
    StopBacksound();
    PlayMusicStream(gameMusic);
}

void StopBacksound() {
    StopMusicStream(menuMusic);
    StopMusicStream(gameMusic);
}

void UpdateMusic() {
    UpdateMusicStream(menuMusic);
    UpdateMusicStream(gameMusic);
}


void UnloadSounds() {
    UnloadSound(clickSound);
    UnloadSound(explosionSound);
    UnloadSound(gameOverSound);
    UnloadSound(placeBlockSound);
    UnloadMusicStream(menuMusic);
    UnloadMusicStream(gameMusic);
}