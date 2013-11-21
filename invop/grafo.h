#ifndef GRAFO_H
#define GRAFO_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <cmath>				//time
#include <stdio.h>
#include <fstream>
#include <set>

using namespace std;

bool funcionSort (vector <int> i, vector <int> j) { return ( i.size() > j.size() ); }


struct arista{
    int nodo1;
    int nodo2;

    arista(int n, int m){
        this->nodo1 = n;
        this->nodo2 = m;
    }

};

class grafo{
    public:
        int cantNodos;
        vector< vector<int> > lista_global;
        vector< vector<int> > matriz_ady;
        vector<arista> aristas;

        //Constructor: Construye un grafo vacío
        grafo(){
            grafo(0);
        }

        //Constructor: Construye un grafo con n nodos sin aristas
        grafo(int n){
            constructor(n);
        }

        //Construye el grafo a partir de la entrada estándar
        // (llamar después del constructor vacío)
        void inicializar() {
            int n, m, a, b, c;
            cin >> n;
            constructor(n);
            if(n != 0) {
                cin >> m;
                for(int i = 0; i < m; ++i){
                    cin >> a;
                    cin >> b;
                    asociar(a,b);
                }
            }
        }

        //Construye un grafo con aristas aleatorias
        // (llamar después del constructor básico)
        void generar_aristas_aleatorias() {
            //cambio esta funcion para que genere un grafo completamente random
            srand(time(NULL));
            for(int i = 0; i < this->cantNodos; ++i)
                for(int j = i; j < this->cantNodos; ++j)
                    if(rand()%2 == 0) asociar(i+1, j+1);
        }

        void asociar(int nodo1, int nodo2){
            nodo1 = nodo1-1;
            nodo2 = nodo2-1;

            if(nodo1 != nodo2){
                //cout << "asociar(" << nodo1+1 << "," << nodo2+1 << ")" << endl;

                /* Agrego la arista faltante  */
                this->aristas.push_back(arista(nodo1,nodo2));

                //agrego cada nodo en la lista del otro nodo incidente

                (this->lista_global[nodo1]).push_back(nodo2);
                (this->lista_global[nodo2]).push_back(nodo1);

                (this->matriz_ady[nodo1][nodo2]) = 1;
                (this->matriz_ady[nodo2][nodo1]) = 1;
            }
        }


        int grado(int nodo) const{
            nodo--;
            return this->lista_global[nodo].size();
        }

        bool hayArista(int nodo1, int nodo2) const{
            return (matriz_ady[nodo2-1][nodo1-1] == 1);
        }

        // Devuelve un vector de int con los vecinos del nodo 'n'
        vector<int> vecinos(int n){
            vector<int> vecinos;
            //cout << "vecino de " << n + 1 << ": ";
            for (int i = 0; i < (this->lista_global[n]).size(); ++i){
                //cout  << (*this->lista_global[n])[i] << " ";
                vecinos.push_back((this->lista_global[n])[i]);
            }
            //cout << endl;
            return vecinos;
        }

        int nodoDeGradoMaximo(){
            int nodoMax = -1;
            int gradoMax = -1;
            for (int i = 0; i < cantNodos; i++ ){
                if (grado(i+1) > gradoMax){
                    nodoMax = i;
                    gradoMax = grado(i+1);
                }
            }
            return nodoMax;
        }

        // Devuelve si el nodo 'nodo' pertenece al vector de nodos 'clique'
        bool esDelClique (int nodo, const vector<int> clique){
            for(int i = 0; i < clique.size(); ++i){
                if (nodo == clique[i]){
                    return true;
                }
            }
            return false;
        }

        // Devuelve si el nodo 'nodo' se conecta con la clique 'clique'
        bool seConectaConCliqueActual(int nodo, vector <int> clique){
            for (int i = 0; i < clique.size();++i){
                //TODO: Revisar, creo que hayArista toma 1..n pero acá se pasan 0..n-1
                if ( !(hayArista(clique[i], nodo)) ){
                    return false;
                }
            }
            return true;
        }

        // Devuelve si el conjunto 'nodos' representa una clique
        bool es_clique(vector<bool> nodos) {
            vector<int> clique = vector<int>();
            for (int i=0; i<cantNodos; i++) {
                if (nodos[i]) {
                    for (int j=0; j<clique.size(); j++) {
                        if(matriz_ady[i][clique[j]]==0) return false;
                    }
                    clique.push_back(i);
                }
            }
            return true;
        }


        /* agarro un nodo que lo paso por parametro
           visito a todos sus vecinos
           y a los que no estan marcados, los visito recursivamente y los marco
           */
        // empiezo con contador en uno, cuando es impar encontre un ciclo impar

        void devolverCicloImpar(int nodoInicial, int nodo, int nodoFinal,const vector<int> &predecesor, vector < vector <int> > &elvec, vector<bool> &marcado){

            vector<int> vec;

            int temp =  nodo;
            //cout << endl;

            //cout << nodoFinal + 1 << " " ;
            vec.push_back(nodoFinal + 1 );
            marcado[nodoFinal] = true;

            int contador =0;
            while(temp != nodoFinal ){
                //cout << temp+1 << " ";
                vec.push_back(temp+1);
                marcado[temp] = true;
                temp = predecesor[temp];
                if (contador++ > cantNodos){
                    cout << "HAY ALGO MAL ACA " << contador << " " << cantNodos <<  endl;
                    break;
                }
            }
            //cout << nodoFinal + 1 << endl;
            vec.push_back(nodoFinal + 1);

            elvec.push_back(vec);
        }

        // probar con marcado pasado por referencia y sin. y con  esa restriccion del predecesor en el if y sin

        int dfsRecur(int nodoInicial, int nodo, vector <int> &predecesor, int contador, vector< vector <int> > & vec2vec, vector<bool> &marcado){

            //cout << "me llaman con el nodo " << nodo +1<< endl;
            for (int i = 0; i < vecinos(nodo).size(); i++)
            {
                int elvecino = vecinos(nodo)[i]; 
                if (marcado[elvecino]) continue;

                if(predecesor[elvecino] == -1){

                    //cout << elvecino +1 << endl;
                    predecesor[elvecino] = nodo;
                    //contador++;
                    dfsRecur(nodoInicial, elvecino,predecesor, contador + 1, vec2vec, marcado);
                    predecesor[elvecino] = -1;



                }else{
                    if((contador % 2) == 1 ){//&& elvecino != predecesor[nodo]){		// encontre un ciclo impar
                        //cout << "el " << elvecino + 1 << " el predecor de " << nodo + 1  << " es " << predecesor[nodo] + 1 << " " <<  contador << endl;
                        devolverCicloImpar(nodoInicial, nodo, elvecino , predecesor, vec2vec, marcado);
                        return 0;
                    } 

                }

            }

            return 0;

        }


        void dfs(int nodo, vector< vector <int> > & vec2vec, vector<bool> &marcado){

            vector<int> predecesor(cantNodos,-1);
            predecesor[nodo] = -2;
            int contador = 1;
            dfsRecur(nodo, nodo, predecesor, contador, vec2vec, marcado);

        }



        int generarCliquesMaximales(vector< vector<int> > &vec2vec){	


            vector<int> vec;
            set<int> setPuestos;

            for (int i = 0; i < cantNodos; i++)
                setPuestos.insert(i+1);

            while( !setPuestos.empty() ){

                std::set<int>::iterator it=setPuestos.begin();

                //cout << "empiezo con " << *it << endl;
                
               // if( setPuestos.find(*it) != setPuestos.end()){
					vec = generarCliqueMaximal(*it, setPuestos);
					vec2vec.push_back(vec);
				//}

                //cout << "Nueva clique" << endl;

                /*
                   for (int i = 0; i < vec.size(); i++)
                   cout << vec[i] << " " ;

                   cout << endl;
                   */
            }
        }


        vector<int> generarCliqueMaximal(int nodo, set<int> &setPuestos){

            vector<int> clique;
            set <int> seConectanConCliqueActual;

            nodo = nodo -1;
            for (int j = 0; j < (this->lista_global[nodo]).size(); ++j){
                //cout  << "los vecinos de " <<(this->lista_global[nodo])[j] << " ";
                seConectanConCliqueActual.insert((this->lista_global[nodo])[j] +1 );
            }

            clique.push_back(nodo+1);
            setPuestos.erase(nodo+1);
            //cout << "agrego el " << nodoMax + 1 << endl;
            //seConectanConCliqueActual.erase(nodoMax+1);


            while( !seConectanConCliqueActual.empty() ){		//mientras no es vacio, voy agregando nodos y reduciendo el set

                int elQueMasAporta;
                // agrego al que mas aporta dentro del set que se conectan con la clique


                int maximo_aporte = 0;

                for (set<int>::iterator it= seConectanConCliqueActual.begin(); it!=seConectanConCliqueActual.end(); ++it){				
                    int aporta = grado(*it);
                    //cout << "esta el" << *it << endl;
                    if (aporta > maximo_aporte){
                        elQueMasAporta = *it;
                        maximo_aporte = aporta;
                    }
                }

                clique.push_back(elQueMasAporta);
                setPuestos.erase(elQueMasAporta);
                seConectanConCliqueActual.erase(elQueMasAporta);


                // saco a todos los que no son adyacentes al nodo que acabo de agregar
                for (set<int>::iterator it= seConectanConCliqueActual.begin(); it!=seConectanConCliqueActual.end(); ++it){				
                    if ( !hayArista(*it, elQueMasAporta)){
                        seConectanConCliqueActual.erase(*it);
                    }
                }

            }
            return clique;
        }


		void imprimirVectorRestricciones( vector< vector<int> > vec){
	
			for (int i = 0; i < vec.size(); i++)
			{
				for (int j = 0; j < vec[i].size(); j++)
				{
					cout << vec[i][j] << " " ;
				}
				cout << endl;
			}
			
			cout << "me dio " << vec.size() << "cliques" << endl;
		}

		void cliques( vector< vector<int> > &elvec){
		 
			generarCliquesMaximales(elvec);
			
			sort (elvec.begin(), elvec.end(), funcionSort); 
			
			vector < vector<int> > temp;
			
			for (int i = 0; i < elvec.size(); i++)
			{
				if (elvec[i].size() > 4){
				temp.push_back(elvec[i]);
				}
			}
			
			elvec = temp;
			
			imprimirVectorRestricciones(elvec);
		 
			//draw();
		 
		}


		void ciclosImpares( vector< vector<int> > &elvec){
		 
			vector<bool> marcado(cantNodos,false);
			
			for (int k = 0; k < cantNodos; k++)
			{
				dfs(k, elvec, marcado);
			}
			
			vector < vector<int> > temp;
			
			
			
			
			sort (elvec.begin(), elvec.end(), funcionSort); 
			
			for (int k = 0; k < 100; k++)
			{
				temp.push_back(elvec[k]);
			}
			
			elvec = temp;
			//imprimirVectorRestricciones(elvec);
			
			//draw();
			
		}


        // Determina cuanto varía la frontera de la clique 'clique'
        // al agregar el nodo 'nodo'
        int cuanto_aporta(int nodo, vector<int> clique){
            // hay que sumar el grado del nodo y restarle todas las aristas
            // que conectan al nodo candidato con los nodos del clique
            int yaEstaban = 0;
            for (int i=0; i < clique.size();++i){
                if ( (hayArista(clique[i],nodo)) ){
                    yaEstaban++;
                } 
            }
            return grado(nodo) - yaEstaban;
        }

                void print(){
            cout << "cantidad de nodos: " << this->cantNodos << endl;
            for(int i = 0; i < this->cantNodos; ++i){
                cout << "[" << i+1 << "] ";
                if(this->lista_global[i].size() == 0){
                    cout << "-> null " << endl;
                }else{
                    cout << "-> ";
                    for(int j = 0; j < (this->lista_global[i]).size(); ++j){
                        cout << (lista_global[i])[j]+1 << " -> ";
                    }
                    cout << "null " << endl;
                }
            }
        }

        void draw(){
            bool yaVistas[this->cantNodos][this->cantNodos];
            for(int i = 0; i < this->cantNodos; ++i){
                for(int j = 0; j < this->cantNodos; ++j){
                    yaVistas[i][j] = false;
                }
            }

            ofstream graph;

            graph.open("grafo.dot");
            graph << "graph graphname{\nrankdir=\"LR\";\n";

            for(int i = 0; i < this->cantNodos; ++i){
                if(this->lista_global[i].size() != 0){
                    for(int j = 0; j < (this->lista_global[i]).size(); ++j){
                        if(yaVistas[i][(this->lista_global[i])[j]] == false || yaVistas[(this->lista_global[i])[j]][i] == false ){
                            graph << i+1 << " -- "  << (this->lista_global[i])[j]+1 << "\n";
                            yaVistas[i][(this->lista_global[i])[j]] = true;
                            yaVistas[(this->lista_global[i])[j]][i] = true;
                        }
                    }
                }else{
                    graph << i+1 << ";\n";
                }
            }

            graph << "}\n";
            graph.close();
            int res = system("dot -Tpng grafo.dot -o grafo.png");
            //system("rm -rf grafo.dot");
        }

        void draw(vector<int> clique){

            ofstream graph;
            graph.open("grafo.dot");
            graph << "graph graphname{\nrankdir=\"LR\";\n";

            for(int i = 0; i < clique.size(); ++i)
                graph << clique[i] << " [style=filled, fillcolor=red]" << endl;

            bool yaVistas[this->cantNodos][this->cantNodos];
            for(int i = 0; i < this->cantNodos; ++i){
                for(int j = 0; j < this->cantNodos; ++j){
                    yaVistas[i][j] = false;
                }
            }

            for(int i = 0; i < this->cantNodos; ++i){
                if(this->lista_global[i].size() != 0){
                    for(int j = 0; j < (this->lista_global[i]).size(); ++j){
                        if(yaVistas[i][(this->lista_global[i])[j]] == false || yaVistas[(this->lista_global[i])[j]][i] == false ){
                            graph << i+1 << " -- "  << (this->lista_global[i])[j]+1 << "\n";
                            yaVistas[i][(this->lista_global[i])[j]] = true;
                            yaVistas[(this->lista_global[i])[j]][i] = true;
                        }
                    }
                }else{
                    graph << i+1 << ";\n";
                }
            }

            graph << "}\n";
            graph.close();
            system("dot -Tpng grafo.dot -o grafo.png");
            //system("rm -rf grafo.dot");
        }

    private:
        void constructor(int n) {
            //cargo la cantidad de nodos
            this->cantNodos = n;

            vector <int> vec;

            //inicializo todas las listas vacias
            for(int i = 0; i < n; i++){
                (this->lista_global).push_back(vector <int>()) ;
            }

            for(int i = 0; i < n; i++){
                (this->matriz_ady).push_back(vector <int>(n)) ;
            }
        }

        };

// Devuelve la cantidad de vecinos de una vecindad de tamaño V en
// un grafo de n nodos


#endif 
