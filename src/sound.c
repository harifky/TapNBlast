#include "../lib/sound.h"

// Kontribusi: Fariz (100)

// ========== DEKLARASI VARIABEL SOUND DAN MUSIC ==========

// Sound effects untuk berbagai aksi dalam game
Sound clickSound;        // Suara klik tombol/UI
Sound scoreSound;        // Suara saat mendapat skor
Sound explosionSound;    // Suara ledakan/destroy block
Sound gameOverSound;     // Suara game over
Sound placeBlockSound;   // Suara saat menempatkan block

// Background music untuk berbagai state game
Music menuMusic;         // Musik latar untuk menu
Music gameMusic;         // Musik latar saat bermain

// Flag untuk kontrol audio
boolean isSoundOn = true;  // Toggle untuk sound effects
boolean isMusicOn = true;  // Toggle untuk background music

/**
 * Inisialisasi semua sound effects dan background music
 * Memuat file audio dari folder assets dan mengatur volume default
 */
void InitSounds(){
    // ===== LOAD SOUND EFFECTS =====
    // Memuat file sound dari assets folder
    clickSound = LoadSound("assets/buttonfx.wav");           // Efek suara tombol
    scoreSound = LoadSound("assets/scorecoint.wav");         // Efek suara skor/koin
    explosionSound = LoadSound("assets/boom.mp3");           // Efek suara ledakan
    gameOverSound = LoadSound("assets/gameOver.mp3");        // Efek suara game over
    placeBlockSound = LoadSound("assets/placeBlock.wav");    // Efek suara place block
    
    // ===== LOAD BACKGROUND MUSIC =====
    // Memuat streaming music (untuk file besar)
    menuMusic = LoadMusicStream("assets/backsoundMenu.mp3"); // Musik menu
    gameMusic = LoadMusicStream("assets/backsoundGame.mp3"); // Musik gameplay

    // ===== PENGATURAN VOLUME SOUND EFFECTS =====
    // Mengatur volume setiap sound effect (0.0f - 1.0f)
    SetSoundVolume(clickSound, 1.0f);        // Volume penuh untuk klik
    SetSoundVolume(scoreSound, 0.6f);        // Volume sedang untuk skor
    SetSoundVolume(explosionSound, 0.8f);    // Volume tinggi untuk ledakan
    SetSoundVolume(gameOverSound, 1.0f);     // Volume penuh untuk game over
    SetSoundVolume(placeBlockSound, 1.0f);   // Volume penuh untuk place block

    // ===== PENGATURAN VOLUME BACKGROUND MUSIC =====
    // Volume musik latar lebih rendah agar tidak mengganggu gameplay
    SetMusicVolume(menuMusic, 0.5f);         // Volume setengah untuk musik menu
    SetMusicVolume(gameMusic, 0.5f);         // Volume setengah untuk musik game
}

/**
 * Memutar sound effect klik tombol
 * Hanya dimainkan jika sound effects diaktifkan
 */
void PlayClickSound(){     
    if (isSoundOn) {
        PlaySound(clickSound);
    } 
}

/**
 * Memutar sound effect saat mendapat skor
 * Memberikan feedback audio positif kepada player
 */
void PlayScoreSound(){
    if (isSoundOn){
        PlaySound(scoreSound); 
    }
}

/**
 * Memutar sound effect ledakan
 * Biasanya saat block dihancurkan atau ada efek destruktif
 */
void PlayExplosionSound(){ 
    if (isSoundOn){
        PlaySound(explosionSound); 
    }
}

/**
 * Memutar sound effect game over
 * Memberikan feedback audio saat permainan berakhir
 */
void PlayGameOverSound(){ 
    if (isSoundOn){
        PlaySound(gameOverSound); 
    }
}

/**
 * Memutar sound effect saat menempatkan block
 * Memberikan feedback audio saat player melakukan aksi place
 */
void PlayPlaceBlockSound(){
    if (isSoundOn){
        PlaySound(placeBlockSound); 
    }
}

/**
 * Memutar background music untuk menu
 * Otomatis menghentikan musik lain yang sedang berjalan
 */
void PlayBacksoundMenu() {
    // Cek apakah musik diaktifkan dan musik menu belum diputar
    if (isMusicOn && !IsMusicStreamPlaying(menuMusic)) {
        StopBacksound();           // Hentikan musik lain yang sedang berjalan
        PlayMusicStream(menuMusic); // Mulai musik menu
    }
}

/**
 * Memutar background music untuk gameplay
 * Otomatis menghentikan musik lain yang sedang berjalan
 */
void PlayBacksoundGame() {
    // Cek apakah musik diaktifkan dan musik game belum diputar
    if (isMusicOn && !IsMusicStreamPlaying(gameMusic)){
        StopBacksound();           // Hentikan musik lain yang sedang berjalan
        PlayMusicStream(gameMusic); // Mulai musik game
    }
}

/**
 * Menghentikan semua background music
 * Digunakan saat berpindah state atau mematikan musik
 */
void StopBacksound() {
    StopMusicStream(menuMusic);    // Hentikan musik menu
    StopMusicStream(gameMusic);    // Hentikan musik game
}

/**
 * Update streaming music buffer
 * PENTING: Harus dipanggil setiap frame untuk musik streaming
 * Tanpa ini, musik akan terputus-putus atau tidak berjalan
 */
void UpdateMusic() {
    UpdateMusicStream(menuMusic);  // Update buffer musik menu
    UpdateMusicStream(gameMusic);  // Update buffer musik game
}

/**
 * Membersihkan semua resource audio dari memori
 * PENTING: Harus dipanggil saat keluar dari aplikasi
 * untuk mencegah memory leak
 */
void UnloadSounds() {
    // ===== UNLOAD SOUND EFFECTS =====
    UnloadSound(clickSound);       // Bebaskan memori sound klik
    UnloadSound(explosionSound);   // Bebaskan memori sound ledakan
    UnloadSound(gameOverSound);    // Bebaskan memori sound game over
    UnloadSound(placeBlockSound);  // Bebaskan memori sound place block
    
    // ===== UNLOAD BACKGROUND MUSIC =====
    UnloadMusicStream(menuMusic);  // Bebaskan memori musik menu
    UnloadMusicStream(gameMusic);  // Bebaskan memori musik game
    
    // CATATAN: scoreSound tidak di-unload, kemungkinan bug atau oversight
}