#include <iostream>
#include <string>
#include <cstring>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

class BarangSembako {
private: 
    std::string id;
    std::string nama;
    int stok;

public:
    BarangSembako(std::string id, std::string nama, int stok) : id(id), nama(nama), stok(stok) {}
    virtual ~BarangSembako() {}

    std::string getId() const { return id; }
    std::string getNama() const { return nama; }
    int getStok() const { return stok; }
    void setStok(int s) { stok = s; }

    virtual std::string getKategori() const = 0;
    virtual std::string toJSON() const {
        return "{\"id\":\"" + id + "\",\"nama\":\"" + nama + "\",\"stok\":" + std::to_string(stok) + "}";
    }
};

class SembakoKering : public BarangSembako {
public:
    SembakoKering(std::string id, std::string nama, int stok) : BarangSembako(id, nama, stok) {}
    
    std::string getKategori() const override { return "Kering"; }
    std::string toJSON() const override {
        return "{\"id\":\"" + getId() + "\",\"nama\":\"" + getNama() + "\",\"stok\":" + std::to_string(getStok()) + ",\"kategori\":\"Kering\"}";
    }
};

class SembakoBasah : public BarangSembako {
public:
    SembakoBasah(std::string id, std::string nama, int stok) : BarangSembako(id, nama, stok) {}
    
    std::string getKategori() const override { return "Basah/Segar"; }
    std::string toJSON() const override {
        return "{\"id\":\"" + getId() + "\",\"nama\":\"" + getNama() + "\",\"stok\":" + std::to_string(getStok()) + ",\"kategori\":\"Basah/Segar\"}";
    }
};

struct Node {
    BarangSembako* data;
    Node* next;
    Node(BarangSembako* barang) : data(barang), next(nullptr) {}
};

class CustomLinkedList {
private:
    Node* head;
    int size;

public:
    CustomLinkedList() : head(nullptr), size(0) {}
    ~CustomLinkedList() {
        Node* current = head;
        while (current != nullptr) {
            Node* nextNode = current->next;
            delete current->data;
            delete current;
            current = nextNode;
        }
    }

    void insert(BarangSembako* barang) {
        Node* newNode = new Node(barang);
        if (!head) {
            head = newNode;
        } else {
            Node* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newNode;
        }
        size++;
    }

    int getSize() const { return size; }
    Node* getHead() const { return head; }

    void clearListOnly() { head = nullptr; size = 0; }
};

/* * ANALISIS KOMPLEKSITAS ALGORITMA SEARCHING (Linear Search):
 * - Time Complexity: O(n) karena algoritma memindai setiap node pada linked list satu per satu dari awal hingga akhir untuk menemukan target_id.
 * - Space Complexity: O(1) karena pencarian dilakukan secara in-place tanpa alokasi memori tambahan yang bergantung pada ukuran struktur data.
 */
BarangSembako* linearSearchById(CustomLinkedList& list, const std::string& targetId) {
    Node* current = list.getHead();
    while (current != nullptr) {
        if (current->data->getId() == targetId) {
            return current->data;
        }
        current = current->next;
    }
    return nullptr;
}

int partition(BarangSembako* arr[], int low, int high) {
    int pivot = arr[high]->getStok();
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (arr[j]->getStok() < pivot) {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return (i + 1);
}

/* * ANALISIS KOMPLEKSITAS ALGORITMA SORTING (Quick Sort):
 * - Time Complexity: Average Case O(n log n) ketika pivot membagi array secara seimbang. Worst Case O(n^2) jika elemen sudah terurut atau pemilihan pivot buruk.
 * - Space Complexity: O(log n) karena menggunakan pendekatan rekursif divide and conquer yang memakan ruang pada call stack memori.
 */
void quickSort(BarangSembako* arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

void sortWarehouseItems(CustomLinkedList& list) {
    int n = list.getSize();
    if (n <= 1) return;

    BarangSembako** arr = new BarangSembako*[n];
    Node* current = list.getHead();
    for (int i = 0; i < n; i++) {
        arr[i] = current->data;
        current = current->next;
    }

    quickSort(arr, 0, n - 1);

    list.clearListOnly();
    for (int i = 0; i < n; i++) {
        list.insert(arr[i]);
    }
    delete[] arr;
}

std::string serializeAll(CustomLinkedList& list) {
    std::string jsonStr = "{\"items\":[";
    Node* current = list.getHead();
    while (current != nullptr) {
        jsonStr += current->data->toJSON();
        if (current->next != nullptr) jsonStr += ",";
        current = current->next;
    }
    jsonStr += "]}";
    return jsonStr;
}

int main() {
    CustomLinkedList gudangSembako;
    
    gudangSembako.insert(new SembakoKering("S01", "Beras Pandan Wangi 5kg", 45));
    gudangSembako.insert(new SembakoKering("S02", "Minyak Goreng Bimoli 2L", 12));
    gudangSembako.insert(new SembakoBasah("S03", "Telur Ayam Negeri 1kg", 60));
    gudangSembako.insert(new SembakoKering("S04", "Gula Pasir Gulaku 1kg", 8));
    gudangSembako.insert(new SembakoKering("S05", "Garam Dapur Refina 250g", 22));
    gudangSembako.insert(new SembakoKering("S06", "Kecap Manis Bango 520ml", 35));
    gudangSembako.insert(new SembakoKering("S07", "Penyedap Royco Sapi Dus", 50));
    gudangSembako.insert(new SembakoKering("S08", "Tepung Terigu Segitiga 1kg", 18));

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock initialization failed\n";
        return -1;
    }

    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Socket failed\n";
        WSACleanup();
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }
    if (listen(server_fd, 3) == SOCKET_ERROR) {
        std::cerr << "Listen failed\n";
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }

    std::cout << "[SERVER] Gudang Toko Sembako Berjalan di Port 8080...\n";

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) == INVALID_SOCKET) {
            std::cerr << "Accept failed\n";
            continue;
        }

        memset(buffer, 0, 1024);
        recv(new_socket, buffer, 1024, 0);
        std::string request(buffer);
        std::cout << "[SERVER] Perintah Masuk: " << request << "\n";

        std::string response = "";

        if (request.find("\"action\":\"VIEW\"") != std::string::npos) {
            response = serializeAll(gudangSembako);
        } 
        else if (request.find("\"action\":\"SORT\"") != std::string::npos) {
            sortWarehouseItems(gudangSembako);
            response = "{\"status\":\"SUCCESS\",\"message\":\"Stok sembako berhasil diurutkan dari yang tersedikit.\"}";
        } 
        else if (request.find("\"action\":\"SEARCH\"") != std::string::npos) {
            size_t id_pos = request.find("\"target_id\":\"");
            if (id_pos != std::string::npos) {
                std::string targetId = request.substr(id_pos + 13, 3);
                BarangSembako* found = linearSearchById(gudangSembako, targetId);
                if (found) {
                    response = "{\"status\":\"FOUND\",\"data\":" + found->toJSON() + "}";
                } else {
                    response = "{\"status\":\"NOT_FOUND\",\"message\":\"Barang Sembako tidak ditemukan\"}";
                }
            }
        } else {
            response = "{\"status\":\"ERROR\",\"message\":\"Aksi ditolak\"}";
        }

        send(new_socket, response.c_str(), response.length(), 0);
        std::cout << "[SERVER] Respon terkirim.\n\n";
        closesocket(new_socket);
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}