//
// Book : Version du chapitre 24 de l'ouvrage.
// "Programming -- Principles and Practice Using C++" de Bjarne Stroustrup (2ème édition : 2014)
// Commit initial : 14/09/2018 
// Commit en cours : 15/09/2018 - Exos page 923
// Note : compilé avec l'option C++17 à cause de l'utilisation de if constexpr pour simuler du SFINAE dans le template d'addition de matrices
// Caractères spéciaux : [ ]   '\n'   {  }   ||   ~   _     @

#include "pch.h"		// En-tête indispensable sous Windows et VC++
#include "MyHeader.h"	// Header personnel (en partie dérivé de std_lib_facilities de BS)

#include "Matrix11.h"		// Header from BS PPP2 (la version Matrix.h était a priori écrite en C++98)
#include "MatrixIO11.h"		// Header from BS PPP2
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

Matrix<double> operator*(Matrix<double,2>& m, Matrix<double>& u)
{
	// Exo 12 : produit d'une matrice de dimension 2 et d'une matrice de dimension 1 => Cela donne une matrice de dimension 1
	// En algèbre linéaire, une matrice A de dimensions m lignes et n colonnes(matrice m×n) représente une application linéaire ƒ d'un espace de dimension n vers un espace de dimension m. 
	// Une matrice colonne V de n lignes est une matrice n×1, et représente un vecteur v d'un espace vectoriel de dimension n. Le produit A×V représente ƒ(v).
	// Le produit de deux matrices ne peut se définir que si le nombre de colonnes de la première matrice est le même que le nombre de lignes de la deuxième matrice, c'est-à-dire lorsqu'elles sont de type compatible.
	
	if (m.dim2() != u.dim1()) My_error("Matrices incompatibles");
	const Index n = m.dim1();	// Nombre de lignes de la matrice
	Matrix<double> v(n);
	for (Index i = 0; i < n; ++i) v(i) = dot_product(m[i], u);	// dot_product is equivalent to result+=arg1(i)*arg2(i) for each i in the Matrix where result starts out as 0
	return v;

}

template<int N>
Matrix<double,N> operator+(Matrix<double, N>& m, Matrix<double,N>& u)
{
	// Exo 12 : addition de deux matrices de dimension N => les matrices doivent absolument être identiques
	// Cet overloading n'accepte que des matrices à 3 dimensions au MAXIMUM => La généralisation passe probablement par des templates variadic ?
	// constexpr (C++17 only) est utilisé pour "in short, if-constexpr extends the compile-time subset of the C++ language to include if-statements." 
	// What’s more, if a branch of the if-constexpr is not hit, then it will not even be compiled.
	// With if - constexpr at your disposal, you don’t need to resort to elaborate meta - programming techniques like template pattern - matching and SFINAE.
	// https://hackernoon.com/a-tour-of-c-17-if-constexpr-3ea62f62ff65
	
	if (N>3) My_error("L'addition matricielle n'accepte pas des matrices à plus de 3 dimensions"); // = concept car une matrice possédant 4 dimensions ne compile pas dans le main (le code de BS ne le supporte pas)
	
	if constexpr (N==1) {
	
		if (m.dim1() != u.dim1()) My_error("Matrices incompatibles");
		Matrix<double, 1> v(m.dim1());
		for (Index i = 0; i < m.dim1(); ++i) v(i) = m(i) + u(i);
		return v;
	
	} else if constexpr (N == 2) {
	
		if (m.dim1() != u.dim1()) My_error("Matrices incompatibles");
		if (m.dim2() != u.dim2()) My_error("Matrices incompatibles");
		Matrix<double, 2> v(m.dim1(), m.dim2());
		for (Index i = 0; i < m.dim1(); ++i)
			for (int j = 0; j < m.dim2(); ++j)
				v(i, j) = m(i, j) + u(i, j);
		return v;
		
	} else if constexpr (N == 3) {
		
		if (m.dim1() != u.dim1()) My_error("Matrices incompatibles");
		if (m.dim2() != u.dim2()) My_error("Matrices incompatibles");
		if (m.dim3() != u.dim3()) My_error("Matrices incompatibles");
		Matrix<double, 3> v(m.dim1(), m.dim2(), m.dim3());
		// Code à implémenter pour une matrice à 3 dimensions
		// TC++PL4, pages 837 & 838, propose une version assez complexe
		return v; 
		
	}
		else My_error("L'addition matricielle n'accepte pas des matrices à plus de 3 dimensions");
	   
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
		
		// DRILL page 922
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
		
		
		// Exos 1&2 page 923
		auto triple = [](int d) {return d*3; };	// Lambda function
		auto triple2 = [](int& d) {d *= 3; };	// Lambda function avec plus de side effect car elle modifie les valeurs de la matrice passée en référence
		Matrix<int> k(5);
		k = 3;
		Matrix<int> l(5);
		l = Numeric_lib::apply(triple, k);					// Equivalent à l = apply([](int d) {return d * 3; }, k);
		cout << k << endl;
		cout << l << endl;
		k.apply(triple2);						// Triple chaque élément de k passé par référence (autre exemple page 903)
		cout << k << endl;						
		k.apply([](int& d,int multiple=2) {d *= multiple; });	// Autre function object (= functor) qui multiplie k[i] par un nombre donné, ici 2 = Equivalent à k *=2;
		cout << k << endl;
		
		// Exo 4 page 923
		Index Ind = 5;
		MatrixG A = random_matrix(Ind, 10);	// 5 rows x 5 columns populated with random numbers < 10
		VectorG B = random_vector(Ind, 10);	// 5 rows x 1 column populated with random numbers < 10

		cout << "A = " << A << endl;
		cout << "B = " << B << endl;
	
		VectorG X = classical_gaussian_elimination(A, B);
		cout << "classical elim solution is x = " << X << endl;
		VectorG v = A * X;
		cout << "A*x = " << v << endl;
		
		// Exo 5 page 923
		MatrixG A1(2,2); //{ (0, 1) (1, 0) }; Matrix11.h ne comporte pas de Matrix_initializer permettant un assignement direct de valeurs via une liste (cf TC++PL4 pages 831 & 832)
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
		
		// Exo 12 : produit matriciel
		Matrix<double, 2> A4(2, 2);
		A4(0, 0) = 1;
		A4(0, 1) = 0;
		A4(1, 0) = 2;
		A4(1, 1) = -1;

		Matrix<double> A5(2);
		A5(0) = 3;
		A5(1) = -2;
				
		cout << "Produit matriciel A4*A5 = " << A4*A5 << endl;

		MatrixG A6 = random_matrix(10, 20);	// 10 rows x 10 columns populated with double random numbers < 20
		VectorG A7 = random_vector(10, 20);	// 10 rows x 1 column populated with random numbers < 20
		cout << "Produit matriciel A6*A7 = " << A6 * A7 << endl<<endl;

		// Exo 12bis : addition de matrices
		
		// 1D 
		Matrix<double> A8(2);
		A8(0) = 1;
		A8(1) = 2;
		
		Matrix<double> A9(2);
		A9(0) = 3;
		A9(1) = 4;
		
		cout << "Addition matricielle A8+A9 (1 dimension) = " << A8 + A9 << endl;

		// 2D 
		Matrix<double,2> A10(3,2);
		A10(0, 0) = 1;
		A10(0, 1) = 3;
		A10(1, 0) = 1;
		A10(1, 1) = 0;
		A10(2, 0) = 1;
		A10(2, 1) = 2;

		Matrix<double,2> A11(3,2);
		A11(0, 0) = 0;
		A11(0, 1) = 0;
		A11(1, 0) = 7;
		A11(1, 1) = 5;
		A11(2, 0) = 2;
		A11(2, 1) = 1;

		cout << "Addition matricielle A10+A11 (2 dimensions) = " << A10 + A11 << endl;

		// Le code de BS ne supporte pas << sur une matrice à 3 dimensions => Il faudrait faire évoluer MatrixIO.h (ex : nested for...loop)
		// Matrix<double, 3> A12(3, 2,3);
		// Matrix<double, 3> A13(3, 2,3);
		// cout << "Addition matricielle A12+A13 (3 dimensions) = " << A12 + A13 << endl;

		keep_window_open();

		return 0;
	}
	
	
	catch (runtime_error& e)
	{	// this code is to produce error messages
		cout << e.what() << '\n';
		keep_window_open("q");	// For some Windows(tm) setups
	}
}