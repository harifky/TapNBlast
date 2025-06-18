#include "../lib/ui.h"
#include "../lib/animation.h"
#include "../lib/leaderboard.h"
#include "../lib/global.h"

// Kontribusi: Fariz (80), Faiz (10), Rifky (10)

// Variabel global untuk menyimpan state permainan saat ini
GameState currentGameState = GAME_STATE_MENU;

/**
 * Fungsi untuk menggambar grid permainan utama
 * Menggambar background grid, border, dan setiap sel dengan efek visual
 */
void DrawGrids() {
    // Gambar background grid dengan sudut membulat
    DrawRectangleRounded(
        (Rectangle){
            gridOriginX - 5,                    // X dengan padding
            gridOriginY - 5,                    // Y dengan padding  
            TILE_SIZE * GRID_SIZE + 10,         // Lebar total grid + padding
            TILE_SIZE * GRID_SIZE + 10          // Tinggi total grid + padding
        }, 
        0.05f,      // Radius sudut (5% dari ukuran)
        10,         // Jumlah segmen untuk sudut
        UI_GRID_BG  // Warna background grid
    );
    
    // Gambar border/garis tepi grid
    DrawRectangleRoundedLines(
        (Rectangle){
            gridOriginX - 5, 
            gridOriginY - 5, 
            TILE_SIZE * GRID_SIZE + 10, 
            TILE_SIZE * GRID_SIZE + 10
        }, 
        0.05f, 
        10, 
        UI_BORDER  // Warna border
    );
    
    // Loop untuk menggambar setiap sel dalam grid
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            // Tentukan warna sel: kosong atau berisi blok
            Color c = (grid[y][x] == 0) ? UI_EMPTY_CELL : blockColors[grid[y][x]-1];
            float scale = 1.0f;  // Skala untuk animasi (belum diimplementasi)
            
            // Hitung ukuran dan posisi sel
            int size = (int)(TILE_SIZE * scale);
            int dx = gridOriginX + x * TILE_SIZE + (TILE_SIZE - size) / 2;  // Posisi X dengan centering
            int dy = gridOriginY + y * TILE_SIZE + (TILE_SIZE - size) / 2;  // Posisi Y dengan centering
            
            if (grid[y][x] == 0) {
                // Gambar sel kosong dengan sudut sedikit membulat
                DrawRectangleRounded(
                    (Rectangle){dx, dy, size - 2, size - 2},
                    0.2f,   // Radius sudut 20%
                    1,      // Segmen minimal
                    c       // Warna sel kosong
                );
            } else {
                // Gambar sel berisi blok dengan sudut lebih membulat
                DrawRectangleRounded(
                    (Rectangle){dx, dy, size - 2, size - 2},
                    0.3f,   // Radius sudut 30%
                    1, 
                    c       // Warna sesuai jenis blok
                );
                
                // Tambahkan efek glossy/mengkilap pada blok
                DrawRectangleRounded(
                    (Rectangle){dx + size/4, dy + size/4, size/2, size/6},
                    0.5f,
                    1,
                    (Color){255, 255, 255, 80}  // Putih transparan untuk efek glossy
                );
            }
        }
    }
}

/**
 * Fungsi untuk menggambar panel skor di pojok kiri atas
 */
void DrawScorePanel() {
    // Gambar background panel skor
    DrawRectangleRounded(
        (Rectangle){10, 10, 140, 85},  // Posisi dan ukuran panel
        0.15f,      // Radius sudut 15%
        10,         // Segmen sudut
        UI_PANEL    // Warna background panel
    );
    
    // Gambar border panel skor
    DrawRectangleRoundedLines(
        (Rectangle){10, 10, 140, 85},
        0.15f,
        10,
        UI_BORDER   // Warna border
    );
    
    // Gambar teks "SCORE"
    DrawText("SCORE", 25, 15, 20, UI_TEXT);
    
    // Gambar nilai skor dengan efek shadow untuk keterbacaan lebih baik
    DrawText(TextFormat("%d", score), 28, 42, 32, (Color){0, 0, 0, 60});  // Shadow gelap
    DrawText(TextFormat("%d", score), 25, 40, 32, UI_SCORE_TEXT);         // Teks utama
}

/**
 * Inisialisasi menu utama dan semua tombol
 * Mengatur posisi, warna, dan teks untuk setiap tombol
 */
void InitMainMenu() {
    currentMenuState = MENU_MAIN;
    menuAnimTime = 0.0f;        // Reset waktu animasi
    titleBounce = 0.0f;         // Reset animasi bounce judul
    backgroundOffset = 0.0f;    // Reset offset background

    // Inisialisasi tombol PLAY
    playButton = (MenuButton){
        .rect       = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 120, 200, 50},
        .color      = (Color){70, 130, 180, 255},      // Steel Blue
        .hoverColor = (Color){100, 149, 237, 255},     // Cornflower Blue saat hover
        .textColor  = WHITE,
        .animScale  = 1.0f,     // Skala awal untuk animasi
        .text = "PLAY"
    };

    // Inisialisasi tombol SETTINGS
    settingsButton = (MenuButton){
        .rect       = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 200, 50},
        .color      = (Color){60, 179, 113, 255},      // Medium Sea Green
        .hoverColor = (Color){90, 195, 140, 255},
        .textColor  = WHITE,
        .animScale  = 1.0f,
        .text = "SETTINGS"
    };

    // Inisialisasi tombol LEADERBOARD
    leaderboardButton = (MenuButton){
        .rect       = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 20, 200, 50},
        .color      = (Color){180, 90, 255, 255},       // Ungu
        .hoverColor = (Color){255, 165, 50, 255},       // Orange saat hover
        .textColor  = WHITE,
        .animScale  = 1.0f,
        .text = "LEADERBOARD"
    };

    // Inisialisasi tombol EXIT
    exitButton = (MenuButton){
        .rect       = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 90, 200, 50},
        .color      = (Color){220, 20, 60, 255},       // Crimson (merah)
        .hoverColor = (Color){255, 69, 100, 255},
        .textColor  = WHITE,
        .animScale  = 1.0f,
        .text = "EXIT"
    };

    // Inisialisasi tombol BACK (untuk kembali dari sub-menu)
    backButton = (MenuButton){
        .rect       = {50, SCREEN_HEIGHT - 80, 120, 40},
        .color      = (Color){128, 128, 128, 255},     // Abu-abu
        .hoverColor = (Color){160, 160, 160, 255},
        .textColor  = WHITE,
        .animScale  = 1.0f,
        .text = "BACK"
    };

    // Inisialisasi tombol ABOUT
    aboutButton = (MenuButton){
        .rect = {SCREEN_WIDTH / 2 - 140, 350, 120, 30},
        .color = (Color){255, 165, 0, 255},            // Orange
        .hoverColor = (Color){255, 185, 50, 255},
        .textColor = WHITE,
        .animScale = 1.0f,
        .text = "ABOUT"
    };
}

/**
 * Update animasi dan interaksi untuk tombol menu
 * Mendeteksi hover, klik, dan mengatur animasi scale
 */
void UpdateMenuButton(MenuButton* button) {
    Vector2 mousePos = GetMousePosition();
    bool wasPressed = button->isPressed;
    
    // Deteksi apakah mouse berada di atas tombol
    button->isHovered = CheckCollisionPointRec(mousePos, button->rect);
    
    // Deteksi klik pada tombol
    button->isPressed = button->isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    
    // Play sound effect saat tombol diklik (hanya sekali per klik)
    if (button->isPressed && !wasPressed) {
        PlayClickSound();
    }
    
    // Animasi scale smooth: membesar saat hover, normal saat tidak
    float targetScale = button->isHovered ? 1.05f : 1.0f;
    button->animScale += (targetScale - button->animScale) * 0.15f;  // Interpolasi smooth
}

/**
 * Menggambar tombol dengan animasi dan efek visual
 */
void DrawMenuButton(MenuButton* button) {
    Rectangle animRect = button->rect;
    
    // Terapkan animasi scale pada ukuran tombol
    float scaleDiff = (button->animScale - 1.0f) * animRect.width;
    animRect.x -= scaleDiff / 2;                    // Posisi X disesuaikan agar tetap center
    animRect.width += scaleDiff;                    // Lebar bertambah
    animRect.height += scaleDiff * 0.25f;           // Tinggi bertambah sedikit
    
    // Pilih warna berdasarkan status hover
    Color buttonColor = button->isHovered ? button->hoverColor : button->color;
    
    // Gambar shadow tombol untuk efek depth
    DrawRectangleRounded(
        (Rectangle){animRect.x + 3, animRect.y + 3, animRect.width, animRect.height},
        0.15f, 10, (Color){0, 0, 0, 80}  // Shadow hitam transparan
    );
    
    // Gambar tombol utama
    DrawRectangleRounded(animRect, 0.15f, 10, buttonColor);
    
    // Gambar border tombol
    DrawRectangleRoundedLines(animRect, 0.15f, 10, (Color){255, 255, 255, 100});
    
    // Gambar efek glossy di bagian atas tombol
    DrawRectangleRounded(
        (Rectangle){
            animRect.x + animRect.width * 0.1f,      // 10% dari kiri
            animRect.y + animRect.height * 0.1f,     // 10% dari atas
            animRect.width * 0.8f,                   // 80% lebar
            animRect.height * 0.3f                   // 30% tinggi
        },
        0.2f, 5, (Color){255, 255, 255, 60}  // Putih transparan
    );
    
    // Gambar teks tombol di tengah
    int textWidth = MeasureText(button->text, 20);
    DrawText(
        button->text,
        animRect.x + (animRect.width - textWidth) / 2,   // Center horizontal
        animRect.y + (animRect.height - 20) / 2,         // Center vertical
        20,
        button->textColor
    );
}

/**
 * Menggambar background animasi untuk menu
 * Menggunakan gradient dan pattern blok bergerak
 */
void DrawMenuBackground() {
    // Update offset untuk animasi background bergerak
    backgroundOffset += 0.5f;
    if (backgroundOffset > TILE_SIZE) backgroundOffset = 0;
    
    // Gambar gradient background dari atas ke bawah
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
                          (Color){25, 25, 40, 255},     // Warna atas (biru gelap)
                          (Color){15, 15, 25, 255});    // Warna bawah (lebih gelap)
    
    // Gambar pattern blok mengambang dengan animasi
    for (int y = -1; y < SCREEN_HEIGHT / TILE_SIZE + 2; y++) {
        for (int x = -1; x < SCREEN_WIDTH / TILE_SIZE + 2; x++) {
            // Hanya gambar pada posisi tertentu untuk membuat pattern
            if ((x + y) % 4 == 0) {
                // Hitung alpha (transparansi) berdasarkan waktu dan posisi untuk animasi berkilau
                float alpha = 20 + 10 * sin(menuAnimTime * 0.8f + x * 0.5f + y * 0.3f);
                DrawRectangleRounded(
                    (Rectangle){
                        x * TILE_SIZE + backgroundOffset,    // Posisi dengan offset bergerak
                        y * TILE_SIZE + backgroundOffset,
                        TILE_SIZE * 0.8f,                    // 80% ukuran tile
                        TILE_SIZE * 0.8f
                    },
                    0.3f, 5, (Color){100, 100, 150, (int)alpha}  // Warna dengan alpha animasi
                );
            }
        }
    }
}

/**
 * Menggambar judul game dengan animasi bounce
 */
void DrawGameTitle() {
    // Hitung offset bounce berdasarkan sinus waktu
    titleBounce = sin(menuAnimTime * 2.0f) * 3.0f;
    
    // Gambar shadow judul untuk efek depth
    DrawText("TAP N BLAST", SCREEN_WIDTH/2 - 140 + 3, 80 + titleBounce + 3, 48, (Color){0, 0, 0, 120});
    
    // Gambar judul utama dengan efek gradient (disimulasikan dengan multiple draw)
    DrawText("TAP N BLAST", SCREEN_WIDTH/2 - 140, 80 + titleBounce, 48, (Color){255, 215, 0, 255}); // Gold
    DrawText("TAP N BLAST", SCREEN_WIDTH/2 - 140, 78 + titleBounce, 48, (Color){255, 255, 255, 180}); // White highlight
    
    // Gambar subtitle
    DrawText("Block Puzzle Game", SCREEN_WIDTH/2 - 85, 140, 18, (Color){200, 200, 200, 255});
}

/**
 * Menggambar menu utama lengkap dengan semua elemen
 */
void DrawMainMenu() {
    DrawMenuBackground();  // Background animasi
    DrawGameTitle();       // Judul dengan animasi
    
    // Update dan gambar semua tombol
    UpdateMenuButton(&playButton);
    UpdateMenuButton(&settingsButton);
    UpdateMenuButton(&leaderboardButton);
    UpdateMenuButton(&exitButton);
    
    DrawMenuButton(&playButton);
    DrawMenuButton(&settingsButton);
    DrawMenuButton(&leaderboardButton);
    DrawMenuButton(&exitButton);
    
    // Gambar info versi di pojok kanan bawah
    DrawText("v0.0.1", SCREEN_WIDTH - 60, SCREEN_HEIGHT - 25, 12, (Color){150, 150, 150, 255});
}

/**
 * Menggambar menu pengaturan dengan toggle sound dan music
 */
void DrawSettingsMenu() {
    DrawMenuBackground();
    
    // Judul settings
    DrawText("SETTINGS", SCREEN_WIDTH/2 - 80, 100, 36, (Color){255, 255, 255, 255});
    
    // Panel pengaturan
    DrawRectangleRounded((Rectangle){SCREEN_WIDTH/2 - 150, 200, 300, 200}, 0.1f, 10, (Color){40, 40, 60, 200});
    DrawRectangleRoundedLines((Rectangle){SCREEN_WIDTH/2 - 150, 200, 300, 200}, 0.1f, 10, (Color){100, 100, 150, 255});

    // Area klik untuk toggle sound dan music
    Rectangle soundRect = {SCREEN_WIDTH/2 - 60, 230, 160, 25};
    Rectangle musicRect = {SCREEN_WIDTH/2 - 60, 260, 160, 25};
    
    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, soundRect)) {
            isSoundOn = !isSoundOn;  // Toggle sound
        } else if (CheckCollisionPointRec(mouse, musicRect)) {
            isMusicOn = !isMusicOn;  // Toggle music

            // Kontrol playback music berdasarkan setting dan state
            if (!isMusicOn) {
                StopBacksound();
            } else {
                // Mainkan musik sesuai dengan state menu/game saat ini
                if (currentMenuState == MENU_MAIN) {
                    PlayBacksoundMenu();
                } else {
                    PlayBacksoundGame();
                }
            }
        }
    }

    // Label pengaturan
    DrawText("Sound", soundRect.x - 60, soundRect.y + 3, 20, WHITE);
    DrawText("Music", musicRect.x - 60, musicRect.y + 3, 20, WHITE);
    
    // Toggle switch untuk Sound
    DrawRectangle(soundRect.x + 140, soundRect.y + 2, 40, 20, (Color){100, 100, 100, 255}); // Background switch
    DrawRectangle(isSoundOn ? soundRect.x + 162 : soundRect.x + 142, soundRect.y + 4, 16, 16, isSoundOn ? GREEN : RED); // Toggle button

    // Toggle switch untuk Music
    DrawRectangle(musicRect.x + 140, musicRect.y + 2, 40, 20, (Color){100, 100, 100, 255}); // Background switch
    DrawRectangle(isMusicOn ? musicRect.x + 162 : musicRect.x + 142, musicRect.y + 4, 16, 16, isMusicOn ? GREEN : RED); // Toggle button

    // Tombol About dan Back
    UpdateMenuButton(&aboutButton);
    DrawMenuButton(&aboutButton);
    if (aboutButton.isPressed) {
        currentMenuState = MENU_ABOUT;
    }
    
    UpdateMenuButton(&backButton);
    DrawMenuButton(&backButton);
}

/**
 * Menggambar menu About dengan informasi game dan cara bermain
 */
void DrawAboutMenu() {
    DrawMenuBackground();

    DrawText("ABOUT", SCREEN_WIDTH/2 - 60, 100, 36, (Color){255, 255, 255, 255});

    // Panel konten about
    Rectangle contentBox = {SCREEN_WIDTH/2 - 250, 150, 500, 400};
    DrawRectangleRounded(contentBox, 0.1f, 10, (Color){30, 35, 50, 220});
    DrawRectangleRoundedLines(contentBox, 0.1f, 10, (Color){100, 120, 180, 255});

    // Judul game
    DrawText("TAP N BLAST", SCREEN_WIDTH/2 - 80, 180, 24, (Color){255, 215, 0, 255});

    // Deskripsi game
    int descY = 210;
    int lineHeight = 20;
    DrawText("adalah permainan puzzle blok yang menantang", SCREEN_WIDTH/2 - 185, descY, 16, WHITE);
    DrawText("pemain untuk menyusun blok secara strategis", SCREEN_WIDTH/2 - 175, descY + lineHeight, 16, WHITE);
    DrawText("untuk meraih skor setinggi mungkin.", SCREEN_WIDTH/2 - 130, descY + lineHeight * 2, 16, WHITE);
    
    // Cara bermain
    DrawText("HOW TO PLAY:", SCREEN_WIDTH/2 - 55, 300, 18, (Color){100, 255, 100, 255});
    
    int leftMargin = SCREEN_WIDTH/2 - 200;
    DrawText("- Pilih Blok yang tersedia", leftMargin, 330, 14, WHITE);
    DrawText("- Letakkan Blok di dalam grid", leftMargin, 350, 14, WHITE);
    DrawText("- Penuhkan satu baris grid dan dapatkan Skor!", leftMargin, 370, 14, WHITE);
    
    // Garis pemisah
    DrawLineEx((Vector2){SCREEN_WIDTH/2 - 200, 400}, (Vector2){SCREEN_WIDTH/2 + 200, 400}, 1, (Color){100, 100, 150, 100});
    
    // Credit
    DrawText("Made with Raylib, C, & love", SCREEN_WIDTH/2 - 100, 420, 14, (Color){150, 150, 150, 255});
    
    UpdateMenuButton(&backButton);
    DrawMenuButton(&backButton);
}

/**
 * Menggambar menu leaderboard dengan tabel skor tertinggi
 */
void DrawLeaderboardMenu() {
    DrawMenuBackground();

    DrawText("LEADERBOARD", SCREEN_WIDTH / 2 - 100, 80, 36, GOLD);

    // Setup tabel leaderboard
    int startX = SCREEN_WIDTH / 2 - 200;
    int startY = 130;
    int rowHeight = 30;  // Tinggi setiap baris

    // Panel tabel
    DrawRectangleRounded((Rectangle){startX, startY, 400, 320}, 0.1f, 10, (Color){40, 40, 60, 200});
    DrawRectangleRoundedLines((Rectangle){startX, startY, 400, 320}, 0.1f, 10, (Color){100, 100, 150, 255});

    // Header tabel
    DrawText("Rank", startX + 10, startY + 10, 18, WHITE);
    DrawText("Name", startX + 70, startY + 10, 18, WHITE);
    DrawText("Score", startX + 220, startY + 10, 18, WHITE);
    DrawText("Time", startX + 310, startY + 10, 18, WHITE);

    // Ambil data leaderboard
    LeaderboardEntry entries[MAX_LEADERBOARD_ENTRIES];
    int count = GetLeaderboardAsArray(entries, 10);  // Ambil 10 entry teratas

    // Tampilkan setiap entry leaderboard
    for (int i = 0; i < count; i++) {
        char rankStr[5];
        char scoreStr[12];
        char timeStr[12];

        // Format data untuk ditampilkan
        sprintf(rankStr, "%d.", i + 1);              // Ranking dengan titik
        sprintf(scoreStr, "%d", entries[i].score);   // Skor
        FormatDuration(entries[i].duration, timeStr, sizeof(timeStr));  // Waktu bermain

        int y = startY + 40 + i * rowHeight;  // Posisi Y untuk baris ini
        
        // Tampilkan data dalam kolom yang sesuai
        DrawText(rankStr, startX + 10, y, 16, WHITE);
        DrawText(entries[i].username, startX + 70, y, 16, WHITE);
        DrawText(scoreStr, startX + 220, y, 16, WHITE);
        DrawText(timeStr, startX + 310, y, 16, WHITE);
    }

    UpdateMenuButton(&backButton);
    DrawMenuButton(&backButton);
}

/**
 * Fungsi utama untuk update dan render menu
 * Menangani state menu dan input pengguna
 */
int UpdateMainMenu() {
    menuAnimTime += GetFrameTime();  // Update timer animasi

    switch (currentMenuState) {
        case MENU_MAIN:
            PlayBacksoundMenu();  // Pastikan musik menu berjalan
            DrawMainMenu();
            
            // Handle klik tombol di menu utama
            if (playButton.isPressed) {
                return 1; // Mulai permainan
            }
            if (settingsButton.isPressed) {
                currentMenuState = MENU_SETTINGS;
            }
            if (aboutButton.isPressed) {
                currentMenuState = MENU_ABOUT;
            }
            if (leaderboardButton.isPressed) {
                currentMenuState = MENU_LEADER;
            }
            if (exitButton.isPressed) {
                return -1; // Keluar dari program
            }
            break;
            
        case MENU_SETTINGS:
            DrawSettingsMenu();
            if (backButton.isPressed) {
                currentMenuState = MENU_MAIN;  // Kembali ke menu utama
            }
            break;
            
        case MENU_ABOUT:
            DrawAboutMenu();
            if (backButton.isPressed) {
                currentMenuState = MENU_SETTINGS;  // Kembali ke settings
            }
            break;
            
        case MENU_LEADER:
            DrawLeaderboardMenu();
            if (backButton.isPressed) {
                currentMenuState = MENU_MAIN;  // Kembali ke menu utama
            }
            break;
    }
    
    return 0; // Tetap di menu
}

/**
 * Fungsi untuk menggambar panel Game Over
 * Menampilkan overlay gelap, panel utama, skor, waktu bermain, 
 * input username, dan status high score
 */
void DrawGameOverPanel(UsernameInput* usernameInput, int score, int duration) {
    // Gambar overlay latar belakang semi-transparan untuk efek gelap
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 180});

    // Tentukan ukuran dan posisi panel utama di tengah layar
    int panelWidth = 400;
    int panelHeight = 300;
    int panelX = (SCREEN_WIDTH - panelWidth) / 2;  // Posisi horizontal tengah
    int panelY = (SCREEN_HEIGHT - panelHeight) / 2; // Posisi vertikal tengah

    // Gambar panel utama dengan sudut melengkung
    DrawRectangleRounded(
        (Rectangle){panelX, panelY, panelWidth, panelHeight},
        0.15f,  // Tingkat kelengkungan sudut (15%)
        10,     // Jumlah segmen untuk sudut halus
        (Color){40, 40, 60, 255}  // Warna biru gelap solid
    );

    // Gambar garis tepi panel untuk memberikan definisi visual
    DrawRectangleRoundedLines(
        (Rectangle){panelX, panelY, panelWidth, panelHeight},
        0.15f, 10, (Color){200, 200, 200, 255}  // Warna abu-abu terang
    );

    // Tampilkan judul "GAME OVER!" di bagian atas panel
    DrawText("GAME OVER!", panelX + (panelWidth/2) - 80, panelY + 30, 28, (Color){255, 100, 100, 255});

    // Format durasi waktu bermain menjadi string yang dapat dibaca
    char timeStr[16];
    FormatDuration(duration, timeStr, sizeof(timeStr));

    // Tampilkan skor final dan waktu bermain
    DrawText(TextFormat("Final Score: %d", score), panelX + 30, panelY + 80, 20, WHITE);
    DrawText(TextFormat("Time Played: %s", timeStr), panelX + 30, panelY + 110, 20, WHITE);

    // Cek apakah skor ini masuk dalam leaderboard
    int rank = GetLeaderboardRank(score);
    if (rank <= MAX_LEADERBOARD_ENTRIES) { 
        // Tampilkan pesan high score dengan warna emas jika masuk leaderboard
        DrawText(TextFormat("New High Score! Rank #%d", rank), panelX + 30, panelY + 140, 18, (Color){255, 215, 0, 255});
    }

    // Label untuk input username
    DrawText("Enter your name:", panelX + 30, panelY + 170, 18, WHITE);

    // Definisi kotak input username
    Rectangle inputBox = {panelX + 30, panelY + 195, panelWidth - 60, 30};
    
    // Gambar background kotak input dengan warna gelap
    DrawRectangleRounded(inputBox, 0.1f, 5, (Color){20, 20, 30, 255});
    
    // Gambar garis tepi kotak input
    DrawRectangleRoundedLines(inputBox, 0.1f, 5, WHITE);

    // Tampilkan teks yang sedang diketik user
    DrawText(usernameInput->inputText, inputBox.x + 5, inputBox.y + 5, 20, WHITE);

    // Animasi kursor berkedip saat input aktif
    if (usernameInput->isActive && ((GetTime() * 2) - (int)(GetTime() * 2)) > 0.5f) {
        int textWidth = MeasureText(usernameInput->inputText, 20);
        DrawText("_", inputBox.x + 5 + textWidth, inputBox.y + 5, 20, WHITE);
    }

    // Logika dan instruksi berdasarkan status input
    if (usernameInput->isActive) {
        // Handle tombol ENTER saat input aktif
        if (IsKeyPressed(KEY_ENTER)) {
            // Jika input kosong, gunakan "Anonymous" sebagai default
            if (usernameInput->currentLength == 0) {
                strcpy(usernameInput->inputText, "Anonymous");
                usernameInput->currentLength = strlen("Anonymous");
            }
            // Tandai input sudah selesai dan nonaktifkan
            usernameInput->isComplete = true;
            usernameInput->isActive = false;
        }
        // Tampilkan instruksi penggunaan
        DrawText("Press ENTER to confirm (or leave blank for Anonymous)", panelX + 30, panelY + 240, 14, (Color){200, 200, 200, 255});
        DrawText("Press BACKSPACE to delete", panelX + 30, panelY + 260, 14, (Color){200, 200, 200, 255});
    } else if (usernameInput->isComplete) {
        // Tampilkan konfirmasi bahwa skor telah disimpan
        DrawText("Score saved!", panelX + 30, panelY + 240, 16, (Color){100, 255, 100, 255});
    }
}

/**
 * Fungsi untuk menggambar panel "Next Blocks"
 * Menampilkan 3 blok berikutnya yang dapat dipilih pemain
 * Termasuk visual status (digunakan/kosong/dipilih) dan nomor slot
 */
void DrawNextBlocks(int selectedIndex, boolean* blockUsed){
    // Tentukan posisi panel di bawah grid game dan di tengah layar
    int panelWidth = 380;
    int panelHeight = 130;
    int panelX = (SCREEN_WIDTH - panelWidth) / 2;
    int panelY = gridOriginY + GRID_SIZE * TILE_SIZE + 20;  // 20px di bawah grid
    
    // Gambar panel background dengan sudut melengkung
    DrawRectangleRounded(
        (Rectangle){panelX, panelY, panelWidth, panelHeight},
        0.15f, 10, UI_PANEL  // Menggunakan konstanta warna UI
    );
    
    // Gambar garis tepi panel
    DrawRectangleRoundedLines(
        (Rectangle){panelX, panelY, panelWidth, panelHeight},
        0.15f, 10, UI_BORDER
    );
    
    // Tampilkan judul panel
    DrawText("NEXT BLOCKS", panelX + (panelWidth/2) - 60, panelY + 10, 16, UI_TEXT);
    
    // Konfigurasi ukuran dan spacing slot blok
    int slotSize = 80;      // Ukuran kotak slot
    int blockSpacing = 100; // Jarak antar slot
    int startX = panelX + (panelWidth - 3 * blockSpacing + (blockSpacing - slotSize)) / 2;  // Posisi awal untuk centering
    
    // Loop untuk menggambar 3 slot blok
    for (int i = 0; i < 3; i++) {
        int type = GetQueueAt(i);  // Ambil tipe blok dari queue
        int slotX = startX + i * blockSpacing;
        int slotY = panelY + 35;
        
        // Tentukan status slot
        boolean isUsed = blockUsed[i];    // Apakah blok sudah digunakan
        boolean isEmpty = (type == -1);   // Apakah slot kosong
        
        // Tentukan warna background slot berdasarkan status
        Color slotBg;
        if (isUsed) {
            slotBg = (Color){ 40, 40, 40, 200 };      // Abu gelap untuk yang sudah digunakan
        } else if (isEmpty) {
            slotBg = (Color){ 60, 60, 60, 200 };      // Abu sedang untuk slot kosong
        } else if (i == selectedIndex) {
            slotBg = (Color){ 100, 180, 255, 255 };   // Biru terang untuk yang dipilih
        } else {
            slotBg = (Color){ 80, 80, 80, 150 };      // Abu normal untuk slot tersedia
        }
        
        // Gambar background slot
        DrawRectangleRounded(
            (Rectangle){slotX, slotY, slotSize, slotSize},
            0.15f, 10, slotBg
        );
        
        // Tentukan warna border berdasarkan status
        Color borderColor;
        if (isUsed) {
            borderColor = (Color){60, 60, 60, 255};       // Abu gelap
        } else if (i == selectedIndex) {
            borderColor = (Color){255, 255, 255, 255};    // Putih untuk highlight
        } else {
            borderColor = (Color){120, 120, 120, 255};    // Abu normal
        }
        
        // Gambar border slot
        DrawRectangleRoundedLines(
            (Rectangle){slotX, slotY, slotSize, slotSize},
            0.15f, 10, borderColor
        );
        
        // Render konten slot berdasarkan status
        if (isUsed) {
            // Tampilkan teks "USED" untuk slot yang sudah digunakan
            int textWidth = MeasureText("USED", 12);
            DrawText("USED", 
                slotX + (slotSize - textWidth) / 2, 
                slotY + slotSize / 2 - 6, 
                12, (Color){100, 100, 100, 255}
            );
        } else if (type != -1) {
            // Gambar blok untuk slot yang berisi
            
            // Hitung bounding box blok untuk centering yang akurat
            int minX = 0, maxX = 0, minY = 0, maxY = 0;
            boolean firstTile = true;
            
            // Iterasi semua tile dalam blok untuk mencari batas
            for (int j = 0; j < MAX_BLOCK_SIZE; j++) {
                if (firstTile) {
                    // Inisialisasi dengan tile pertama
                    minX = maxX = blockShapes[type][j].x;
                    minY = maxY = blockShapes[type][j].y;
                    firstTile = false;
                } else {
                    // Update batas minimum dan maksimum
                    if (blockShapes[type][j].x < minX) minX = blockShapes[type][j].x;
                    if (blockShapes[type][j].x > maxX) maxX = blockShapes[type][j].x;
                    if (blockShapes[type][j].y < minY) minY = blockShapes[type][j].y;
                    if (blockShapes[type][j].y > maxY) maxY = blockShapes[type][j].y;
                }
            }
            
            // Hitung dimensi blok
            int blockWidth = (maxX - minX + 1);
            int blockHeight = (maxY - minY + 1);
            
            // Hitung skala yang pas untuk slot dengan margin 20px
            int availableSpace = slotSize - 20;
            float scaleX = (float)availableSpace / (blockWidth * TILE_SIZE);
            float scaleY = (float)availableSpace / (blockHeight * TILE_SIZE);
            float blockScale = (scaleX < scaleY) ? scaleX : scaleY;  // Ambil skala terkecil
            
            // Batasi skala maksimum agar tidak terlalu besar
            if (blockScale > 0.8f) blockScale = 0.8f;
            
            // Hitung posisi center untuk penempatan blok
            float scaledTileSize = TILE_SIZE * blockScale;
            int blockCenterX = slotX + slotSize / 2;
            int blockCenterY = slotY + slotSize / 2;
            
            // Hitung offset untuk centering berdasarkan bounding box
            float offsetX = -(minX + maxX) * scaledTileSize / 2;
            float offsetY = -(minY + maxY) * scaledTileSize / 2;
            
            // Gambar setiap tile dalam blok
            for (int j = 0; j < MAX_BLOCK_SIZE; j++) {
                float tileX = blockCenterX + offsetX + blockShapes[type][j].x * scaledTileSize;
                float tileY = blockCenterY + offsetY + blockShapes[type][j].y * scaledTileSize;
                
                // Gambar tile utama dengan warna blok
                DrawRectangleRounded(
                    (Rectangle){
                        tileX - scaledTileSize/2, 
                        tileY - scaledTileSize/2, 
                        scaledTileSize, 
                        scaledTileSize
                    },
                    0.3f, 1, blockColors[type-1]  // Warna sesuai tipe blok
                );
                
                // Tambahkan efek glossy/highlight untuk visual yang lebih menarik
                DrawRectangleRounded(
                    (Rectangle){
                        tileX - scaledTileSize/2 + scaledTileSize/4, 
                        tileY - scaledTileSize/2 + scaledTileSize/4, 
                        scaledTileSize/2, 
                        scaledTileSize/6
                    },
                    0.5f, 1, (Color){255, 255, 255, 80}  // Putih semi-transparan
                );
            }
        } else {
            // Tampilkan "EMPTY" untuk slot kosong
            int textWidth = MeasureText("EMPTY", 12);
            DrawText("EMPTY", 
                slotX + (slotSize - textWidth) / 2, 
                slotY + slotSize / 2 - 6, 
                12, (Color){150, 150, 150, 255}
            );
        }
        
        // Gambar nomor slot di bawah setiap slot
        Color numberColor;
        if (isUsed) {
            numberColor = (Color){80, 80, 80, 255};       // Abu gelap untuk yang digunakan
        } else if (type == -1) {
            numberColor = (Color){100, 100, 100, 255};    // Abu sedang untuk kosong
        } else {
            numberColor = UI_TEXT;                        // Warna normal untuk tersedia
        }
        
        // Format dan tampilkan nomor (1, 2, 3)
        char numberText[8];
        sprintf(numberText, "%d", i+1);
        int numberWidth = MeasureText(numberText, 22);
        DrawText(numberText, 
            slotX + (slotSize - numberWidth) / 2, 
            slotY + slotSize + 5, 
            22, numberColor
        );
    }
}

/**
 * Fungsi untuk menggambar bayangan/preview blok yang akan ditempatkan
 * Memberikan feedback visual kepada pemain tentang posisi dan validitas penempatan
 */
void DrawBlockShadow(int cursorX, int cursorY, int blockType) {
    // Validasi tipe blok
    if (blockType < 1 || blockType > 36) return;
    
    // Konversi posisi kursor ke koordinat grid
    int gridX = (int)((cursorX - gridOriginX) / TILE_SIZE);
    int gridY = (int)((cursorY - gridOriginY) / TILE_SIZE);
    
    // Cek apakah blok dapat ditempatkan di posisi ini
    boolean canPlace = CanPlaceBlock(gridX, gridY, blockType);
    
    // Gambar bayangan untuk setiap tile dalam blok
    for (int i = 0; i < MAX_BLOCK_SIZE; i++) {
        int bx = gridX + (int)blockShapes[blockType][i].x;  // Posisi X tile dalam grid
        int by = gridY + (int)blockShapes[blockType][i].y;  // Posisi Y tile dalam grid
        
        // Skip tile yang berada di luar batas grid
        if (bx < 0 || bx >= GRID_SIZE || by < 0 || by >= GRID_SIZE) continue;
        
        // Konversi kembali ke koordinat layar
        int screenX = gridOriginX + bx * TILE_SIZE;
        int screenY = gridOriginY + by * TILE_SIZE;
        
        // Tentukan warna bayangan berdasarkan apakah blok dapat ditempatkan
        Color shadowColor;
        Color borderColor;
        
        if (canPlace) {
            // Blok dapat ditempatkan - gunakan warna normal dengan transparansi
            shadowColor = blockColors[blockType-1];
            shadowColor.a = 180;  // Transparansi untuk efek bayangan
            borderColor = BLACK;
        } else {
            // Blok tidak dapat ditempatkan - gunakan warna merah peringatan
            shadowColor = (Color){255, 50, 50, 180};   // Merah transparan
            borderColor = (Color){200, 0, 0, 255};     // Merah gelap untuk border
        }
        
        // Gambar tile bayangan dengan sedikit offset untuk efek depth
        DrawRectangle(
            screenX + 1,  // Offset 1px untuk efek bayangan
            screenY + 1,
            TILE_SIZE - 4, TILE_SIZE - 4,  // Sedikit lebih kecil dari ukuran asli
            shadowColor
        );
        
        // Gambar border tile
        DrawRectangleLines(
            screenX, screenY,
            TILE_SIZE - 2, TILE_SIZE - 2,
            borderColor
        );
        
        if (canPlace) {
            // Tambahkan highlight putih di sudut untuk visual yang lebih baik
            DrawRectangle(screenX + 5, screenY + 5, 5, 5, WHITE);
        } else {
            // Gambar tanda X untuk menunjukkan tidak dapat ditempatkan
            DrawLine(screenX + 5, screenY + 5, 
                    screenX + TILE_SIZE - 7, screenY + TILE_SIZE - 7, 
                    (Color){255, 255, 255, 200});
            DrawLine(screenX + TILE_SIZE - 7, screenY + 5, 
                    screenX + 5, screenY + TILE_SIZE - 7, 
                    (Color){255, 255, 255, 200});
        }
    }
}

/**
 * Fungsi untuk menginisialisasi tombol-tombol pada menu pause
 * Mengatur posisi, warna, dan teks untuk setiap tombol
 */
void InitPauseMenuButtons() {
    // Inisialisasi tombol "Lanjutkan"
    resumeButton = (MenuButton){
        .rect       = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 40, 200, 50},  // Posisi dan ukuran
        .color      = (Color){70, 130, 180, 255},      // Steel Blue - warna normal
        .hoverColor = (Color){100, 149, 237, 255},     // Cornflower Blue - warna saat hover
        .textColor  = WHITE,                           // Warna teks
        .animScale  = 1.0f,                           // Skala animasi awal
        .text = "LANJUTKAN"                           // Teks tombol
    };

    // Inisialisasi tombol "Keluar ke Menu Utama"
    quitToMenuButton = (MenuButton){
        .rect       = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 30, 200, 50},  // Posisi di bawah tombol lanjutkan
        .color      = (Color){220, 20, 60, 255},       // Crimson - warna merah untuk aksi destruktif
        .hoverColor = (Color){255, 69, 100, 255},      // Merah lebih terang saat hover
        .textColor  = WHITE,                           // Warna teks putih
        .animScale  = 1.0f,                           // Skala animasi awal
        .text = "KELUAR"                              // Teks tombol
    };
}

/**
 * Fungsi untuk menggambar dan menangani menu pause
 * Menampilkan overlay, panel, tombol-tombol, dan mengembalikan aksi yang dipilih
 * 
 * @return int - 0: tetap di pause, 1: lanjutkan game, 2: keluar ke menu utama
 */
int DrawPauseMenu() {
    // Gambar overlay gelap semi-transparan di atas game
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 150});

    // Tentukan ukuran dan posisi panel pause di tengah layar
    int panelWidth = 300;
    int panelHeight = 200;
    int panelX = (SCREEN_WIDTH - panelWidth) / 2;
    int panelY = (SCREEN_HEIGHT - panelHeight) / 2;

    // Gambar panel utama dengan sudut melengkung
    DrawRectangleRounded(
        (Rectangle){panelX, panelY, panelWidth, panelHeight}, 
        0.15f, 10,                              // 15% kelengkungan, 10 segmen
        (Color){40, 40, 60, 255}               // Warna biru gelap
    );
    
    // Gambar border panel
    DrawRectangleRoundedLines(
        (Rectangle){panelX, panelY, panelWidth, panelHeight}, 
        0.15f, 10, 
        (Color){200, 200, 200, 255}            // Abu terang untuk border
    );

    // Tampilkan judul "PAUSE" di tengah bagian atas panel
    DrawText("PAUSE", 
            panelX + (panelWidth / 2) - MeasureText("PAUSE", 30) / 2,  // Center horizontal
            panelY + 30,                                               // 30px dari atas panel
            30,                                                        // Ukuran font
            WHITE);                                                    // Warna putih

    // Update status tombol-tombol (deteksi hover, klik, animasi)
    UpdateMenuButton(&resumeButton);
    UpdateMenuButton(&quitToMenuButton);

    // Gambar kedua tombol
    DrawMenuButton(&resumeButton);
    DrawMenuButton(&quitToMenuButton);

    // Handle aksi tombol dan kembalikan nilai sesuai
    if (resumeButton.isPressed) {
        return 1; // Lanjutkan permainan
    }
    if (quitToMenuButton.isPressed) {
        return 2; // Keluar ke menu utama (dan tampilkan game over input)
    }

    return 0; // Tetap di menu pause, tidak ada aksi
}