#ifndef GRAFO_H
#define GRAFO_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <cmath>				//time
#include <stdio.h>
#include <fstream>


using namespace std;
typedef pair <vector<bool>, int> Solucion;
unsigned long long combinatorio(int n, int k);
int f(int, int, int, int);

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
			/*
			 * for(int i = 0; i < this->lista_global[nodo1-1].size(); ++i){
				//cout << "tiene el nodo "<< nodo1 - 1 << " conexion con " << nodo2 - 1 << endl;
				if (lista_global[nodo1-1][i] == (nodo2-1)){
					//cout << "si" << endl;
					return true;
				}
			}
			*/
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
			for (int i = 0; i < cantNodos; ++i ){
				if (grado(i+1) > gradoMax){
					nodoMax = i;
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

		// Devuelve el v-ésimo vecino de la solucion s
		Solucion obtener_vecino(Solucion &s, Solucion &vAnterior, int v) {
			Solucion vecino;
			int nodos_distintos = 1;//Me dice cuantos nodos debo agregar/sacar
			int vecinos = combinatorio(cantNodos,1);
			while (v >= vecinos) {
				nodos_distintos++;
				v -= vecinos;
				vecinos = combinatorio(cantNodos, nodos_distintos);
			}
			if (v==0) {
				vecino = make_pair(s.first, s.second);
				for (int i=0; i<nodos_distintos; i++) {
					alternar(vecino, i);
				}
			} else {
				vecino = make_pair(vAnterior.first, vAnterior.second);
				int ov = 0;
				for (int i=cantNodos-1; i>=0; i--) {
					if (s.first[i]!=vAnterior.first[i]) {
						if (i==cantNodos-1-ov) {
							ov ++;
							alternar(vecino, i);
						} else {
							if (ov>0) {
								alternar(vecino, i);
								alternar(vecino, i+1);
								int j = i+2;
								while(ov>0 && j<cantNodos && vecino.first[j]==s.first[j]) {
									alternar(vecino, j);
									j++; ov--;
								}
								if (ov==0) break;								
							} else {
								alternar(vecino, i);
								alternar(vecino, i+1);
								break;
							}
						}
					}
				}
			}
			if (!es_clique(vecino.first)) vecino.second = 0;
			return vecino;
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
			system("dot -Tpng grafo.dot -o grafo.png");
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
					
					bool esDelCliqueI = (esDelClique( i+1, clique));
					
					for(int j = 0; j < (this->lista_global[i]).size(); ++j){
						if(yaVistas[i][(this->lista_global[i])[j]] == false || yaVistas[(this->lista_global[i])[j]][i] == false ){
							graph << i+1 << " -- "  << (this->lista_global[i])[j]+1;
							
							bool esDelCliqueJ = (esDelClique( (this->lista_global[i])[j]+1, clique));
							
							if (esDelCliqueI && esDelCliqueJ){
								 graph << "[penwidth=3 color=red]";
							}else if ((esDelCliqueI && !esDelCliqueJ) || (!esDelCliqueI && esDelCliqueJ)) {
								 graph << "[color=blue]";
							}
							graph << "\n";
							
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

		void alternar(Solucion &v, int i) {
			v.first[i] = !v.first[i];
			//Recalcular vecino.second (frontera) en función del nodo
			// sacado/agregado (no importa si es o no una clique, todavía)
			if (v.first[i]) {
				for (int j=0; j<i; j++) {
					if (hayArista(i+1,j+1)) {
						if (v.first[j]) {
							v.second--;
						} else v.second++;
					}
				}
				for (int j=i+1; j<cantNodos; j++) {
					if (hayArista(i+1,j+1)) {
						if (v.first[j]) {
							v.second--;
						} else v.second++;
					}
				}
			} else {
				for (int j=0; j<i; j++) {
					if (hayArista(i+1,j+1)) {
						if (v.first[j]) {
							v.second++;
						} else v.second--;
					}
				}
				for (int j=i+1; j<cantNodos; j++) {
					if (hayArista(i+1,j+1)) {
						if (v.first[j]) {
							v.second++;
						} else v.second--;
					}
				}				
			}
		}
};

// Devuelve la cantidad de vecinos de una vecindad de tamaño V en
	// un grafo de n nodos
unsigned long long cant_vecinos(int n, int V) {
	// 1 -> n
	// 2 -> n + n(n-1) /2 = n + (n 2)
	// 3 -> n + (n 2) + (n 3)
	// n -> n + (n 2) + ... (n n) = sum{i=1..n} (n i)
	unsigned long long res = 0;
	for (int i=1; i<=V; i++) {
		res += combinatorio(n,i);
	}
	return res;
}

unsigned long long combinatorio(int n, int k) {
	unsigned long long k1 = n-k;
	unsigned long long k2 = k;
	if (k > n/2) {
		k1 = k;
		k2 = n-k;
	}
	unsigned long long res = 1;
	k1++;
	for (unsigned long long i=k1; i<=n; i++) {
		res*=i;
	}
	for (unsigned long long i=2; i<=k2; i++) {
		res/=i;
	}
	return res;
}

Solucion convertir(int cantNodos, pair<vector<int>, int> solucionGolosa) {
	Solucion res = make_pair(vector<bool>(cantNodos), solucionGolosa.second);
	for (int i=0; i<cantNodos; i++) {
		res.first[i]=false;
	}
	for (int i=0; i<solucionGolosa.first.size(); i++) {
		res.first[solucionGolosa.first[i]-1] = true;
	}
	return res;
}

void salida(int cardinalFrontera, vector<int> clique){
		cout << cardinalFrontera << " " << clique.size() << " ";
		for (int i = 0; i < clique.size(); ++i){
			cout << clique[i] << " " ;
		}
		cout << endl;
}

#endif 
