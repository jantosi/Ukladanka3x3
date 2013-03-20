#include <iostream>
#include <ctime>
#include <cmath>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

#define N 3
#define GLEBOKOSC_REKURSJI 6
//

class Stan{
public:
    int plansza[N][N];
    int pozycjaDziuryWiersz, pozycjaDziuryKolumna;

    void wypiszPlansze()
    {
        for(int i=0;i<N;i++)
        {
            for(int j=0;j<N;j++)
            {
                cout << plansza[i][j] << "\t";
            }
            cout << "\n";
        }
    }

    void randomPlansza(bool notrandom=false)
    {
        vector<int> dostepneKafelki;

        for(int i=1;i<(N*N);i++)
        {
            dostepneKafelki.push_back(i);
        }
        dostepneKafelki.push_back(-1);

        if(!notrandom) random_shuffle(dostepneKafelki.begin(),dostepneKafelki.end());

        for(int i=0; i<N; i++)
            for(int j=0; j<N; j++)
            {
                plansza[i][j] = dostepneKafelki.at(j+i*N);
                if(dostepneKafelki.at(j+i*N)==-1)
                {
                    pozycjaDziuryKolumna = j;
                    pozycjaDziuryWiersz = i;
                }
            }
    }

    bool weryfikujStan()
    {
        bool isOk = true;
        if(plansza[N-1][N-1]==-1)
        {
            for(int i=0; i<N; i++)
            {
                for(int j=0; j<N; j++)
                {
                    if(i==N-1 && j==N-1) continue;
                    if(plansza[i][j]!=(j+i*N+1)) return false;
                }
            }
        }
        else isOk=false;

        return isOk;
    }

    bool operator==(const Stan &stan2) {
        return (this->plansza == stan2.plansza);
    }

    friend ostream& operator<<(ostream &wyjscie, const Stan &stan)
    {
        for(int i=0;i<N;i++)
        {
            for(int j=0;j<N;j++)
            {
                wyjscie << stan.plansza[i][j] << "\t";
            }
            wyjscie << "\n";
        }
        return wyjscie;
    }
};

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
    int ordernum;
    Stan stan;
    vector<Edge*> edges;

    //vector wskaznikow na operatory
    vector <Vertex* (Vertex::*)(int,int)> wskOperatory;

    Vertex(Stan stan);
    ~Vertex();

    void printout();

    Vertex* executeOperator(int index, int wiersz, int kolumna);
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

Vertex::Vertex(Stan stan) {
    this->stan = stan;
    this->visited = false;

    this->wskOperatory.push_back(&Vertex::operatorD);
    this->wskOperatory.push_back(&Vertex::operatorG);
    this->wskOperatory.push_back(&Vertex::operatorP);
    this->wskOperatory.push_back(&Vertex::operatorL);
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
    if(wiersz>=N-1) return NULL;

    Vertex* vertex = new Vertex(this->stan);

    vertex->stan.plansza[wiersz][kolumna] =
        vertex->stan.plansza[wiersz+1][kolumna];

    vertex->stan.plansza[wiersz+1][kolumna] = -1;

    vertex->stan.pozycjaDziuryWiersz++;

    return vertex;
}

Vertex* Vertex::operatorG(int wiersz, int kolumna)
{
    if(wiersz<=0) return NULL;

    Vertex* vertex = new Vertex(this->stan);

    vertex->stan.plansza[wiersz][kolumna] =
        vertex->stan.plansza[wiersz-1][kolumna];

    vertex->stan.plansza[wiersz-1][kolumna] = -1;

    vertex->stan.pozycjaDziuryWiersz--;

    return vertex;
}

Vertex* Vertex::operatorL(int wiersz, int kolumna)
{
    if(kolumna<=0) return NULL;

    Vertex* vertex = new Vertex(this->stan);

    vertex->stan.plansza[wiersz][kolumna] =
        vertex->stan.plansza[wiersz][kolumna-1];

    vertex->stan.plansza[wiersz][kolumna-1] = -1;

    vertex->stan.pozycjaDziuryKolumna--;

    return vertex;
}

Vertex* Vertex::operatorP(int wiersz, int kolumna)
{
    if(kolumna>=N-1) return NULL;

    Vertex* vertex = new Vertex(this->stan);

    vertex->stan.plansza[wiersz][kolumna] =
        vertex->stan.plansza[wiersz][kolumna+1];

    vertex->stan.plansza[wiersz][kolumna+1] = -1;

    vertex->stan.pozycjaDziuryKolumna++;

    return vertex;
}

Vertex* Vertex::executeOperator(int index, int wiersz, int kolumna)
{
    //cout << "V::eO " << index << " " << wiersz << " " << kolumna << endl;
    return (this->*wskOperatory.at(index))(wiersz,kolumna);
}

void Vertex::printout()
{
    cout << "Wierzcholek #" << this->ordernum << " "<< this->stan << endl;
    cout << "Lista krawedzi: \n";
    for(vector<Edge*>::iterator i=edges.begin();i<edges.end();i++)
    {
        (*i)->printout();
    }
}

class Graf
{
public:
    vector<Vertex*> vertices;
    bool isDigraph;

    Graf(bool digraph=false,int initialSize=0)
    {
        this->isDigraph = digraph;
        vertices.resize(initialSize);
    }

    int addVertexWithCheck(Vertex* tmp) {
        //cout<<"addVwC\n";
        //tmp->printout();
        //cout << "\n\n";
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

    bool DFSFindVertex(Vertex* start, Vertex* end, int&pathlength, vector<int>&path)
    {
        start->visited = true;

        if(start->stan == end->stan)
        {
            pathlength=0;
            return true;
        }

        for(size_t i=0; i<start->edges.size(); i++) //foreach edge
        {
            Vertex* current = start->edges.at(i)->other_end;
            if(!current->visited)
            {
                bool found = DFSFindVertex(current, end, pathlength,path);
                if(found)
                {
                    pathlength++;
                    path.push_back(current->ordernum);
                    return found;
                }
            }
        }
    }
    bool BFSFindVertex(Vertex* start, Vertex* end, int &pathlength, vector<int>&path) {
        queue <Vertex* > do_odwiedzenia;
        do_odwiedzenia.push(start);

        path.push_back(start->ordernum);
        bool koniec = false;
        pathlength = 0;

        while( !do_odwiedzenia.empty() && !koniec ){
            Vertex* aktualny;
            aktualny = do_odwiedzenia.front();
            do_odwiedzenia.pop();
            if (!aktualny->visited){
                aktualny->visited = true;
                sort(aktualny->edges.begin(), aktualny->edges.end());

                for (size_t i = 0; i < aktualny->edges.size() ; i++){
                    if( aktualny->edges.at(i)->other_end->stan == end->stan ){
                        koniec = true;
                    }
                    do_odwiedzenia.push(aktualny->edges.at(i)->other_end);

                    path.push_back(aktualny->edges.at(i)->other_end->ordernum);
                    pathlength++;
                }
            }
        }

        return koniec;
    }
    void unvisitAllVertices()
    {
        for(size_t i=0; i<this->vertices.size(); i++)
        {
            this->vertices.at(i)->visited = false;
        }
    }
};

void tworzGraf(Vertex* vertexStart, Graf* graf, int glebokoscRekursji=0) {
    if(glebokoscRekursji>=GLEBOKOSC_REKURSJI) return;
    Vertex* vertexResult;

    for(size_t i=0; i<vertexStart->wskOperatory.size(); i++)
    {
        //cout << "tG:i" << i << endl;
        vertexResult = vertexStart->executeOperator(i, vertexStart->stan.pozycjaDziuryWiersz, vertexStart->stan.pozycjaDziuryKolumna);
        if(vertexResult!=NULL)
        {
            //cout << "not null\n";
            int pozycja = graf->addVertexWithCheck(vertexResult);
            if (pozycja != -1) {
                //cout << "pozycja="<<pozycja<<endl;
                graf->makeEdge(vertexStart, graf->getVertex(pozycja), 0);
                tworzGraf(vertexResult, graf, ++glebokoscRekursji);
            }
        }
    }
}

//

int main()
{
    srand(time(NULL));
    Stan start;
    start.randomPlansza();

    Stan stop;
    stop.randomPlansza(true);

    start.wypiszPlansze();

    Graf* graf = new Graf(true,0);

    Vertex* vertexStart = new Vertex(start);

    graf->addVertex(vertexStart,-1);
    tworzGraf(vertexStart, graf);

    Vertex* vertexStop = new Vertex(stop);

    int pathlength=0; vector<int> path;
    graf->DFSFindVertex(vertexStart,vertexStop,pathlength,path);

    cout << pathlength;
    for(size_t i =0; i<path.size(); i++)
    {
        Vertex* tmp = graf->getVertex(path.at(i));
        tmp->printout();
    }

    return 0;
}
