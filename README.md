# Proyek Akhir Algoritma dan Pemrograman - Sistem Manajemen Gudang Sembako

Proyek ini mengimplementasikan sistem manajemen inventaris gudang toko sembako berbasis Client-Server secara real-time.

## Panduan Kompilasi dan Eksekusi (Windows / WinSock2)

Karena program ini dikembangkan menggunakan pustaka jaringan Windows (`winsock2.h`), proses kompilasi wajib menghubungkan library `-lws2_32` agar socket dapat bekerja dengan baik.

Buka **VS Code** dan gunakan fitur **Split Terminal** untuk menjalankan Server dan Client secara bersamaan.

### Langkah 1: Jalankan Server
Buka terminal pertama (untuk Server), lakukan kompilasi, dan eksekusi file executable server:
```bash
g++ -o server server.cpp -lws2_32
.\server.exe
```

### Langkah 2: Jalankan Client
Buka terminal kedua (untuk Client), lakukan kompilasi, dan eksekusi file executable client:
```bash
g++ -o client client.cpp -lws2_32
.\client.exe
```
