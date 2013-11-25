#include <ilcplex/ilocplex.h>
#include <ilcplex/cplex.h>
#include <string>
#include <vector>
#include "grafo.h"
#include <stdio.h>
#include <stdlib.h>

#define TOL 1E-05
#define CLIQUES 1
#define CICLOS_IMPARES 2



using namespace std;



ILOSTLBEGIN


/* The following structure will hold the information we need to 
   pass to the cut callback function */

struct cutinfo {
    CPXLPptr lp;
    int      numcols;
    int      num;
    double   *x;
    int      *beg;
    int      *ind; 
    double   *val;
    double   *rhs;
};
typedef struct cutinfo CUTINFO, *CUTINFOptr;



static int CPXPUBLIC
mycutcallback      (CPXCENVptr env, void *cbdata, int wherefrom,
						void *cbhandle, int *useraction_p);

static int
makeusercuts       (CPXENVptr env, CPXLPptr lp, CUTINFOptr cutinfo,
						vector< vector<int> > vectorRestricciones,
						vector< vector<int> > vectorRestricciones2);

string convertInt(int n)
{
    stringstream ss;	//create a stringstream
    ss << n;	//add number to the stream
    return ss.str();//return a string with the contents of the stream
}

bool funcionComparar (pair < double, int > i,
					  pair < double, int > j)
{
	return ( i.first > j.first );
}


int main(){

    // Datos de la instancia de conjunto independiente.
    grafo g = grafo(0);
    g.inicializar();    	//inicializo en grafo por entrada standard
    int n = g.cantNodos;    // cuantos vertices hay en el grafo
	
	//g.heuristica();
	// encuentro las cliques y los ciclos impares

	vector < vector <int> > vectorRestriccionesCliques;
	vector < vector <int> > vectorRestriccionesCiclos;
	g.cliques(vectorRestriccionesCliques);
	g.ciclosImpares(vectorRestriccionesCiclos);
	
	int cantCortes = vectorRestriccionesCliques.size() +
						vectorRestriccionesCiclos.size();
	
	// Genero el problema de cplex.
    int status;
    CPXENVptr env; 	// Puntero al entorno.
    CPXLPptr lp; 	// Puntero al LP

    // Creo el entorno.
    env = CPXopenCPLEX(&status);

    // Creo la variable usecutinfo para guardarme informacion para el callback
    CUTINFO usercutinfo;

    usercutinfo.x   = NULL;
    usercutinfo.beg = NULL;
    usercutinfo.ind = NULL; 
    usercutinfo.val = NULL;
    usercutinfo.rhs = NULL;

    if (env == NULL) {
	cerr << "Error creando el entorno" << endl;
	exit(1);
    }

    // Creo el LP.
    lp = CPXcreateprob(env, &status, "Conjunto independiente");

    if (lp == NULL) {
	cerr << "Error creando el LP" << endl;
	exit(1);
    }

    // Definimos las variables. No es obligatorio pasar los nombres
    // de las variables, pero facilita el debug. La info es la siguiente:
    double *ub, *lb, *objfun; // Cota superior, cota inferior, coeficiente de la funcion objetivo.
    char *xctype, **colnames; // tipo de la variable (por ahora son siempre continuas), string con el nombre de la variable.
    ub = new double[n]; 
    lb = new double[n];
    objfun = new double[n];
    xctype = new char[n];
    colnames = new char*[n];

    for (int i = 0; i < n; i++) {
	ub[i] = 1.0;
	lb[i] = 0.0;
	objfun[i] = 1;//pj[i];
	xctype[i] = 'B'; // 'C' es continua, 'B' binaria, 'I' Entera. 
	// Nombre de la variable.
	stringstream name;
	name << "x_" << i+1;
	string namestr(name.str());
	int len = strlen(namestr.c_str());
	colnames[i] = new char[len+1];
	strcpy(colnames[i], namestr.c_str());
    }


    // Agrego las columnas.
    status = CPXnewcols(env, lp, n, objfun, lb, ub, xctype, colnames);

    if (status) {
	cerr << "Problema agregando las variables CPXnewcols" << endl;
	exit(1);
    }

    // Libero las estructuras.
    for (int i = 0; i < n; i++) {
	delete[] colnames[i];
    }

    delete[] ub;
    delete[] lb;
    delete[] objfun;
    delete[] xctype;
    delete[] colnames;

    // CPLEX pór defecto minimiza. Le cambiamos el sentido a la funcion objetivo.
    CPXchgobjsen(env, lp, CPX_MAX);

    // Generamos de a una las restricciones. En este caso, hay una sola.
    // Estos valores en genral estan fijos. Indican:
    // ccnt = numero nuevo de columnas en las restricciones.
    // rcnt = cuantas restricciones es estan pasando en las restricciones.
    // nzcnt = # de coeficientes != 0 a ser agregados a la matriz. Solo se pasan los valores que no son cero.
    // zero = 0. En realidad, esto reemplaza al parametro rmatbeg ya que se agrega una unica restriccion. Si se agregaran
    // de a mas de una, rmatbeg (ver documentacion de CPXaddrows) deberia tener la posicion en la que comienza cada 
    // restriccion.

    for (int i = 0; i < g.cantNodos; i++){
	int ccnt = 0, zero = 0, nzcnt = 0, rcnt = 1;
	char sense = 'L'; // Sentido de la desigualdad. 'G' es mayor o igual y 'E' para igualdad.
	double rhs = 1; // termino independiente. En nuestro caso 1.

	int *cutind = new int[2]; // Array con los indices de las variables con coeficientes != 0 en la desigualdad.
	double *cutval = new double[2]; // Array que en la posicion i tiene coeficiente ( != 0) de la variable cutind[i] en la restriccion.
	// Podria ser que algun coeficiente sea cero. Pero a los sumo vamos a tener n coeficientes. CPLEX va a leer hasta la cantidad
	// nzcnt que le pasemos.

	nzcnt = 0;
	
	
		for (int j = i+1; j < g.cantNodos ; j++){

			if (g.hayArista(i,j))
			{
			cutind[nzcnt] = i;
			cutval[nzcnt] = 1;
			nzcnt++;
			cutind[nzcnt] = j;
			cutval[nzcnt] = 1;
			nzcnt++;
				if (nzcnt == 2){
					// Esta rutina agrega la restriccion al lp.
					status = CPXaddrows(env, lp, ccnt, rcnt, nzcnt, &rhs, &sense, &zero, cutind, cutval, NULL, NULL);
					nzcnt = 0;
				} 
			}

		}
	}


    if (status) {
	cerr << "Problema agregando restriccion de capacidad." << endl;
	exit(1);
    }

    // Seteo de algunos parametros.
    // Para desactivar la salida poner CPX_OFF.
    status = CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_ON);

    if (status) {
	cerr << "Problema seteando SCRIND" << endl;
	exit(1);
    }

    // Por ahora no va a ser necesario, pero mas adelante si. Setea el tiempo
    // limite de ejecucion.
    status = CPXsetdblparam(env, CPX_PARAM_TILIM, 3600);

    if (status) {
	cerr << "Problema seteando el tiempo limite" << endl;
	exit(1);
    }

    // Escribimos el problema a un archivo .lp.
    status = CPXwriteprob(env, lp, "conjunto.lp", NULL);

    if (status) {
	cerr << "Problema escribiendo modelo" << endl;
	exit(1);
    }


    /* Turn on traditional search for use with control callbacks */

    status = CPXsetintparam (env, CPX_PARAM_MIPSEARCH, CPX_MIPSEARCH_TRADITIONAL);
    if (status) {
	cerr << "Problema seteando traditional search" << endl;
	exit(1);
    }
    /* Let MIP callbacks work on the original model */

    status = CPXsetintparam (env, CPX_PARAM_MIPCBREDLP, CPX_OFF);
    if ( status ){
	cerr << "Problema seteando para que acepte callback" << endl;
	exit(1);
    }
		
		
	// saco las optimizaciones de CPLEX
    CPXsetintparam (env, CPX_PARAM_MIPCBREDLP, CPX_OFF);
	CPXsetintparam(env, CPX_PARAM_PREIND, 0);
	CPXsetintparam(env, CPX_PARAM_PRELINEAR, 0);
	CPXsetintparam(env, CPX_PARAM_EACHCUTLIM, 0);
	CPXsetintparam(env, CPX_PARAM_CUTPASS, 0);
	CPXsetintparam(env, CPX_PARAM_FRACCUTS, -1);
	CPXsetintparam(env, CPX_PARAM_HEURFREQ, -1);
	CPXsetintparam(env, CPX_PARAM_RINSHEUR, -1);
	CPXsetintparam(env, CPX_PARAM_REDUCE, 0);
	CPXsetintparam(env, CPX_PARAM_IMPLBD, -1);
	CPXsetintparam(env, CPX_PARAM_MCFCUTS, -1);
	CPXsetintparam(env, CPX_PARAM_ZEROHALFCUTS, -1);
	CPXsetintparam(env, CPX_PARAM_MIRCUTS, -1);
	CPXsetintparam(env, CPX_PARAM_GUBCOVERS, -1);
	CPXsetintparam(env, CPX_PARAM_FLOWPATHS, -1);
	CPXsetintparam(env, CPX_PARAM_FLOWCOVERS, -1);
	CPXsetintparam(env, CPX_PARAM_DISJCUTS, -1);
	CPXsetintparam(env, CPX_PARAM_COVERS, -1);
	CPXsetintparam(env, CPX_PARAM_CLIQUES, -1);
	CPXsetintparam(env, CPX_PARAM_THREADS, 1);
	CPXsetintparam(env, CPX_PARAM_MIPSEARCH, 1);
	
	// seteo la estrategia de seleccion de nodos
	CPXsetintparam(env, CPX_PARAM_NODESEL, CPX_NODESEL_DFS);	// dfs
	//CPXsetintparam(env, CPX_PARAM_NODESEL, CPX_NODESEL_BESTEST); 
	
	// seteo la estrategia de branching
	CPXsetintparam(env, CPX_PARAM_BRDIR , 1);
	
	//status = CPXsetintparam(env, CPX_PARAM_VARSEL, -1);
    
    //cut and branch:
    //status = CPXgetcallbacknodeinfo(env, cbdata, CPX_CALLBACK_MIP_NODE,
    //                 0, CPX_CALLBACK_INFO_NODE_DEPTH, &numiinf);
    
    /* Create user cuts for noswot problem */

    status = makeusercuts (env, lp, &usercutinfo,vectorRestriccionesCliques, vectorRestriccionesCiclos);
    if ( status ){
	cerr << "Problema haciendo el mekeusercuts" << endl;
	exit(1);
    }

    /* Seteo el callback */

    status = CPXsetusercutcallbackfunc (env, mycutcallback, &usercutinfo);	// tercer parametro info
    if ( status ){
	cerr << "Problema haciendo el callback" << endl;
	exit(1);
    }  

    // Tomamos el tiempo de resolucion utilizando CPXgettime.
    double inittime, endtime;
    status = CPXgettime(env, &inittime);

    // Optimizamos el problema.
    status = CPXmipopt(env, lp);

    status = CPXgettime(env, &endtime);

    if (status) {
	cerr << "Problema optimizando CPLEX" << endl;
	exit(1);
    }

    double objval;
    status = CPXgetobjval(env, lp, &objval);

    if (status) {
	cerr << "Problema obteniendo mejor solucion entera" << endl;
	exit(1);
    }

    cout << "Datos de la resolucion: " << "\t" << objval << "\t" << (endtime - inittime) << endl; 

    // Tomamos los valores de la solucion y los escribimos a un archivo.
    std::string outputfile = "conjunto.sol";
    ofstream solfile(outputfile.c_str());

    // Tambien imprimimos el estado de la solucion.
    int solstat;
    char statstring[510];
    CPXCHARptr p;

    solstat = CPXgetstat(env, lp);
    p = CPXgetstatstring(env, solstat, statstring);
    string statstr(statstring);

    // Tomamos los valores de todas las variables. Estan numeradas de 0 a n-1.
    double *sol = new double[n];
    status = CPXgetx(env, lp, sol, 0, n - 1);

    if (status) {
	cerr << "Problema obteniendo la solucion del LP" << endl;
	exit(1);
    }


    // Solo escribimos las variables distintas de cero (tolerancia, 1E-05).
    solfile << "Status de la solucion: " << statstr << endl;
    std::vector<int> vec;
    for (int i = 0; i < n; i++) {
	if (sol[i] > TOL) {
	    solfile << "x_" << i+1 << " = " << sol[i] << endl;
	    vec.push_back(i+1);
	}
    }

    delete [] sol;
    solfile.close();
    //g.draw(vec);
    return 0;
}



/* This simple routine frees up the pointer *ptr, and sets *ptr
   to NULL */

    static void
free_and_null (char **ptr)
{
    if ( *ptr != NULL ) {
	free (*ptr);
	*ptr = NULL;
    }
} /* END free_and_null */ 


    static int CPXPUBLIC 
mycutcallback (CPXCENVptr env,
	void       *cbdata,
	int        wherefrom,
	void       *cbhandle,
	int        *useraction_p)
{
    int status = 0;

	CUTINFOptr cutinfo = (CUTINFOptr) cbhandle;

    int      numcols  = cutinfo->numcols;
    int      numcuts  = cutinfo->num;
    double   *x       = cutinfo->x;
    int      *beg     = cutinfo->beg;
    int      *ind     = cutinfo->ind;
    double   *val     = cutinfo->val;
    double   *rhs     = cutinfo->rhs;
    int      *cutind  = NULL;
    double   *cutval  = NULL;
    double   cutvio;
    int      addcuts = 0;
    int      i, j, k, cutnz;

    *useraction_p = CPX_CALLBACK_DEFAULT; 

    status = CPXgetcallbacknodex (env, cbdata, wherefrom, x,
	    0, numcols-1); 
    if ( status ) {
	fprintf(stderr, "Failed to get node solution.\n");
	return (status);
}

	vector < pair <double, int> >  cutvios;
	vector <int*> vectorCutind;
	vector <double*> vectorCutval;
	vector<int> vectorCutnz;
	
    for (i = 0; i < numcuts; i++) {
	cutvio = -rhs[i];
	//cout << "este es el corte " << i << endl;
	
	//cout << "cutvio es " << cutvio << endl;
	k = beg[i];
	cutnz = beg[i+1] - k;
	cutind = ind + k;
	cutval = val + k;
	for (j = 0; j < cutnz; j++) {
	    //cout << "j es " << j << " y el otro indice es " << cutind[j] << endl;
	    cutvio += x[cutind[j]] * cutval[j];
	    
	}
	pair < double, int > parCutvio = make_pair(cutvio,i);
	cutvios.push_back( parCutvio );
	vectorCutind.push_back(cutind);
	vectorCutval.push_back(cutval);
	vectorCutnz.push_back(cutnz);
	
    }
    
    // seteo la cantidad maxima de cortes por iteracion
    int cantMaxCortesPorIteracion = 4;
    
    sort(cutvios.begin(), cutvios.end(), funcionComparar);
    
		for ( int i = 0 ; i < cantMaxCortesPorIteracion; i++) {
			int elcorte = cutvios[i].second;
			if(cutvios[i].first > 0.01){ 
				cout << "agrego el corte " << elcorte  <<endl;
				status = CPXcutcallbackadd (env, cbdata, wherefrom,
				vectorCutnz[elcorte], rhs[elcorte], 'L',
				vectorCutind[elcorte], vectorCutval[elcorte], 1);
					if ( status ) {
						fprintf (stderr, "Failed to add cut.\n");
						return (status);
					}
			addcuts++;
			}
		}
	
    
    /* Tell CPLEX that cuts have been created */ 
    if ( addcuts > 0 ) {
	*useraction_p = CPX_CALLBACK_SET; 
    }

    return (status);

} /* END mycutcallback */


    static int
makeusercuts (CPXENVptr  env,
	CPXLPptr   lp,
	CUTINFOptr usercutinfo,
	vector< vector<int> > vectorRestricciones,
	vector< vector<int> > vectorRestriccionesCiclos)
{
    int status = 0;
	
    // ponemos el termino independiente
    // en el caso de la clique 1
    //en el caso de ciclo impar, tamano de ciclo (impar -1) / 2

    int    *cutbeg = NULL;
    int    *cutind = NULL;
    double *cutval = NULL;
    double *cutrhs = NULL; 
    bool *cutMarcados = NULL; 

    int i, varind;
    int cantidadVariables = 0;
    for(int i = 0; i < vectorRestricciones.size(); i++){
		cantidadVariables += vectorRestricciones[i].size();
    }
    
    for(int i = 0; i < vectorRestriccionesCiclos.size(); i++){
		cantidadVariables += vectorRestriccionesCiclos[i].size();
    }
    
    int nz   = cantidadVariables;		// cantidad de variables que no son cero
    int cuts = vectorRestricciones.size() + vectorRestriccionesCiclos.size();	// cantidad de cortes

	cout << "son " << vectorRestricciones.size() << " cliques y " << vectorRestriccionesCiclos.size() << " ciclos" << endl;
		

    int cur_numcols = CPXgetnumcols (env, lp);

    usercutinfo->lp = lp;
    usercutinfo->numcols = cur_numcols;

    usercutinfo->x = (double *) malloc (cur_numcols * sizeof (double));
    if ( usercutinfo->x == NULL ) {
	fprintf (stderr, "No memory for solution values.\n");
	//goto TERMINATE;
    }

    cutbeg = (int *)    malloc ((cuts+1) * sizeof (int));
    cutind = (int *)    malloc (nz * sizeof (int));
    cutval = (double *) malloc (nz * sizeof (double));
    cutrhs = (double *) malloc (cuts * sizeof (double));
	cutMarcados = (bool *) malloc (cuts * sizeof (bool));
	
    if ( cutbeg == NULL ||
	    cutind == NULL ||
	    cutval == NULL ||
	    cutrhs == NULL  ||
	    cutMarcados == NULL ) {
	fprintf (stderr, "No memory.\n");
	status = CPXERR_NO_MEMORY;
//	goto TERMINATE;
    } 

    char varname[256];
    char buffer[256];
    int contadorVariableNZ = 0;
	for (i = 0; i < cuts; i++) {	    // itera por la cantidad de restricciones
	
		if (i < vectorRestricciones.size() ){
		for ( int j = 0; j < vectorRestricciones[i].size(); j++ ){  // itera por cada restriccion
			
			strcpy(varname,"x_");
			sprintf(buffer, "%d",vectorRestricciones[i] [j] +1); 
			strcat(varname,buffer) ; // el nombre de la i-esima variable 
			status = CPXgetcolindex (env, lp, varname, &varind);	// varname tiene que ser un puntero
			if ( status )  {
			fprintf (stderr, "Failed to get index from variable name.\n");
		//	goto TERMINATE;
			}

		cutind[contadorVariableNZ] = varind;
		cutval[contadorVariableNZ] = 1;	// el coeficiente de cada variables
		contadorVariableNZ++;			// por cada restriccion incremento el contador, para que agregue todas las variables
		}
		
		}else{
			for ( int j = 0; j < vectorRestriccionesCiclos[i - vectorRestricciones.size()].size(); j++ ){  // itera por cada restriccion
			
			strcpy(varname,"x_");
			sprintf(buffer, "%d",vectorRestriccionesCiclos[i - vectorRestricciones.size()] [j] +1); 
			strcat(varname,buffer) ; // el nombre de la i-esima variable 
			status = CPXgetcolindex (env, lp, varname, &varind);	// varname tiene que ser un puntero
			if ( status )  {
			fprintf (stderr, "Failed to get index from variable name.\n");
		//	goto TERMINATE;
			}

			cutind[contadorVariableNZ] = varind;
			cutval[contadorVariableNZ] = 1;	// el coeficiente de cada variables
			contadorVariableNZ++;			// por cada restriccion incremento el contador, para que agregue todas las variables
		}

		}
		
    }
   
    if (contadorVariableNZ-1 == cantidadVariables){
	cout << "no esta poniendo todas las variables" << endl;
    }

    int contador = 0;
    for (i = 0; i < cuts; i++) {
		if (i < vectorRestricciones.size()){
			cutbeg[i] = contador;
			//cout << "el corte " << i << " empieza en " << contador << endl;
			contador += vectorRestricciones[i].size();  // le sumo la cantidad de variables distintas de 0 que tenia la iesima restriccion
				cutrhs[i] = 1;	    // si es clique
		}
		else{
		cutbeg[i] = contador;
		//cout << "el corte " << i << " empieza en " << contador << endl;
		contador += vectorRestriccionesCiclos[i-vectorRestricciones.size()].size();  // le sumo la cantidad de variables distintas de 0 que tenia la iesima restriccion
			
		cutrhs[i] = (vectorRestriccionesCiclos[i-vectorRestricciones.size()].size() - 1) / 2;	    // si es ciclo impar
		//cout << "el rhs es " << cutrhs[i] << endl;
		}
	}
    
    cutbeg[cuts] = contador; //beg[cuts];	// ????????????

    usercutinfo->num      = cuts;
    usercutinfo->beg      = cutbeg;
    usercutinfo->ind      = cutind;
    usercutinfo->val      = cutval;
    usercutinfo->rhs      = cutrhs;
    
	cout << "creo todos los cortes" << endl;
		

TERMINATE:

    if ( status ) {
	free_and_null ((char **) &cutbeg);
	free_and_null ((char **) &cutind);
	free_and_null ((char **) &cutval);
	free_and_null ((char **) &cutrhs);
	free_and_null ((char **) &cutMarcados);
    }

    return (status);

} /* END makeusercuts */

