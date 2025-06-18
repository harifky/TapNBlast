# Tugas Besar SDA
## TapNBlast
### Kelompok 3 RIFAFA

Tugas Besar ini merupakan Tugas Akhir dari Mata kuliah SDA untuk menerapkan materi-materi yang diperoleh selama perkuliahan.
Pada Tugas ini kami memilih topik sendiri dimana yang kami buat adalah Program Aplikasi Game berjudul "Tap N Blast". Kelompok kami beranggotakan 3 orang yaitu:

- Muhammad Faiz Ramdhani (051)
- Muhammad Naufal Alfarizky (052)
- Rifky Hermawan (060)

## Deskripsi Program

Tap N Blast adalah permainan puzzle berbasis grid 2D yang mengadaptasi gameplay dari Block Blast. Pemain diberikan tiga balok acak yang harus ditempatkan ke dalam grid statis untuk membentuk garis lurus horizontal atau vertikal agar dapat dihancurkan dan memperoleh skor. Permainan ini menuntut strategi, ketelitian, dan manajemen ruang, dengan sistem yang memanfaatkan berbagai struktur data seperti queue, stack, tree, linked list, dan array untuk memastikan mekanisme berjalan dengan optimal dan efisien

## Instalasi

Sebelum melakukan instalasi, pastikan Anda telah menginstal raylib di komputer Anda. Jika belum, silakan mengunduhnya dari [situs resminya](https://https://www.raylib.com/) 

### Instalasi untuk menggunakan VSCode

Untuk memainkan game ini dapat dengan mengunduh file main.exe nya langsung atau dengan melakukan clone pada repositori ini.

Berikut langkah-langkah jika ingin melakukan clone pada repositori ini:
* copy link repositori berikut [Repositori-TapNBlast](https://github.com/harifky/TapNBlast.git)
* buka file mananger
* cari dan pilih folder yang akan digunakan untuk menyimpan hasil clone repositori ini
* buka folder yang telah dipilih menggunakan git bash atau cmd
* ketikkan git clone "link-repositori"
* tunggu hingga selesai
* Jika sudah selesai buka folder repositori menggunakan VS Code
* Lakukan compile dengan menekan win + shift + B
* Setelah selesai maka file main.exe akan muncul didalam forlder TapNBlast
* Ketikkan ./main.exe pada cmd atau double klik pada file main.exe di file manager

### Instalasi untuk menggunakan DevC++

Jika ingin menggunakan DevC++ maka dapat dengan mengikuti langkah-langkah berikut:
* Download file zip yang berisi file project untuk Devc++ dari link berikut [Folder-Aplikasi](https://bit.ly/4kJlSN7)
* Ekstrak file zip tersebut
* Buka file project yang telah di ekstrak menggunakan DevC++
* Lakukan konfigurasi raylib dengan mengikuti langkah-langkah berikut:
    * Klik menu Project > Project Option > Parameters kemudian isi bagian linker dengan -lraylib -lopengl32 -lgdi32 -lwinmm
    * Klik menu Tools > Compiler Options > Libraries Tambahkan path menuju folder lib di tempat raylib kalian disimpan. Contoh C:\raylib\w64devkit\x86_64-w64-mingw32\lib
    * * Klik menu Tools > Compiler Options > C Includes Tambahkan path menuju folder include di tempat raylib kalian disimpan. Contoh C:\raylib\w64devkit\x86_64-w64-mingw32\include
* Lakukan compile
* Jika sudah selesai maka file main.exe akan muncul didalam forlder TapNBlast


## Cara Bermain

### Tujuan

Menyusun blok yang diberikan pada area grid berukuran 9x9 agar dapat memenuhi satu baris/kolom. Baris/kolom yang penuh akan meledak, dan akan memberikan skor. Permainan akan berakhir (gameover) jika blok yang diberikan tidak dapat diletakkan lagi didalam grid. Dapatkan Skor setinggi-tingginya agar kamu dapat berada pada top leaderboard!

### Control Permainan

Untuk memainkan permainan ini kita dapat menggunakan:

* memilih 1 dari 3 blok yang diberikan menggunakan tombol angka "1", "2", atau "3"
* Meletakkan Blok dengan cara mengarahkan dan klik kiri menggunakan mouse
* melakukan Undo aksi dengan menggunakan tombol "Z"
* melakukan Jeda Permain dengan menggunakan tombol "P"
