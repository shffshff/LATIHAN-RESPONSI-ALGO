#include <iostream>
#include <stack>
#include <string>
#include <iomanip>
using namespace std;

struct PelakuKorupsi {
    string nama;
    string jabatan;
    long long nominal;
    PelakuKorupsi *left, *right;
};

struct RiwayatAksi {
    string jenisAksi; // "tambah" atau "hapus"
    PelakuKorupsi* pelaku;
};

stack<RiwayatAksi> riwayatUndo;

PelakuKorupsi* buatPelakuBaru(string nama, string jabatan, long long nominal) {
    PelakuKorupsi* baru = new PelakuKorupsi;
    baru->nama = nama;
    baru->jabatan = jabatan;
    baru->nominal = nominal;
    baru->left = baru->right = nullptr;
    return baru;
}

PelakuKorupsi* tambahPelaku(PelakuKorupsi* root, string nama, string jabatan, long long nominal) {
    if (!root) {
        PelakuKorupsi* baru = buatPelakuBaru(nama, jabatan, nominal);
        riwayatUndo.push({"tambah", baru});
        return baru;
    }
    if (nama < root->nama)
        root->left = tambahPelaku(root->left, nama, jabatan, nominal);
    else
        root->right = tambahPelaku(root->right, nama, jabatan, nominal);
    return root;
}

PelakuKorupsi* cariPelakuTerkecil(PelakuKorupsi* node) {
    while (node && node->left)
        node = node->left;
    return node;
}

PelakuKorupsi* hapusPelaku(PelakuKorupsi* root, string nama, PelakuKorupsi*& backup) {
    if (!root) return nullptr;
    if (nama < root->nama)
        root->left = hapusPelaku(root->left, nama, backup);
    else if (nama > root->nama)
        root->right = hapusPelaku(root->right, nama, backup);
    else {
        backup = new PelakuKorupsi(*root);
        if (!root->left) {
            PelakuKorupsi* temp = root->right;
            delete root;
            return temp;
        } else if (!root->right) {
            PelakuKorupsi* temp = root->left;
            delete root;
            return temp;
        }
        PelakuKorupsi* temp = cariPelakuTerkecil(root->right);
        root->nama = temp->nama;
        root->jabatan = temp->jabatan;
        root->nominal = temp->nominal;
        root->right = hapusPelaku(root->right, temp->nama, backup);
    }
    return root;
}

void tampilkanHeaderTabel() {
    cout << "+====================+===========================+===========================+\n";
    cout << "| Nama Pelaku       | Jabatan                   | Kerugian Negara (Rp)     |\n";
    cout << "+====================+===========================+===========================+\n";
}

void tampilkanFooterTabel() {
    cout << "+====================+===========================+===========================+\n";
}

void tampilkanDataUrutNama(PelakuKorupsi* root) {
    if (root) {
        tampilkanDataUrutNama(root->left);
        cout << "| " << left << setw(18) << root->nama
             << "| " << setw(25) << root->jabatan
             << "| Rp" << setw(25) << root->nominal << "|" << endl;
        tampilkanDataUrutNama(root->right);
    }
}

PelakuKorupsi* batalkanAksiTerakhir(PelakuKorupsi* root) {
    if (riwayatUndo.empty()) {
        cout << "Tidak ada aksi untuk dibatalkan.\n";
        return root;
    }
    RiwayatAksi terakhir = riwayatUndo.top(); riwayatUndo.pop();
    if (terakhir.jenisAksi == "tambah") {
        cout << "Membatalkan penambahan data: " << terakhir.pelaku->nama << endl;
        PelakuKorupsi* dummy = nullptr;
        root = hapusPelaku(root, terakhir.pelaku->nama, dummy);
    } else if (terakhir.jenisAksi == "hapus") {
        cout << "Membatalkan penghapusan data: " << terakhir.pelaku->nama << endl;
        root = tambahPelaku(root, terakhir.pelaku->nama, terakhir.pelaku->jabatan, terakhir.pelaku->nominal);
    }
    return root;
}

int main() {
    PelakuKorupsi* root = nullptr;
    int pilihan;
    do {
        cout << "\n+===============================================+\n";
        cout << "|     SISTEM MONITORING DATA KORUPSI DAERAH     |\n";
        cout << "+===============================================+\n";
        cout << "| 1. Tambah Data                                |\n";
        cout << "| 2. Lihat Daftar Pelaku Korupsi                |\n";
        cout << "| 3. Hapus Data                                 |\n";
        cout << "| 4. Batalkan Aksi Terakhir                     |\n";
        cout << "| 0. Keluar                                     |\n";
        cout << "+-----------------------------------------------+\n";
        cout << "Masukkan pilihan: "; cin >> pilihan;
        cin.ignore();

        if (pilihan == 1) {
            string nama, jabatan;
            long long nominal;
            cout << "+-----------------------------------------------+\n";
            cout << "|           Tambah Data Pelaku Korupsi          |\n" << "+-----------------------------------------------+\n";
            cout << "Masukkan Nama: "; getline(cin, nama);
            cout << "Masukkan Jabatan: "; getline(cin, jabatan);
            cout << "Masukkan Kerugian Negara (Rp): "; cin >> nominal;
            root = tambahPelaku(root, nama, jabatan, nominal);
            cout << "Data berhasil ditambahkan.\n";
        } else if (pilihan == 2) {
            cout << "\n+-----------------------------------------------+\n";
            cout << "|     DAFTAR PELAKU KORUPSI (A - Z)             |\n";
            tampilkanHeaderTabel();
            tampilkanDataUrutNama(root);
            tampilkanFooterTabel();
        } else if (pilihan == 3) {
            string nama;
            cout << "Masukkan Nama yang ingin dihapus: "; getline(cin, nama);
            PelakuKorupsi* backup = nullptr;
            root = hapusPelaku(root, nama, backup);
            if (backup) {
                riwayatUndo.push({"hapus", backup});
                cout << "Data berhasil dihapus.\n";
            } else {
                cout << "Data tidak ditemukan.\n";
            }
        } else if (pilihan == 4) {
            root = batalkanAksiTerakhir(root);
        }
    } while (pilihan != 0);
    return 0;
}
