#include "grafo.h"


int main(){


 grafo g = grafo(0);
 g.inicializar();    //inicializo en grafo por entrada standard
 
	vector<int> vec;
	set<int> setPuestos;
 
	for (int i = 0; i < g.cantNodos; i++)
	{
		setPuestos.insert(i+1);
	}
 
 
 	
	while( !setPuestos.empty() ){
	
	std::set<int>::iterator it=setPuestos.begin();
	
	cout << "empiezo con " << *it << endl;
	vec = g.generarCliqueMaximal(*it, setPuestos);

	cout << endl ;
	cout << "Nueva clique" << endl;
	
	for (int i = 0; i < vec.size(); i++)
	{
		cout << vec[i] << " " ;
	}
	cout << endl;
	

	}
 
}
