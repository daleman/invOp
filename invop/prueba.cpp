#include "grafo.h"


bool funcionSort (vector <int> i, vector <int> j) { return ( i.size() > j.size() ); }


int main(){


 grafo g = grafo(0);
 g.inicializar();    //inicializo en grafo por entrada standard
 /*
	vector < vector<int> > vec;
	g.generarCliquesMaximales(vec);
	
	// using function as comp
	std::sort (vec.begin(), vec.end(), funcionSort); 
	
	
	for (int i = 0; i < vec.size(); i++)
	{
		for (int j = 0; j < vec[i].size(); j++)
		{
			cout << vec[i][j] << " " ;
		}
		cout << endl;
	}
	
	cout << "me dio " << vec.size() << "cliques" << endl;
 */
 
 vector< vector<int> > elvec;
 for (int k = 0; k < g.cantNodos; k++)
 {
	g.dfs(k, elvec);
	}
	std::sort (elvec.begin(), elvec.end(), funcionSort); 
	
	
	for (int i = 0; i < elvec.size(); i++)
	{
		for (int j = 0; j < (elvec[i]).size(); j++)
		{
			cout << elvec[i][j] << " " ;
		}
		cout << endl;
	}
	
	cout << "me dio " << elvec.size() << "ciclos impares" << endl;


 
	//g.draw();
 
}
