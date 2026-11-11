#include <iostream>
#include <string>
using namespace std;



struct NodeAntrian {
    string data;
    NodeAntrian *next;
};

struct Antrian {
    NodeAntrian *head;
    NodeAntrian *tail;
};

struct Menu {
    string nama;
    string kategori;
    int harga;
    bool tersedia;
    Antrian antrian;
    Menu *left;
    Menu *right;
};

enum JenisAksi { PESAN, UPDATE }; //Membedakan jenis aksi yang dilakukan oleh pengguna

struct Aksi {
    JenisAksi jenis;
    string menu;
    string pelanggan;   
    bool masukWaiting;  
    Aksi *next;
};

Menu *root = NULL;
Aksi *top = NULL;

//Mengososngkan antrian baru
void initQueue(Antrian &antrian) {
    antrian.head = NULL;
    antrian.tail = NULL;
}

//Mengecek apakah antrian kosong
bool queueKosong(Antrian &antrian) {
    return antrian.head == NULL;
}

//Memasukkan pelanggan ke belakang antrian
void enqueue(Antrian &antrian, const string &nama) {
    NodeAntrian *baru = new NodeAntrian{nama, NULL};
    if (queueKosong(antrian)) {
        antrian.head = antrian.tail = baru;
    } else {
        antrian.tail->next = baru;
        antrian.tail = baru;
    }
}

//Memasukkan pelanggan ke depan antrian
void enqueueFront(Antrian &antrian, const string &nama) {
    NodeAntrian *baru = new NodeAntrian{nama, antrian.head};
    antrian.head = baru;
    if (antrian.tail == NULL) antrian.tail = baru;
}

//Mengeluarkan pelanggan dari antrian paling depan
string dequeue(Antrian &antrian) {
    NodeAntrian *depan = antrian.head;
    string nama = depan->data;
    antrian.head = antrian.head->next;
    if (antrian.head == NULL) antrian.tail = NULL;
    delete depan;
    return nama;
}

//Menghapus pelanggan dari belakang
void removeLast(Antrian &antrian) {
    if (queueKosong(antrian)) return;
    if (antrian.head == antrian.tail) {
        delete antrian.head;
        antrian.head = antrian.tail = NULL;
        return;
    }
    NodeAntrian *last = antrian.head;
    while (last->next != antrian.tail) last = last->next;
    delete antrian.tail;
    antrian.tail = last;
    antrian.tail->next = NULL;
}

//Mengosongkan seluruh antrian
void clearQueue(Antrian &antrian) {
    while (!queueKosong(antrian)) dequeue(antrian);
}
 
Menu *cari(Menu *node, const string &nama) {
    if (node == NULL) return NULL;
    if (nama == node->nama) return node;
    if (nama < node->nama) return cari(node->left, nama);
    return cari(node->right, nama);
}
 
Menu *tambah(Menu *node, Menu *baru) {
    if (node == NULL) return baru;
    if (baru->nama < node->nama) node->left = tambah(node->left, baru);
    else node->right = tambah(node->right, baru);
    return node;
}

//Mencari nama menu paling awal secara alfabetis
Menu *minNode(Menu *node) {
    while (node->left != NULL) node = node->left;
    return node;
}
 
Menu *hapus(Menu *node, const string &nama) {
    if (node == NULL) return NULL;
    if (nama < node->nama) node->left = hapus(node->left, nama);
    else if (nama > node->nama) node->right = hapus(node->right, nama);
    else {
        if (node->left == NULL || node->right == NULL) {
            Menu *anak = (node->left != NULL) ? node->left : node->right;
            clearQueue(node->antrian);
            delete node;
            return anak;
        }
        Menu *successor = minNode(node->right);
        node->nama = successor->nama;
        node->kategori = successor->kategori;
        node->harga = successor->harga;
        node->tersedia = successor->tersedia;

        Antrian temp = node->antrian;
        node->antrian = successor->antrian;
        successor->antrian = temp;
        node->right = hapus(node->right, successor->nama);
    }
    return node;
}

//Menampilakan seluruh menu secara inorder
void tampilInorder(Menu *node, int &no) {
    if (node == NULL) return;
    tampilInorder(node->left, no);
    cout << no++ << ". " << node->nama << " | " << node->kategori
         << " | Rp" << node->harga << " | "
         << (node->tersedia ? "Tersedia" : "Habis") << endl;
    tampilInorder(node->right, no);
}
 
void push(JenisAksi jenis, const string &menu, const string &pelanggan, bool waiting) {
    top = new Aksi{jenis, menu, pelanggan, waiting, top};
}
 
//Fitur
void tambahMenu() {
    string nama, kategori;
    int harga;
    cout << "Nama menu     : "; getline(cin, nama);
    if (cari(root, nama) != NULL) {
        cout << "Menu sudah ada" << endl;
        return;
    }
    cout << "Kategori      : "; getline(cin, kategori);
    cout << "Harga         : "; cin >> harga;
    cin.ignore(1000, '\n');
 
    Menu *menu = new Menu;
    menu->nama = nama;
    menu->kategori = kategori;
    menu->harga = harga;
    menu->tersedia = true;
    initQueue(menu->antrian);
    menu->left = menu->right = NULL;
    root = tambah(root, menu);
    cout << "Menu berhasil ditambahkan" << endl;
}
 
void lihatMenu() {
    if (root == NULL) {
        cout << "Belum ada menu" << endl;
        return;
    }
    int no = 1;
    tampilInorder(root, no);
}
 
void catatPesanan() {
    string pelanggan, nama;
    cout << "Nama pelanggan: "; getline(cin, pelanggan);
    cout << "Nama menu     : "; getline(cin, nama);
    Menu *menu = cari(root, nama);
    if (menu == NULL) {
        cout << "Menu tidak ditemukan" << endl;
        return;
    }
    if (menu->tersedia) {
        cout << "Pesanan " << pelanggan << " berhasil dibuat" << endl;
        push(PESAN, menu->nama, pelanggan, false);
    } else {
        enqueue(menu->antrian, pelanggan);
        cout << "Menu sedang habis, " << pelanggan << " masuk ke dalam waiting list" << endl;
        push(PESAN, menu->nama, pelanggan, true);
    }
}
 
void menuTersediaKembali() {
    string nama;
    cout << "Nama menu     : "; getline(cin, nama);
    Menu *menu = cari(root, nama);
    if (menu == NULL) {
        cout << "Menu tidak ditemukan" << endl;
        return;
    }
    if (menu->tersedia) {
        cout << "Menu sudah tersedia" << endl;
        return;
    }
    menu->tersedia = true;
    cout << "Menu tersedia kembali" << endl;
    string pelanggan = "";
    if (!queueKosong(menu->antrian)) {
        pelanggan = dequeue(menu->antrian);
        cout << "Pesanan langsung diberikan kepada " << pelanggan << " dari waiting list" << endl;
    }
    push(UPDATE, menu->nama, pelanggan, false);
}
 
void menuHabis() {
    string nama;
    cout << "Nama menu     : "; getline(cin, nama);
    Menu *menu = cari(root, nama);
    if (menu == NULL) {
        cout << "Menu tidak ditemukan" << endl;
        return;
    }
    if (!menu->tersedia) {
        cout << "Menu sudah habis" << endl;
        return;
    }
    menu->tersedia = false;
    cout << "Menu " << menu->nama << " ditandai habis" << endl;
}
 
void hapusMenu() {
    string nama;
    cout << "Nama menu     : "; getline(cin, nama);
    if (cari(root, nama) == NULL) {
        cout << "Menu tidak ditemukan" << endl;
        return;
    }
    root = hapus(root, nama);
    cout << "Menu berhasil dihapus" << endl;
}
 
void undo() {
    if (top == NULL) {
        cout << "Tidak ada aksi untuk di-undo" << endl;
        return;
    }
    Aksi *last = top;
    top = top->next;
    Menu *menu = cari(root, last->menu);
 
    if (last->jenis == PESAN) {
        if (last->masukWaiting && menu != NULL) removeLast(menu->antrian);
        cout << "Undo: Pesanan menu " << last->menu << " dibatalkan" << endl;
    } else {
        if (menu != NULL) {
            menu->tersedia = false;
            if (last->pelanggan != "") enqueueFront(menu->antrian, last->pelanggan);
        }
        cout << "Undo: Menu " << last->menu << " kembali habis" << endl;
    }
    delete last;
}
 
void tampilAntrian() {
    string nama;
    cout << "Nama menu     : "; getline(cin, nama);
    Menu *menu = cari(root, nama);
    if (menu == NULL) {
        cout << "Menu tidak ditemukan" << endl;
        return;
    }
    if (queueKosong(menu->antrian)) {
        cout << "Antrian kosong" << endl;
        return;
    }
    for (NodeAntrian *pelanggan = menu->antrian.head; pelanggan != NULL; pelanggan = pelanggan->next)
        cout << pelanggan->data << " menunggu menu " << menu->nama << endl;
}
int main() {
    int pilih;
    do {
        cout << "\n===== RESTORAN KELUARGA =====" << endl;
        cout << "1. Tambah menu" << endl;
        cout << "2. Lihat daftar menu" << endl;
        cout << "3. Catat pesanan" << endl;
        cout << "4. Menu tersedia kembali" << endl;
        cout << "5. Hapus menu" << endl;
        cout << "6. Undo" << endl;
        cout << "7. Tampilkan antrian" << endl;
        cout << "8. Tandai menu habis" << endl;
        cout << "0. Keluar" << endl;
        cout << "Pilih: ";
        cin >> pilih;
        cin.ignore(1000, '\n');
        cout << endl;
 
        switch (pilih) {
            case 1: tambahMenu(); break;
            case 2: lihatMenu(); break;
            case 3: catatPesanan(); break;
            case 4: menuTersediaKembali(); break;
            case 5: hapusMenu(); break;
            case 6: undo(); break;
            case 7: tampilAntrian(); break;
            case 8: menuHabis(); break;
            case 0: cout << "Terima kasih" << endl; break;
            default: cout << "Pilihan tidak valid" << endl;
        }
    } while (pilih != 0);
}
 