#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h> // fabs için

#define YAZARLAR_DOSYASI "Yazarlar.csv"
#define OGRENCILER_DOSYASI "Ogrenciler.csv"
#define KITAPLAR_DOSYASI "Kitaplar.csv"
#define KITAP_YAZAR_DOSYASI "KitapYazar.csv"
#define ODUNC_KAYITLARI_DOSYASI "KitapOdunc.csv" 

#define MAX_AD_SOYAD 50
#define MAX_KITAP_AD 100
#define ISBN_UZUNLUK 14
#define OGRENCI_NO_UZUNLUK 9
#define ETIKET_NO_UZUNLUK 25
#define DURUM_UZUNLUK MAX_AD_SOYAD
#define TARIH_UZUNLUK 11

typedef struct Yazar {
    int yazarID;
    char ad[MAX_AD_SOYAD];
    char soyad[MAX_AD_SOYAD];
    struct Yazar* next;
} Yazar;

typedef struct Ogrenci {
    char ogrenciNo[OGRENCI_NO_UZUNLUK];
    char ad[MAX_AD_SOYAD];
    char soyad[MAX_AD_SOYAD];
    int puan;
    struct Ogrenci* onceki;
    struct Ogrenci* next;
} Ogrenci;

typedef struct KitapKopya {
    char etiketNumarasi[ETIKET_NO_UZUNLUK];
    char durum[DURUM_UZUNLUK];
    struct KitapKopya* next;
} KitapKopya;

typedef struct Kitap {
    char ad[MAX_KITAP_AD];
    char isbn[ISBN_UZUNLUK];
    int adet;
    KitapKopya* kopyalar;
    struct Kitap* next;
} Kitap;

typedef struct KitapYazarIliski {
    char isbn[ISBN_UZUNLUK];
    int yazarID;
} KitapYazarIliski;

typedef struct OduncIslem {
    char ogrenciID[OGRENCI_NO_UZUNLUK];
    char kitapEtiketNo[ETIKET_NO_UZUNLUK];
    int islemTuru;
    char tarih[TARIH_UZUNLUK];
} OduncIslem;

typedef struct {
    KitapYazarIliski* iliskiler;
    int sayi;
} KitapYazarVeriYoneticisi;

typedef struct {
    OduncIslem* islemler;
    int sayi;
} OduncVeriYoneticisi;

typedef struct {
    int gun;
    int ay;
    int yil;
} Tarih;


typedef void (*YazarIslemFonksiyonu)(Yazar*);
typedef int  (*YazarKosulFonksiyonu)(Yazar*, void*);

typedef void (*OgrenciIslemFonksiyonu)(Ogrenci*);
typedef int  (*OgrenciKosulFonksiyonu)(Ogrenci*, void*);

typedef void (*KitapIslemFonksiyonu)(Kitap*);
typedef int  (*KitapKosulFonksiyonu)(Kitap*, void*);


void        guvenliInputAlma      (char* buffer, int size);

Tarih       stringToTarih         (const char* tarihStr);
int         TarihlerArasiGunSayar (Tarih t1_basit, Tarih t2_basit);
void        bugununTarihiniAl     (char* tarihBuffer);

int         herZamanDogruKosuluYazar   (Yazar* yazar, void* veri);
int         herZamanDogruKosuluKitap   (Kitap* kitap, void* veri);
int         herZamanDogruKosuluOgrenci (Ogrenci* ogrenci, void* veri);
int         cezaliOgrenciKosulu        (Ogrenci* ogrenci, void* veri);

void        yazarlariKaydetAnlik              (Yazar* yazarListesi);
void        ogrencileriKaydetAnlik            (Ogrenci* ogrList);
void        kitaplariKaydetAnlik              (Kitap* kitapList);
void        kitapYazarIliskileriniKaydetAnlik (KitapYazarVeriYoneticisi* kitapYazarDB);
void        oduncKayitlariniKaydetAnlik       (OduncVeriYoneticisi* oduncVeri);

void        yazarListesiniGezVeIsle (Yazar* listeHead, YazarIslemFonksiyonu islemYap, YazarKosulFonksiyonu kosuluSaglarMi,
                                     void* kosulVerisi, const char* baslikMesaji, const char* bulunamadiMesaji);
void        kitapListniGezVeIsle    (Kitap* listeHead, KitapIslemFonksiyonu islemYap, KitapKosulFonksiyonu kosuluSaglarMi,
                                     void* kosulVerisi, const char* baslikMesaji, const char* bulunamadiMesaji);
void        ogrListniGezVeIsle      (Ogrenci* listeHead, OgrenciIslemFonksiyonu islemYap, OgrenciKosulFonksiyonu kosuluSaglarMi,
                                     void* kosulVerisi, const char* baslikMesaji, const char* bulunamadiMesaji, const char* tabloBasligi);

Yazar*      yazarBulByID         (Yazar* yazarListesi, int yazarID);
void        yazarEkle            (Yazar** yazarHead, int* yeniYazarID, const char* ad, const char* soyad);
void        yazarSil             (Yazar** yazarHead, KitapYazarVeriYoneticisi* kitapYazarDB, int yazarID);
void        yazarGuncelle        (Yazar* yazarListesi, int yazarID, const char* yeniAd, const char* yeniSoyad);
void        yazarBilgisiniYazdir (Yazar* yazar); 
void        yazarlariListele     (Yazar* yazarListesi);

Ogrenci*    ogrenciBulByID           (Ogrenci* ogrList, const char* ogrenciNo);
void        ogrenciEkle              (Ogrenci** ogrListHead, const char* ogrenciNo, const char* ad, const char* soyad);
void        ogrenciSil               (Ogrenci** ogrListHead, const char* ogrenciNo);
void        ogrenciGuncelle          (Ogrenci* ogrList, const char* ogrenciNo, const char* yeniAd, const char* yeniSoyad, int yeniPuan);
void        ogrenciBilgisiniYazdir   (Ogrenci* ogrenci); 
void        ogrencileriListele       (Ogrenci* ogrList);
void        cezaliOgrencileriListele (Ogrenci* ogrList); 

Kitap*      kitapBulByISBN                           (Kitap* kitapList, const char* isbn);
KitapKopya* kitapKopyasiBulByEtiket                  (Kitap* kitap, const char* etiketNo);
void        kitapEkle                                (Kitap** kitapListHead, const char* ad, const char* isbn, int adet);
void        kitapSil                                 (Kitap** kitapListHead, const char* isbn);
void        kitapGuncelle                            (Kitap* kitapList, const char* isbn, const char* yeniAd);
void        kitapVeKopyalariniYazdir                 (Kitap* kitap); 
void        kitaplariListele                         (Kitap* kitapList);
void        raftakiKitaplariListele                  (Kitap* kitapList);
void        kitapYazarEslestir                       (KitapYazarVeriYoneticisi* kitapYazarDB, Kitap* kitapList, Yazar* yazarListesi, const char* isbn, int yazarID);
void        kitabinYazariniGuncelle                  (KitapYazarVeriYoneticisi* kitapYazarDB, Kitap* kitapList, Yazar* yazarListesi, const char* isbn, int eskiYazarID, int yeniYazarID);
void        kitapOduncAl                             (Ogrenci* ogrList, Kitap* kitapList, OduncVeriYoneticisi* oduncVeri, const char* ogrenciID, const char* kitapISBN);
void        kitapTeslimEt                            (Ogrenci* ogrList, Kitap* kitapList, OduncVeriYoneticisi* oduncVeri, const char* ogrenciID, const char* kitapEtiketNo);
void        kitapTeslimEtmemisOgrencileriListele     (Ogrenci* ogrList, Kitap* kitapList);
void        zamanindaTeslimEdilmeyenKitaplariListele (Kitap* kitapList, OduncVeriYoneticisi* oduncVeri);

void        yazarlariYukle              (Yazar** yazarHead, int* yeniYazarID);
void        ogrencileriYukle            (Ogrenci** ogrListHead);
void        kitaplariYukle              (Kitap** kitapListHead);
void        kitapYazarIliskileriniYukle (KitapYazarVeriYoneticisi* kitapYazarDB);
void        oduncKayitlariniYukle       (OduncVeriYoneticisi* oduncVeri);
void        tumVerileriYukle            (Yazar** yazarHead, Ogrenci** ogrListHead, Kitap** kitapListHead,
                                         int* yeniYazarID, KitapYazarVeriYoneticisi* kitapYazarDB, OduncVeriYoneticisi* oduncVeri);

void        bellekTemizle     (Yazar** yazarHead, Ogrenci** ogrListHead, Kitap** kitapListHead,
                               KitapYazarVeriYoneticisi* kitapYazarDB, OduncVeriYoneticisi* oduncVeri);

void        ogrenciMenuGoster (Ogrenci** ogrListHead, Kitap* kitapList, OduncVeriYoneticisi* oduncVeri);
void        kitapMenuGoster   (Kitap** kitapListHead, Yazar* yazarListesi, KitapYazarVeriYoneticisi* kitapYazarDB, OduncVeriYoneticisi* oduncVeri);
void        yazarMenuGoster   (Yazar** yazarHead, int* yeniYazarID, Kitap* kitapList, KitapYazarVeriYoneticisi* kitapYazarDB);
void        anaMenuGoster     (Yazar** yazarHead, Ogrenci** ogrListHead, Kitap** kitapListHead,
                               int* yeniYazarID, KitapYazarVeriYoneticisi* kitapYazarDB, OduncVeriYoneticisi* oduncVeri);

// --- main ---
int main() {
    Yazar* yazarListesi = NULL;
    Ogrenci* ogrList = NULL;
    Kitap* kitapList = NULL;
    int sonYazarID = 0;

    KitapYazarVeriYoneticisi kitapYazarVerileri = {NULL, 0};
    OduncVeriYoneticisi oduncVerileri = {NULL, 0};

    printf("Kutuphane Otomasyonu Baslatiliyor...\n");
    tumVerileriYukle(&yazarListesi, &ogrList, &kitapList, &sonYazarID, &kitapYazarVerileri, &oduncVerileri);
    printf("\nVeriler yuklendi. Ana menuye geciliyor.\n");

    anaMenuGoster(&yazarListesi, &ogrList, &kitapList, &sonYazarID, &kitapYazarVerileri, &oduncVerileri);

    bellekTemizle(&yazarListesi, &ogrList, &kitapList, &kitapYazarVerileri, &oduncVerileri);
    printf("Program basariyla sonlandirildi.\n");
    return 0;
}


void guvenliInputAlma(char* buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        } else {
            int ch;
            if (len == size - 1 && buffer[len - 1] != '\n') {
                 while ((ch = getchar()) != '\n' && ch != EOF);
            }
        }
    } else {
        buffer[0] = '\0';
    }
}

Tarih stringToTarih (const char* tarihStr) {
    Tarih t = {0,0,0};
    if (tarihStr && strlen(tarihStr) == 10) {
        if (sscanf(tarihStr, "%d.%d.%d", &t.gun, &t.ay, &t.yil) != 3) {
            fprintf(stderr, "HATA: Tarih string formati gecersiz: %s\n", tarihStr);
            t.gun = 0; t.ay = 0; t.yil = 0;
        }
    }
    return t;
}

int TarihlerArasiGunSayar (Tarih t1_basit, Tarih t2_basit) {
    struct tm tm1 = {0}, tm2 = {0};
    tm1.tm_year = t1_basit.yil - 1900;
    tm1.tm_mon  = t1_basit.ay - 1;
    tm1.tm_mday = t1_basit.gun;
    tm1.tm_hour = 12; tm1.tm_isdst = -1;

    tm2.tm_year = t2_basit.yil - 1900;
    tm2.tm_mon  = t2_basit.ay - 1;
    tm2.tm_mday = t2_basit.gun;
    tm2.tm_hour = 12; tm2.tm_isdst = -1;

    time_t time1_t = mktime(&tm1);
    time_t time2_t = mktime(&tm2);
    if (time1_t == (time_t)(-1) || time2_t == (time_t)(-1))
        return -1;
    return (int)(fabs(difftime(time2_t, time1_t)) / (60.0 * 60.0 * 24.0));
}

void bugununTarihiniAl (char* tarihBuffer) {
    time_t simdi = time(NULL);
    struct tm *yerelZaman = localtime(&simdi);

    if (yerelZaman != NULL) {
        strftime(tarihBuffer, TARIH_UZUNLUK, "%d.%m.%Y", yerelZaman);
    } else {
        strcpy(tarihBuffer, "00.00.0000");
        fprintf(stderr, "HATA: Yerel zaman alinamadi.\n");
    }
}


int herZamanDogruKosuluYazar (Yazar* yazar, void* veri) {
    (void)yazar;
    (void)veri;
    return 1;
}

int herZamanDogruKosuluKitap (Kitap* kitap, void* veri) {
    (void)kitap;
    (void)veri;
    return 1;
}

int herZamanDogruKosuluOgrenci (Ogrenci* ogrenci, void* veri) {
    (void)ogrenci;
    (void)veri;
    return 1;
}

int cezaliOgrenciKosulu (Ogrenci* ogrenci, void* veri) {
    (void)veri;
    if (ogrenci)
        return ogrenci->puan <= 0;

    return 0;
}


Yazar* yazarBulByID(Yazar* yazarListesi, int yazarID) {
    Yazar* temp = yazarListesi;
    int bulundu = 0;

    while (temp != NULL && !bulundu) {
        if (temp->yazarID == yazarID) {
            bulundu = 1;
        } else {
            temp = temp->next;
        }
    }
    return (bulundu ? temp : NULL);
}

void yazarEkle(Yazar** yazarHead, int* yeniYazarID, const char* ad, const char* soyad) {
    Yazar* yeniYazar = (Yazar*)malloc(sizeof(Yazar));
    if (!yeniYazar) {
        printf("HATA: Yazar icin bellek ayrilamadi!\n");
        return;
    }

    (*yeniYazarID)++;
    yeniYazar->yazarID = *yeniYazarID;
    strncpy(yeniYazar->ad, ad, MAX_AD_SOYAD - 1);
    yeniYazar->ad[MAX_AD_SOYAD - 1] = '\0';

    strncpy(yeniYazar->soyad, soyad, MAX_AD_SOYAD - 1);
    yeniYazar->soyad[MAX_AD_SOYAD - 1] = '\0';
    yeniYazar->next = NULL;

    if (*yazarHead == NULL || (*yazarHead)->yazarID > yeniYazar->yazarID) {
        yeniYazar->next = *yazarHead;
        *yazarHead = yeniYazar;
    } else {
        Yazar* temp = *yazarHead;
        int devam = 1;
        while (temp->next != NULL && devam) {
            if (temp->next->yazarID < yeniYazar->yazarID) {
                temp = temp->next;
            } else {
                devam = 0;
            }
        }
        yeniYazar->next = temp->next;
        temp->next = yeniYazar;
    }
    printf("'%s %s' (ID: %d) adli yazar eklendi.\n", ad, soyad, yeniYazar->yazarID);
    yazarlariKaydetAnlik(*yazarHead);
}

void yazarSil(Yazar** yazarHead, KitapYazarVeriYoneticisi* kitapYazarDB, int yazarID) {
    Yazar* silinecek = NULL;
    Yazar* onceki = NULL; 
    int silindi = 0, i;

    if (*yazarHead == NULL) {
        printf("Yazar listesi bos.\n");
        return;
    }

    if ((*yazarHead)->yazarID == yazarID) {
        silinecek = *yazarHead;
        *yazarHead = (*yazarHead)->next;
        silindi = 1;
    } else {
        onceki = *yazarHead; 
        int devam = 1;
        while (onceki->next != NULL && devam) {
            if (onceki->next->yazarID != yazarID) {
                onceki = onceki->next;
            } else {
                devam = 0;
            }
        }
        if (!devam && onceki->next != NULL) {
            silinecek = onceki->next;
            onceki->next = silinecek->next;
            silindi = 1;
        }
    }

    if (silindi && silinecek) {
        printf("'%s %s' (ID: %d) adli yazar silindi.\n", silinecek->ad, silinecek->soyad, silinecek->yazarID);
        free(silinecek);
        for (i = 0; i < kitapYazarDB->sayi; i++) {
            if (kitapYazarDB->iliskiler[i].yazarID == yazarID) {
                kitapYazarDB->iliskiler[i].yazarID = -1;
            }
        }
        yazarlariKaydetAnlik(*yazarHead);
        kitapYazarIliskileriniKaydetAnlik(kitapYazarDB);
    } else {
        printf("%d ID'li yazar bulunamadi.\n", yazarID);
    }
}

void yazarGuncelle(Yazar* yazarListesi, int yazarID, const char* yeniAd, const char* yeniSoyad) {
    Yazar* guncellenecek = yazarBulByID(yazarListesi, yazarID);

    if (guncellenecek) {
        strncpy(guncellenecek->ad, yeniAd, MAX_AD_SOYAD - 1); 
        guncellenecek->ad[MAX_AD_SOYAD - 1] = '\0';
        
        strncpy(guncellenecek->soyad, yeniSoyad, MAX_AD_SOYAD - 1); 
        guncellenecek->soyad[MAX_AD_SOYAD - 1] = '\0';
        printf("%d ID'li yazar bilgileri guncellendi.\n", yazarID);
        yazarlariKaydetAnlik(yazarListesi);
    } else {
        printf("%d ID'li yazar bulunamadi.\n", yazarID);
    }
}

void yazarlariKaydetAnlik (Yazar* yazarListesi) {
    FILE* fp = fopen(YAZARLAR_DOSYASI, "w");
    if (!fp) {
        printf("HATA: %s dosyasi yazmak icin acilamadi!\n", YAZARLAR_DOSYASI);
        return;
    }

    Yazar* temp = yazarListesi;
    while (temp != NULL) {
        fprintf(fp, "%d,%s,%s\n", temp->yazarID, temp->ad, temp->soyad); temp = temp->next;
    }

    fclose(fp);
}

void yazarBilgisiniYazdir (Yazar* yazar) {
    if (yazar)
        printf("ID: %d, Ad: %s, Soyad: %s\n", yazar->yazarID, yazar->ad, yazar->soyad);
}

void yazarlariListele (Yazar* yazarListesi) {
    yazarListesiniGezVeIsle(yazarListesi, yazarBilgisiniYazdir, herZamanDogruKosuluYazar, NULL,
                            "\n--- TUM YAZARLAR LISTESI ---\n", "Listelenecek yazar yok.");
}

void yazarListesiniGezVeIsle (Yazar* listeHead, YazarIslemFonksiyonu islemYap, YazarKosulFonksiyonu kosuluSaglarMi,
                              void* kosulVerisi, const char* baslikMesaji, const char* bulunamadiMesaji) {

    if (baslikMesaji) {
        printf("%s", baslikMesaji);
    }

    if (!listeHead) {
         printf("%s\n", bulunamadiMesaji ? bulunamadiMesaji : "Liste bos veya listelenecek oge bulunamadi.");
    }

    Yazar* temp = listeHead;
    int enAzBirOgeIsledim = 0;

    while (temp != NULL) {
        if (kosuluSaglarMi == NULL || kosuluSaglarMi(temp, kosulVerisi)) {
            if (islemYap != NULL) {
                islemYap(temp);
                enAzBirOgeIsledim = 1;
            }
        }
        temp = temp->next;
    }

    if (!enAzBirOgeIsledim && listeHead) { // Liste boş değil ama koşula uyan olmadı
        printf("%s\n", bulunamadiMesaji ? bulunamadiMesaji : "Belirtilen kosula uygun oge bulunamadi.");
    }
    if (baslikMesaji)
        printf("-----------------------------------------\n");
}


void yazarlariYukle(Yazar** yazarHead, int* yeniYazarID) {
    Yazar* currentYazar = *yazarHead; 
    Yazar* nextNodeYazar;             

    while (currentYazar != NULL) {
        nextNodeYazar = currentYazar->next;
        free(currentYazar);
        currentYazar = nextNodeYazar;
    }

    *yazarHead = NULL;
    *yeniYazarID = 0;

    FILE* fp = fopen(YAZARLAR_DOSYASI, "r");
    if (!fp) {
        printf("%s dosyasi acilamadi (yukleme). Bos liste ile baslaniyor.\n", YAZARLAR_DOSYASI);
        return;
    }

    char satir[256];
    int maxID = 0;

    while (fgets(satir, sizeof(satir), fp)) {
        int id;
        char ad[MAX_AD_SOYAD], soyad[MAX_AD_SOYAD];
        if (sscanf(satir, "%d,%49[^,],%49[^\n]", &id, ad, soyad) == 3) {
            Yazar* yeni = (Yazar*)malloc(sizeof(Yazar));
            if(!yeni) {
                printf("Yazar yukleme bellek hatasi\n");
                fclose(fp);
                return;
            }
            yeni->yazarID = id;
            strncpy(yeni->ad, ad, MAX_AD_SOYAD - 1);
            yeni->ad[MAX_AD_SOYAD - 1] = '\0';

            strncpy(yeni->soyad, soyad, MAX_AD_SOYAD - 1);
            yeni->soyad[MAX_AD_SOYAD - 1] = '\0';
            yeni->next = NULL;

            if (*yazarHead == NULL || (*yazarHead)->yazarID > yeni->yazarID) {
                yeni->next = *yazarHead;
                *yazarHead = yeni;
            } else {
                Yazar* temp = *yazarHead;
                int devam = 1;
                while (temp->next != NULL && devam) {
                    if (temp->next->yazarID < yeni->yazarID) {
                        temp = temp->next;
                    } else {
                        devam = 0;
                    }
                }
                yeni->next = temp->next; temp->next = yeni;
            }
            if (id > maxID) maxID = id;
        }
    }
    fclose(fp);
    *yeniYazarID = maxID;

    printf("%s dosyasindan yazarlar yuklendi. Atanan Son Yazar ID: %d\n", YAZARLAR_DOSYASI, *yeniYazarID);
}


Ogrenci* ogrenciBulByID(Ogrenci* ogrList, const char* ogrenciNo) {
    Ogrenci* temp = ogrList;
    int bulundu = 0;

    while (temp != NULL && !bulundu) {
        if (strcmp(temp->ogrenciNo, ogrenciNo) == 0) {
            bulundu = 1;
        } else {
            temp = temp->next;
        }
    }
    return bulundu ? temp : NULL;
}

void ogrenciEkle(Ogrenci** ogrListHead, const char* ogrenciNo, const char* ad, const char* soyad) {
    if (ogrenciBulByID(*ogrListHead, ogrenciNo) != NULL) {
        printf("HATA: %s numarali ogrenci zaten mevcut.\n", ogrenciNo);
        return;
    }
    if (strlen(ogrenciNo) != 8) {
        printf("HATA: Ogrenci numarasi 8 haneli olmalidir.\n");
        return;
    }

    Ogrenci* yeniOgrenci = (Ogrenci*)malloc(sizeof(Ogrenci));
    if (!yeniOgrenci) {
        printf("HATA: Ogrenci icin bellek ayrilamadi!\n");
        return;
    }

    strncpy(yeniOgrenci->ogrenciNo, ogrenciNo, OGRENCI_NO_UZUNLUK - 1);
    yeniOgrenci->ogrenciNo[OGRENCI_NO_UZUNLUK - 1] = '\0';

    strncpy(yeniOgrenci->ad, ad, MAX_AD_SOYAD - 1);
    yeniOgrenci->ad[MAX_AD_SOYAD - 1] = '\0';

    strncpy(yeniOgrenci->soyad, soyad, MAX_AD_SOYAD - 1);
    yeniOgrenci->soyad[MAX_AD_SOYAD - 1] = '\0';

    yeniOgrenci->puan = 100;
    yeniOgrenci->onceki = NULL;
     yeniOgrenci->next = NULL;

    if (*ogrListHead == NULL) {
        *ogrListHead = yeniOgrenci;
    } else {
        Ogrenci* temp = *ogrListHead;
        while (temp->next != NULL) temp = temp->next;
        temp->next = yeniOgrenci;
        yeniOgrenci->onceki = temp;
    }
    printf("'%s %s' (%s) adli ogrenci eklendi.\n", ad, soyad, ogrenciNo);
    ogrencileriKaydetAnlik(*ogrListHead);
}

void ogrenciSil(Ogrenci** ogrListHead, const char* ogrenciNo) {
    Ogrenci* silinecek = ogrenciBulByID(*ogrListHead, ogrenciNo);

    if (silinecek) {
        if (silinecek->onceki)
            silinecek->onceki->next = silinecek->next;
        else
            *ogrListHead = silinecek->next;
        if (silinecek->next)
            silinecek->next->onceki = silinecek->onceki;

        printf("'%s %s' (%s) adli ogrenci silindi.\n", silinecek->ad, silinecek->soyad, silinecek->ogrenciNo);
        free(silinecek);
        ogrencileriKaydetAnlik(*ogrListHead);

    } else {
        printf("%s numarali ogrenci bulunamadi.\n", ogrenciNo);
    }
}

void ogrenciGuncelle(Ogrenci* ogrList, const char* ogrenciNo, const char* yeniAd, const char* yeniSoyad, int yeniPuan) {
    Ogrenci* guncellenecek = ogrenciBulByID(ogrList, ogrenciNo);
    if (guncellenecek) {
        strncpy(guncellenecek->ad, yeniAd, MAX_AD_SOYAD - 1);
        guncellenecek->ad[MAX_AD_SOYAD - 1] = '\0';

        strncpy(guncellenecek->soyad, yeniSoyad, MAX_AD_SOYAD - 1);
        guncellenecek->soyad[MAX_AD_SOYAD - 1] = '\0';

        guncellenecek->puan = yeniPuan;
        printf("%s numarali ogrenci bilgileri guncellendi.\n", ogrenciNo);
        ogrencileriKaydetAnlik(ogrList);
    } else {
        printf("%s numarali ogrenci bulunamadi.\n", ogrenciNo);
    }
}

void ogrencileriKaydetAnlik (Ogrenci* ogrList) {
    FILE* fp = fopen(OGRENCILER_DOSYASI, "w");

    if (!fp) {
        printf("HATA: %s dosyasi yazmak icin acilamadi!\n", OGRENCILER_DOSYASI);
        return;
    }

    Ogrenci* temp = ogrList;
    while (temp != NULL) {
        fprintf(fp, "%s,%s,%s,%d\n", temp->ogrenciNo, temp->ad, temp->soyad, temp->puan);
        temp = temp->next;
    }
    fclose(fp);
}

void ogrenciBilgisiniYazdir (Ogrenci* ogrenci) {
    if (ogrenci) {
       char adSoyad[MAX_AD_SOYAD * 2 + 2];
       snprintf(adSoyad, sizeof(adSoyad), "%s %s", ogrenci->ad, ogrenci->soyad);
       printf("%-11s %-20.20s %d\n", ogrenci->ogrenciNo, adSoyad, ogrenci->puan);
    }
}

void ogrencileriListele (Ogrenci* ogrList) {
    ogrListniGezVeIsle(ogrList, ogrenciBilgisiniYazdir, herZamanDogruKosuluOgrenci, NULL,
                       "\n--- TUM OGRENCI LISTESI ---\n", "Listelenecek ogrenci yok.",
                       "Ogrenci No  Ad Soyad              Puan\n----------- --------------------- ----\n");
}

void ogrListniGezVeIsle (Ogrenci* listeHead, OgrenciIslemFonksiyonu islemYap, OgrenciKosulFonksiyonu kosuluSaglarMi,
                         void* kosulVerisi, const char* baslikMesaji, const char* bulunamadiMesaji, const char* tabloBasligi) {
    if (baslikMesaji)
        printf("%s", baslikMesaji);

    if (!listeHead) {
         printf("%s\n", bulunamadiMesaji ? bulunamadiMesaji : "Liste bos veya listelenecek oge bulunamadi.");
    }

    Ogrenci* temp = listeHead;
    int enAzBirOgeIsledim = 0;
    int tabloBasligiYazdirildiMi = 0;

    while (temp != NULL) {
        if (kosuluSaglarMi == NULL || kosuluSaglarMi(temp, kosulVerisi)) {
            if (tabloBasligi && !tabloBasligiYazdirildiMi) {
                printf("%s", tabloBasligi);
                tabloBasligiYazdirildiMi = 1;
            }
            if (islemYap != NULL) {
                islemYap(temp);
                enAzBirOgeIsledim = 1;
            }
        }
        temp = temp->next;
    }

    if (!enAzBirOgeIsledim && listeHead) {
         printf("%s\n", bulunamadiMesaji ? bulunamadiMesaji : "Belirtilen kosula uygun oge bulunamadi.");
    }
    if (baslikMesaji)
        printf("--------------------------------------------------\n");
}

void cezaliOgrencileriListele (Ogrenci* ogrList) {
    ogrListniGezVeIsle(ogrList, ogrenciBilgisiniYazdir, cezaliOgrenciKosulu, NULL,
                       "\n--- CEZALI OGRENCILER (Puan <= 0) ---\n", "Cezali ogrenci bulunmamaktadir.",
                       "Ogrenci No  Ad Soyad              Puan\n----------- --------------------- ----\n");
}


void ogrencileriYukle(Ogrenci** ogrListHead) {
    Ogrenci* currentOgrenci = *ogrListHead; 
    Ogrenci* nextNodeOgrenci;               

    while (currentOgrenci != NULL) {
        nextNodeOgrenci = currentOgrenci->next;
        free(currentOgrenci);
        currentOgrenci = nextNodeOgrenci;
    }

    *ogrListHead = NULL;

    FILE* fp = fopen(OGRENCILER_DOSYASI, "r");
    if (!fp) {
        printf("%s dosyasi acilamadi (yukleme). Bos liste.\n", OGRENCILER_DOSYASI);
        return;
    }

    char satir[256];

    while (fgets(satir, sizeof(satir), fp)) {
        char no[OGRENCI_NO_UZUNLUK], ad[MAX_AD_SOYAD], soyad[MAX_AD_SOYAD]; int puan;
        if (sscanf(satir, "%8[^,],%49[^,],%49[^,],%d", no, ad, soyad, &puan) == 4) {
            Ogrenci* yeni = (Ogrenci*)malloc(sizeof(Ogrenci));
            if(!yeni) {
                printf("Ogrenci yukleme bellek hatasi\n");
                fclose(fp);
                return;
            }
            strncpy(yeni->ogrenciNo, no, OGRENCI_NO_UZUNLUK - 1);
            yeni->ogrenciNo[OGRENCI_NO_UZUNLUK - 1] = '\0';

            strncpy(yeni->ad, ad, MAX_AD_SOYAD - 1);
            yeni->ad[MAX_AD_SOYAD - 1] = '\0';

            strncpy(yeni->soyad, soyad, MAX_AD_SOYAD - 1);
            yeni->soyad[MAX_AD_SOYAD - 1] = '\0';

            yeni->puan = puan;
            yeni->onceki = NULL;
            yeni->next = NULL;

            if (*ogrListHead == NULL) {
                *ogrListHead = yeni;
            } else {
                Ogrenci* temp = *ogrListHead;
                while (temp->next != NULL) {
                    temp = temp->next;
                }
                temp->next = yeni;
                yeni->onceki = temp;
            }
        }
    }
    fclose(fp);
    printf("%s dosyasindan ogrenciler yuklendi.\n", OGRENCILER_DOSYASI);
}


Kitap* kitapBulByISBN(Kitap* kitapList, const char* isbn) {
    Kitap* temp = kitapList;
    int bulundu = 0;

    while (temp != NULL && !bulundu) {
        if (strcmp(temp->isbn, isbn) == 0) {
            bulundu = 1;
        } else {
            temp = temp->next;
        }
    }
    return bulundu ? temp : NULL;
}

KitapKopya* kitapKopyasiBulByEtiket(Kitap* kitap, const char* etiketNo) {
    if (!kitap)
        return NULL;

    KitapKopya* kopya = kitap->kopyalar;
    int bulundu = 0;

    while (kopya != NULL && !bulundu) {
        if (strcmp(kopya->etiketNumarasi, etiketNo) == 0) {
            bulundu = 1;
        } else {
            kopya = kopya->next;
        }
    }
    return (bulundu ? kopya : NULL);
}

void kitapEkle(Kitap** kitapListHead, const char* ad, const char* isbn, int adet) {
    int i;
    if (kitapBulByISBN(*kitapListHead, isbn) != NULL) {
        printf("HATA: %s ISBN numarali kitap zaten mevcut.\n", isbn);
        return;
    }
    if (strlen(isbn) != 13) {
        printf("HATA: ISBN 13 haneli olmalidir.\n");
        return;
    }
    Kitap* yeniKitap = (Kitap*)malloc(sizeof(Kitap));
    if (!yeniKitap) { 
        printf("HATA: Kitap icin bellek ayrilamadi!\n");
        return; 
    }

    strncpy(yeniKitap->ad, ad, MAX_KITAP_AD - 1);
    yeniKitap->ad[MAX_KITAP_AD - 1] = '\0';

    strncpy(yeniKitap->isbn, isbn, ISBN_UZUNLUK - 1);
    yeniKitap->isbn[ISBN_UZUNLUK - 1] = '\0';

    yeniKitap->adet = 0;
    yeniKitap->kopyalar = NULL;
    yeniKitap->next = NULL;

    for (i = 0; i < adet; i++) {
        KitapKopya* yeniKopya = (KitapKopya*)malloc(sizeof(KitapKopya));
        if (!yeniKopya) {
            printf("HATA: Kitap kopyasi icin bellek ayrilamadi! %d. kopya eklenemedi.\n", i + 1);
        } else {
            sprintf(yeniKopya->etiketNumarasi, "%s_%d", isbn, yeniKitap->adet + 1);
            strncpy(yeniKopya->durum, "RAFTA", DURUM_UZUNLUK - 1);
            yeniKopya->durum[DURUM_UZUNLUK - 1] = '\0';

            yeniKopya->next = yeniKitap->kopyalar;
            yeniKitap->kopyalar = yeniKopya;
            yeniKitap->adet++;
        }
    }

    if (*kitapListHead == NULL || strcmp((*kitapListHead)->ad, yeniKitap->ad) > 0 ||
        (strcmp((*kitapListHead)->ad, yeniKitap->ad) == 0 && strcmp((*kitapListHead)->isbn, yeniKitap->isbn) > 0)) {
        yeniKitap->next = *kitapListHead;
        *kitapListHead = yeniKitap;
    } else {
        Kitap* temp = *kitapListHead;
        int devam = 1;
        while (temp->next != NULL && devam) {
            if (strcmp(temp->next->ad, yeniKitap->ad) < 0 ||
                (strcmp(temp->next->ad, yeniKitap->ad) == 0 && strcmp(temp->next->isbn, yeniKitap->isbn) < 0)) {
                temp = temp->next;
            } else {
                devam = 0;
            }
        }
        yeniKitap->next = temp->next;
        temp->next = yeniKitap;
    }
    printf("'%s' (ISBN: %s) %d kopya ile eklendi.\n", ad, isbn, yeniKitap->adet);
    kitaplariKaydetAnlik(*kitapListHead);
}

void kitapSil(Kitap** kitapListHead, const char* isbn) {
    Kitap* kontrolKitap = kitapBulByISBN(*kitapListHead, isbn);
    if (!kontrolKitap) {
        printf("%s ISBN'li kitap bulunamadi.\n", isbn);
        return;
    }

    KitapKopya* k = kontrolKitap->kopyalar;
    int oduncAlinmisVar = 0;
    int devamKopyaKontrol = 1;

    while(k != NULL && devamKopyaKontrol){
        if(strcmp(k->durum, "RAFTA") != 0) {
            oduncAlinmisVar = 1;
            devamKopyaKontrol = 0;
        }
        if (devamKopyaKontrol) k = k->next;
    }

    if(oduncAlinmisVar){
        printf("HATA: %s ISBN'li kitabin odunc alinmis kopyalari var. Silinemez.\n", isbn);
        return;
    }

    Kitap* silinecek = NULL, *onceki = NULL;
    int silindi = 0;

    if (strcmp((*kitapListHead)->isbn, isbn) == 0) {
        silinecek = *kitapListHead;
        *kitapListHead = (*kitapListHead)->next;
        silindi = 1;
    } else {
        onceki = *kitapListHead;
        int devamSil = 1;
        while (onceki->next != NULL && devamSil) {
            if (strcmp(onceki->next->isbn, isbn) != 0) {
                 onceki = onceki->next;
            } else {
                devamSil = 0;
            }
        }
        if (!devamSil && onceki->next !=NULL) {
            silinecek = onceki->next;
            onceki->next = silinecek->next;
            silindi = 1;
        }
    }

    if (silindi && silinecek) {
        printf("'%s' (ISBN: %s) adli kitap ve tum kopyalari silindi.\n", silinecek->ad, silinecek->isbn);
        KitapKopya* kopya = silinecek->kopyalar, *tempKopya;
        while (kopya != NULL) {
            tempKopya = kopya; kopya = kopya->next;
            free(tempKopya);
        }
        free(silinecek);
        kitaplariKaydetAnlik(*kitapListHead);
    } else if (!silindi) {
        printf("%s ISBN'li kitap silinirken bulunamadi.\n", isbn);
    }
}

void kitapGuncelle(Kitap* kitapList, const char* isbn, const char* yeniAd) {
    Kitap* guncellenecek = kitapBulByISBN(kitapList, isbn);

    if (guncellenecek) {
        strncpy(guncellenecek->ad, yeniAd, MAX_KITAP_AD - 1);
        guncellenecek->ad[MAX_KITAP_AD - 1] = '\0';
        printf("'%s' adli kitabin yeni adi: '%s' olarak guncellendi (ISBN: %s).\n", guncellenecek->ad, yeniAd, isbn); 
        kitaplariKaydetAnlik(kitapList);
    } else {
        printf("%s ISBN'li kitap bulunamadi.\n", isbn);
    }
}

void kitaplariKaydetAnlik (Kitap* kitapList) {
    FILE* fp = fopen(KITAPLAR_DOSYASI, "w");
    if (!fp) {
        printf("HATA: %s dosyasi yazmak icin acilamadi!\n", KITAPLAR_DOSYASI);
        return;
    }

    Kitap* ktp = kitapList;

    while (ktp != NULL) {
        KitapKopya* kopya = ktp->kopyalar;
        while (kopya != NULL) {
            fprintf(fp, "%s,%s,%s,%s\n", ktp->ad, ktp->isbn, kopya->etiketNumarasi, kopya->durum); kopya = kopya->next;
        }
        ktp = ktp->next;
    }
    fclose(fp);
}

void kitapYazarEslestir(KitapYazarVeriYoneticisi* kitapYazarDB, Kitap* kitapList, Yazar* yazarListesi, const char* isbn, int yazarID) {
    int i, zatenVar = 0;

    if (!kitapBulByISBN(kitapList, isbn)) {
        printf("HATA: %s ISBN'li kitap bulunamadi.\n", isbn);
        return;
    }
    if (!yazarBulByID(yazarListesi, yazarID)) {
        printf("HATA: %d ID'li yazar bulunamadi.\n", yazarID);
        return;
    }

    for(i = 0; i < kitapYazarDB->sayi && !zatenVar; i++){
        if(strcmp(kitapYazarDB->iliskiler[i].isbn, isbn) == 0 && kitapYazarDB->iliskiler[i].yazarID == yazarID){
            zatenVar = 1;
        }
    }

    if(zatenVar) {
        printf("Bu kitap-yazar eslesmesi zaten mevcut.\n");
        return;
    }

    KitapYazarIliski* yeniIliskiler = realloc(kitapYazarDB->iliskiler, (kitapYazarDB->sayi + 1) * sizeof(KitapYazarIliski));
    if (!yeniIliskiler) { printf("HATA: Kitap-Yazar iliskisi icin bellek ayrilamadi!\n");
        return;
    }
    kitapYazarDB->iliskiler = yeniIliskiler;
    strncpy(kitapYazarDB->iliskiler[kitapYazarDB->sayi].isbn, isbn, ISBN_UZUNLUK - 1);

    kitapYazarDB->iliskiler[kitapYazarDB->sayi].isbn[ISBN_UZUNLUK - 1] = '\0';
    kitapYazarDB->iliskiler[kitapYazarDB->sayi].yazarID = yazarID;

    (kitapYazarDB->sayi)++;
    printf("%s ISBN'li kitap ile %d ID'li yazar eslestirildi.\n", isbn, yazarID);
    kitapYazarIliskileriniKaydetAnlik(kitapYazarDB);
}

void kitabinYazariniGuncelle(KitapYazarVeriYoneticisi* kitapYazarDB, Kitap* kitapList, Yazar* yazarListesi, const char* isbn, int eskiYazarID, int yeniYazarID) {
    int bulundu = 0, i;

    if (!kitapBulByISBN(kitapList, isbn)) {
        printf("HATA: %s ISBN'li kitap bulunamadi.\n", isbn);
        return;
    }

    if (yeniYazarID != -1 && !yazarBulByID(yazarListesi, yeniYazarID)) {
        printf("HATA: %d ID'li yeni yazar bulunamadi.\n", yeniYazarID);
        return;
    }

    for (i = 0; i < kitapYazarDB->sayi && !bulundu; i++) {
        if (strcmp(kitapYazarDB->iliskiler[i].isbn, isbn) == 0 && kitapYazarDB->iliskiler[i].yazarID == eskiYazarID) {
            kitapYazarDB->iliskiler[i].yazarID = yeniYazarID;

            printf("%s ISBN'li kitabin %d ID'li yazari, %d ID'li yazar ile guncellendi.\n", isbn, eskiYazarID, yeniYazarID);
            bulundu = 1;
            kitapYazarIliskileriniKaydetAnlik(kitapYazarDB);
        }
    }
    if (!bulundu) {
        printf("%s ISBN'li kitap icin %d ID'li yazarla eslesme bulunamadi.\n", isbn, eskiYazarID);
    }
}

void kitapYazarIliskileriniKaydetAnlik (KitapYazarVeriYoneticisi* kitapYazarDB) {
    FILE* fp = fopen(KITAP_YAZAR_DOSYASI, "w");
    int i;

    if (!fp) {
        printf("HATA: %s dosyasi yazmak icin acilamadi!\n", KITAP_YAZAR_DOSYASI);
        return;
    }
    for (i = 0; i < kitapYazarDB->sayi; i++) {
        fprintf(fp, "%s,%d\n", kitapYazarDB->iliskiler[i].isbn, kitapYazarDB->iliskiler[i].yazarID);
    }
    fclose(fp);
}

void kitapOduncAl(Ogrenci* ogrList, Kitap* kitapList, OduncVeriYoneticisi* oduncVeri, const char* ogrenciID, const char* kitapISBN) {
    Ogrenci* ogrenci = ogrenciBulByID(ogrList, ogrenciID);
    Kitap* kitap = kitapBulByISBN(kitapList, kitapISBN);
    if (!ogrenci) {
        printf("HATA: %s numarali ogrenci bulunamadi.\n", ogrenciID);
        return;
    }
    if (!kitap) {
        printf("HATA: %s ISBN'li kitap bulunamadi.\n", kitapISBN);
        return;
    }
    if (ogrenci->puan <= 0) {
        printf("HATA: Ogrencinin puani (%d) yetersiz. Kitap odunc alamaz.\n", ogrenci->puan);
        return;
    }

    KitapKopya* uygunKopya = NULL;
    KitapKopya* k_ptr = kitap->kopyalar;

    int devam = 1;
    while (k_ptr != NULL && devam) {
        if (strcmp(k_ptr->durum, "RAFTA") == 0) {
            uygunKopya = k_ptr;
            devam = 0;
        }
        if (devam) k_ptr = k_ptr->next;
    }

    if (uygunKopya) {
        strncpy(uygunKopya->durum, ogrenciID, DURUM_UZUNLUK - 1); 
        uygunKopya->durum[DURUM_UZUNLUK - 1] = '\0';

        OduncIslem* yeniIslemler = realloc(oduncVeri->islemler, (oduncVeri->sayi + 1) * sizeof(OduncIslem));
        if (!yeniIslemler) {
            printf("HATA: Odunc islemi icin bellek ayrilamadi;!\n");
            strncpy(uygunKopya->durum, "RAFTA", DURUM_UZUNLUK - 1); 
            uygunKopya->durum[DURUM_UZUNLUK-1]='\0'; 
            return;
        }
        oduncVeri->islemler = yeniIslemler;
        strncpy(oduncVeri->islemler[oduncVeri->sayi].ogrenciID, ogrenciID, OGRENCI_NO_UZUNLUK - 1);
        oduncVeri->islemler[oduncVeri->sayi].ogrenciID[OGRENCI_NO_UZUNLUK - 1] = '\0';

        strncpy(oduncVeri->islemler[oduncVeri->sayi].kitapEtiketNo, uygunKopya->etiketNumarasi, ETIKET_NO_UZUNLUK - 1);
        oduncVeri->islemler[oduncVeri->sayi].kitapEtiketNo[ETIKET_NO_UZUNLUK - 1] = '\0';

        oduncVeri->islemler[oduncVeri->sayi].islemTuru = 0;
        bugununTarihiniAl(oduncVeri->islemler[oduncVeri->sayi].tarih);
        (oduncVeri->sayi)++;

        printf("'%s' etiketli kitap %s (%s) tarafindan %s tarihinde odunc alindi.\n",
               uygunKopya->etiketNumarasi, ogrenci->ad, ogrenci->ogrenciNo, oduncVeri->islemler[oduncVeri->sayi-1].tarih);

               kitaplariKaydetAnlik(kitapList);
        oduncKayitlariniKaydetAnlik(oduncVeri);
    } else {
        printf("HATA: '%s' (ISBN: %s) adli kitabin rafta uygun kopyasi bulunamadi. ISLEM BASARISIZ.\n", kitap->ad, kitap->isbn);
    }
}

void oduncKayitlariniKaydetAnlik (OduncVeriYoneticisi* oduncVeri) {
    int i;
    FILE* fp = fopen(ODUNC_KAYITLARI_DOSYASI, "w");

    if (!fp) {
        printf("HATA: %s dosyasi yazmak icin acilamadi!\n", ODUNC_KAYITLARI_DOSYASI);
        return;
    }

    for (i = 0; i < oduncVeri->sayi; i++) {
        fprintf(fp, "%s,%s,%d,%s\n", oduncVeri->islemler[i].ogrenciID, oduncVeri->islemler[i].kitapEtiketNo, oduncVeri->islemler[i].islemTuru, oduncVeri->islemler[i].tarih);
    }
    fclose(fp);
}

void kitapTeslimEt(Ogrenci* ogrList, Kitap* kitapList, OduncVeriYoneticisi* oduncVeri, const char* ogrenciID, const char* kitapEtiketNo) {
    Ogrenci* ogrenci = ogrenciBulByID(ogrList, ogrenciID);
    if (!ogrenci) {
        printf("HATA: %s numarali ogrenci bulunamadi.\n", ogrenciID);
        return;
    }

    char actualISBN[ISBN_UZUNLUK];
    if (strlen(kitapEtiketNo) >= 13) {
        strncpy(actualISBN, kitapEtiketNo, 13);
        actualISBN[13] = '\0';
    } else {
        printf("HATA: Gecersiz kitap etiket numarasi formati: %s\n", kitapEtiketNo);
        return;
    }

    Kitap* kitap = kitapBulByISBN(kitapList, actualISBN);
    if (!kitap) {
        printf("HATA: %s etiket numarasina ait ana kitap bulunamadi (ISBN: %s).\n", kitapEtiketNo, actualISBN);
        return;
    }

    KitapKopya* kopya = kitapKopyasiBulByEtiket(kitap, kitapEtiketNo);
    if (!kopya) {
        printf("HATA: %s etiketli kitap kopyasi bulunamadi.\n", kitapEtiketNo);
        return;
    }
    if (strcmp(kopya->durum, ogrenciID) != 0) {
        printf("HATA: %s etiketli kitap %s tarafindan odunc alinmamis. Mevcut durum: %s\n", kitapEtiketNo, ogrenciID, kopya->durum); 
        return;
    }

    char oduncAlmaTarihiStr[TARIH_UZUNLUK] = "";
    int oduncKaydiBulundu = 0;
    int cezaUygulandi = 0;

    int i = oduncVeri->sayi - 1;
    int devam = 1;

    while(i >= 0 && devam) {
        if (strcmp(oduncVeri->islemler[i].ogrenciID, ogrenciID) == 0 &&
            strcmp(oduncVeri->islemler[i].kitapEtiketNo, kitapEtiketNo) == 0 &&
            oduncVeri->islemler[i].islemTuru == 0) {

            strcpy(oduncAlmaTarihiStr, oduncVeri->islemler[i].tarih);
            oduncKaydiBulundu = 1;
            devam = 0;
        }
        i--;
    }

    if (oduncKaydiBulundu) {
        char bugunStr[TARIH_UZUNLUK]; bugununTarihiniAl(bugunStr);
        Tarih tOdunc = stringToTarih(oduncAlmaTarihiStr);
        Tarih tBugun = stringToTarih(bugunStr);
        int fark = TarihlerArasiGunSayar(tOdunc, tBugun);

        if (fark == -1) {
            printf("UYARI: Tarih farki hesaplanirken hata olustu. Ceza kontrolu yapilamiyor.\n");
        } else {
            printf("Kitap %s tarihinde odunc alinmis, bugun %s. Gecen gun sayisi: %d gun.\n", oduncAlmaTarihiStr, bugunStr, fark);
            if (fark > 15) {
                ogrenci->puan -= 10;
                printf("%s numarali ogrenciye gec teslim icin -10 ceza puani verildi. Yeni Puan: %d\n",
                       ogrenciID, ogrenci->puan);
                cezaUygulandi = 1;
            }
        }
    } else {
        printf("UYARI: %s etiketli kitap icin %s adina aktif odunc alma kaydi bulunamadi. Ceza hesaplanamiyor.\n", kitapEtiketNo, ogrenciID);
    }

    strncpy(kopya->durum, "RAFTA", DURUM_UZUNLUK - 1); 
    kopya->durum[DURUM_UZUNLUK - 1] = '\0';

    OduncIslem* yeniIslemler = realloc(oduncVeri->islemler, (oduncVeri->sayi + 1) * sizeof(OduncIslem));

    if (!yeniIslemler) {
        printf("HATA: Teslim islemi icin bellek ayrilamadi!\n");
        strncpy(kopya->durum, ogrenciID, DURUM_UZUNLUK - 1); 
        kopya->durum[DURUM_UZUNLUK - 1] = '\0'; 
        return;
    }
    oduncVeri->islemler = yeniIslemler;
    strncpy(oduncVeri->islemler[oduncVeri->sayi].ogrenciID, ogrenciID, OGRENCI_NO_UZUNLUK - 1);

    oduncVeri->islemler[oduncVeri->sayi].ogrenciID[OGRENCI_NO_UZUNLUK - 1] = '\0';
    strncpy(oduncVeri->islemler[oduncVeri->sayi].kitapEtiketNo, kitapEtiketNo, ETIKET_NO_UZUNLUK - 1);
    oduncVeri->islemler[oduncVeri->sayi].kitapEtiketNo[ETIKET_NO_UZUNLUK - 1] = '\0';

    oduncVeri->islemler[oduncVeri->sayi].islemTuru = 1;
    bugununTarihiniAl(oduncVeri->islemler[oduncVeri->sayi].tarih);
    (oduncVeri->sayi)++;

    printf("'%s' etiketli kitap %s (%s) tarafindan %s tarihinde teslim edildi.\n",
           kitapEtiketNo, ogrenci->ad, ogrenci->ogrenciNo, oduncVeri->islemler[oduncVeri->sayi-1].tarih);

    kitaplariKaydetAnlik(kitapList);
    oduncKayitlariniKaydetAnlik(oduncVeri);

    if (cezaUygulandi) {
        ogrencileriKaydetAnlik(ogrList);
    }
}

void kitapVeKopyalariniYazdir (Kitap* kitap) {
    if (kitap) {
        printf("Kitap Adi: %s, ISBN: %s, Kopya Adedi: %d\n", kitap->ad, kitap->isbn, kitap->adet);
        KitapKopya* kopya = kitap->kopyalar;

        if (!kopya)
            printf("  >> Bu kitaba ait kopya bulunmamaktadir.\n");
        else {
            printf("  Kopyalar:\n");
            while (kopya != NULL) {
                printf("    Etiket: %s, Durum: %s\n", kopya->etiketNumarasi, kopya->durum);
                kopya = kopya->next;
            }
        }
    }
}


void kitaplariListele (Kitap* kitapList) {
    kitapListniGezVeIsle(kitapList, kitapVeKopyalariniYazdir, herZamanDogruKosuluKitap, NULL,
                           "\n--- TUM KITAPLAR VE KOPYALARI ---\n", "Listelenecek kitap yok.");
}

void raftakiKitaplariListele (Kitap* kitapList) {
    printf("\n--- RAFTAKI KITAPLAR ---\n");
    Kitap* ktp = kitapList;
    int genelBulundu = 0;
    int baslikYazdirildi = 0;

    if (!ktp) {
        printf("Kayitli kitap yok.\n");
    } else {
        while (ktp != NULL) {
            KitapKopya* kopya = ktp->kopyalar;
            while (kopya != NULL) {
                if (strcmp(kopya->durum, "RAFTA") == 0) {
                    if (!baslikYazdirildi) {
                        printf("Kitap Adi                      ISBN            Etiket No\n");
                        printf("------------------------------ --------------- ------------------------\n");
                        baslikYazdirildi = 1;
                    }
                    printf("%-30.30s %-15s %s\n", ktp->ad, ktp->isbn, kopya->etiketNumarasi);
                    genelBulundu = 1;
                }
                kopya = kopya->next;
            }
            ktp = ktp->next;
        }
    }
    if (!genelBulundu && ktp != NULL) { // rafta kitap yoksa ama kitap listesi boş değilse
        printf("Rafta hic kitap bulunmamaktadir.\n");
    }

    printf("------------------------------------------------------------------\n");
}

void kitapTeslimEtmemisOgrencileriListele(Ogrenci* ogrList, Kitap* kitapList) {
    printf("\n--- KITAP TESLIM ETMEMIS OGRENCILER ---\n");
    int genelBulundu = 0; Ogrenci* ogr = ogrList;
    if (!ogr) {
        printf("Kayitli ogrenci yok.\n");
        return;
    }

    while (ogr != NULL) {
        Kitap* ktp = kitapList;
        char adSoyadBirlestirilmis[MAX_AD_SOYAD * 2 + 2];
        snprintf(adSoyadBirlestirilmis, sizeof(adSoyadBirlestirilmis), "%s %s", ogr->ad, ogr->soyad);

        while (ktp != NULL) {
            KitapKopya* kopya = ktp->kopyalar;
            while (kopya != NULL) {
                if (strcmp(kopya->durum, ogr->ogrenciNo) == 0) {
                    if (!genelBulundu) {
                        printf("Ogrenci No  Ad Soyad          Kitap Adi (ISBN)             Kitap Etiket No\n");
                        printf("----------- ----------------- ------------------------------ ------------------------\n");
                        genelBulundu = 1;
                    }
                    char kitapBilgi[MAX_KITAP_AD + ISBN_UZUNLUK + 5];
                    snprintf(kitapBilgi, sizeof(kitapBilgi), "%s (%s)", ktp->ad, ktp->isbn);
                    printf("%-11s %-17.17s %-30.30s %s\n", ogr->ogrenciNo, adSoyadBirlestirilmis, kitapBilgi, kopya->etiketNumarasi);
                }
                kopya = kopya->next;
            }
            ktp = ktp->next;
        }
        ogr = ogr->next;
    }
    if (!genelBulundu)
        printf("Kimsenin uzerinde kitap bulunmamaktadir.\n");
    printf("--------------------------------------------------------------------------------------\n");
}

void zamanindaTeslimEdilmeyenKitaplariListele(Kitap* kitapList, OduncVeriYoneticisi* oduncVeri) {
    printf("\n--- ZAMANINDA TESLIM EDILMEYEN KITAPLAR (15 GUNDEN FAZLA ODUNCTE OLANLAR) ---\n");
    char bugunStr[TARIH_UZUNLUK];
    bugununTarihiniAl(bugunStr);

    Tarih tBugun = stringToTarih(bugunStr);
    int bulunduGenel = 0;

    Kitap* ktp = kitapList;
    if(!ktp) {
        printf("Kayitli kitap yok.\n");
        return;
    }

    while (ktp != NULL) {
        KitapKopya* kopya = ktp->kopyalar;

        while (kopya != NULL) {
            if (strcmp(kopya->durum, "RAFTA") != 0) {
                char ogrenciIDTut[OGRENCI_NO_UZUNLUK];
                strncpy(ogrenciIDTut, kopya->durum, OGRENCI_NO_UZUNLUK - 1);
                ogrenciIDTut[OGRENCI_NO_UZUNLUK - 1] = '\0';

                char oduncAlmaTarihiStr[TARIH_UZUNLUK] = "";
                int oduncKaydiBulundu = 0;

                int i = oduncVeri->sayi - 1;
                int devam = 1;
                while(i >= 0 && devam) {
                    if (strcmp(oduncVeri->islemler[i].kitapEtiketNo, kopya->etiketNumarasi) == 0 &&
                        strcmp(oduncVeri->islemler[i].ogrenciID, ogrenciIDTut) == 0 &&
                        oduncVeri->islemler[i].islemTuru == 0) {

                        strcpy(oduncAlmaTarihiStr, oduncVeri->islemler[i].tarih);
                        oduncKaydiBulundu = 1;
                        devam = 0;
                    }
                    i--;
                }

                if (oduncKaydiBulundu) {
                    Tarih tOdunc = stringToTarih(oduncAlmaTarihiStr);
                    int fark = TarihlerArasiGunSayar(tOdunc, tBugun);
                    printf("\n%d", fark);
                    if (fark > 15) {
                        if(!bulunduGenel) {
                            printf("Ogrenci No  Kitap Adi (ISBN)             Kitap Etiket No         Odunc Tarihi Gecen Gun\n");
                            printf("----------- ------------------------------ ----------------------- ------------ ----------\n");
                            bulunduGenel = 1;
                        }
                        char kitapBilgi[MAX_KITAP_AD + ISBN_UZUNLUK + 5];
                        snprintf(kitapBilgi, sizeof(kitapBilgi), "%s (%s)", ktp->ad, ktp->isbn);
                        printf("%-11s %-30.30s %-23s %-12s %10d\n", ogrenciIDTut, kitapBilgi, kopya->etiketNumarasi, oduncAlmaTarihiStr, fark);
                    }
                }
            }
            kopya = kopya->next;
        }
        ktp = ktp->next;
    }
    if (!bulunduGenel)
        printf("Zamaninda teslim edilmeyen (15 gunden fazla oduncte olan) kitap bulunmamaktadir.\n");
    printf("--------------------------------------------------------------------------------------\n");
}

void kitapListniGezVeIsle (Kitap* listeHead, KitapIslemFonksiyonu islemYap, KitapKosulFonksiyonu kosuluSaglarMi,
                             void* kosulVerisi, const char* baslikMesaji, const char* bulunamadiMesaji) {

    if (baslikMesaji)
        printf("%s", baslikMesaji);

        if (!listeHead) {
         printf("%s\n", bulunamadiMesaji ? bulunamadiMesaji : "Liste bos veya listelenecek oge bulunamadi.");
    }

    Kitap* temp = listeHead;
    int enAzBirOgeIsledim = 0;
    while (temp != NULL) {
        if (kosuluSaglarMi == NULL || kosuluSaglarMi(temp, kosulVerisi)) {
            if (islemYap != NULL) {
                islemYap(temp);
                enAzBirOgeIsledim = 1;
            }
        }
        temp = temp->next;
    }
     if (!enAzBirOgeIsledim && listeHead) {
         printf("%s\n", bulunamadiMesaji ? bulunamadiMesaji : "Belirtilen kosula uygun oge bulunamadi.");
    }
    if (baslikMesaji) printf("-----------------------------------\n");
}


void kitaplariYukle(Kitap** kitapListHead) {
    Kitap* ktp = *kitapListHead, *ktp_next;
    while (ktp != NULL) {
        KitapKopya* kopya = ktp->kopyalar;
        KitapKopya* kopya_next;

        while (kopya != NULL) {
            kopya_next = kopya->next;
            free(kopya);
            kopya = kopya_next;
        }

        ktp_next = ktp->next;
        free(ktp);
        ktp = ktp_next;
    }
    *kitapListHead = NULL;

    FILE* fp = fopen(KITAPLAR_DOSYASI, "r");
    if (!fp) {
        printf("%s dosyasi acilamadi (yukleme). Bos liste.\n", KITAPLAR_DOSYASI);
        return;
    }

    char satir[512];

    while (fgets(satir, sizeof(satir), fp)) {
        char kitapAdi[MAX_KITAP_AD], isbn[ISBN_UZUNLUK], etiket[ETIKET_NO_UZUNLUK], durum[DURUM_UZUNLUK];

        if (sscanf(satir, "%99[^,],%13[^,],%24[^,],%49[^\n]", kitapAdi, isbn, etiket, durum) == 4) {
            Kitap* mevcutKitap = kitapBulByISBN(*kitapListHead, isbn);

            if (!mevcutKitap) {
                mevcutKitap = (Kitap*)malloc(sizeof(Kitap));
                if(!mevcutKitap) {
                    printf("Kitap yukleme ana bellek hatasi\n");
                    fclose(fp);
                    return;
                }

                strncpy(mevcutKitap->ad, kitapAdi, MAX_KITAP_AD - 1);
                mevcutKitap->ad[MAX_KITAP_AD - 1] = '\0';

                strncpy(mevcutKitap->isbn, isbn, ISBN_UZUNLUK - 1);
                mevcutKitap->isbn[ISBN_UZUNLUK - 1] = '\0';

                mevcutKitap->adet = 0;
                mevcutKitap->kopyalar = NULL;
                mevcutKitap->next = NULL;

                if (*kitapListHead == NULL || strcmp((*kitapListHead)->ad, mevcutKitap->ad) > 0 ||
                    (strcmp((*kitapListHead)->ad, mevcutKitap->ad) == 0 && strcmp((*kitapListHead)->isbn, mevcutKitap->isbn) > 0)) {
                    mevcutKitap->next = *kitapListHead;
                    *kitapListHead = mevcutKitap;
                } else {
                    Kitap* temp = *kitapListHead;
                    int devam = 1;
                    while (temp->next != NULL && devam) {
                        if (strcmp(temp->next->ad, mevcutKitap->ad) < 0 ||
                            (strcmp(temp->next->ad, mevcutKitap->ad) == 0 && strcmp(temp->next->isbn, mevcutKitap->isbn) < 0)) {
                            temp = temp->next;
                        } else {
                            devam = 0;
                        }
                    }
                    mevcutKitap->next = temp->next;
                    temp->next = mevcutKitap;
                }
            }
            KitapKopya* yeniKopya = (KitapKopya*)malloc(sizeof(KitapKopya));

            if(!yeniKopya) {
                printf("Kitap yukleme kopya bellek hatasi\n");
                fclose(fp);
                return;
            }
            strncpy(yeniKopya->etiketNumarasi, etiket, ETIKET_NO_UZUNLUK - 1);
            yeniKopya->etiketNumarasi[ETIKET_NO_UZUNLUK - 1] = '\0';

            strncpy(yeniKopya->durum, durum, DURUM_UZUNLUK - 1);
            yeniKopya->durum[DURUM_UZUNLUK - 1] = '\0';

            yeniKopya->next = mevcutKitap->kopyalar;
            mevcutKitap->kopyalar = yeniKopya;
            mevcutKitap->adet++;
        }
    }
    fclose(fp);
    printf("%s dosyasindan kitaplar ve kopyalari yuklendi.\n", KITAPLAR_DOSYASI);
}



void oduncKayitlariniYukle(OduncVeriYoneticisi* oduncVeri) {
    free(oduncVeri->islemler);
    oduncVeri->islemler = NULL;
    oduncVeri->sayi = 0;

    FILE* fp = fopen(ODUNC_KAYITLARI_DOSYASI, "r");
    if (!fp) {
        printf("%s dosyasi acilamadi (yukleme). Bos odunc kayit listesi.\n", ODUNC_KAYITLARI_DOSYASI);
        return;
    }

    char satir[256];

    while (fgets(satir, sizeof(satir), fp)) {
        char ogrID[OGRENCI_NO_UZUNLUK], etiket[ETIKET_NO_UZUNLUK], tarih[TARIH_UZUNLUK];
        int tur;

        if (sscanf(satir, "%8[^,],%24[^,],%d,%10[^\n]", ogrID, etiket, &tur, tarih) == 4) {
            OduncIslem* yeniKayitlar = realloc(oduncVeri->islemler, (oduncVeri->sayi + 1) * sizeof(OduncIslem));

            if (!yeniKayitlar) {
                printf("HATA: Odunc kaydi yukleme bellek hatasi!\n");
                fclose(fp);
                return;
            }
            oduncVeri->islemler = yeniKayitlar;
            strncpy(oduncVeri->islemler[oduncVeri->sayi].ogrenciID, ogrID,OGRENCI_NO_UZUNLUK - 1);
            oduncVeri->islemler[oduncVeri->sayi].ogrenciID[OGRENCI_NO_UZUNLUK - 1] = '\0';

            strncpy(oduncVeri->islemler[oduncVeri->sayi].kitapEtiketNo, etiket,ETIKET_NO_UZUNLUK - 1);
            oduncVeri->islemler[oduncVeri->sayi].kitapEtiketNo[ETIKET_NO_UZUNLUK - 1] = '\0';

            oduncVeri->islemler[oduncVeri->sayi].islemTuru = tur;
            strncpy(oduncVeri->islemler[oduncVeri->sayi].tarih, tarih,TARIH_UZUNLUK - 1);

            oduncVeri->islemler[oduncVeri->sayi].tarih[TARIH_UZUNLUK - 1] = '\0';
            (oduncVeri->sayi)++;
        }
    }
    fclose(fp);
    printf("%s dosyasindan odunc kayitlari yuklendi.\n", ODUNC_KAYITLARI_DOSYASI);
}

void kitapYazarIliskileriniYukle(KitapYazarVeriYoneticisi* kitapYazarDB) {
    free(kitapYazarDB->iliskiler);
    kitapYazarDB->iliskiler = NULL;
    kitapYazarDB->sayi = 0;

    FILE* fp = fopen(KITAP_YAZAR_DOSYASI, "r");
    if (!fp) {
        printf("%s dosyasi acilamadi (yukleme). Bos iliski listesi.\n", KITAP_YAZAR_DOSYASI);
        return;
    }

    char satir[256];

    while (fgets(satir, sizeof(satir), fp)) {
        char isbn[ISBN_UZUNLUK]; int yazarID;
        if (sscanf(satir, "%13[^,],%d", isbn, &yazarID) == 2) {
            KitapYazarIliski* yeniIliskiler = realloc(kitapYazarDB->iliskiler, (kitapYazarDB->sayi + 1) * sizeof(KitapYazarIliski));
            if (!yeniIliskiler) {
                printf("HATA: Kitap-Yazar iliskisi yukleme bellek hatasi!\n");
                fclose(fp);
                return;
            }

            kitapYazarDB->iliskiler = yeniIliskiler;
            strncpy(kitapYazarDB->iliskiler[kitapYazarDB->sayi].isbn, isbn, ISBN_UZUNLUK - 1);
            kitapYazarDB->iliskiler[kitapYazarDB->sayi].isbn[ISBN_UZUNLUK - 1] = '\0';

            kitapYazarDB->iliskiler[kitapYazarDB->sayi].yazarID = yazarID;
            (kitapYazarDB->sayi)++;
        }
    }
    fclose(fp);
    printf("%s dosyasindan kitap-yazar iliskileri yuklendi.\n", KITAP_YAZAR_DOSYASI);
}


void tumVerileriYukle(Yazar** yazarHead, Ogrenci** ogrListHead, Kitap** kitapListHead,
                      int* yeniYazarID, KitapYazarVeriYoneticisi* kitapYazarDB, OduncVeriYoneticisi* oduncVeri) {
    yazarlariYukle(yazarHead, yeniYazarID);
    ogrencileriYukle(ogrListHead);
    kitaplariYukle(kitapListHead);
    kitapYazarIliskileriniYukle(kitapYazarDB);
    oduncKayitlariniYukle(oduncVeri);
}


void ogrenciMenuGoster(Ogrenci** ogrListHead, Kitap* kitapList, OduncVeriYoneticisi* oduncVeri) {
    int secim = 0;
    char no[OGRENCI_NO_UZUNLUK], ad[MAX_AD_SOYAD], soyad[MAX_AD_SOYAD];
    int puan;
    char isbn_val[ISBN_UZUNLUK];
    char etiket_val[ETIKET_NO_UZUNLUK];
    int devamMenu = 1;

    while(devamMenu) {
        printf("\n--- OGRENCI ISLEMLERI ---\n"
               "1- Ogrenci Ekle\n2- Ogrenci Sil\n3- Ogrenci Guncelle\n"
               "4- Ogrenci Bilgisi Goruntule (ID)\n6- Tum Ogrencileri Listele\n"
               "7- Kitap Teslim Etmemis Ogrencileri Listele\n8- Cezali Ogrencileri Listele\n"
               "9- Kitap Odunc Al\n10- Kitap Teslim Et\n99- Ana Menuye Don\nSeciminiz: ");
        if (scanf("%d", &secim) != 1) {
            printf("Gecersiz giris. Lutfen bir sayi girin.\n");
            secim = 0;
        }
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);


        if (secim == 1) {
            printf("Ogrenci No (8 hane): "); 
            guvenliInputAlma(no, OGRENCI_NO_UZUNLUK);
            printf("Ad: "); 
            guvenliInputAlma(ad, MAX_AD_SOYAD);
            printf("Soyad: "); 
            guvenliInputAlma(soyad, MAX_AD_SOYAD);

            ogrenciEkle(ogrListHead, no, ad, soyad);
        } else if (secim == 2) {
            printf("Silinecek Ogrenci No: "); 
            guvenliInputAlma(no, OGRENCI_NO_UZUNLUK);
            ogrenciSil(ogrListHead, no);

        } else if (secim == 3) {
            printf("Guncellenecek Ogrenci No: "); 
            guvenliInputAlma(no, OGRENCI_NO_UZUNLUK);
            Ogrenci* ogr_ptr = ogrenciBulByID(*ogrListHead, no);
            if (ogr_ptr) {
                printf("Yeni Ad (Mevcut: %s): ", ogr_ptr->ad); 
                guvenliInputAlma(ad, MAX_AD_SOYAD);
                printf("Yeni Soyad (Mevcut: %s): ", ogr_ptr->soyad); 
                guvenliInputAlma(soyad, MAX_AD_SOYAD);
                printf("Yeni Puan (Mevcut: %d): ", ogr_ptr->puan);

                if(scanf("%d", &puan)!=1) {
                    printf("Puan icin gecersiz giris.\n");
                    while(getchar()!='\n');

                } else {
                    while(getchar()!='\n');
                    ogrenciGuncelle(*ogrListHead, no, ad, soyad, puan);
                }

            } else {
                printf("%s numarali ogrenci bulunamadi.\n", no);
            }

        } else if (secim == 4) {
            printf("Goruntulenecek Ogrenci No: ");
            guvenliInputAlma(no, OGRENCI_NO_UZUNLUK);

            Ogrenci* ogr_ptr = ogrenciBulByID(*ogrListHead, no);
            if(ogr_ptr){
                printf("Ogrenci Bilgileri:\n");
                printf("  No: %s\n  Ad: %s\n  Soyad: %s\n  Puan: %d\n",
                       ogr_ptr->ogrenciNo, ogr_ptr->ad, ogr_ptr->soyad, ogr_ptr->puan);
                printf("Kitap Hareketleri:\n");
                int hareketBulundu = 0;
                int i; 
                for(i=0; i < oduncVeri->sayi; i++){
                    if(strcmp(oduncVeri->islemler[i].ogrenciID, no) == 0){
                        printf("  - Etiket: %s, Islem: %s, Tarih: %s\n", oduncVeri->islemler[i].kitapEtiketNo,
                               (oduncVeri->islemler[i].islemTuru == 0 ? "Odunc Alma" : "Teslim Etme"), oduncVeri->islemler[i].tarih);

                        hareketBulundu = 1;
                    }
                }
                if(!hareketBulundu)
                printf("  Bu ogrenciye ait kitap hareketi bulunamadi.\n");

            } else {
                printf("%s numarali ogrenci bulunamadi.\n", no);
            }

        } else if (secim == 6) {
            ogrencileriListele(*ogrListHead);

        } else if (secim == 7) {
            kitapTeslimEtmemisOgrencileriListele(*ogrListHead, kitapList);

        } else if (secim == 8) {
            cezaliOgrencileriListele(*ogrListHead);

        } else if (secim == 9) {
            printf("Odunc Alacak Ogrenci No: ");
            guvenliInputAlma(no, OGRENCI_NO_UZUNLUK);

            printf("Odunc Alinacak Kitabin ISBN'i: ");
            guvenliInputAlma(isbn_val, ISBN_UZUNLUK);
            kitapOduncAl(*ogrListHead, kitapList, oduncVeri, no, isbn_val);

        } else if (secim == 10) {
            printf("Teslim Edecek Ogrenci No: ");
            guvenliInputAlma(no, OGRENCI_NO_UZUNLUK);

            printf("Teslim Edilecek Kitabin Etiket No: ");
            guvenliInputAlma(etiket_val, ETIKET_NO_UZUNLUK);
            kitapTeslimEt(*ogrListHead, kitapList, oduncVeri, no, etiket_val);

        } else if (secim == 99) {
            printf("Ana menuye donuluyor...\n");
            devamMenu = 0;

        } else if (secim != 0) {
            printf("Gecersiz secim!\n");
        }
    }
}

void kitapMenuGoster(Kitap** kitapListHead, Yazar* yazarListesi, KitapYazarVeriYoneticisi* kitapYazarDB, OduncVeriYoneticisi* oduncVeri) { 
    int secim = 0, devamMenu = 1, adet_val, yazarID_val, eskiYazarID_val;
    char ad_val[MAX_KITAP_AD], isbn_val[ISBN_UZUNLUK]; 

    while(devamMenu) {
        printf("\n--- KITAP ISLEMLERI ---\n"
               "1- Kitap Ekle\n2- Kitap Sil\n3- Kitap Guncelle (Adini)\n"
               "4- Kitap Bilgisi Goruntule (Adina Gore)\n5- Raftaki Kitaplari Listele\n"
               "6- Zamaninda Teslim Edilmeyen Kitaplari Listele\n"
               "7- Kitap-Yazar Eslestir\n8- Kitabin Yazarini Guncelle\n"
               "9- Tum Kitaplari Listele\n99- Ana Menuye Don\nSeciminiz: ");
        if (scanf("%d", &secim) != 1) {
            printf("Gecersiz giris.\n");
            while(getchar()!='\n');
            secim=0;
        }
        else {
            while(getchar()!='\n');
        }

        if (secim == 1) {
            printf("Kitap Adi: ");
            guvenliInputAlma(ad_val, MAX_KITAP_AD);

            printf("ISBN (13 hane): ");
            guvenliInputAlma(isbn_val, ISBN_UZUNLUK);

            printf("Adet: ");
            if(scanf("%d", &adet_val) != 1) {
                printf("Adet icin gecersiz giris.\n");
                while(getchar()!='\n');
            } else {
                while(getchar()!='\n');
                kitapEkle(kitapListHead, ad_val, isbn_val, adet_val);
            }

        } else if (secim == 2) {
            printf("Silinecek Kitabin ISBN'i: ");
            guvenliInputAlma(isbn_val, ISBN_UZUNLUK);

            kitapSil(kitapListHead, isbn_val);

        } else if (secim == 3) {
            printf("Guncellenecek Kitabin ISBN'i: ");
            guvenliInputAlma(isbn_val, ISBN_UZUNLUK);

            Kitap* ktp_ptr = kitapBulByISBN(*kitapListHead, isbn_val);
            if (ktp_ptr) {
                printf("Yeni Kitap Adi (Mevcut: %s): ", ktp_ptr->ad);
                guvenliInputAlma(ad_val, MAX_KITAP_AD);

                kitapGuncelle(*kitapListHead, isbn_val, ad_val);
            } else {
                printf("%s ISBN'li kitap bulunamadi.\n", isbn_val);
            }

        } else if (secim == 4) {
            printf("Goruntulenecek Kitap Adi (veya bir kismi): ");
            guvenliInputAlma(ad_val, MAX_KITAP_AD);

            Kitap* temp = *kitapListHead; int bulundu = 0;
            while(temp != NULL){
                if(strstr(temp->ad, ad_val) != NULL){
                    printf("Kitap Adi: %s, ISBN: %s, Toplam Kopya: %d\n", temp->ad, temp->isbn, temp->adet);
                    KitapKopya* k_ptr = temp->kopyalar;

                    if(!k_ptr)
                        printf("  >> Kopyasi yok.\n");
                    else
                        printf("  Kopyalar:\n");

                    while(k_ptr != NULL) {
                        printf("    Etiket: %s, Durum: %s\n", k_ptr->etiketNumarasi, k_ptr->durum);
                        k_ptr = k_ptr->next;
                    }
                    bulundu = 1;
                }
                temp = temp->next;
            }
            if(!bulundu)
                printf("'%s' adini iceren kitap bulunamadi.\n", ad_val);

        } else if (secim == 5) {
            raftakiKitaplariListele(*kitapListHead);

        } else if (secim == 6) {
             zamanindaTeslimEdilmeyenKitaplariListele(*kitapListHead, oduncVeri);

        } else if (secim == 7) {
            printf("Eslestirilecek Kitabin ISBN'i: ");
            guvenliInputAlma(isbn_val, ISBN_UZUNLUK);

            printf("Eslestirilecek Yazar ID: ");

            if(scanf("%d", &yazarID_val)!=1) {
                printf("Yazar ID icin gecersiz giris.\n");
                while(getchar()!='\n');
            } else {
                while(getchar()!='\n');
                kitapYazarEslestir(kitapYazarDB, *kitapListHead, yazarListesi, isbn_val, yazarID_val);
            }

        } else if (secim == 8) {
            printf("Yazari Guncellenecek Kitabin ISBN'i: ");
            guvenliInputAlma(isbn_val, ISBN_UZUNLUK);

            printf("Degistirilecek Eski Yazar ID: ");
            if(scanf("%d", &eskiYazarID_val)!=1) {
                printf("Eski Yazar ID icin gecersiz giris.\n");
                while(getchar()!='\n');
            } else {
                while(getchar()!='\n');
                printf("Yeni Yazar ID (-1 yazar yok): ");

                if(scanf("%d", &yazarID_val)!=1) {
                    printf("Yeni Yazar ID icin gecersiz giris.\n");
                    while(getchar()!='\n');
                } else {
                    while(getchar()!='\n');
                    kitabinYazariniGuncelle(kitapYazarDB, *kitapListHead, yazarListesi, isbn_val, eskiYazarID_val, yazarID_val);
                }
            }

        } else if (secim == 9) {
            kitaplariListele(*kitapListHead);
        } else if (secim == 99) {
            printf("Ana menuye donuluyor...\n");
            devamMenu = 0;

        } else if (secim !=0 ) {
            printf("Gecersiz secim!\n");
        }
    }
}

void yazarMenuGoster(Yazar** yazarHead, int* yeniYazarID, Kitap* kitapList, KitapYazarVeriYoneticisi* kitapYazarDB) {
    int devamMenu = 1, secim = 0, yazarID_val;
    char ad_val[MAX_AD_SOYAD], soyad_val[MAX_AD_SOYAD];

    while(devamMenu) {
        printf("\n--- YAZAR ISLEMLERI ---\n"
               "1- Yazar Ekle\n2- Yazar Sil\n3- Yazar Guncelle\n"
               "4- Yazar Bilgisi Goruntule (Adina Gore)\n5- Tum Yazarlari Listele\n"
               "99- Ana Menuye Don\nSeciminiz: ");

        if (scanf("%d", &secim) != 1) {
            printf("Gecersiz giris.\n");
            while(getchar()!='\n');
            secim=0;
        } else {
            while (getchar() != '\n');
        }

        if (secim == 1) {
            printf("Yazar Adi: ");
            guvenliInputAlma(ad_val, MAX_AD_SOYAD);

            printf("Yazar Soyadi: ");
            guvenliInputAlma(soyad_val, MAX_AD_SOYAD);

            yazarEkle(yazarHead, yeniYazarID, ad_val, soyad_val);

        } else if (secim == 2) {
            printf("Silinecek Yazar ID: ");

            if(scanf("%d", &yazarID_val)!=1) {
                printf("Yazar ID icin gecersiz giris.\n");
                while(getchar()!='\n');
            } else {
                while(getchar()!='\n');
                yazarSil(yazarHead, kitapYazarDB, yazarID_val);
            }

        } else if (secim == 3) {
            printf("Guncellenecek Yazar ID: ");
            if(scanf("%d", &yazarID_val)!=1) {
                printf("Yazar ID icin gecersiz giris.\n");
                while(getchar()!='\n');
            } else {
                while(getchar()!='\n');
                Yazar* yzr_ptr = yazarBulByID(*yazarHead, yazarID_val);
                if (yzr_ptr) {
                    printf("Yeni Ad (Mevcut: %s): ", yzr_ptr->ad);
                    guvenliInputAlma(ad_val, MAX_AD_SOYAD);

                    printf("Yeni Soyad (Mevcut: %s): ", yzr_ptr->soyad);
                    guvenliInputAlma(soyad_val, MAX_AD_SOYAD);

                    yazarGuncelle(*yazarHead, yazarID_val, ad_val, soyad_val);
                } else {
                    printf("%d ID'li yazar bulunamadi.\n", yazarID_val);
                }
            }

        } else if (secim == 4) {
            printf("Goruntulenecek Yazar Adi (veya bir kismi): ");
            guvenliInputAlma(ad_val, MAX_AD_SOYAD);

            Yazar* temp = *yazarHead;
            int bulundu = 0, kitapBulundu = 0, i;

            printf("Bulunan Yazarlar ve Kitaplari:\n");
            while(temp != NULL){
                if(strstr(temp->ad, ad_val) != NULL || strstr(temp->soyad, ad_val) != NULL){
                    printf("Yazar ID: %d, Ad: %s, Soyad: %s\n  Kitaplari:\n", temp->yazarID, temp->ad, temp->soyad);
                    kitapBulundu = 0; 
                    for(i = 0; i < kitapYazarDB->sayi; i++){
                        if(kitapYazarDB->iliskiler[i].yazarID == temp->yazarID){
                            Kitap* ktp_ptr = kitapBulByISBN(kitapList, kitapYazarDB->iliskiler[i].isbn);

                            if(ktp_ptr) {
                                printf("    - %s (ISBN: %s)\n", ktp_ptr->ad, ktp_ptr->isbn);
                                kitapBulundu = 1;
                            }
                        }
                    }
                    if(!kitapBulundu)
                        printf("    Bu yazara ait kayitli kitap bulunamadi.\n");
                    bulundu = 1;
                }
                temp = temp->next;
            }
            if(!bulundu)
                printf("'%s' adini iceren yazar bulunamadi.\n", ad_val);

        } else if (secim == 5) { 
            yazarlariListele(*yazarHead);
        } else if (secim == 99) {
            printf("Ana menuye donuluyor...\n");
            devamMenu = 0;

        } else if (secim != 0) {
            printf("Gecersiz secim!\n");
        }
    }
}

void anaMenuGoster(Yazar** yazarHead, Ogrenci** ogrListHead, Kitap** kitapListHead,
                   int* yeniYazarID, KitapYazarVeriYoneticisi* kitapYazarDB, OduncVeriYoneticisi* oduncVeri) {
    int secim = 0, devamAnaMenu = 1;

    while (devamAnaMenu) {
        printf("\nANA MENU\n1- Ogrenci Islemleri\n2- Kitap Islemleri\n3- Yazar Islemleri\n4- Cikis\nSeciminiz: ");
        if (scanf("%d", &secim) != 1) {
            printf("Gecersiz giris. Sayi girin.\n");
            while(getchar()!='\n');
            secim = 0;

        } else {
            while(getchar()!='\n');

            switch (secim) {
                case 1:
                    ogrenciMenuGoster(ogrListHead, *kitapListHead, oduncVeri);
                    break;
                case 2:
                    kitapMenuGoster(kitapListHead, *yazarHead, kitapYazarDB, oduncVeri);
                    break;
                case 3:
                    yazarMenuGoster(yazarHead, yeniYazarID, *kitapListHead, kitapYazarDB);
                    break;
                case 4:
                    printf("Program sonlandiriliyor...\n");
                    devamAnaMenu = 0;
                    break;
                default:
                    if (secim != 0) printf("Gecersiz secim (1-4 arasi girin).\n");
                    break;
            }
        }
    }
}


void bellekTemizle(Yazar** yazarHead, Ogrenci** ogrListHead, Kitap** kitapListHead,
                   KitapYazarVeriYoneticisi* kitapYazarDB, OduncVeriYoneticisi* oduncVeri) {
    Yazar* yzr = *yazarHead, *yazarNext;
    while (yzr != NULL) {
        yazarNext = yzr->next;
        free(yzr);
        yzr = yazarNext;
    }
    *yazarHead = NULL;

    Ogrenci* ogr = *ogrListHead;
    Ogrenci* ogr_next;

    while (ogr != NULL) {
        ogr_next = ogr->next;
        free(ogr);
        ogr = ogr_next;
    }
    *ogrListHead = NULL;

    Kitap* ktp = *kitapListHead;
    Kitap* ktp_next;

    while (ktp != NULL) {
        KitapKopya* kopya = ktp->kopyalar;
        KitapKopya* kopya_next; 

        while (kopya != NULL) {
            kopya_next = kopya->next;
            free(kopya);
            kopya = kopya_next;
        }
        ktp_next = ktp->next;
        free(ktp);
        ktp = ktp_next;
    }
    *kitapListHead = NULL;

    free(kitapYazarDB->iliskiler);
    kitapYazarDB->iliskiler = NULL;
    kitapYazarDB->sayi = 0;

    free(oduncVeri->islemler);
    oduncVeri->islemler = NULL;
    oduncVeri->sayi = 0;

    printf("Tum bellek temizlendi.\n");
}