#include <iostream>
#include <ctime>
#include <cmath>
#include <vector>
#include <algorithm>
#include <queue>
#include <stack>
using namespace std;

#define GLEBOKOSC_REKURSJI 20
//zgodnie z wymaganiami pusty kafelek ma byc oznaczany jako 0
#define PUSTE_POLE_PLANSZA 0

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

    //okresla, która stronê zostala przmieszczona dziura w porownaniu do stanu poprzedniego
    //mozliwe kierunki: LPGD
    char kierunekPrzemieszczeniaDziury;

    Stan() {
    	this->plansza = NULL;
    	this->kierunekPrzemieszczeniaDziury = NULL;
    }
	Stan(const Stan &stan2) {

		this->liczbaKolumn = stan2.liczbaKolumn;
		this->liczbaWierszy = stan2.liczbaWierszy;
		this->pozycjaDziuryKolumna = stan2.pozycjaDziuryKolumna;
		this->pozycjaDziuryWiersz = stan2.pozycjaDziuryWiersz;
		this->kierunekPrzemieszczeniaDziury = stan2.kierunekPrzemieszczeniaDziury;

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
			this->kierunekPrzemieszczeniaDziury = stan2.kierunekPrzemieszczeniaDziury;

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

//    bool weryfikujStan()
//    {
//        bool isOk = true;
//        if(plansza[this->liczbaWierszy-1][this->liczbaKolumn-1]==PUSTE_POLE_PLANSZA)
//        {
//            for(int i=0; i<this->liczbaWierszy; i++)
//            {
//                for(int j=0; j<this->liczbaKolumn; j++)
//                {
//                    if(i==this->liczbaWierszy-1 && j==this->liczbaKolumn-1) continue;
//                    if(plansza[i][j]!=(j+i*this->liczbaWierszy+1)) return false;
//                }
//            }
//        }
//        else isOk = false;
//
//        return isOk;
//    }

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
    Vertex* BFS_parent;

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
    this->BFS_parent = NULL;

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
    if(wiersz>= this->stan.liczbaWierszy -1) return NULL;

    Vertex* vertex = new Vertex(this->stan);

    vertex->stan.plansza[wiersz][kolumna] =
        vertex->stan.plansza[wiersz+1][kolumna];

    vertex->stan.plansza[wiersz+1][kolumna] = PUSTE_POLE_PLANSZA;

    vertex->stan.pozycjaDziuryWiersz++;
    vertex->stan.kierunekPrzemieszczeniaDziury = 'D';

    return vertex;
}

Vertex* Vertex::operatorG(int wiersz, int kolumna)
{
    if(wiersz<=0) return NULL;

    Vertex* vertex = new Vertex(this->stan);

    vertex->stan.plansza[wiersz][kolumna] =
        vertex->stan.plansza[wiersz-1][kolumna];

    vertex->stan.plansza[wiersz-1][kolumna] = PUSTE_POLE_PLANSZA;

    vertex->stan.pozycjaDziuryWiersz--;
    vertex->stan.kierunekPrzemieszczeniaDziury = 'G';

    return vertex;
}

Vertex* Vertex::operatorL(int wiersz, int kolumna)
{
    if(kolumna<=0) return NULL;

    Vertex* vertex = new Vertex(this->stan);

    vertex->stan.plansza[wiersz][kolumna] =
        vertex->stan.plansza[wiersz][kolumna-1];

    vertex->stan.plansza[wiersz][kolumna-1] = PUSTE_POLE_PLANSZA;

    vertex->stan.pozycjaDziuryKolumna--;
    vertex->stan.kierunekPrzemieszczeniaDziury = 'L';

    return vertex;
}

Vertex* Vertex::operatorP(int wiersz, int kolumna)
{
    if(kolumna>=this->stan.liczbaKolumn-1) return NULL;

    Vertex* vertex = new Vertex(this->stan);

    vertex->stan.plansza[wiersz][kolumna] =
        vertex->stan.plansza[wiersz][kolumna+1];

    vertex->stan.plansza[wiersz][kolumna+1] = PUSTE_POLE_PLANSZA;

    vertex->stan.pozycjaDziuryKolumna++;
    vertex->stan.kierunekPrzemieszczeniaDziury = 'P';

    return vertex;
}

Vertex* Vertex::executeOperator(int index, int wiersz, int kolumna)
{
    return (this->*wskOperatory.at(index))(wiersz,kolumna);
}

void Vertex::printout()
{
    cout << "Wierzcholek #" << this->ordernum << "\n"<< this->stan << endl;
    if(this->BFS_parent)
        cout << "BFS_parent: " << this->BFS_parent << " prowadzi do #" << this->BFS_parent->ordernum << "\n";
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

    bool DFSFindVertex(Vertex* start, Vertex* end, int&pathlength, vector<int>&path)
    {
        start->visited = true;

        if(start->stan == end->stan)
        {
            pathlength = 0;
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
		queue<Vertex*> do_odwiedzenia;
		do_odwiedzenia.push(start);

		//path.push_back(start->ordernum);
		bool koniec = false;
		pathlength = 0;

		while (!do_odwiedzenia.empty() && !koniec) {
			Vertex* aktualny;
			aktualny = do_odwiedzenia.front();
			do_odwiedzenia.pop();

			sort(aktualny->edges.begin(), aktualny->edges.end());

			path.push_back(aktualny->ordernum);

			for (size_t i = 0; i < aktualny->edges.size(); i++) {
				if (aktualny->edges.at(i)->other_end->visited == false) {
					aktualny->edges.at(i)->other_end->visited = true;
					do_odwiedzenia.push(aktualny->edges.at(i)->other_end);

                    //zapisz parenta nastepnemu wierzcholkowi
                    aktualny->edges.at(i)->other_end->BFS_parent = aktualny;
				}
			}

			if (aktualny->stan == end->stan) {
				koniec = true;
			} else
				pathlength++;

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

void tworzGraf(Vertex* vertexStart, Graf* graf) {
	Vertex* vertexResult;

	//kolejka wierzcholkow, dla ktorych maja byc generowane podWierzcholki z mozliwymi stanami
	queue<Vertex*> verticesToProcessing;
	verticesToProcessing.push(vertexStart);

	int aktualnaGlebokosc = 1;
	//dopoki sa wierzcholki, dla ktorych ma byc generowane przejscie, dopoty to generujemy
	while (!verticesToProcessing.empty()) {
		Vertex* actualVertex = verticesToProcessing.front();
		verticesToProcessing.pop();

		//dla kazdego wierzcholka generuje sie przejscia do nastepnego stanu (moga byc max 4 przejscia)
		for (size_t op = 0; op < actualVertex->wskOperatory.size(); op++) {
			vertexResult = actualVertex->executeOperator(op, actualVertex->stan.pozycjaDziuryWiersz, actualVertex->stan.pozycjaDziuryKolumna);
			if (vertexResult) {
				int pozycja = graf->addVertexWithCheck(vertexResult);
				if (pozycja != -1) {
					graf->makeEdge(actualVertex, graf->getVertex(pozycja), 0);

					//jesli generowanie ma isc glebiej to dodajemy kolejne wierzcholki do kolejki
					if (aktualnaGlebokosc < GLEBOKOSC_REKURSJI)
						verticesToProcessing.push(vertexResult);
				}
			}
		}
		aktualnaGlebokosc++;
	}

//    for(size_t i = 0; i<vertexStart->wskOperatory.size(); i++)
//    {
//        vertexResult = vertexStart->executeOperator(i, vertexStart->stan.pozycjaDziuryWiersz, vertexStart->stan.pozycjaDziuryKolumna);
//        if(vertexResult!=NULL)
//        {
//            int pozycja = graf->addVertexWithCheck(vertexResult);
//            if (pozycja != -1) {
//                graf->makeEdge(vertexStart, graf->getVertex(pozycja), 0);
//                tworzGraf(vertexResult, graf, ++glebokoscRekursji);
//            }
//        }
//    }

}

void printUsageInfo()
{
    cout << "USAGE:\n";
    cout << "TODO;\n";
}

int argZeroToInt(char* arg)
{
    cout << "[" << arg << "]\n";
    if(arg == "-a" || arg == "--a")
        return 0;
    if(arg == "-b" || arg == "--bfs")
        return 1;
    if(arg == "-d" || arg == "--dfs")
        return 2;
    if(arg == "-i" || arg == "--idfs")
        return 3;
}

int main(int argc, char* argv[])
{


    srand(time(NULL));

    //parse args
    if(argc<3)
    {
        cout << "Arguments missing.\n";
        printUsageInfo();
    }
    else
    {
        if((argv[1]=="-a" || argv[1]=="--a")&&argc<4)
        {
            cout << "Arguments missing.\n";
            printUsageInfo();
        }
        else
        {
            switch(argZeroToInt(argv[1]))
            {
                case 0:
                    cout << "A*";
                break;
                case 1:
                    cout << "BFS";
                break;
                case 2:
                    cout << "DFS";
                break;
                case 3:
                    cout << "iDFS";
                break;
            }
        }
    }
    //


    Stan start;
    //start.tworzPlansze(true); //mozna wygenerowac sobie losowa plansze tylko nalezy najpierw ustawic rozmiar planszy
    start.wczytajPlansze();

    Stan stop;
    stop.liczbaWierszy = start.liczbaWierszy;
    stop.liczbaKolumn = start.liczbaKolumn;
    stop.tworzPlansze(false);

    Graf* graf = new Graf(true,0);

    Vertex* vertexStart = new Vertex(start);
    graf->addVertex(vertexStart,-1);

    tworzGraf(vertexStart, graf);

    Vertex* vertexStop = new Vertex(stop);

    int pathlength = 0; vector<int> path;

    bool koniec = graf->DFSFindVertex(vertexStart, vertexStop, pathlength, path);

    cout <<"DFS: " << endl;
	if (!koniec) {
		cout << "BRAK ROZWIAZANIA" << endl;
	} else {

		cout << "LICZBA RUCHOW: " << pathlength << endl;
		for (int i = (path.size() - 1); i >= 0; i--) {
			Stan tmp = graf->getVertex(path.at(i))->stan;
			cout << tmp.kierunekPrzemieszczeniaDziury;
		}
	}

    path.clear();
    pathlength = 0;
    graf->unvisitAllVertices();

    koniec = graf->BFSFindVertex(vertexStart, vertexStop, pathlength, path);

	cout << "\n\nBFS: " << endl;
	if (!koniec) {
		cout << "BRAK ROZWIAZANIA" << endl;
	} else {

		//poczawszy od konca skacz po parentach
		Vertex* tmpvptr = graf->getVertex(vertexStop->stan);
		//test
		//Vertex* tmpvptr = graf->getVertex(4);
		//
		int liczbaruchow = 0;
		stack<char> resultPath;
		while(tmpvptr!= vertexStart && tmpvptr!=NULL)
		{
		    resultPath.push(tmpvptr->stan.kierunekPrzemieszczeniaDziury);
		    tmpvptr=tmpvptr->BFS_parent;
		    liczbaruchow++;
		}
		cout << "LICZBA RUCHOW: " << liczbaruchow << "\n";
		while(!resultPath.empty())
		{
		    char tmp = resultPath.top();
		    resultPath.pop();
		    cout << tmp;
		}
	}

	delete vertexStart;
	delete vertexStop;
	delete graf;

	return 0;

}
