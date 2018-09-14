//
// Book : Version du chapitre 24 de l'ouvrage.
// "Programming -- Principles and Practice Using C++" de Bjarne Stroustrup (2ème édition : 2014)
// Commit initial : 14/09/2018 
// Commit en cours : Drill de la page 922
// Caractères spéciaux : [ ]   '\n'   {  }   ||   ~   _     @

#include "pch.h"		// En-tête indispensable sous Windows et VC++
#include "MyHeader.h"	// Header personnel (en partie dérivé de std_lib_facilities de BS)

#include "Matrix.h"		// Header from BS PPP2
#include "MatrixIO.h"	// Header from BS PPP2
#include <complex>
#include <numeric>

using namespace std;
using namespace Numeric_lib;

using cmplx = complex<double>;	// To avoid the verbosity


int main()
{
	try
	{
		// A mettre dans la 1ère ligne du "main" pour afficher les caractères accentués sous la console Windows
		std::locale::global(std::locale("fr-FR"));

		cout << sizeof(char)<<" - " << sizeof(short)<<" - " << sizeof(int)<<" - " << sizeof(long)<<" - " << sizeof(float) << " - " << sizeof(double)<<" - " << sizeof(int*)<<" - " << sizeof(double*) << endl;
		/* DRILL page 922
		Matrix<int> a(10);
		Matrix<int> b(100);
		Matrix<double> c(10);
		Matrix<int, 2> d(10, 10);
		Matrix<int, 3> e(10, 10, 10);

		cout << sizeof(a) << " - " << sizeof(b) << " - " << sizeof(c) << " - " << sizeof(d) << " - " << sizeof(e)<< endl;
		
		a = 1;	// Tous les éléments de la matrice sont initialisés à 1
		cout << a << endl;
		b = 2;
		cout << b << endl;
		c = 3.3;
		cout << c << endl;
		d = 4;
		cout << d << endl;
		e = 5;
		cout << e(1,1,1) << endl;	// L'implémentation du cout d'une matrice avec n>2 n'a pas été implémenté dans MatrixIO.h

		
		double x = 0;
		cout << "Saisir un nombre dont l'on souhaite la racine carrée (CTRL Z pour sortir) :" << endl;
		while (cin>>x) {
			double r = sqrt(x);
			if (errno) My_error("erreur sur la racine carrée");
			cout << "La racine carrée de " << x << " est : " << r << endl;
		
		}
		keep_window_open();

		Matrix<int, 2> f(2, 5);	// matrice de 2 lignes sur 5 colonnes (= 10 éléments)
		cout << "Saisir 10 doubles pour populer une matrice 2 x 5 :" << endl;
		for (int i = 0; i < f.dim1(); ++i)
			for (int j = 0; j < f.dim2(); ++j) 
				cin >> f(i, j);

		cout << f << endl;

		int n = 0;
		int m = 0;
		cout << "Saisir deux entiers n et m pour définir une table de multiplication :" << endl;
		cin >> n >> m;
		Matrix<int, 2> g(n, m);	
		
		for (int i = 0; i < n; ++i)
			for (int j = 0; j < m; ++j)
				g(i, j)=i*j;


		cout << g << endl;

		
		Matrix<cmplx, 2> h(2, 2);	// matrice de 2 lignes sur 5 colonnes (= 10 éléments)
		cmplx sum{};
		cout << "Saisie de 4 nombres complexes au format double, sous la forme (partie réelle, partie imaginaire) :" << endl;
		for (int i = 0; i < h.dim1(); ++i)
			for (int j = 0; j < h.dim2(); ++j)
			{
				cin >> h(i, j);
				sum += h(i, j);
			}
		cout << h << endl;
		*/

		// Exo 1 page 923



		keep_window_open();

		return 0;
	}
	/*
	catch (XXXXXXX::Invalid&)
	{
		cerr << "error: ???????????????????? ! \n";
		keep_window_open("q");	// For some Windows(tm) setup
		return 1;
	}
	*/

	catch (runtime_error& e)
	{	// this code is to produce error messages
		cout << e.what() << '\n';
		keep_window_open("q");	// For some Windows(tm) setups
	}
}