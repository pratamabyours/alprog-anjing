#include <iostream>
#include <string>
#include <cstring>
#include <iomanip>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

enum ModeAksi { VIEW_ALL, SEARCH_ITEM, NORMAL_RESPONSE };

void cetakBarisTabel(std::string id, std::string nama, std::string stok, std::string kategori) {
    std::cout << "| " << std::setw(6) << std::left << id 
              << "| " << std::setw(30) << std::left << nama 
              << "| " << std::setw(10) << std::left << stok 
              << "| " << std::setw(15) << std::left << kategori << " |\n";
}

void parseDanTampilkanTabel(std::string jsonResponse) {
    std::cout << "\n==================================================================\n";
    std::cout << "                   DAFTAR STOK GUDANG SEMBAKO                     \n";
    std::cout << "==================================================================\n";
    cetakBarisTabel("ID", "NAMA BARANG", "STOK", "KATEGORI");
    std::cout << "------------------------------------------------------------------\n";

    size_t pos = 0;
    bool adaData = false;

    while ((pos = jsonResponse.find("{\"id\":", pos)) != std::string::npos) {
        adaData = true;
        
        size_t id_start = jsonResponse.find("\"id\":\"", pos) + 6;
        size_t id_end = jsonResponse.find("\",", id_start);
        std::string id = jsonResponse.substr(id_start, id_end - id_start);

        size_t nama_start = jsonResponse.find("\"nama\":\"", pos) + 8;
        size_t nama_end = jsonResponse.find("\",", nama_start);
        std::string nama = jsonResponse.substr(nama_start, nama_end - nama_start);

        size_t stok_start = jsonResponse.find("\"stok\":", pos) + 7;
        size_t stok_end = jsonResponse.find(",", stok_start);
        if (stok_end == std::string::npos || stok_end > jsonResponse.find("}", stok_start)) {
            stok_end = jsonResponse.find("}", stok_start);
        }
        std::string stok = jsonResponse.substr(stok_start, stok_end - stok_start);

        std::string kategori = "-";
        size_t kat_start = jsonResponse.find("\"kategori\":\"", pos);
        if (kat_start != std::string::npos && kat_start < jsonResponse.find("}", pos)) {
            kat_start += 12;
            size_t kat_end = jsonResponse.find("\"", kat_start);
            kategori = jsonResponse.substr(kat_start, kat_end - kat_start);
        }

        cetakBarisTabel(id, nama, stok, kategori);
        pos = id_end;
    }

    if (!adaData) {
        std::cout << "|             Waduh, data gudang masih kosong!                    |\n";
    }
    std::cout << "==================================================================\n\n";
}

void parseDanTampilkanDetailItem(std::string jsonResponse) {
    if (jsonResponse.find("\"status\":\"NOT_FOUND\"") != std::string::npos) {
        std::cout << "\n[INFO]: Barang tidak ditemukan dalam sistem.\n\n";
        return;
    }

    size_t data_pos = jsonResponse.find("\"data\":");
    if (data_pos == std::string::npos) return;

    size_t id_start = jsonResponse.find("\"id\":\"", data_pos) + 6;
    size_t id_end = jsonResponse.find("\",", id_start);
    std::string id = jsonResponse.substr(id_start, id_end - id_start);

    size_t nama_start = jsonResponse.find("\"nama\":\"", data_pos) + 8;
    size_t nama_end = jsonResponse.find("\",", nama_start);
    std::string nama = jsonResponse.substr(nama_start, nama_end - nama_start);

    size_t stok_start = jsonResponse.find("\"stok\":", data_pos) + 7;
    size_t stok_end = jsonResponse.find(",", stok_start);
    if (stok_end == std::string::npos || stok_end > jsonResponse.find("}", stok_start)) {
        stok_end = jsonResponse.find("}", stok_start);
    }
    std::string stok = jsonResponse.substr(stok_start, stok_end - stok_start);

    std::string kategori = "-";
    size_t kat_start = jsonResponse.find("\"kategori\":\"", data_pos);
    if (kat_start != std::string::npos) {
        kat_start += 12;
        size_t kat_end = jsonResponse.find("\"", kat_start);
        kategori = jsonResponse.substr(kat_start, kat_end - kat_start);
    }

    std::cout << "\n========================================\n";
    std::cout << "         HASIL PENCARIAN BARANG         \n";
    std::cout << "========================================\n";
    std::cout << " ID Barang : " << id << "\n";
    std::cout << " Nama      : " << nama << "\n";
    std::cout << " Stok      : " << stok << "\n";
    std::cout << " Kategori  : " << kategori << "\n";
    std::cout << "========================================\n\n";
}

void sendRequest(const std::string& jsonRequest, ModeAksi mode) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock initialization failed\n";
        return;
    }

    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;
    char buffer[2048] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Socket creation error\n";
        WSACleanup();
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        std::cerr << "Koneksi ke server gagal!\n\n";
        closesocket(sock);
        WSACleanup();
        return;
    }

    send(sock, jsonRequest.c_str(), jsonRequest.length(), 0);

    recv(sock, buffer, 2048, 0);
    std::string response(buffer);

    if (mode == VIEW_ALL) {
        parseDanTampilkanTabel(response);
    } else if (mode == SEARCH_ITEM) {
        parseDanTampilkanDetailItem(response);
    } else {
        if (response.find("\"status\":\"SUCCESS\"") != std::string::npos) {
            size_t msg_start = response.find("\"message\":\"") + 11;
            size_t msg_end = response.find("\"}", msg_start);
            std::cout << "\n[SERVER RESPONSE]: " << response.substr(msg_start, msg_end - msg_start) << "\n\n";
        } else {
            std::cout << "\n[SERVER RESPONSE]: " << response << "\n\n";
        }
    }

    closesocket(sock);
    WSACleanup();
}

int main() {
    int pilihan;
    std::string targetId;

    do {
        std::cout << "=== MENU UTAMA SYSTEM ===\n";
        std::cout << "1. Lihat Semua Data\n";
        std::cout << "2. Urutkan Data Berdasarkan Stok\n";
        std::cout << "3. Cari Data Berdasarkan ID\n";
        std::cout << "4. Keluar\n";
        std::cout << "Pilih menu: ";
        std::cin >> pilihan;

        switch (pilihan) {
            case 1:
                sendRequest("{\"action\":\"VIEW\"}", VIEW_ALL);
                break;
            case 2:
                sendRequest("{\"action\":\"SORT\"}", NORMAL_RESPONSE);
                break;
            case 3:
                std::cout << "Masukkan ID Barang: ";
                std::cin >> targetId;
                sendRequest("{\"action\":\"SEARCH\",\"target_id\":\"" + targetId + "\"}", SEARCH_ITEM);
                break;
            case 4:
                std::cout << "Keluar dari program.\n";
                break;
            default:
                std::cout << "Pilihan tidak valid.\n\n";
        }
    } while (pilihan != 4);

    return 0;
}