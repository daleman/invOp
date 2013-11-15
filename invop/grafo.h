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


#define tr(c,i) for(typeof((c).begin() i = (c).begin(); i != (c).end(); i++)

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
		
		/*
		int nodoSiguienteCliqueMaximal (vector <int> clique, vector <bool> estanEnClique){
			
			int maximo_aporte = 0;
			int elQueMasAporta = -1;
			
			for (int i = 0 ; i < cantNodos; ++i){				
				if ( se_conecta_con_clique(i, clique) && !estanEnClique[i] ){
					int aporta = grado(i+1);
					if (aporta > maximo_aporte){
						elQueMasAporta = i+1;
					}
				}
			}
			
		}
		*/
	
	
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
			if((contador % 2) == 1 && elvecino != predecesor[nodo]){		// encontre un ciclo impar
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
		vec = generarCliqueMaximal(*it, setPuestos);
		
		vec2vec.push_back(vec);
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
