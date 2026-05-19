#include <iostream>
#include <string>

using namespace std;

// ==========================================
// 1. ABSTRAKSI (Abstract Class) & ENKAPSULASI
// ==========================================
class ItemPustaka {
protected: 
    // Enkapsulasi: Data di-protect agar tidak diubah sembarangan
    int id_item;
    string judul;
    int stok;

public:
    ItemPustaka(int _id, string _judul, int _stok) 
        : id_item(_id), judul(_judul), stok(_stok) {}
    
    virtual ~ItemPustaka() {}

    // Getter & Setter (Enkapsulasi)
    int getId() const { return id_item; }
    string getJudul() const { return judul; }
    int getStok() const { return stok; }
    void kurangiStok() { if(stok > 0) stok--; }

    // Polimorfisme: Pure Virtual Function (Wajib di-override)
    virtual void tampilkanInfo() const = 0;
    virtual string toJson() const = 0; 
};

// ==========================================
// 2. PEWARISAN (Inheritance) & POLIMORFISME
// ==========================================
class BukuTeks : public ItemPustaka {
private:
    string isbn;

public:
    BukuTeks(int _id, string _judul, int _stok, string _isbn)
        : ItemPustaka(_id, _judul, _stok), isbn(_isbn) {}

    // Polimorfisme: Method Overriding
    void tampilkanInfo() const override {
        cout << "[Buku Teks] ID: " << id_item << " | Judul: " << judul 
             << " | Stok: " << stok << " | ISBN: " << isbn << "\n";
    }

    // Simulasi pembuatan format JSON untuk dikirim via Socket
    string toJson() const override {
        return "{\"tipe\":\"buku\", \"id\":" + to_string(id_item) + 
               ", \"judul\":\"" + judul + "\", \"stok\":" + to_string(stok) + "}";
    }
};

class JurnalAkademik : public ItemPustaka {
private:
    string doi; // Digital Object Identifier

public:
    JurnalAkademik(int _id, string _judul, int _stok, string _doi)
        : ItemPustaka(_id, _judul, _stok), doi(_doi) {}

    void tampilkanInfo() const override {
        cout << "[Jurnal] ID: " << id_item << " | Judul: " << judul 
             << " | Stok: " << stok << " | DOI: " << doi << "\n";
    }

    string toJson() const override {
        return "{\"tipe\":\"jurnal\", \"id\":" + to_string(id_item) + 
               ", \"judul\":\"" + judul + "\", \"stok\":" + to_string(stok) + "}";
    }
};

// ==========================================
// 3. STRUKTUR DATA MANUAL (Bonus +10%)
// ==========================================
// Node untuk Single Linked List
struct Node {
    ItemPustaka* data;
    Node* next;
    
    Node(ItemPustaka* _data) : data(_data), next(nullptr) {}
};

class InventarisPerpustakaan {
private:
    Node* head;

public:
    InventarisPerpustakaan() : head(nullptr) {}

    // Tambah data di awal (Push Front)
    void tambahItem(ItemPustaka* itemBaru) {
        Node* newNode = new Node(itemBaru);
        newNode->next = head;
        head = newNode;
    }

    // Tampilkan semua isi Linked List
    void lihatSemuaKoleksi() const {
        Node* temp = head;
        cout << "--- Daftar Inventaris ---\n";
        while (temp != nullptr) {
            temp->data->tampilkanInfo(); // Memanggil method polimorfik
            temp = temp->next;
        }
        cout << "-------------------------\n";
    }

    // ==========================================
    // 4. ALGORITMA SEARCHING MANUAL (Linear Search)
    // ==========================================
    ItemPustaka* cariBerdasarkanId(int targetId) {
        Node* temp = head;
        while (temp != nullptr) {
            if (temp->data->getId() == targetId) {
                return temp->data; // Ketemu!
            }
            temp = temp->next;
        }
        return nullptr; // Tidak ketemu
    }
    
    // Catatan untuk tim: Tambahkan method MergeSort() di sini nanti
    // void urutkanBerdasarkanJudul() { ... }
};

// ==========================================
// FUNGSI MAIN (Simulasi Eksekusi)
// ==========================================
int main() {
    InventarisPerpustakaan serverPerpus;

    // 1. Memasukkan data ke Linked List
    serverPerpus.tambahItem(new BukuTeks(101, "Kalkulus Edisi 9", 5, "978-12345"));
    serverPerpus.tambahItem(new BukuTeks(102, "Aljabar Linear", 3, "978-67890"));
    serverPerpus.tambahItem(new JurnalAkademik(201, "IEEE Microplastic Reduction", 2, "10.1109/XX"));

    // 2. Menampilkan Data (Membuktikan Polimorfisme berjalan)
    serverPerpus.lihatSemuaKoleksi();

    // 3. Simulasi Client mencari buku via Algoritma Searching
    int idDicari = 101;
    cout << "\n[Sistem] Mencari ID " << idDicari << "...\n";
    ItemPustaka* hasilCari = serverPerpus.cariBerdasarkanId(idDicari);

    if (hasilCari != nullptr) {
        cout << "[Ditemukan] Info: \n";
        hasilCari->tampilkanInfo();
        
        // 4. Simulasi generate JSON untuk dikirim ke Client via Socket
        cout << "\n[Network] Data JSON yang akan dikirim ke Client:\n";
        cout << hasilCari->toJson() << "\n";
    } else {
        cout << "[Gagal] Buku dengan ID tersebut tidak ditemukan.\n";
    }

    return 0;
}