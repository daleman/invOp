#include <ilcplex/ilocplex.h>
#include <ilcplex/cplex.h>
#include <string>
#include <vector>
#include "grafo.h"

#define TOL 1E-05

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
   makeusercuts       (CPXENVptr env, CPXLPptr lp, CUTINFOptr cutinfo);


int main(){

  // Datos de la instancia de conjunto independiente.
  grafo g = grafo(0);
  g.inicializar();    //inicializo en grafo por entrada standard
  int n = g.cantNodos;    // cuantos vertices hay en el grafo

  // Genero el problema de cplex.
  int status;
  CPXENVptr env; // Puntero al entorno.
  CPXLPptr lp; // Puntero al LP
   
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

    
  // Definimos las variables. No es obligatorio pasar los nombres de las variables, pero facilita el debug. La info es la siguiente:
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
    xctype[i] = 'B'; // 'C' es continua, 'B' binaria, 'I' Entera. Para LP (no enteros), este parametro tiene que pasarse como NULL. No lo vamos a usar por ahora..
    // Nombre de la variable.
    stringstream name;
    name << "x_" << i+1;
    string namestr(name.str());
    int len = strlen(namestr.c_str());
    colnames[i] = new char[len+1];
    strcpy(colnames[i], namestr.c_str());
  }
  
  
  // Agrego las columnas.
  status = CPXnewcols(env, lp, n, objfun, lb, ub, xctype, colnames);    //hay que ver lo de LA FUNCION OBJETIVO
  
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
  
    for (int i = 1; i <= g.cantNodos; i++){
         int ccnt = 0, zero = 0, nzcnt = 0, rcnt = 1;
         char sense = 'L'; // Sentido de la desigualdad. 'G' es mayor o igual y 'E' para igualdad.
         double rhs = 1; // termino independiente. En nuestro caso 1.
 
        int *cutind = new int[2]; // Array con los indices de las variables con coeficientes != 0 en la desigualdad.
        double *cutval = new double[2]; // Array que en la posicion i tiene coeficiente ( != 0) de la variable cutind[i] en la restriccion.
    // Podria ser que algun coeficiente sea cero. Pero a los sumo vamos a tener n coeficientes. CPLEX va a leer hasta la cantidad
    // nzcnt que le pasemos.
    
        nzcnt = 0;
        for (int j = i+1; j <= g.cantNodos ; j++){
                 
            if (g.hayArista(i,j))
                {
                  cutind[nzcnt] = i-1;
                  cutval[nzcnt] = 1;
                  nzcnt++;
                  cutind[nzcnt] = j-1;
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
   /* Create user cuts for noswot problem */

   status = makeusercuts (env, lp, &usercutinfo);
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
  g.draw(vec);
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
      goto TERMINATE;
   }

   for (i = 0; i < numcuts; i++) {
      cutvio = -rhs[i];
      k = beg[i];
      cutnz = beg[i+1] - k;
      cutind = ind + k;
      cutval = val + k;
      for (j = 0; j < cutnz; j++) {
         cutvio += x[cutind[j]] * cutval[j];
      }

      /* Use a cut violation tolerance of 0.01 */

      if ( cutvio > 0.01 ) { 
         status = CPXcutcallbackadd (env, cbdata, wherefrom,
                                     cutnz, rhs[i], 'L',
                                     cutind, cutval, 1);
         if ( status ) {
            fprintf (stderr, "Failed to add cut.\n");
            goto TERMINATE;
         }
         addcuts++;
      }
   }

   /* Tell CPLEX that cuts have been created */ 
   if ( addcuts > 0 ) {
      *useraction_p = CPX_CALLBACK_SET; 
   }

TERMINATE:

   return (status);

} /* END mycutcallback */


/* Valid cuts for noswot 
   cut1: X21 - X22 <= 0
   cut2: X22 - X23 <= 0
   cut3: X23 - X24 <= 0
   cut4: 2.08 X11 + 2.98 X21 + 3.47 X31 + 2.24 X41 + 2.08 X51 
         + 0.25 W11 + 0.25 W21 + 0.25 W31 + 0.25 W41 + 0.25 W51
         <= 20.25
   cut5: 2.08 X12 + 2.98 X22 + 3.47 X32 + 2.24 X42 + 2.08 X52
         + 0.25 W12 + 0.25 W22 + 0.25 W32 + 0.25 W42 + 0.25 W52
         <= 20.25
   cut6: 2.08 X13 + 2.98 X23 + 3.4722 X33 + 2.24 X43 + 2.08 X53
         + 0.25 W13 + 0.25 W23 + 0.25 W33 + 0.25 W43 + 0.25 W53
         <= 20.25
   cut7: 2.08 X14 + 2.98 X24 + 3.47 X34 + 2.24 X44 + 2.08 X54
         + 0.25 W14 + 0.25 W24 + 0.25 W34 + 0.25 W44 + 0.25 W54
         <= 20.25
   cut8: 2.08 X15 + 2.98 X25 + 3.47 X35 + 2.24 X45 + 2.08 X55
         + 0.25 W15 + 0.25 W25 + 0.25 W35 + 0.25 W45 + 0.25 W55
         <= 16.25
*/

static int
makeusercuts (CPXENVptr  env,
              CPXLPptr   lp,
              CUTINFOptr usercutinfo)
{
   int status = 0;

   int beg[] = {0, 2, 4, 6, 16, 26, 36, 46, 56};	// ponemos los inicios de cada restriccion, que seran el inicio de la anterior mas
													// el tamano del vector de cada restriccion agregada


	int cantidadRestricciones = bla;
	// ponemos los valores de los coeficientes, todos 1, tantos como variables de restricciones
   double val[cantidadVariables];
	int count = 0;
	for (int i = 0; i < cantidadRestricciones; i++)
	{
		for (int j = 0; j < restriccion[i].size(); j++)
		{
			val[count] = 1;
			count++;
		}
		
		
	}
	 
   char *varname[] = 					
	 									// ponemos en orden los nombres de las variables
   
   {"X21", "X22", 
    "X22", "X23", 
    "X23", "X24",
    "X11", "X21", "X31", "X41", "X51",
    "W11", "W21", "W31", "W41", "W51",
    "X12", "X22", "X32", "X42", "X52",
    "W12", "W22", "W32", "W42", "W52",
    "X13", "X23", "X33", "X43", "X53",
    "W13", "W23", "W33", "W43", "W53",
    "X14", "X24", "X34", "X44", "X54",
    "W14", "W24", "W34", "W44", "W54",
    "X15", "X25", "X35", "X45", "X55",
    "W15", "W25", "W35", "W45", "W55"};


	// ponemos el termino independiente
	// en el caso de la clique 1
	//en el caso de ciclo impar, tamano de ciclo (impar -1) / 2
	
   double rhs[cantidadRestricciones];
   for (int i = 0; i < cantidadRestricciones ; i++)
   {
	   rhs[i] = 1;	// para cliques
   }
   	
   int    *cutbeg = NULL;
   int    *cutind = NULL;
   double *cutval = NULL;
   double *cutrhs = NULL; 

   int i, varind;
   int nz   = cantidadVariables;		// cantidad de variables que no son cero
   int cuts = cantidadRestricciones;	// cantidad de cortes

   int cur_numcols = CPXgetnumcols (env, lp);

   usercutinfo->lp = lp;
   usercutinfo->numcols = cur_numcols;

   usercutinfo->x = (double *) malloc (cur_numcols * sizeof (double));
   if ( usercutinfo->x == NULL ) {
      fprintf (stderr, "No memory for solution values.\n");
      goto TERMINATE;
   }

   cutbeg = (int *)    malloc ((cuts+1) * sizeof (int));
   cutind = (int *)    malloc (nz * sizeof (int));
   cutval = (double *) malloc (nz * sizeof (double));
   cutrhs = (double *) malloc (cuts * sizeof (double));

   if ( cutbeg == NULL ||
        cutind == NULL ||
        cutval == NULL ||
        cutrhs == NULL   ) {
      fprintf (stderr, "No memory.\n");
      status = CPXERR_NO_MEMORY;
      goto TERMINATE;
   } 
      
   for (i = 0; i < nz; i++) {
      status = CPXgetcolindex (env, lp, varname[i], &varind);
      if ( status )  {
         fprintf (stderr,
                  "Failed to get index from variable name.\n");
         goto TERMINATE;
      }
      cutind[i] = varind;
      cutval[i] = val[i];
   }

   for (i = 0; i < cuts; i++) {
      cutbeg[i] = beg[i];
      cutrhs[i] = rhs[i];
   }
   cutbeg[cuts] = beg[cuts];

   usercutinfo->num      = cuts;
   usercutinfo->beg      = cutbeg;
   usercutinfo->ind      = cutind;
   usercutinfo->val      = cutval;
   usercutinfo->rhs      = cutrhs;

TERMINATE:

   if ( status ) {
      free_and_null ((char **) &cutbeg);
      free_and_null ((char **) &cutind);
      free_and_null ((char **) &cutval);
      free_and_null ((char **) &cutrhs);
   }
 
   return (status);

} /* END makeusercuts */

/* A constraint that cuts off the optimal solution of noswot:
   W11 + W12 + W13 + W14 + W15 <= 3
 */

