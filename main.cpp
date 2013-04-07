#include <iostream>
#include <ctime>
#include <cmath>
#include <vector>
#include <algorithm>
#include <queue>
#include <stack>
#include <string.h>

using namespace std;

//zgodnie z wymaganiami pusty kafelek ma byc oznaczany jako 0
#define PUSTE_POLE_PLANSZA 0
#define STRATEGIA_WSZERZ 1
#define STRATEGIA_W_GLAB 2
#define STRATEGIA_W_GLAB_Z_POGLEBIANIEM 3
#define DEBUG_MODE true

class ParametryWykonania {
public:
    int liczbaPrzetworzonychStanow;
    int liczbaOdwiedzonychStanow;
    int maxGlebokoscGrafu;
    int glebokoscRozwiazania;
    int dlugoscRozwiazania;
    vector <char> ruchyRozwiazujace;
    vector <int> wierzcholkiOdwiedzone;

    ParametryWykonania() {
        this->liczbaPrzetworzonychStanow = 0;
        this->liczbaOdwiedzonychStanow = 0;
        this->maxGlebokoscGrafu = 0;
        this->glebokoscRozwiazania = 0;
        this->dlugoscRozwiazania = 0;
    }

    void printout() {
        cout << "liczba przetworzonych stanow: [" << liczbaPrzetworzonychStanow << "]\n";
        cout << "liczba odwiedzonych stanow: [" << liczbaOdwiedzonychStanow << "]\n";
        cout << "maksymalna glebokosc grafu: [" << maxGlebokoscGrafu << "]\n";
        cout << "glebokosc, na ktorej znajduje sie rozwiazanie: [" << glebokoscRozwiazania << "]\n";
    }

    void printRozwiazanie() {
        cout << this->dlugoscRozwiazania << "\n";
        for (size_t i = 0; i < this->ruchyRozwiazujace.size(); i++) {
            cout << this->ruchyRozwiazujace.at(i);
        }
        cout<<endl;
    }

};

class Stan{
private:
    void inicjalizujPlansze() {
        plansza = new int *[this->liczbaWierszy];
        for (int i = 0; i < this->liczbaWierszy; i++)
            plansza[i] = new int[liczbaKolumn];
    }

public:
    int **plansza;
    int liczbaWierszy, liczbaKolumn;
    int pozycjaDziuryWiersz, pozycjaDziuryKolumna;

    //okresla, która stronê zostala przmieszczony klocek w porownaniu do stanu poprzedniego
    //mozliwe kierunki: LPGD
    char kierunekPrzemieszczeniaKlocka;

    Stan() {
        this->plansza = NULL;
        this->kierunekPrzemieszczeniaKlocka = NULL;
    }
    Stan(const Stan &stan2) {

        this->liczbaKolumn = stan2.liczbaKolumn;
        this->liczbaWierszy = stan2.liczbaWierszy;
        this->pozycjaDziuryKolumna = stan2.pozycjaDziuryKolumna;
        this->pozycjaDziuryWiersz = stan2.pozycjaDziuryWiersz;
        this->kierunekPrzemieszczeniaKlocka = stan2.kierunekPrzemieszczeniaKlocka;

        inicjalizujPlansze();

        for (int i = 0; i < this->liczbaWierszy; i++) {
            for (int j = 0; j < this->liczbaKolumn; j++) {
                this->plansza[i][j] = stan2.plansza[i][j];
            }
        }
    }
    ~Stan() {
        if (this->plansza != NULL) {
            for (int i = 0; i < this->liczbaWierszy; i++) {
                delete[] plansza[i];
            }
            delete[] this->plansza;
        }
    }

    Stan& operator=(const Stan &stan2) {
        if (this != &stan2) {
            this->liczbaKolumn = stan2.liczbaKolumn;
            this->liczbaWierszy = stan2.liczbaWierszy;
            this->pozycjaDziuryKolumna = stan2.pozycjaDziuryKolumna;
            this->pozycjaDziuryWiersz = stan2.pozycjaDziuryWiersz;
            this->kierunekPrzemieszczeniaKlocka = stan2.kierunekPrzemieszczeniaKlocka;

            if (this->plansza != NULL) {
                for (int i = 0; i < this->liczbaWierszy; i++) {
                    delete[] plansza[i];
                }
                delete[] this->plansza;
            }
            inicjalizujPlansze();

            for (int i = 0; i < this->liczbaWierszy; i++) {
                for (int j = 0; j < this->liczbaKolumn; j++) {
                    this->plansza[i][j] = stan2.plansza[i][j];
                }
            }
        }
        return *this;
    }

    void wczytajPlansze() {
        cin >> this->liczbaWierszy >> this->liczbaKolumn;

        inicjalizujPlansze();

        //przyjmujemy, ze zostaje wprowadzona poprawnie skonstruuowana plansza
        //tzn. elementy sie nie powtarzaja
        for (int i = 0; i < this->liczbaWierszy; i++) {
            for (int j = 0; j < this->liczbaKolumn; j++) {
                cin >> plansza[i][j];

                if (plansza[i][j] == PUSTE_POLE_PLANSZA) {
                    this->pozycjaDziuryKolumna = j;
                    this->pozycjaDziuryWiersz = i;
                }
            }
        }
    }

    void wypiszPlansze()
    {
        for(int i=0;i<this->liczbaWierszy;i++)
        {
            for(int j=0;j<this->liczbaWierszy;j++)
            {
                cout << plansza[i][j] << "\t";
            }
            cout << "\n";
        }
    }

    void tworzPlansze(bool random = true)
    {
        inicjalizujPlansze();

        vector<int> dostepneKafelki;

        for(int i=1;i<(this->liczbaWierszy*this->liczbaKolumn);i++)
        {
            dostepneKafelki.push_back(i);
        }
        dostepneKafelki.push_back(PUSTE_POLE_PLANSZA);

        if(random) random_shuffle(dostepneKafelki.begin(),dostepneKafelki.end());

        for(int i=0; i<this->liczbaWierszy; i++)
            for(int j=0; j<this->liczbaKolumn; j++)
            {
            plansza[i][j] = dostepneKafelki.at(j+i*this->liczbaWierszy);
            if(dostepneKafelki.at(j+i*this->liczbaWierszy) == PUSTE_POLE_PLANSZA)
            {
                pozycjaDziuryKolumna = j;
                pozycjaDziuryWiersz = i;
            }
        }
    }
    bool sprawdzRozwiazywalnosc() {

        bool parzystoscWierszaDziury = (this->pozycjaDziuryWiersz + 1) % 2 == 0;
        bool parzystoscSzerokosciPlanszy = (this->liczbaKolumn) % 2 == 0;

        //pomocnicza tablica jednowymiarowa przechpwujaca elementy planszy
        int tmpPlansza[this->liczbaWierszy * this->liczbaKolumn];
        for (int i = 0; i < this->liczbaWierszy; i++) {
            for (int j = 0; j < this->liczbaKolumn; j++) {
                tmpPlansza[j + this->liczbaWierszy * i] = this->plansza[i][j];
            }
        }

        int rozmiar = sizeof(tmpPlansza)/sizeof(tmpPlansza[0]);
        int liczbaInwersji = 0;

        //dla kazdego puzzla(klocka) obliczamy ile jest PRZED nim klockow o mniejszym numerze nie uwzgledniajac dziury
        for (int i = 0; i < rozmiar; i++) {
            for (int j = i; j < rozmiar; j++) {
                if (tmpPlansza[i] > tmpPlansza[j] && tmpPlansza[i] != PUSTE_POLE_PLANSZA && tmpPlansza[j] != PUSTE_POLE_PLANSZA) {
                    liczbaInwersji++;
                }
            }
        }
        bool parzystosLiczbyInwersji = liczbaInwersji % 2  == 0;

        //jesli szerokosc ukladanki jest liczba nieparzysta to ukladanka ma rozwiazanie wtedy i tylko wtedy gdy liczba inwersji jest liczba parzysta

        //jesli szerokosc ukladanki jest liczba parzysta ukladanka ma rozwiazanie wtedy i tylko wtedy gdy
        //parzystosc liczby inwersji jest taka sama jak parzystosc numeru wiersza w ktorym jest dziura
        return !parzystoscSzerokosciPlanszy? (parzystosLiczbyInwersji == true) : (parzystosLiczbyInwersji == parzystoscWierszaDziury);
    }

    bool operator==(const Stan &stan2) {
        //przyjmujemy, ¿e plansze, które generujemy maja zawsze taki sam rozmiar
        for (int i = 0; i < this->liczbaWierszy; i++)
            for (int j = 0; j < this->liczbaKolumn; j++)
                if (this->plansza[i][j] != stan2.plansza[i][j])
                    return false;

        return true;
    }

    friend ostream& operator<<(ostream &wyjscie, const Stan &stan)
    {
        for(int i=0;i<stan.liczbaWierszy;i++)
        {
            for(int j=0;j<stan.liczbaKolumn;j++)
            {
                wyjscie << stan.plansza[i][j] << "\t";
            }
            if (i < stan.liczbaWierszy - 1)
                wyjscie << "\n";
        }
        return wyjscie;
    }
};
////Heurystyki
//heurystyka pierwsza opiera siê na liczbie klocków które s¹ na nie swoim miejscu
float heurystyka1(Stan aktualny,Stan wzorcowy){
    //zak³adam ze wymiary plansz stanów sa identyczne
    float wynik=0;
    for(int w = 0 ; w<aktualny.liczbaWierszy;w++){
        for (int k = 0;k<aktualny.liczbaKolumn;k++){
            if(aktualny.plansza[w][k] != wzorcowy.plansza[w][k]){
                wynik++;
            }
        }
    }
    return wynik;
}
//heurystyka druga opiera siê na tym jak bardzo klocki
//s¹ oddalone od swojego po³o¿enia koñcowego
float heurystyka2(Stan aktualny){
    //zak³adam ze wymiary plansz stanów sa identyczne
    float wynik=0;
    for(int w = 0 ; w<aktualny.liczbaWierszy;w++){
        for (int k = 0;k<aktualny.liczbaKolumn;k++){
            int powinnobyc=w*aktualny.liczbaKolumn+k+1;
            wynik+=abs(aktualny.plansza[w][k]-powinnobyc);
        }
    }
    return wynik;
}
class Edge;
class Vertex
{
private:
    //operatory
    Vertex* operatorP(int wiersz, int kolumna);
    Vertex* operatorL(int wiersz, int kolumna);
    Vertex* operatorG(int wiersz, int kolumna);
    Vertex* operatorD(int wiersz, int kolumna);

public:
    bool visited;
    Vertex* parent;
    char* kolejnoscOperatorow;
    int odlegloscOdPoczatku;
    // do ASTAR'a
    float odlegloscHeurystyczna;
    bool doOdwiedzenia;

    int ordernum;
    Stan stan;
    vector<Edge*> edges;

    //vector wskaznikow na operatory
    vector <Vertex* (Vertex::*)(int,int)> wskOperatory;

    Vertex(Stan stan, char* kolejnosOperatorow);
    ~Vertex();

    void printout();

    Vertex* executeOperator(int index, int wiersz, int kolumna);
    float getF();
};

class Edge
{
public:
    int weight;
    Vertex* other_end;
    Edge(int weight, Vertex* other_end=NULL)
    {
        this->weight = weight;
        this->other_end = other_end;
    }
    void printout()
    {
        cout << "\tKrawedz; waga: " << weight << "; prowadzi do: ";
        if(other_end == NULL) cout << "NULL";
        else cout << "wierzcholek #" << other_end->ordernum;
        cout << endl;
    }
};

Vertex::Vertex(Stan stan, char* kolejnoscOperatorow) {
    this->stan = stan;
    this->visited = false;
    this->parent = NULL;
    this->kolejnoscOperatorow = kolejnoscOperatorow;
    this->odlegloscOdPoczatku = 0;

    for (int i = 0; i < 4; i++) {
        if (toupper(this->kolejnoscOperatorow[i]) == 'L' ) {
            this->wskOperatory.push_back(&Vertex::operatorL);
        } else if (toupper(this->kolejnoscOperatorow[i]) == 'P' ) {
            this->wskOperatory.push_back(&Vertex::operatorP);
        } else if (toupper(this->kolejnoscOperatorow[i]) == 'G' ) {
            this->wskOperatory.push_back(&Vertex::operatorG);
        } else if (toupper(this->kolejnoscOperatorow[i]) == 'D' ) {
            this->wskOperatory.push_back(&Vertex::operatorD);
        }
    }
}

Vertex::~Vertex()
{
    for(size_t i=0; i<edges.size(); i++)
    {
        delete edges.at(i);
    }
}

Vertex* Vertex::operatorD(int wiersz, int kolumna)
{
    if(wiersz>= this->stan.liczbaWierszy -1) return NULL;

    Vertex* vertex = new Vertex(this->stan, this->kolejnoscOperatorow);

    vertex->stan.plansza[wiersz][kolumna] =
            vertex->stan.plansza[wiersz+1][kolumna];

    vertex->stan.plansza[wiersz+1][kolumna] = PUSTE_POLE_PLANSZA;

    vertex->stan.pozycjaDziuryWiersz++;
    vertex->stan.kierunekPrzemieszczeniaKlocka = 'G';

    return vertex;
}

Vertex* Vertex::operatorG(int wiersz, int kolumna)
{
    if(wiersz<=0) return NULL;

    Vertex* vertex = new Vertex(this->stan, this->kolejnoscOperatorow);

    vertex->stan.plansza[wiersz][kolumna] =
            vertex->stan.plansza[wiersz-1][kolumna];

    vertex->stan.plansza[wiersz-1][kolumna] = PUSTE_POLE_PLANSZA;

    vertex->stan.pozycjaDziuryWiersz--;
    vertex->stan.kierunekPrzemieszczeniaKlocka = 'D';

    return vertex;
}

Vertex* Vertex::operatorL(int wiersz, int kolumna)
{
    if(kolumna<=0) return NULL;

    Vertex* vertex = new Vertex(this->stan, this->kolejnoscOperatorow);

    vertex->stan.plansza[wiersz][kolumna] =
            vertex->stan.plansza[wiersz][kolumna-1];

    vertex->stan.plansza[wiersz][kolumna-1] = PUSTE_POLE_PLANSZA;

    vertex->stan.pozycjaDziuryKolumna--;
    vertex->stan.kierunekPrzemieszczeniaKlocka = 'P';

    return vertex;
}

Vertex* Vertex::operatorP(int wiersz, int kolumna)
{
    if(kolumna>=this->stan.liczbaKolumn-1) return NULL;

    Vertex* vertex = new Vertex(this->stan, this->kolejnoscOperatorow);

    vertex->stan.plansza[wiersz][kolumna] =
            vertex->stan.plansza[wiersz][kolumna+1];

    vertex->stan.plansza[wiersz][kolumna+1] = PUSTE_POLE_PLANSZA;

    vertex->stan.pozycjaDziuryKolumna++;
    vertex->stan.kierunekPrzemieszczeniaKlocka = 'L';

    return vertex;
}

Vertex* Vertex::executeOperator(int index, int wiersz, int kolumna)
{
    return (this->*wskOperatory.at(index))(wiersz,kolumna);
}

float Vertex::getF(){
    return this->odlegloscHeurystyczna+this->odlegloscOdPoczatku;
}

void Vertex::printout()
{
    cout << "Wierzcholek #" << this->ordernum << "\n"<< this->stan << endl;
    cout << "Odleglosc od poczatku: " << this->odlegloscOdPoczatku << endl;
    if(this->parent)
        cout << "parent: " << this->parent << " prowadzi do #" << this->parent->ordernum << "\n";
    cout << "Lista krawedzi: \n";
    for(vector<Edge*>::iterator i=edges.begin();i<edges.end();i++)
    {
        (*i)->printout();
    }
}

class Graf
{
private:
    void BFSGenerujStany(Vertex* vertexStart, Vertex* vertexStop) {
        Vertex* vertexResult;

        this->addVertex(vertexStart, -1);

        //kolejka wierzcholkow, dla ktorych maja byc generowane podWierzcholki z mozliwymi stanami
        queue<Vertex*> verticesToProcessing;
        verticesToProcessing.push(vertexStart);

        bool koniec = (vertexStart->stan == vertexStop->stan ? true : false);
        //dopoki sa wierzcholki, dla ktorych ma byc generowane przejscie, dopoty to generujemy
        while (!verticesToProcessing.empty() && !koniec) {
            Vertex* actualVertex = verticesToProcessing.front();
            verticesToProcessing.pop();

            //dla kazdego wierzcholka generuje sie przejscia do nastepnego stanu (moga byc max 4 przejscia)
            for (size_t op = 0; op < actualVertex->wskOperatory.size(); op++) {
                vertexResult = actualVertex->executeOperator(op, actualVertex->stan.pozycjaDziuryWiersz, actualVertex->stan.pozycjaDziuryKolumna);
                if (vertexResult) {
                    int pozycja = this->addVertexWithCheck(vertexResult);
                    if (pozycja != -1) {
                        this->getVertex(pozycja)->odlegloscOdPoczatku = actualVertex->odlegloscOdPoczatku + 1;
                        this->makeEdge(actualVertex, this->getVertex(pozycja), 0);
                        //jesli generowanie ma isc glebiej to dodajemy kolejne wierzcholki do kolejki
                        if (vertexResult->stan == vertexStop->stan) {
                            koniec = true;
                            break;
                        }
                        else
                            verticesToProcessing.push(vertexResult);
                    }
                }
            }
        }
    }

    void DFSGenerujStany(Vertex* vertexStart, Vertex* vertexStop) {
        Vertex* vertexResult;

        this->addVertex(vertexStart, -1);

        stack<Vertex*> tmpStack;
        stack<Vertex*> verticesToProcessing;
        verticesToProcessing.push(vertexStart);

        bool koniec = (vertexStart->stan == vertexStop->stan ? true : false);
        while (!verticesToProcessing.empty() && !koniec) {
            Vertex* actualVertex = verticesToProcessing.top();
            verticesToProcessing.pop();

            for (size_t op = 0; op < actualVertex->wskOperatory.size(); op++) {
                vertexResult = actualVertex->executeOperator(op, actualVertex->stan.pozycjaDziuryWiersz, actualVertex->stan.pozycjaDziuryKolumna);
                if (vertexResult) {
                    int pozycja = this->addVertexWithCheck(vertexResult);
                    if (pozycja != -1) {
                        this->getVertex(pozycja)->odlegloscOdPoczatku = actualVertex->odlegloscOdPoczatku + 1;
                        this->makeEdge(actualVertex, this->getVertex(pozycja), 0);

                        //jesli generowanie ma isc glebiej to dodajemy kolejne wierzcholki na pomocniczy stos
                        //aby pozniej w glownym stosie miec wierzcholki ustawione w porzadku malejacych numerow
                        if (vertexResult->stan == vertexStop->stan) {
                            koniec = true;
                            break;
                        } else
                            tmpStack.push(vertexResult);
                    }
                }
            }

            if (koniec)
                break;

            //umiesczamy na glowym stosie wierzcholki do odwiedzenia,
            //ktore ustawione sa w porzadku malejacych numerow - zawsze zaczynamy od sasiada o najnizszym numerze
            while (!tmpStack.empty()) {
                verticesToProcessing.push(tmpStack.top());
                tmpStack.pop();
            }

        }
    }

    void IDFSGenerujStany(Vertex* vertexStart, Vertex* vertexStop) {
        Vertex* vertexResult;

        stack<Vertex*> tmpStack;
        stack<Vertex*> verticesToProcessing;

        int glebokosc = 1;

        bool koniec = (vertexStart->stan == vertexStop->stan ? true : false);
        if (koniec)
            this->addVertex(vertexStart, -1);

        while (!koniec) {

            this->clearEdges();
            this->vertices.clear();

            verticesToProcessing.push(vertexStart);
            this->addVertex(vertexStart, -1);

            while (!verticesToProcessing.empty()) {
                Vertex* actualVertex = verticesToProcessing.top();
                verticesToProcessing.pop();

                if (actualVertex->odlegloscOdPoczatku < glebokosc) {
                    for (size_t op = 0; op < actualVertex->wskOperatory.size(); op++) {
                        vertexResult = actualVertex->executeOperator(op, actualVertex->stan.pozycjaDziuryWiersz, actualVertex->stan.pozycjaDziuryKolumna);
                        if (vertexResult) {
                            int pozycja = this->addVertexWithCheck(vertexResult);
                            if (pozycja != -1) {
                                this->getVertex(pozycja)->odlegloscOdPoczatku = actualVertex->odlegloscOdPoczatku + 1;
                                this->makeEdge(actualVertex, this->getVertex(pozycja), 0);

                                //jesli generowanie ma isc glebiej to dodajemy kolejne wierzcholki na pomocniczy stos
                                //aby pozniej w glownym stosie miec wierzcholki ustawione w porzadku malejacych numerow
                                if (vertexResult->stan == vertexStop->stan) {
                                    koniec = true;
                                    break;
                                } else
                                    tmpStack.push(vertexResult);
                            }
                        }
                    }
                }

                if (koniec)
                    break;

                //umiesczamy na glowym stosie wierzcholki do odwiedzenia,
                //ktore ustawione sa w porzadku malejacych numerow - zawsze zaczynamy od sasiada o najnizszym numerze
                while (!tmpStack.empty()) {
                    verticesToProcessing.push(tmpStack.top());
                    tmpStack.pop();
                }

            }
            if (koniec)
                break;

            //jesli kolejka jest pusta i nie znaleziono rozwiazania to zwiêkszamy glebokosc
            glebokosc++;
            //cout <<"zwiekszono glebokosc\n";
        }
    }
public:
    vector<Vertex*> vertices;
    bool isDigraph;

    Graf(bool digraph=false,int initialSize=0)
    {
        this->isDigraph = digraph;
        vertices.resize(initialSize);
    }

    int addVertexWithCheck(Vertex* tmp) {
        for (size_t i = 0; i < vertices.size(); i++)
        {
            if (vertices[i]->stan == tmp->stan)
                return -1;

        }

        vertices.push_back(tmp);
        tmp->ordernum = vertices.size() -1;
        return tmp->ordernum;
    }

    Vertex* getVertex(int orderno)
    {
        return this->vertices.at(orderno);
    }

    Vertex* getVertex(Stan stan)
    {
        for(size_t i=0; i<this->vertices.size();i++)
        {
            if(this->vertices.at(i)->stan == stan)
                return this->vertices.at(i);
        }
    }

    int addVertex(Vertex* vertex,int place=-1)
    {
        if(place==-1)
        {
            vertices.push_back(vertex);
            vertex->ordernum = vertices.size() -1;
            return vertex->ordernum;
        }
        else
        {
            vertices[place] = vertex;
            vertex->ordernum = place;
            return place;
        }
    }
    void makeEdge(Vertex* from, Vertex* to, int weight=1)
    {
        Edge* tmp = new Edge(weight,to);
        from->edges.push_back(tmp);
        if(!isDigraph)
        {
            Edge* tmp2 = new Edge(weight,from);
            to->edges.push_back(tmp2);
        }
    }
    void printout()
    {
        for(size_t i=0; i<vertices.size(); i++)
        {
            if(vertices.at(i)!=NULL)
            {
                vertices.at(i)->printout();
                cout << endl;
            }
        }

    }

    int getMaxGlebokosc() {
        int maxGlebokosc = 0;
        for (size_t i = 0; i < this->vertices.size(); i++) {
            if (vertices.at(i)->odlegloscOdPoczatku > maxGlebokosc)
                maxGlebokosc = vertices.at(i)->odlegloscOdPoczatku;
        }

        return maxGlebokosc;
    }

    bool DFSFindVertex(Vertex* start, Vertex* end, ParametryWykonania* parametryWykonania)
    {
        parametryWykonania->liczbaPrzetworzonychStanow++;
        start->visited = true;

        if(start->stan == end->stan)
        {
            parametryWykonania->dlugoscRozwiazania = 0;
            return true;
        }

        for(size_t i=0; i<start->edges.size(); i++) //foreach edge
        {
            Vertex* current = start->edges.at(i)->other_end;

            if(!current->visited)
            {
                parametryWykonania->liczbaOdwiedzonychStanow++;
                parametryWykonania->wierzcholkiOdwiedzone.push_back(current->ordernum);

                bool found = DFSFindVertex(current, end, parametryWykonania);
                if(found)
                {
                    parametryWykonania->dlugoscRozwiazania++;
                    parametryWykonania->ruchyRozwiazujace.push_back(current->stan.kierunekPrzemieszczeniaKlocka);
                    return found;
                }
            }
        }
    }
    void IDFSFindVertex(Vertex* start, Vertex* end, ParametryWykonania* parametryWykonania)
    {
        stack<Vertex*> doOdwiedzenia;
        stack<Vertex*> stackTmp;

        parametryWykonania->liczbaOdwiedzonychStanow = 0;
        parametryWykonania->liczbaPrzetworzonychStanow = 1;
        int glebokosc = 1;

        bool koniec = false;

        while (!koniec) {

            doOdwiedzenia.push(start);
            this->unvisitAllVertices();
            parametryWykonania->wierzcholkiOdwiedzone.clear();
            parametryWykonania->liczbaOdwiedzonychStanow = 0;

            while (!doOdwiedzenia.empty()) {
                Vertex* actualVertex = doOdwiedzenia.top();
                doOdwiedzenia.pop();

                if (actualVertex->odlegloscOdPoczatku < glebokosc) {
                    for (size_t i = 0; i < actualVertex->edges.size(); i++) {
                        parametryWykonania->liczbaPrzetworzonychStanow++;
                        if (actualVertex->edges.at(i)->other_end->visited == false) {
                            actualVertex->edges.at(i)->other_end->visited = true;

                            stackTmp.push(actualVertex->edges.at(i)->other_end);

                            //zapisz parenta nastepnemu wierzcholkowi
                            actualVertex->edges.at(i)->other_end->parent = actualVertex;
                        }
                    }

                    while (!stackTmp.empty()) {
                        doOdwiedzenia.push(stackTmp.top());
                        stackTmp.pop();
                    }

                    parametryWykonania->wierzcholkiOdwiedzone.push_back(actualVertex->ordernum);
                    parametryWykonania->liczbaOdwiedzonychStanow++;

                    if (actualVertex->stan == end->stan) {
                        koniec = true;
                        break;
                    }
                }
            }

            if (koniec)
                break;

            glebokosc ++;
        }
        parametryWykonania->maxGlebokoscGrafu = this->getMaxGlebokosc();
        parametryWykonania->glebokoscRozwiazania = this->getVertex(end->stan)->odlegloscOdPoczatku;


        //zapisanie ruchow rozwiazujacych
        //zaczynajac od konca przechodzimy po rodzicach danego wierzcholka
        Vertex* tmpvptr = this->getVertex(end->stan);
        stack<char> resultPathTmp;
        while (tmpvptr != start && tmpvptr != NULL) {
            resultPathTmp.push(tmpvptr->stan.kierunekPrzemieszczeniaKlocka);
            tmpvptr = tmpvptr->parent;
            parametryWykonania->dlugoscRozwiazania++;
        }
        while (!resultPathTmp.empty()) {
            char tmp = resultPathTmp.top();
            resultPathTmp.pop();
            parametryWykonania->ruchyRozwiazujace.push_back(tmp);
        }
    }
    bool ASTARfindVertex(Vertex* start, Vertex* end, ParametryWykonania* parametryWykonania,int heuristicId){
        vector<Vertex*> doOdwiedzenia;
        vector<Vertex*> zostawione;
        this->unvisitAllVertices();

        vector <Vertex*> path;

        doOdwiedzenia.push_back(start);
        start->doOdwiedzenia=true;
        while( !doOdwiedzenia.empty() ){
            //wybranie do odwiedzenia z najmniejsz¹ F
            Vertex* actualVertex =doOdwiedzenia.at(0);
            int actualIndex=0;
            for (size_t i =0; i<doOdwiedzenia.size();i++){
                if ( doOdwiedzenia.at(i)->getF() < actualVertex->getF() ){
                    actualVertex=doOdwiedzenia.at(i);
                    actualIndex=i;
                }
            }
            parametryWykonania->liczbaOdwiedzonychStanow++;
            parametryWykonania->liczbaPrzetworzonychStanow++;

            //usuwam z doOdwiedzenia
            actualVertex->doOdwiedzenia=false;
            doOdwiedzenia.erase(doOdwiedzenia.begin()+actualIndex);
            // dodaje do odwiedzonych
            actualVertex->visited=true;
            zostawione.push_back(actualVertex);
            //sprawdzam czy koniec
            if( actualVertex->stan == end->stan ){
                doOdwiedzenia.push_back(actualVertex);
                path.push_back(actualVertex);
                Vertex* tmp=actualVertex->parent;
                while (tmp){

                    path.push_back(tmp);
                    tmp=tmp->parent;
                }
                //odwracam kolejnosc
                reverse(path.begin(),path.end());
                // usuwam ze œcie¿ki wêze³ pocz¹tkowy
                path.erase(path.begin());
                //dodaje do parametrów wykonania
                parametryWykonania->dlugoscRozwiazania=path.size();
                parametryWykonania->glebokoscRozwiazania = actualVertex->odlegloscOdPoczatku;
                for(size_t i = 0; i < path.size(); i++){
                    parametryWykonania->wierzcholkiOdwiedzone.push_back( path.at(i)->ordernum);
                    parametryWykonania->ruchyRozwiazujace.push_back( path.at(i)->stan.kierunekPrzemieszczeniaKlocka);
                }
                return true;
            }

            for (int i=0; i< actualVertex->edges.size();i++){
                Vertex* nextVertex= actualVertex->edges.at(i)->other_end;
                if (nextVertex->visited){
                    continue;
                }
                else if( !nextVertex->doOdwiedzenia ){
                    nextVertex->doOdwiedzenia=true;
                    doOdwiedzenia.push_back(nextVertex);
                    nextVertex->parent=actualVertex;
                    //obliczanie G,H,F sasiada
                    nextVertex->odlegloscOdPoczatku=actualVertex->odlegloscOdPoczatku+1;
                    if (heuristicId == 1){
                        nextVertex->odlegloscHeurystyczna=heurystyka1(nextVertex->stan,end->stan);
                    } else if (heuristicId==2){
                        nextVertex->odlegloscHeurystyczna=heurystyka2(nextVertex->stan);
                    }

                }
                else{
                    int g=actualVertex->odlegloscOdPoczatku+1;
                    if(g<nextVertex->odlegloscOdPoczatku){
                        nextVertex->parent=actualVertex;
                        nextVertex->odlegloscOdPoczatku=g;
                    }
                }
            }
        }

        return false;
        // je¿eli jesteœmy tutaj to nie ma drogi
    }
    bool BFSFindVertex(Vertex* start, Vertex* end, ParametryWykonania* parametryWykonania)
    {
        queue<Vertex*> do_odwiedzenia;
        do_odwiedzenia.push(start);
        //path.push_back(start->ordernum);
        bool koniec = false;
        parametryWykonania->liczbaOdwiedzonychStanow = 0;
        parametryWykonania->liczbaPrzetworzonychStanow = 1;

        while (!do_odwiedzenia.empty() && !koniec) {
            Vertex* aktualny;
            aktualny = do_odwiedzenia.front();
            do_odwiedzenia.pop();

            for (size_t i = 0; i < aktualny->edges.size(); i++) {
                parametryWykonania->liczbaPrzetworzonychStanow++;
                if (aktualny->edges.at(i)->other_end->visited == false) {
                    aktualny->edges.at(i)->other_end->visited = true;

                    do_odwiedzenia.push(aktualny->edges.at(i)->other_end);

                    //zapisz parenta nastepnemu wierzcholkowi
                    aktualny->edges.at(i)->other_end->parent = aktualny;
                }
            }
            parametryWykonania->wierzcholkiOdwiedzone.push_back(aktualny->ordernum);
            parametryWykonania->liczbaOdwiedzonychStanow++;

            if (aktualny->stan == end->stan) {
                koniec = true;
            }
        }

        parametryWykonania->maxGlebokoscGrafu = this->getMaxGlebokosc();
        parametryWykonania->glebokoscRozwiazania = this->getVertex(end->stan)->odlegloscOdPoczatku;

        //zapisanie ruchow rozwiazujacych
        //zaczynajac od konca przechodzimy po rodzicach danego wierzcholka
        Vertex* tmpvptr = this->getVertex(end->stan);
        stack<char> resultPathTmp;
        while (tmpvptr != start && tmpvptr != NULL) {
            resultPathTmp.push(tmpvptr->stan.kierunekPrzemieszczeniaKlocka);
            tmpvptr = tmpvptr->parent;
            parametryWykonania->dlugoscRozwiazania++;
        }
        while (!resultPathTmp.empty()) {
            char tmp = resultPathTmp.top();
            resultPathTmp.pop();
            parametryWykonania->ruchyRozwiazujace.push_back(tmp);
        }

        return koniec;
    }
    void unvisitAllVertices()
    {
        for(size_t i=0; i<this->vertices.size(); i++)
        {
            this->vertices.at(i)->visited = false;
            this->vertices.at(i)->parent = NULL;
            this->vertices.at(i)->doOdwiedzenia = false;
        }
    }
    void clearEdges() {
        for (size_t i = 0; i < this->vertices.size(); i++) {
            this->vertices.at(i)->edges.clear();
        }
    }

    bool genrujStany(Vertex* vertexStart, Vertex* vertexStop, int strategiaGenerowania) {

        if (vertexStart->stan.sprawdzRozwiazywalnosc()) {
            switch (strategiaGenerowania) {
            case STRATEGIA_WSZERZ:
                {
                    this->BFSGenerujStany(vertexStart, vertexStop);
                    break;
                }
            case STRATEGIA_W_GLAB:
                {
                    this->DFSGenerujStany(vertexStart, vertexStop);
                    break;
                }
            case STRATEGIA_W_GLAB_Z_POGLEBIANIEM:
                {
                    this->IDFSGenerujStany(vertexStart, vertexStop);
                    break;
                }
            }

            return true;
        } else
            return false;

    }
};



void printUsageInfo()
{
    cout << "USAGE:\n";
    cout << "TODO;\n";
}

int argToInt(char* arg)
{
    if(strcmp(arg,"-a") == 0 || strcmp(arg,"--a") == 0)
        return 0;
    if(strcmp(arg,"-b") == 0 || strcmp(arg,"--bfs") == 0)
        return 1;
    if(strcmp(arg,"-d") == 0 ||strcmp(arg,"--dfs") == 0)
        return 2;
    if(strcmp(arg,"-i") == 0 || strcmp(arg,"--idfs") == 0)
        return 3;
}

int main(int argc, char* argv[])
{
    const int ARGUMENTS_MISSING_EX = 1000;

    srand(time(0));

    try {
        //parse args
        if (argc < 3) {
            throw ARGUMENTS_MISSING_EX;

        } else {
            if ( (strcmp(argv[1],"-a") == 0 || strcmp(argv[1],"--a") == 0) && argc < 4) {

                throw ARGUMENTS_MISSING_EX;

            } else {

                // jesli jako porzadek podano "R" to nalezy losowac za kazdym razem kolejnosc operatorow
                if (toupper(argv[2][0]) == 'R') {
                    char kolejnoscWzorcowa[] = "LPGD";
                    random_shuffle(&kolejnoscWzorcowa[0], &kolejnoscWzorcowa[4]);

                    argv[2] = kolejnoscWzorcowa;
                }
                if (DEBUG_MODE)
                    cout << "kolejnosc operatorow: " << argv[2] << endl;

                Stan start;

                if (DEBUG_MODE){
                    cout << "Podaj stan poczatkowy: "<<endl;
                }
                //mozna wygenerowac sobie losowa plansze tylko nalezy najpierw ustawic rozmiar planszy

              // Plansza z palca. Przydatna do debugowania
                   start.liczbaKolumn=4;
                   start.liczbaWierszy=4;
                   start.tworzPlansze(true);

                   start.plansza[0][0]=1;
                   start.plansza[0][1]=2;
                   start.plansza[0][2]=3;
                   start.plansza[0][3]=4;

                   start.plansza[1][0]=5;
                   start.plansza[1][1]=6;
                   start.plansza[1][2]=7;
                   start.plansza[1][3]=0;

                   start.plansza[2][0]=9;
                   start.plansza[2][1]=10;
                   start.plansza[2][2]=11;
                   start.plansza[2][3]=8;

                   start.plansza[3][0]=13;
                   start.plansza[3][1]=14;
                   start.plansza[3][2]=15;
                   start.plansza[3][3]=12;

                   start.pozycjaDziuryWiersz=1;
                   start.pozycjaDziuryKolumna=3;
                   cout<<"Plansza z palca"<<endl;

                   start.wypiszPlansze();
                //Plansza ze standardowego wejœcia
              //  start.wczytajPlansze();

                //w argv[2] znajduje siê porz¹dek przeszukiwania podany przez uzytkownika
                // chyba ¿e jest A* wtedy jet tam numer strategii
                Vertex* vertexStart;
                if((strcmp(argv[1],"-a") == 0 || strcmp(argv[1],"--a") == 0)){
                    vertexStart = new Vertex(start, "LPGD");
                }
                else{
                    vertexStart = new Vertex(start, argv[2]);
                }

                Stan stop;
                stop.liczbaWierszy = start.liczbaWierszy;
                stop.liczbaKolumn = start.liczbaKolumn;
                stop.tworzPlansze(false);

                Vertex* vertexStop = new Vertex(stop, argv[2]);


                Graf* graf = new Graf(true, 0);

                switch (argToInt(argv[1])) {
                case 0:
                    {
                        if (DEBUG_MODE)
                            cout << "A*"<<endl;

                        int id_strategii;
                        if (strcmp(argv[2],"1") == 0){
                            id_strategii=1;
                        } else if(strcmp(argv[2],"2") == 0){
                            id_strategii=2;
                        }
                        else{
                            id_strategii=3;
                        }

                        bool rozwiazywalne= graf->genrujStany(vertexStart, vertexStop,id_strategii);
                        if (rozwiazywalne){

                            int id_heurystyki;
                            if (strcmp(argv[3],"1") == 0){
                                id_heurystyki=1;
                            }
                            else{
                                id_heurystyki=2;
                            }
                            ParametryWykonania* parametryWykonania = new ParametryWykonania();
                            bool found=graf->ASTARfindVertex(vertexStart,vertexStop,parametryWykonania,id_heurystyki);
                            if (found){
                                if (DEBUG_MODE){
                                    parametryWykonania->printout();
                                }
                                parametryWykonania->printRozwiazanie();
                            }
                            else{
                                cout<<"-1"<<endl;
                            }
                        }
                        else{
                            cout<<"-1"<<endl;
                        }
                        break;
                    }
                case 1:
                    {
                        bool rozwiazanie = graf->genrujStany(vertexStart, vertexStop, STRATEGIA_WSZERZ);

                        if (!rozwiazanie) {
                            cout << "-1\n";
                        } else {
                            ParametryWykonania* parametryWykonania = new ParametryWykonania();

                            graf->BFSFindVertex(vertexStart, vertexStop, parametryWykonania);
                            //graf->printout();

                            if (DEBUG_MODE)
                                parametryWykonania->printout();

                            parametryWykonania->printRozwiazanie();

                            delete parametryWykonania;
                        }

                        break;
                    }
                case 2:
                    {
                        bool rozwiazanie = graf->genrujStany(vertexStart, vertexStop, STRATEGIA_W_GLAB);

                        if (!rozwiazanie) {
                            cout << "-1\n";
                        } else {
                            ParametryWykonania* parametryWykonania = new ParametryWykonania();

                            parametryWykonania->wierzcholkiOdwiedzone.push_back(vertexStart->ordernum);

                            graf->DFSFindVertex(vertexStart, vertexStop, parametryWykonania);

                            parametryWykonania->maxGlebokoscGrafu = graf->getMaxGlebokosc();
                            parametryWykonania->glebokoscRozwiazania = graf->getVertex(stop)->odlegloscOdPoczatku;
                            //graf->printout();

                            //ruchy rozwiazujace sa zapisane w odwrotnej kolejnosci - rekurencja
                            //trzeba je przepisac w odpowiedniej kolejnosci
                            stack <char> stackTmp;
                            for (size_t i = 0; i < parametryWykonania->ruchyRozwiazujace.size(); i++) {
                                stackTmp.push(parametryWykonania->ruchyRozwiazujace.at(i));
                            }
                            for (size_t i = 0; i < stackTmp.size(); i++) {
                                parametryWykonania->ruchyRozwiazujace.at(i) = stackTmp.top();
                                stackTmp.pop();
                            }

                            if (DEBUG_MODE)
                                parametryWykonania->printout();

                            parametryWykonania->printRozwiazanie();

                            delete parametryWykonania;
                        }

                        break;
                    }
                case 3:
                    {
                        bool rozwiazanie = graf->genrujStany(vertexStart, vertexStop, STRATEGIA_W_GLAB_Z_POGLEBIANIEM);

                        if (!rozwiazanie) {
                            cout << "-1\n";
                        } else {
                            ParametryWykonania* parametryWykonania = new ParametryWykonania();

                            graf->IDFSFindVertex(vertexStart, vertexStop, parametryWykonania);
                            //graf->printout();

                            if (DEBUG_MODE)
                                parametryWykonania->printout();

                            parametryWykonania->printRozwiazanie();

                            delete parametryWykonania;
                        }

                        break;
                    }
                }

                delete vertexStart;
                delete vertexStop;
                delete graf;
            }
        }

    }
    catch (int ex) {
        if (ex == ARGUMENTS_MISSING_EX) {
            cout << "Arguments missing.\n";
            printUsageInfo();
        }
    }

    system("pause");
    return 0;

}
