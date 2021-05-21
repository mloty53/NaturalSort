#include<iostream>
#include<time.h>
#include<fstream>
#include <string>
using namespace std;
#define BLOCKING_FACTOR 10 // rozmiar bufora plikow
unsigned long long int iloscOdczytowZDysku = 0; 
unsigned long long int iloscZapisowNaDysk = 0; 
unsigned long long int iloscFazRozdzielenia = 0; //na dwie tasmy wyjsciowe
unsigned long long int iloscFazLaczenia = 0; // w jedna tasme wyjsciowa

struct Prostopadloscian{
	double bokA; 
	double bokB; 
	double wysokosc; 
	double objetosc;
	Prostopadloscian() { }
	Prostopadloscian(double a, double b,double h){
		bokA = a;
		bokB = b;
		wysokosc = h;
		objetosc = a * b * h; 
	}
	Prostopadloscian(double a, double b, double h, double v){
		bokA = a;
		bokB = b;
		wysokosc = h;
		objetosc = v; 
	}
};

struct Plik{
	char* nazwaPliku;
	int ostatniaPozycjaWPliku; 
	Plik(char* nazwa, int pozycja){
		nazwaPliku = nazwa;
		ostatniaPozycjaWPliku = pozycja;
	}
	void wyczyszczeniePliku(){
		remove(nazwaPliku);
	}
};

struct OdczytZPliku{
	Prostopadloscian* bufor; // wskaznik na tablicê struktur z rekordami
	int aktualnyIndex; // nr kolejnego indexu do odczytu
	int rozmiarBufora; 
	int ostatniIndex; // nr indexu ostatniego rekordu w buforze
	bool koniecPliku; 
	Plik* plikOdczytu; // wskaznik na plik, z ktorego bedziemy odczytywac dane
	OdczytZPliku(Plik* plik){
		rozmiarBufora = BLOCKING_FACTOR;
		bufor = new Prostopadloscian[rozmiarBufora];
		aktualnyIndex = rozmiarBufora;
		koniecPliku = false;
		ostatniIndex = 0;
		plikOdczytu = plik;
	}
	Prostopadloscian* podajRekord(){// pobranie rekordu, przy kopiowaniu pliku
		if (aktualnyIndex == rozmiarBufora){
			iloscOdczytowZDysku++;
			double bokA, bokB, wysokosc;
			Prostopadloscian* nowyProstopadloscian;
			ifstream wejscie(plikOdczytu->nazwaPliku);
			if (!wejscie.good()){
				cout << "Ups! Nie udalo nam sie otworzyc pliku: " << plikOdczytu->nazwaPliku << endl;
				return NULL;
			}
			wejscie.seekg(plikOdczytu->ostatniaPozycjaWPliku, ios::beg);// przejscie za ostatnio wczytane dane w pliku
			ostatniIndex = 0;
			while ((ostatniIndex < rozmiarBufora) && (!koniecPliku)){
				if (wejscie >> bokA >> bokB >> wysokosc){
					nowyProstopadloscian = new Prostopadloscian(bokA, bokB,wysokosc);
					bufor[ostatniIndex] = *nowyProstopadloscian; 
					cout << "BokA = " << bokA << "m BokB = " << bokB << "m Wysokosc = " << wysokosc << "m Objetosc = " << bokA*bokB*wysokosc << "m" << endl;
					ostatniIndex++;
				}
				else koniecPliku = true;
			}
			plikOdczytu->ostatniaPozycjaWPliku = wejscie.tellg();// miejsce w ktorym skonczylismy 
			wejscie.close(); 
			aktualnyIndex = 0; 
		}
		if (aktualnyIndex >= ostatniIndex && koniecPliku) return NULL;
		aktualnyIndex++;
		return &(bufor[aktualnyIndex - 1]);
	}
	Prostopadloscian* kolejnyRekord(){//pobranie kolejnego rekordu z bufora
		if (aktualnyIndex == rozmiarBufora){
			iloscOdczytowZDysku++;
			double bokA, bokB, wysokosc, objetosc;
			Prostopadloscian* nowyProstopadloscian;
			ifstream wejscie(plikOdczytu->nazwaPliku); 
			if (!wejscie.good()){
				cout << "Ups! Nie udalo nam sie otworzyc pliku: " << plikOdczytu->nazwaPliku << endl;
				return NULL;
			}
			wejscie.seekg(plikOdczytu->ostatniaPozycjaWPliku, ios::beg);// przejscie za ostatnio wczytane dane w pliku
			ostatniIndex = 0;
			while ((ostatniIndex < rozmiarBufora) && (!koniecPliku)){
				if (wejscie >> bokA >> bokB >> wysokosc >> objetosc){
					nowyProstopadloscian = new Prostopadloscian(bokA, bokB, wysokosc, objetosc);
					bufor[ostatniIndex] = *nowyProstopadloscian; 
					ostatniIndex++;
				}
				else koniecPliku = true; 
			}
			plikOdczytu->ostatniaPozycjaWPliku = wejscie.tellg();// miejsce w ktorym skonczylismy 
			wejscie.close();
			aktualnyIndex = 0; 
		}
		if (aktualnyIndex >= ostatniIndex && koniecPliku) return NULL;
		aktualnyIndex++; 
		return &(bufor[aktualnyIndex - 1]); 
	}
};

struct ZapisDoPliku{
	Prostopadloscian* bufor; // wskaznik na bufor
	int aktualnyIndex; // nr kolejnego indexu do zapisu
	int rozmiarBufora; 
	char czyWyswietlic; 
	Plik* plikZapisu; // wskaznik na plik, do ktorego bedziemy zapisywac dane
	ZapisDoPliku(Plik* plik){
		rozmiarBufora = BLOCKING_FACTOR;
		bufor = new Prostopadloscian[rozmiarBufora];
		aktualnyIndex = 0;
		plikZapisu = plik;
		plikZapisu->wyczyszczeniePliku();
		czyWyswietlic = 'n';
	}
	ZapisDoPliku(Plik* plik, char wyswietlanie){
		rozmiarBufora = BLOCKING_FACTOR;
		bufor = new Prostopadloscian[rozmiarBufora];
		aktualnyIndex = 0;
		plikZapisu = plik;
		plikZapisu->wyczyszczeniePliku();
		czyWyswietlic = wyswietlanie;
	}
	~ZapisDoPliku(){
		iloscZapisowNaDysk++;
		ofstream wyjscie(plikZapisu->nazwaPliku, ios::out | ios::app);// plik zapisu pracuje  w trybie dopisywania danych
		for (int i = 0; i < aktualnyIndex; i++){
			wyjscie << bufor[i].bokA << " " << bufor[i].bokB << " " << bufor[i].wysokosc << " " << bufor[i].objetosc << endl;
			if (czyWyswietlic == 't') cout << "BokA = " << bufor[i].bokA << "m BokB = " << bufor[i].bokB << "m Wysokosc = " << bufor[i].wysokosc << "m Objetosc = " << bufor[i].objetosc << "m" << endl;
		}
		if (czyWyswietlic == 't')cout << endl;
		wyjscie.close();
	}
	bool zapiszPrzySortowaniu(Prostopadloscian* rekord){//zapis rekordu w pliku  - sortowanie
		if (aktualnyIndex == rozmiarBufora){
			iloscZapisowNaDysk++;
			ofstream wyjscie(plikZapisu->nazwaPliku, ios::out | ios::app);// plik zapisu pracuje  w trybie dopisywania danych
			for (aktualnyIndex = 0; aktualnyIndex < rozmiarBufora; aktualnyIndex++){
				wyjscie << bufor[aktualnyIndex].bokA << " " << bufor[aktualnyIndex].bokB << " " << bufor[aktualnyIndex].wysokosc << " " << bufor[aktualnyIndex].objetosc << endl;
				if (czyWyswietlic == 't') cout << "BokA = " << bufor[aktualnyIndex].bokA << "m BokB = " << bufor[aktualnyIndex].bokB << "m Wysokosc = " << bufor[aktualnyIndex].wysokosc << "m Objetosc = " << bufor[aktualnyIndex].objetosc << "m" << endl;
			}
			wyjscie.close();
			aktualnyIndex = 0;
		}
		if (rekord == NULL) return false;
		bufor[aktualnyIndex] = *rekord;
		aktualnyIndex++;
		return true;
	}
	bool zapiszPrzyPrzepisywaniu(Prostopadloscian* rekord){//zapis rekordu w pliku przy przepisywaniu pliku
		if (aktualnyIndex == rozmiarBufora){
			iloscZapisowNaDysk++;
			ofstream wyjscie(plikZapisu->nazwaPliku, ios::out | ios::app);// plik zapisu pracuje  w trybie dopisywania danych
			for (aktualnyIndex = 0; aktualnyIndex < rozmiarBufora; aktualnyIndex++){
				wyjscie << bufor[aktualnyIndex].bokA << " " << bufor[aktualnyIndex].bokB << " " << bufor[aktualnyIndex].wysokosc << endl;
				cout << "BokA = " << bufor[aktualnyIndex].bokA << "m BokB = " << bufor[aktualnyIndex].bokB << "m Wysokosc = " << bufor[aktualnyIndex].wysokosc << "m Objetosc = " << bufor[aktualnyIndex].objetosc << "m" << endl;
			}
			wyjscie.close();
			aktualnyIndex = 0;
		}
		if (rekord == NULL) return false;
		bufor[aktualnyIndex] = *rekord;
		aktualnyIndex++;
		return true;
	}
	void zapiszReszte(){//zapisanie do pliku reszty rekordow
		iloscZapisowNaDysk++;
		ofstream wyjscie(plikZapisu->nazwaPliku, ios::out | ios::app);// plik zapisu pracuje  w trybie dopisywania danych
		for (int i = 0; i < aktualnyIndex; i++){
			wyjscie << bufor[i].bokA << " " << bufor[i].bokB << " " << bufor[i].wysokosc << endl;
			cout << "BokA = " << bufor[i].bokA << "m BokB = " << bufor[i].bokB << "m Wysokosc = " << bufor[i].wysokosc << "m Objetosc = " << bufor[i].objetosc << "m" << endl;
		}
		wyjscie.close();
		aktualnyIndex = 0;
	}
};

void losujRekordy(char* sciezkaDoPliku){
	double bokA, bokB, wysokosc, ulamek;
	int iloscRekordow;
	Plik* wyjsciowy = new Plik(sciezkaDoPliku, 0);
	ZapisDoPliku* tasma = new ZapisDoPliku(wyjsciowy);
	Prostopadloscian* rekord = NULL;
	srand(time(NULL));
	cout << "Ile rekordow chesz zebysmy dla Ciebie wygenerowali:"<<endl;
	cin >> iloscRekordow;
	int zakresUlamka = 100;
	int zakresCalkowity = 100;
	while (iloscRekordow != 0){
		ulamek = rand() % zakresUlamka;
		bokA = rand() % zakresCalkowity + ulamek / zakresUlamka;
		ulamek = rand() % zakresUlamka;
		bokB = rand() % zakresCalkowity + ulamek / zakresUlamka;
		ulamek = rand() % zakresUlamka;
		wysokosc = rand() % zakresCalkowity + ulamek / zakresUlamka;
		rekord = new Prostopadloscian(bokA, bokB, wysokosc);
		tasma->zapiszPrzyPrzepisywaniu(rekord);
		iloscRekordow--;
	}
	tasma->zapiszReszte();
	delete tasma;
}

void wpiszRekordy(char* sciezkaDoPliku){
	double bokA, bokB, wysokosc;
	int iloscRekordow;
	Plik* wyjsciowy = new Plik(sciezkaDoPliku, 0);
	ZapisDoPliku* tasma = new ZapisDoPliku(wyjsciowy);
	Prostopadloscian* rekord = NULL;
	cout << "Ile chcesz wpisac rekordow do pliku:"<<endl;
	cin >> iloscRekordow;
	while (iloscRekordow!=0){
		cout << "Podaj dlugosc boku A:" << endl;
		cin >> bokA;
		cout << "Podaj dlugosc boku B:"<<endl;
		cin >> bokB;
		cout << "Podaj wysokosc:"<<endl;
		cin >> wysokosc;
		rekord = new Prostopadloscian(bokA, bokB, wysokosc);
		tasma->zapiszPrzyPrzepisywaniu(rekord);
		iloscRekordow--;
	}
	tasma->zapiszReszte();
	delete tasma;
}

void wczytajRekordyZPliku(char* sciezkaDoPliku){
	string str;
	cout << "Podaj sciezke do pliku:"<<endl;
	getline(cin, str);// wczytanie znaku nowej linii
	getline(cin, str);// wczytanie sciezki do pliku
	int licznik = 0;
	for (int i = 0; i < str.length(); i++) {
		sciezkaDoPliku[i] = str[i];//string przepisany do naszej sciezki do pliku
		licznik = i;
	}
	sciezkaDoPliku[licznik] = '\0';// dopisanie znaku konca tesktu
}

void menu(char* sciezkaDoPliku){
	int wybranyNumer;
	cout << "Wybierz w jaki sposob ma zostac utworzony plik z rekordami:"<<endl;
	cout << "Losowe generowanie rekordow - wybierz 1"<<endl;
	cout << "Wpisanie rekordow przeze uzytkownika z klawiatury - wybierz 2"<<endl;
	cout << "Wczytanie rekordow z pliku zewnetrznego - wybierz 3"<<endl;
	cin >> wybranyNumer;
	switch (wybranyNumer){
	case 1:
		losujRekordy(sciezkaDoPliku);
		break;
	case 2:
		wpiszRekordy(sciezkaDoPliku);
		break;
	case 3:
		wczytajRekordyZPliku(sciezkaDoPliku);
		break;
	}
} 

void przepisaniePliku(char* plikWejsciowy, char* plikWyjsciowy){
	Plik* plikWej = new Plik(plikWejsciowy, 0);
	Plik* plikWyj = new Plik(plikWyjsciowy, 0);
	OdczytZPliku* buforOryginalny = new OdczytZPliku(plikWej);
	ZapisDoPliku* buforKopia = new ZapisDoPliku(plikWyj);
	cout << "Plik przed posortowaniem: "<<endl;
	while (buforKopia->zapiszPrzySortowaniu(buforOryginalny->podajRekord()));
	delete buforOryginalny;
	delete buforKopia;
}

void rozdzielanieNaTasmy(){
	iloscFazRozdzielenia++;
	char sciezkaWej[] = "kopiaWej.txt";
	char sciezkaWyj1[] = "tasma1.txt";
	char sciezkaWyj2[] = "tasma2.txt";
	Plik* plikWejsciowy = new Plik(sciezkaWej, 0);
	Plik* plikWyjsciowy1 = new Plik(sciezkaWyj1, 0);
	Plik* plikWyjsciowy2 = new Plik(sciezkaWyj2, 0);
	OdczytZPliku* tasmaWej = new OdczytZPliku(plikWejsciowy);
	ZapisDoPliku* tasmaWyj1 = new ZapisDoPliku(plikWyjsciowy1);
	ZapisDoPliku* tasmaWyj2 = new ZapisDoPliku(plikWyjsciowy2);
	Prostopadloscian* rekord = NULL;
	double poprzedniaWartosc = 0;
	ZapisDoPliku* tasma = tasmaWyj1;
	while (1){
		rekord = tasmaWej->kolejnyRekord();
		if (rekord == NULL) break;
		if (poprzedniaWartosc > rekord->objetosc){
			if (tasma == tasmaWyj1) tasma = tasmaWyj2;
			else tasma = tasmaWyj1;
		}
		tasma->zapiszPrzySortowaniu(rekord);
		poprzedniaWartosc = rekord->objetosc;
	}
	delete tasmaWyj1;
	delete tasmaWyj2;
	delete tasmaWej;
}

bool laczenieNaTasme(char czyWyswietlic){
	iloscFazLaczenia++;
	char sciezkaWyj[] = "kopiaWej.txt";
	char sciezkaWej1[] = "tasma1.txt";
	char sciezkaWej2[] = "tasma2.txt";
	Plik* plikWejsciowy1 = new Plik(sciezkaWej1, 0);
	Plik* plikWejsciowy2 = new Plik(sciezkaWej2, 0);
	Plik* plikWyjsciowy = new Plik(sciezkaWyj, 0);
	OdczytZPliku* tasmaWej1 = new OdczytZPliku(plikWejsciowy1);
	OdczytZPliku* tasmaWej2 = new OdczytZPliku(plikWejsciowy2);
	ZapisDoPliku* tasmaWyj = new ZapisDoPliku(plikWyjsciowy, czyWyswietlic);
	double poprzednia1 = 0, poprzednia2 = 0;
	Prostopadloscian* rekord1 = tasmaWej1->kolejnyRekord(), * rekord2 = tasmaWej2->kolejnyRekord();
	if (rekord2 == NULL) return false;
	while (1){
		if ((rekord1 != NULL) && (rekord2 != NULL)){
			if (rekord1->objetosc < poprzednia1){
				while ((rekord2 != NULL) && (rekord2->objetosc > poprzednia2)){
					tasmaWyj->zapiszPrzySortowaniu(rekord2);
					poprzednia2 = rekord2->objetosc;
					rekord2 = tasmaWej2->kolejnyRekord();
				}
				poprzednia1 = 0;
				poprzednia2 = 0;
			}
			else if (rekord2->objetosc < poprzednia2){
				while ((rekord1 != NULL) && (rekord1->objetosc > poprzednia1)){
					tasmaWyj->zapiszPrzySortowaniu(rekord1);
					poprzednia1 = rekord1->objetosc;
					rekord1 = tasmaWej1->kolejnyRekord();
				}
				poprzednia1 = 0;
				poprzednia2 = 0;
			}
			else{
				if (rekord1->objetosc < rekord2->objetosc){
					tasmaWyj->zapiszPrzySortowaniu(rekord1);
					poprzednia1 = rekord1->objetosc;
					rekord1 = tasmaWej1->kolejnyRekord();
				}
				else{
					tasmaWyj->zapiszPrzySortowaniu(rekord2);
					poprzednia2 = rekord2->objetosc;
					rekord2 = tasmaWej2->kolejnyRekord();
				}
			}
		}
		else if (rekord1 == NULL){
			while (rekord2 != NULL){
				tasmaWyj->zapiszPrzySortowaniu(rekord2);
				rekord2 = tasmaWej2->kolejnyRekord();
			}
			break;
		}
		else if (rekord2 == NULL){
			while (rekord1 != NULL){
				tasmaWyj->zapiszPrzySortowaniu(rekord1);
				rekord1 = tasmaWej1->kolejnyRekord();
			}
			break;
		}
	}
	delete tasmaWej1;
	delete tasmaWej2;
	delete tasmaWyj;
	return true;
}

void przepisaniePosortowanego(){//przepisanie pliku po posortowaniu
	char nieposortowany[] = "tasma1.txt";
	char posortowany[] = "posortowany.txt";
	Plik* wejsciowy = new Plik(nieposortowany, 0);
	Plik* wyjsciowy = new Plik(posortowany, 0);
	OdczytZPliku* buforOryginalny = new OdczytZPliku(wejsciowy);
	ZapisDoPliku* buforKopia = new ZapisDoPliku(wyjsciowy);
	cout << "Plik po posortowaniu:"<<endl;
	while (buforKopia->zapiszPrzyPrzepisywaniu(buforOryginalny->kolejnyRekord()));
	buforKopia->zapiszReszte();
	delete buforOryginalny;
	delete buforKopia;
	//remove("tasma1.txt");
	remove("tasma2.txt");
}

int main(){
	char sciezkaDoPliku[] =  "wejsciowy.txt" ;
	bool sortowac = true;
	char czyWyswietlic;
	menu(sciezkaDoPliku);
	char kopiaWejsciowego[] = "kopiaWej.txt";
	przepisaniePliku(sciezkaDoPliku, kopiaWejsciowego);
	cout << "Czy wyswietlac plik po kazdej z faz sortowania?"<<endl;
	cout << "Wybierz t, jesli chcesz wyswietlac."<<endl;
	cout << "Wybierz n, jesli nie chcesz wyswietlac."<<endl;
	cin >> czyWyswietlic;
	while (sortowac){
		rozdzielanieNaTasmy(); 
		sortowac = laczenieNaTasme(czyWyswietlic);
	}
	przepisaniePosortowanego();
	cout << "Ilosc faz rozdzielenia: " << iloscFazRozdzielenia << endl;
	cout << "Ilosc faz laczenia: " << iloscFazLaczenia << endl;
	cout << "Ilosc odczytow z dysku: " << iloscOdczytowZDysku << endl;
	cout << "Ilosc zapisow na dysk: " << iloscZapisowNaDysk << endl;
	system("pause");
	return 0;
}