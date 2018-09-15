//
// Book : Version du chapitre 24 de l'ouvrage.
// "Programming -- Principles and Practice Using C++" de Bjarne Stroustrup (2ème édition : 2014)
// Commit initial : 14/09/2018 
// Commit en cours : 15/09/2018 - Exos page 923
// Caractères spéciaux : [ ]   '\n'   {  }   ||   ~   _     @

#include "pch.h"		// En-tête indispensable sous Windows et VC++
#include "MyHeader.h"	// Header personnel (en partie dérivé de std_lib_facilities de BS)

#include "Matrix.h"		// Header from BS PPP2
#include "MatrixIO.h"	// Header from BS PPP2
#include <complex>
#include <numeric>
#include <random>

using namespace std;
using namespace Numeric_lib;

using cmplx = complex<double>;	// To avoid the verbosity
using Numeric_lib::Index;
using MatrixG = Numeric_lib::Matrix<double, 2>;	// To simplify notation (G = Gaussian)
using VectorG = Numeric_lib::Matrix<double, 1>;

// Needed functions for solving linear equations ******************************************************************************************************
VectorG operator*(const MatrixG& m, const VectorG& u)
{  
	const Index n = m.dim1();
	VectorG v(n);
	for (Index i = 0; i < n; ++i) v(i) = dot_product(m[i], u);
	return v;

}

void classical_elimination(MatrixG& A, VectorG& b)
{
	const Index n = A.dim1();

	for (Index j=0;j<n-1;++j) {
		const double pivot = A(j, j);
		if (pivot == 0) My_error("Elim_failure"); // throw Elim_failure(j);

		for (Index i=j+1;i<n;++i) {
			const double mult = A(i, j) / pivot;
			A[i].slice(j) = scale_and_add(A[j].slice(j), -mult, A[i].slice(j)); // exo 6 : scale_and_add is equivalent to result+=arg1(i)*arg2(i)+arg3(i) for each i in the Matrix where result starts out as 0
			b(i) -= mult * b(j);
				
		}
		
	}

}

VectorG back_substitution(const MatrixG& A, const VectorG& b)
{
	const Index n = A.dim1();
	VectorG x(n);

	for (Index i=n-1;i>=0;--i) {
		double s = b(i) - dot_product(A[i].slice(i + 1), x.slice(i + 1)); // exo 6 : dot_product is equivalent to result+=arg1(i)*arg2(i) for each i in the Matrix where result starts out as 0

		if (double m = A(i, i))
			x(i) = s / m;
		else
			//throw Back_subst_failure(i);
			My_error("throw Back_subst_failure");
	}

	return x;

}

void elim_with_partial_pivot(MatrixG& A, VectorG& b)
{  
	const Index n = A.dim1();

	for (Index j=0;j<n;++j) {
		Index pivot_row = j;

		for (Index k = j + 1; k < n; ++k)
			if (abs(A(k, j)) > abs(A(pivot_row, j))) pivot_row = k;

		if (pivot_row !=j) {
			A.swap_rows(j, pivot_row);
			std::swap(b(j), b(pivot_row));
				
		}
	
		for (Index i=j+1;i<n;++i) {
			const double pivot = A(j, j);
			if (pivot == 0) error("can't solve : pivot==0");
			const double mult = A(i, j) / pivot;
			A[i].slice(j) = scale_and_add(A[j].slice(j), -mult, A[i].slice(j));
			b(i) -= mult * b(j);
		
		}
	
	}

}

VectorG classical_gaussian_elimination (MatrixG A, VectorG b) 
{
	classical_elimination(A, b);
	return back_substitution(A, b);

}


VectorG random_vector(Index n, double max)
{
	VectorG v(n);
	default_random_engine ran{};
	uniform_real_distribution<> ureal{ 0,max };
	
	for (Index i = 0; i < n; ++i)
		v(i) = ureal(ran);

	return v;
	   
}


MatrixG random_matrix(Index n, double max)
{
	MatrixG v(n,n);	// Matrice de double à 2 dimensions
	default_random_engine ran{};
	uniform_real_distribution<> ureal{ 0,max };

	
	for (Index i = 0; i < n; ++i)
		for (int j = 0; j < n; ++j)	
			v(i, j)= ureal(ran);
	
	return v;

}

template<typename T>
void swap_columns(T& A, Index i, Index j)
// Based on swap_rows() from Matrix.h. Be careful : it works only for matrix with one or two dimensions

{
	if (i == j) return;
		
	Index max = A.dim1();	// nb rows
	for (Index ii = 0; ii < max; ++ii) std::swap(A(ii, i), A(ii, j));
}

// *****************************************************************************************************************************************************

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

		// Exos 1&2 page 923
		auto triple = [](int d) {return d*3; };	// Lambda function
		auto triple2 = [](int& d) {d *= 3; };	// Lambda function avec plus de side effect car elle modifie les valeurs de la matrice passée en référence
		Matrix<int> k(5);
		k = 3;
		Matrix<int> l(5);
		l = apply(triple, k);					// Equivalent à l = apply([](int d) {return d * 3; }, k);
		cout << k << endl;
		cout << l << endl;
		k.apply(triple2);						// Triple chaque élément de k passé par référence (autre exemple page 903)
		cout << k << endl;						
		k.apply([](int& d,int multiple=2) {d *= multiple; });	// Autre function object (= functor) qui multiplie k[i] par un nombre donné, ici 2 = Equivalent à k *=2;
		cout << k << endl;
		
		// Exo 4 page 923
		Index Ind = 5;
		MatrixG A = random_matrix(Ind, 10);	// 5 rows x 5 columns populated with random numbers < 10
		VectorG b = random_vector(Ind, 10);	// 1 rows x 5 columns populated with random numbers < 10

		cout << "A = " << A << endl;
		cout << "b = " << b << endl;
	
		VectorG x = classical_gaussian_elimination(A, b);
		cout << "classical elim solution is x = " << x << endl;
		VectorG v = A * x;
		cout << "A*x = " << v << endl;
		
		// Exo 5 page 923
		MatrixG A1(2,2); //{ (0, 1) (1, 0) };
		A1(0, 0) = 0;
		A1(0, 1) = 1;
		A1(1, 0) = 1;
		A1(1, 1) = 0;
		VectorG b1(2);
		b1(0) = 5;
		b1(1) = 6;
		elim_with_partial_pivot(A1, b1);	// Useful otherwise Elim_failure
		cout << "A1 = " << A1 << endl;
		cout << "b1 = " << b1 << endl;
		VectorG x1 = classical_gaussian_elimination(A1, b1);
		cout << "classical elim solution is x1 = " << x1 << endl;
		VectorG v1 = A1 * x1;
		cout << "A1*x1 = " << v1 << endl;
		
		// Exo 7 : steps to implement in order to rewrite the gaussian elimination functions without using the Matrix lib but built-in arrays or vector :
		// 1. Replace using MatrixG = Numeric_lib::Matrix<double, 2> by a built-in array double MatrixG [Ind][2]
		// 2. Replace using VectorG = Numeric_lib::Matrix<double, 1> by a simple vector<double> VectorG (Ind)
		// 3. Replace dot_product by result+=arg1(i)*arg2(i) for each i in the Matrix where result starts out as 0
		// 4. Replace scale_and_add by result+=arg1(i)*arg2(i)+arg3(i) for each i in the Matrix where result starts out as 0
		// 5. Replace A.dim1() by A.size();
		// 6. Replace A(i, j) by A [i][j]
		
		// Exo 10
		srand((int)time(0));
		vector<int>alea{};

		for (int i = 0; i < 1000; ++i)
			alea.push_back(random_number(0, 100));

		for (auto x : alea)
			cout << x << " - ";
		cout << endl;

		// Exo 11 : swap_columns
		Matrix<int,2> A2(3, 2);
		A2(0, 0) = 1;
		A2(0, 1) = 2;
		A2(1, 0) = 3;
		A2(1, 1) = 4;
		A2(2, 0) = 5;
		A2(2, 1) = 6;

		cout << "Matrice A2 originale :" << A2 << endl;
		swap_columns(A2, 0, 1);
		cout << "Matrice A2 avec inversion des colonnes 0 et 1 :" << A2 << endl;

		MatrixG A3 = random_matrix(5, 20);	// 5 rows x 5 columns populated with double random numbers < 20
		cout << "Matrice A3 originale :" << A3 << endl;
		swap_columns(A3, 3, 4);
		cout << "Matrice A3 avec inversion des colonnes 3 et 4 :" << A3 << endl;
		
		keep_window_open();

		return 0;
	}
	
	
	catch (runtime_error& e)
	{	// this code is to produce error messages
		cout << e.what() << '\n';
		keep_window_open("q");	// For some Windows(tm) setups
	}
}