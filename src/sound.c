#include "../lib/sound.h"

Sound clickSound;
Sound scoreSound;
Sound explosionSound;
Sound gameOverSound;
Sound placeBlockSound;

Music menuMusic;
Music gameMusic;

boolean isSoundOn = true;
boolean isMusicOn = true;


void InitSounds(){
    clickSound = LoadSound("assets/buttonfx.wav");
    scoreSound = LoadSound("assets/scorecoint.wav");
    explosionSound = LoadSound("assets/boom.mp3");
    gameOverSound = LoadSound("assets/gameOver.mp3");
    placeBlockSound = LoadSound("assets/placeBlock.wav");
    menuMusic = LoadMusicStream("assets/backsoundMenu.mp3");
    gameMusic = LoadMusicStream("assets/backsoundGame.mp3");

    SetSoundVolume(clickSound, 1.0f);
    SetSoundVolume(scoreSound, 0.6f);
    SetSoundVolume(explosionSound, 0.8f);
    SetSoundVolume(gameOverSound, 1.0f);
    SetSoundVolume(placeBlockSound, 1.0f);

    SetMusicVolume(menuMusic, 0.5f);
    SetMusicVolume(gameMusic, 0.5f);
}

void PlayClickSound(){     
    if (isSoundOn) {
        PlaySound(clickSound);
    } 
}

void PlayScoreSound(){
    if (isSoundOn){
        PlaySound(scoreSound); 
    }
}

void PlayExplosionSound(){ 
    if (isSoundOn){
        PlaySound(explosionSound); 
    }
}

void PlayGameOverSound(){ 
    if (isSoundOn){
        PlaySound(gameOverSound); 
    }
}

void PlayPlaceBlockSound(){
    if (isSoundOn){
        PlaySound(placeBlockSound); 
    }
}

void PlayBacksoundMenu() {
    if (isMusicOn && !IsMusicStreamPlaying(menuMusic)) {   // ← cegah restart
        StopBacksound();
        PlayMusicStream(menuMusic);      // setel volume di sini
    }
}

void PlayBacksoundGame() {
    if (isMusicOn && !IsMusicStreamPlaying(gameMusic)){
        StopBacksound();
        PlayMusicStream(gameMusic);
    }
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