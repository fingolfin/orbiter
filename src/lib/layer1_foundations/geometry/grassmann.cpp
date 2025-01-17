// grassmann.cpp
// 
// Anton Betten
//
// started: June 5, 2009
//
//
// 
//
//

#include "foundations.h"

using namespace std;


namespace orbiter {
namespace layer1_foundations {
namespace geometry {


grassmann::grassmann()
{
	n = 0;
	k = 0;
	q = 0;
	nCkq = NULL;
	F = NULL;
	base_cols = NULL;
	coset = NULL;
	M = NULL;
	M2 = NULL;
	v = NULL;
	w = NULL;
	G = NULL;
}

grassmann::~grassmann()
{
	//cout << "grassmann::~grassmann 1" << endl;
	if (nCkq) {
		FREE_OBJECT(nCkq);
	}
	if (base_cols) {
		FREE_int(base_cols);
	}
	//cout << "grassmann::~grassmann 2" << endl;
	if (coset) {
		FREE_int(coset);
	}
	//cout << "grassmann::~grassmann 3" << endl;
	if (M) {
		FREE_int(M);
	}
	if (M2) {
		FREE_int(M2);
	}
	if (v) {
		FREE_int(v);
	}
	if (w) {
		FREE_int(w);
	}
	//cout << "grassmann::~grassmann 4" << endl;
	if (G) {
		FREE_OBJECT(G);
	}
}

void grassmann::init(int n, int k, field_theory::finite_field *F, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "grassmann::init n=" << n
				<< " k=" << k << " q=" << F->q << endl;
	}


	grassmann::n = n;
	grassmann::k = k;
	grassmann::F = F;
	q = F->q;

	combinatorics::combinatorics_domain D;

	nCkq = NEW_OBJECT(ring_theory::longinteger_object);

	D.q_binomial(*nCkq, n, k, q, 0 /* verbose_level */);
	if (f_v) {
		cout << "grassmann::init nCkq=" << *nCkq << endl;
	}
	



	base_cols = NEW_int(n);
	coset = NEW_int(n);
	M = NEW_int(n * n);
		// changed to n * n to allow for embedded subspaces.
	M2 = NEW_int(n * n);
	v = NEW_int(n);
	w = NEW_int(n);
	if (k > 1) {
		G = NEW_OBJECT(grassmann);
		G->init(n - 1, k - 1, F, verbose_level);
	}
	else {
		G = NULL;
	}
}

long int grassmann::nb_of_subspaces(int verbose_level)
{
	long int nb;
	combinatorics::combinatorics_domain Combi;

	nb = Combi.generalized_binomial(n, k, q);
	return nb;
}

void grassmann::print_single_generator_matrix_tex(
		ostream &ost, long int a)
{
	unrank_lint(a, 0 /*verbose_level*/);
	latex_matrix(ost, M);

	if (F->e > 1) {
		ost << " = ";
		latex_matrix_numerical(ost, M);
	}
	//print_integer_matrix_tex(ost, M, k, n);
}

void grassmann::print_single_generator_matrix_tex_numerical(
		std::ostream &ost, long int a)
{
	unrank_lint(a, 0 /*verbose_level*/);
	latex_matrix_numerical(ost, M);
	//print_integer_matrix_tex(ost, M, k, n);
}

void grassmann::print_set(long int *v, int len)
{
	int i;
	
	for (i = 0; i < len; i++) {
		cout << "subspace " << i << " / " << len
				<< " is " << v[i] << ":" << endl;
		unrank_lint(v[i], 0 /*verbose_level*/);
		latex_matrix(cout, M);
		//print_integer_matrix_width(cout, M,
		//		k, n, n, F->log10_of_q + 1);
	}
}

void grassmann::print_set_tex(ostream &ost, long int *v, int len, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "grassmann::print_set_tex" << endl;
	}
	int i;
	int *Mtx;
	
	Mtx = NEW_int(n * n);

	for (i = 0; i < len; i++) {
		ost << "subspace " << i << " / " << len << " is "
				<< v[i] << ":\\\\" << endl;
		//unrank_lint(v[i], 0 /*verbose_level*/);

		unrank_lint_here_and_compute_perp(
				Mtx, v[i], 0 /*verbose_level*/);


		ost << "$$" << endl;
		//ost << "\\left[" << endl;
		//print_integer_matrix_width(cout,
		// M, k, n, n, F->log10_of_q + 1);
		//print_integer_matrix_tex(ost, M, k, n);
		//ost << "\\right]" << endl;
		//latex_matrix(ost, Mtx);

		F->print_matrix_latex(ost, Mtx, k, n);

#if 0
		ost << "\\qquad" << endl;

		F->print_matrix_numerical_latex(ost, Mtx, k, n);

		ost << "\\qquad" << endl;

		F->print_matrix_latex(ost, Mtx + k * n, n - k, n);

		ost << "\\qquad" << endl;

		F->print_matrix_numerical_latex(ost, Mtx + k * n, n - k, n);
#endif

		ost << "$$" << endl;
	}
	FREE_int(Mtx);
	if (f_v) {
		cout << "grassmann::print_set_tex done" << endl;
	}
}

void grassmann::print_set_tex_with_perp(ostream &ost, long int *v, int len)
{
	int i;
	int *Mtx;

	Mtx = NEW_int(n * n);

	for (i = 0; i < len; i++) {
		ost << "subspace " << i << " / " << len << " and its dual  are "
				<< v[i] << ":\\\\" << endl;
		//unrank_lint(v[i], 0 /*verbose_level*/);

		unrank_lint_here_and_compute_perp(
				Mtx, v[i], 0 /*verbose_level*/);


		ost << "$$" << endl;
		//ost << "\\left[" << endl;
		//print_integer_matrix_width(cout,
		// M, k, n, n, F->log10_of_q + 1);
		//print_integer_matrix_tex(ost, M, k, n);
		//ost << "\\right]" << endl;
		//latex_matrix(ost, Mtx);

		F->print_matrix_latex(ost, Mtx, k, n);

		ost << "\\qquad" << endl;

		F->print_matrix_numerical_latex(ost, Mtx, k, n);

		ost << "\\qquad" << endl;

		F->print_matrix_latex(ost, Mtx + k * n, n - k, n);

		ost << "\\qquad" << endl;

		F->print_matrix_numerical_latex(ost, Mtx + k * n, n - k, n);

		ost << "$$" << endl;
	}
	FREE_int(Mtx);
}


int grassmann::nb_points_covered(int verbose_level)
{
	int nb;
	combinatorics::combinatorics_domain Combi;

	nb = Combi.generalized_binomial(k, 1, q);
	return nb;
}

void grassmann::points_covered(long int *the_points, int verbose_level)
{
	int i, nb;
	long int a;

	nb = nb_points_covered(0 /* verbose_level*/);
	for (i = 0; i < nb; i++) {
		F->PG_element_unrank_modified(v, 1, k, i);
		F->Linear_algebra->mult_vector_from_the_left(v, M, w, k, n);
		F->PG_element_rank_modified_lint(w, 1, n, a);
		the_points[i] = a;
	}
}

void grassmann::unrank_lint_here(int *Mtx, long int rk, int verbose_level)
{
	unrank_lint(rk, verbose_level);
	Int_vec_copy(M, Mtx, k * n);
}

long int grassmann::rank_lint_here(int *Mtx, int verbose_level)
{
	Int_vec_copy(Mtx, M, k * n);
	return rank_lint(verbose_level);
}

void grassmann::unrank_embedded_subspace_lint(long int rk, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i, j;
	
	if (f_v) {
		cout << "grassmann::unrank_embedded_subspace_lint " << rk << endl;
	}
	unrank_lint(rk, verbose_level);
	Int_vec_zero(M + k * n, (n - k) * n);
	if (k == 0) {
		F->Linear_algebra->identity_matrix(M, n);
	}
	else {
		for (i = 0; i < n - k; i++) {
			j = base_cols[k + i];
			M[(k + i) * n + j] = 1;
		}
	}
	if (f_v) {
		cout << "unrank_embedded_subspace_lint done" << endl;
	}
}

long int grassmann::rank_embedded_subspace_lint(int verbose_level)
{
	int f_v = (verbose_level >= 1);
	long int rk;
	
	if (f_v) {
		cout << "grassmann::rank_embedded_subspace_lint " << endl;
	}
	rk = rank_lint(verbose_level);
	return rk;
	
}

void grassmann::unrank_embedded_subspace_lint_here(int *Basis, long int rk, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i, j;

	if (f_v) {
		cout << "grassmann::unrank_embedded_subspace_int_here " << rk << endl;
	}
	unrank_lint_here(Basis, rk, verbose_level);
	Int_vec_zero(Basis + k * n, (n - k) * n);
	if (k == 0) {
		F->Linear_algebra->identity_matrix(Basis, n);
	}
	else {
		for (i = 0; i < n - k; i++) {
			j = base_cols[k + i];
			Basis[(k + i) * n + j] = 1;
		}
	}
	if (f_v) {
		cout << "unrank_embedded_subspace_int_here done" << endl;
	}
}


void grassmann::unrank_lint(long int rk, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	long int r, h, a = 1, A;
	int nb_free_cols = 0;
	long int Q, b, c, i, j;
	number_theory::number_theory_domain NT;
	geometry_global Gg;
	combinatorics::combinatorics_domain Combi;
	
	if (f_v) {
		cout << "grassmann::unrank_lint " << rk << endl;
	}
	if (k == 0) {
		return;
	}
	// null the first row:
	Int_vec_zero(M, n);
	
	
	// find out the value of h:
	// h is the column of the pivot element in the first row
	r = rk;
	if (f_v) {
		cout << "r=" << r << endl;
	}
	h = 0;
	while (h < n) {
		a = Combi.generalized_binomial(n - h - 1, k - 1, q);
		if (f_v) {
			cout << "[" << n - h - 1 << " choose " << k - 1
					<< "]_" << q << " = " << a << endl;
		}
		nb_free_cols = n - h - 1 - (k - 1);
		Q = NT.i_power_j_lint(q, nb_free_cols);
		if (f_v) {
			cout << "Q=" << Q << endl;
		}
		A = a * Q;
		if (f_v) {
			cout << "A=" << A << endl;
		}
		if (r < A) {
			break;
		}
		r -= A;
		if (f_v) {
			cout << "r=" << r << endl;
		}
		h++;
	}
	if (h == n) {
		cout << "grassmann::unrank_lint h == n" << endl;
		cout << "h=" << h << endl;
		cout << "r=" << r << endl;
		exit(1);
	}
	
	// now h has been determined
	if (f_v) {
		cout << "grassmann::unrank_lint " << rk << " h=" << h
				<< " nb_free_cols=" << nb_free_cols << endl;
	}
	base_cols[0] = h;
	M[h] = 1;
	
	// find out the coset number b and the rank c of the subspace:	
	b = r / a;
	c = r % a;
	if (f_v) {
		cout << "r=" << r << " coset " << b
				<< " subspace rank " << c << endl;
	}
	
	// unrank the coset:
	if (nb_free_cols) {
		Gg.AG_element_unrank(q, coset, 1, nb_free_cols, b);
	}
	if (f_v) {
		cout << "grassmann::unrank_lint coset " << b << " = ";
		Int_vec_print(cout, coset, nb_free_cols);
		cout << endl;
	}
	
	//unrank the subspace (if there is one)
	if (k > 1) {
		G->n = n - h - 1;
		G->unrank_lint(c, verbose_level - 1);
		for (j = 0; j < k - 1; j++) {
			base_cols[j + 1] = G->base_cols[j] + h + 1;
		}
	}
	if (f_v) {
		cout << "grassmann::unrank_lint calling "
				"int_vec_complement n=" << n << " k=" << k << " : ";
		Int_vec_print(cout, base_cols, k);
		cout << endl;
	}
	orbiter_kernel_system::Orbiter->Int_vec->complement(base_cols, n, k);
	
	// fill in the coset:
	if (k == 1) {
		for (j = 0; j < nb_free_cols; j++) {
			M[h + 1 + j] = coset[j];
		}
	}
	else {
		for (j = 0; j < nb_free_cols; j++) {
			M[h + 1 + G->base_cols[G->k + j]] = coset[j];
		}
	}


	// copy the subspace (rows i=1,..,k-1):
	if (k > 1) {
		for (i = 0; i < G->k; i++) {
		
			// zero beginning:
			for (j = 0; j <= h; j++) {
				M[(1 + i) * n + j] = 0;
			}
			
			// the non-trivial part of the row:
			for (j = 0; j < G->n; j++) {
				M[(1 + i) * n + h + 1 + j] = G->M[i * G->n + j];
			}
		}
	}
	if (f_v) {
		cout << "grassmann::unrank_lint " << rk << ", we found the matrix" << endl;
		Int_vec_print_integer_matrix_width(cout, M, k, n, n, F->log10_of_q + 1);
		cout << "grassmann::unrank_lint base_cols = ";
		Int_vec_print(cout, base_cols, k);
		cout << endl;
		cout << "grassmann::unrank_lint complement = ";
		Int_vec_print(cout, base_cols + k, n - k);
		cout << endl;
	}
	if (f_v) {
		cout << "grassmann::unrank_lint " << rk << " finished" << endl;
	}
}

long int grassmann::rank_lint(int verbose_level)
{
	int f_v = (verbose_level >= 1);
	long int k1, r, h, a, A;
	int nb_free_cols;
	long int Q, b, c, i, j;
	number_theory::number_theory_domain NT;
	geometry_global Gg;
	combinatorics::combinatorics_domain Combi;
	
	r = 0;
	if (f_v) {
		cout << "grassmann::rank_lint " << endl;
		Int_vec_print_integer_matrix_width(cout,
				M, k, n, n, F->log10_of_q + 1);
	}
	if (k == 0) {
		return 0;
	}
	k1 = F->Linear_algebra->Gauss_int(M, FALSE /*f_special */,
		TRUE /* f_complete */, base_cols,
		FALSE /* f_P */, NULL, k, n, n,
		0 /* verbose_level */);
	
	if (f_v) {
		cout << "grassmann::rank_lint after Gauss:" << endl;
		Int_vec_print_integer_matrix_width(cout,
				M, k, n, n, F->log10_of_q + 1);
	}
	if (k1 != k) {
		cout << "grassmann::rank_lint error, does not have full rank" << endl;
		exit(1);
	}
	if (f_v) {
		cout << "grassmann::rank_lint base_cols: ";
		Int_vec_print(cout, base_cols, k);
		cout << endl;
	}
	

	if (f_v) {
		cout << "grassmann::rank_lint calling int_vec_complement n=" << n
				<< " k=" << k << " : ";
		Int_vec_print(cout, base_cols, k);
		cout << endl;
	}
	orbiter_kernel_system::Orbiter->Int_vec->complement(base_cols, n, k);
	if (f_v) {
		cout << "grassmann::rank_lint complement : ";
		Int_vec_print(cout, base_cols + k, n - k);
		cout << endl;
	}

	for (h = 0; h < base_cols[0]; h++) {
		nb_free_cols = n - h - 1 - (k - 1);
		Q = NT.i_power_j(q, nb_free_cols);
		a = Combi.generalized_binomial(n - h - 1, k - 1, q);
		A = a * Q;
		r += A;
	}
	nb_free_cols = n - h - 1 - (k - 1);
	a = Combi.generalized_binomial(n - h - 1, k - 1, q);
	
	// now h has been determined
	if (f_v) {
		cout << "grassmann::rank_lint h=" << h << " nb_free_cols="
				<< nb_free_cols << " r=" << r << endl;
	}

	// copy the subspace (rows i=1,..,k-1):
	if (k > 1) {
		G->n = n - h - 1;
		for (i = 0; i < G->k; i++) {
		
			// the non-trivial part of the row:
			for (j = 0; j < G->n; j++) {
				G->M[i * G->n + j] = M[(1 + i) * n + h + 1 + j];
			}
		}
	}


	// rank the subspace (if there is one)
	if (k > 1) {
		c = G->rank_lint(verbose_level - 1);
	}
	else {
		c = 0;
	}

	// get in the coset:
	if (k == 1) {
		for (j = 0; j < nb_free_cols; j++) {
			coset[j] = M[h + 1 + j];
		}
	}
	else {
		for (j = 0; j < nb_free_cols; j++) {
			coset[j] = M[h + 1 + G->base_cols[G->k + j]];
		}
	}
	// rank the coset:
	if (nb_free_cols) {
		b = Gg.AG_element_rank(q, coset, 1, nb_free_cols);
	}
	else {
		b = 0;
	}
	if (f_v) {
		cout << "grassmann::rank_lint coset " << b << " = ";
		Int_vec_print(cout, coset, nb_free_cols);
		cout << endl;
	}

		
	// compose the rank from the coset number b
	// and the rank c of the subspace:
	r += b * a + c;	
	if (f_v) {
		cout << "grassmann::rank_lint b * a + c = " << b << " * "
				<< a << " + " << c << endl;
		cout << "grassmann::rank_lint r=" << r << " coset " << b
				<< " subspace rank " << c << endl;
	}
	return r;
}

void grassmann::unrank_longinteger_here(int *Mtx,
		ring_theory::longinteger_object &rk, int verbose_level)
{
	unrank_longinteger(rk, verbose_level);
	Int_vec_copy(M, Mtx, k * n);
}

void grassmann::rank_longinteger_here(int *Mtx,
		ring_theory::longinteger_object &rk, int verbose_level)
{
	Int_vec_copy(Mtx, M, k * n);
	rank_longinteger(rk, verbose_level);
}

void grassmann::unrank_longinteger(
		ring_theory::longinteger_object &rk, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	ring_theory::longinteger_object r, r1, a, A, mA, Q, b, c;
	ring_theory::longinteger_domain D;
	combinatorics::combinatorics_domain C;
	int i, j, h, nb_free_cols = 0;
	geometry_global Gg;
	
	if (f_v) {
		cout << "unrank_longinteger " << rk << endl;
	}
	if (k == 0) {
		return;
	}
	// null the first row:
	for (j = 0; j < n; j++) {
		M[j] = 0;
	}
	
	
	// find out the value of h:
	rk.assign_to(r);
	h = 0;
	while (h < n) {
		C.q_binomial(a, n - h - 1, k - 1, q, 0);
		if (f_v) {
			cout << "[" << n - h - 1 << " choose " << k - 1
					<< "]_" << q << " = " << a << endl;
		}
		nb_free_cols = n - h - 1 - (k - 1);
		Q.create_i_power_j(q, nb_free_cols);
		D.mult(a, Q, A);
		//A = a * Q;
		if (D.compare(r, A) < 0) {
			break;
			}
		A.assign_to(mA);
		mA.negate();
		D.add(r, mA, r1);
		r.swap_with(r1);
		//r -= A;
		h++;
	}
	if (h == n) {
		cout << "grassmann::unrank_longinteger h == n" << endl;
		exit(1);
	}
	
	// now h has been determined
	if (f_v) {
		cout << "grassmann::unrank_longinteger " << rk
				<< " h=" << h << " nb_free_cols=" << nb_free_cols << endl;
	}
	base_cols[0] = h;
	M[h] = 1;
	
	// find out the coset number b and the rank c of the subspace:	
	D.integral_division(r, a, b, c, 0);
	//b = r / a;
	//c = r % a;
	if (f_v) {
		cout << "r=" << r << " coset " << b
				<< " subspace rank " << c << endl;
	}
	
	// unrank the coset:
	if (nb_free_cols) {
		Gg.AG_element_unrank_longinteger(q, coset, 1, nb_free_cols, b);
	}
	if (f_v) {
		cout << "grassmann::unrank_longinteger coset " << b << " = ";
		Int_vec_print(cout, coset, nb_free_cols);
		cout << endl;
	}
	
	//unrank the subspace (if there is one)
	if (k > 1) {
		G->n = n - h - 1;
		G->unrank_longinteger(c, verbose_level - 1);
		for (j = 0; j < k - 1; j++) {
			base_cols[j + 1] = G->base_cols[j] + h + 1;
		}
	}
	if (f_v) {
		cout << "grassmann::unrank_longinteger calling "
				"int_vec_complement n=" << n << " k=" << k << " : ";
		Int_vec_print(cout, base_cols, k);
		cout << endl;
	}
	orbiter_kernel_system::Orbiter->Int_vec->complement(base_cols, n, k);
	
	// fill in the coset:
	if (k == 1) {
		for (j = 0; j < nb_free_cols; j++) {
			M[h + 1 + j] = coset[j];
		}
	}
	else {
		for (j = 0; j < nb_free_cols; j++) {
			M[h + 1 + G->base_cols[G->k + j]] = coset[j];
		}
	}


	// copy the subspace (rows i=1,..,k-1):
	if (k > 1) {
		for (i = 0; i < G->k; i++) {
		
			// zero beginning:
			for (j = 0; j <= h; j++) {
				M[(1 + i) * n + j] = 0;
			}
			
			// the non-trivial part of the row:
			for (j = 0; j < G->n; j++) {
				M[(1 + i) * n + h + 1 + j] = G->M[i * G->n + j];
			}
		}
	}
	if (f_v) {
		cout << "unrank_longinteger " << rk
				<< ", we found the matrix" << endl;
		Int_vec_print_integer_matrix_width(cout, M, k, n, n, F->log10_of_q + 1);
		cout << "grassmann::unrank_longinteger base_cols = ";
		Int_vec_print(cout, base_cols, k);
		cout << endl;
		cout << "grassmann::unrank_longinteger complement = ";
		Int_vec_print(cout, base_cols + k, n - k);
		cout << endl;
	}
	if (f_v) {
		cout << "unrank_longinteger " << rk << " finished" << endl;
	}
}

void grassmann::rank_longinteger(ring_theory::longinteger_object &r,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);
	ring_theory::longinteger_object r1, a, A, Q, b, c, tmp1, tmp2;
	ring_theory::longinteger_domain D;
	combinatorics::combinatorics_domain C;
	int k1, nb_free_cols, h, i, j;
	geometry_global Gg;
	
	r.create(0, __FILE__, __LINE__);
	if (f_v) {
		cout << "grassmann::rank_longinteger " << endl;
		Int_vec_print_integer_matrix_width(cout, M, k, n, n, F->log10_of_q + 1);
	}
	if (k == 0) {
		return;
	}
	k1 = F->Linear_algebra->Gauss_int(M, FALSE /*f_special */,
		TRUE /* f_complete */, base_cols,
		FALSE /* f_P */, NULL, k, n, n,
		0 /* verbose_level */);
	
	if (f_v) {
		cout << "grassmann::rank_longinteger after Gauss:" << endl;
		Int_vec_print_integer_matrix_width(cout,
				M, k, n, n, F->log10_of_q + 1);
	}
	if (k1 != k) {
		cout << "grassmann::rank_longinteger error, does not have full rank" << endl;
		exit(1);
	}
	if (f_v) {
		cout << "grassmann::rank_longinteger base_cols: ";
		Int_vec_print(cout, base_cols, k);
		cout << endl;
	}
	

	if (f_v) {
		cout << "grassmann::rank_longinteger calling int_vec_complement for ";
		Int_vec_print(cout, base_cols, k);
		cout << endl;
	}
	orbiter_kernel_system::Orbiter->Int_vec->complement(base_cols, n, k);
	if (f_v) {
		cout << "yields: ";
		Int_vec_print(cout, base_cols + k, n - k);
		cout << endl;
	}

	for (h = 0; h < base_cols[0]; h++) {
		nb_free_cols = n - h - 1 - (k - 1);
		Q.create_i_power_j(q, nb_free_cols);
		if (f_v) {
			cout << "create_i_power_j q=" << q
					<< " nb_free_cols=" << nb_free_cols
					<< " yields " << Q << endl;
		}
		C.q_binomial(a, n - h - 1, k - 1, q, 0);
		if (f_v) {
			cout << "q_binomial [" << n - h - 1 << "," << k - 1
					<< "]_" << q << " = " << a << endl;
		}
		D.mult(a, Q, A);
		D.add(r, A, r1);
		r.swap_with(r1);
	}
	nb_free_cols = n - h - 1 - (k - 1);
	C.q_binomial(a, n - h - 1, k - 1, q, 0);
	if (f_v) {
		cout << "q_binomial [" << n - h - 1 << "," << k - 1
				<< "]_" << q << " = " << a << endl;
	}
	
	// now h has been determined
	if (f_v) {
		cout << "grassmann::rank_longinteger h=" << h
				<< " nb_free_cols=" << nb_free_cols
				<< " r=" << r << endl;
	}

	// copy the subspace (rows i=1,..,k-1):
	if (k > 1) {
		G->n = n - h - 1;
		for (i = 0; i < G->k; i++) {
		
			// the non-trivial part of the row:
			for (j = 0; j < G->n; j++) {
				G->M[i * G->n + j] = M[(1 + i) * n + h + 1 + j];
			}
		}
	}


	// rank the subspace (if there is one)
	if (k > 1) {
		G->rank_longinteger(c, verbose_level);
	}
	else {
		c.create(0, __FILE__, __LINE__);
	}
	if (f_v) {
		cout << "grassmann::rank_longinteger rank of subspace by induction is " << c << endl;
	}

	// get in the coset:
	if (k == 1) {
		for (j = 0; j < nb_free_cols; j++) {
			coset[j] = M[h + 1 + j];
		}
	}
	else {
		for (j = 0; j < nb_free_cols; j++) {
			coset[j] = M[h + 1 + G->base_cols[G->k + j]];
		}
	}
	// rank the coset:
	if (nb_free_cols) {
		Gg.AG_element_rank_longinteger(q, coset, 1, nb_free_cols, b);
		if (f_v) {
			cout << "AG_element_rank_longinteger for coset ";
			Int_vec_print(cout, coset, nb_free_cols);
			cout << " yields " << b << endl;
		}
	}
	else {
		b.create(0, __FILE__, __LINE__);
	}
	if (f_v) {
		cout << "grassmann::rank_longinteger coset " << b << " = ";
		Int_vec_print(cout, coset, nb_free_cols);
		cout << endl;
	}

		
	// compose the rank from the coset number b
	// and the rank c of the subspace:
	if (f_v) {
		cout << "grassmann::rank_longinteger computing r:=" << r << " + " << b
				<< " * " << a << " + " << c << endl;
	}
	D.mult(b, a, tmp1);
	if (f_v) {
		cout << b << " * " << a << " = " << tmp1 << endl;
	}
	D.add(tmp1, c, tmp2);
	if (f_v) {
		cout << tmp1 << " + " << c << " = " << tmp2 << endl;
	}
	D.add(r, tmp2, r1);
	r.swap_with(r1);
	//r += b * a + c;	
	if (f_v) {
		cout << "grassmann::rank_longinteger r=" << r << " coset " << b
				<< " subspace rank " << c << endl;
	}
}

void grassmann::print()
{
	Int_vec_print_integer_matrix_width(cout, M, k, n, n, F->log10_of_q + 1);
}

int grassmann::dimension_of_join(long int rk1, long int rk2, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int *A;
	int i, r;

	if (f_v) {
		cout << "grassmann::dimension_of_join" << endl;
	}
	A = NEW_int(2 * k * n);
	unrank_lint(rk1, 0);
	for (i = 0; i < k * n; i++) {
		A[i] = M[i];
	}
	unrank_lint(rk2, 0);
	for (i = 0; i < k * n; i++) {
		A[k * n + i] = M[i];
	}
	r = F->Linear_algebra->rank_of_rectangular_matrix(A, 2 * k, n, 0 /*verbose_level*/);
	if (f_v) {
		cout << "grassmann::dimension_of_join done" << endl;
	}
	return r;
}

void grassmann::unrank_lint_here_and_extend_basis(
		int *Mtx, long int rk, int verbose_level)
// Mtx must be n x n
{
	int f_v = (verbose_level >= 1);
	int r, i;
	int *base_cols;
	int *embedding;

	if (f_v) {
		cout << "grassmann::unrank_lint_here_and_extend_basis" << endl;
	}
	unrank_lint(rk, verbose_level);
	Int_vec_copy(M, Mtx, k * n);
	base_cols = NEW_int(n);
	embedding = base_cols + k;
	r = F->Linear_algebra->base_cols_and_embedding(k, n, Mtx,
			base_cols, embedding, 0/*verbose_level*/);
	if (r != k) {
		cout << "r != k" << endl;
		exit(1);
	}
	Int_vec_zero(Mtx + k * n, (n - k) * n);
	for (i = 0; i < n - k; i++) {
		Mtx[(k + i) * n + embedding[i]] = 1;
	}
	
	FREE_int(base_cols);
	if (f_v) {
		cout << "grassmann::unrank_lint_here_and_extend_basis done" << endl;
	}
}

void grassmann::unrank_lint_here_and_compute_perp(
		int *Mtx, long int rk, int verbose_level)
// Mtx must be n x n
{
	int f_v = (verbose_level >= 1);
	int r;
	int *base_cols; // [n]
	//int *embedding;

	if (f_v) {
		cout << "grassmann::unrank_int_here_and_compute_perp" << endl;
		}
	unrank_lint(rk, verbose_level);
	Int_vec_copy(M, Mtx, k * n);
	base_cols = NEW_int(n);
	//embedding = base_cols + k;
	r = F->Linear_algebra->RREF_and_kernel(n, k, Mtx, 0 /* verbose_level */);
	if (r != k) {
		cout << "r != k" << endl;
		exit(1);
		}
	
	FREE_int(base_cols);
	if (f_v) {
		cout << "grassmann::unrank_int_here_and_compute_perp done" << endl;
		}
}

void grassmann::line_regulus_in_PG_3_q(
		long int *&regulus, int &regulus_size, int f_opposite, int verbose_level)
// the equation of the hyperboloid is x_0x_3-x_1x_2 = 0
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int f_v3 = (verbose_level >= 3);
	int u, a;
	int M[8];

	if (f_v) {
		cout << "grassmann::line_regulus_in_PG_3_q" << endl;
	}
	if (n != 4) {
		cout << "grassmann::line_regulus_in_PG_3_q n != 4" << endl;
		exit(1);
	}
	if (k != 2) {
		cout << "grassmann::line_regulus_in_PG_3_q k != 2" << endl;
		exit(1);
	}
	regulus_size = q + 1;
	regulus = NEW_lint(regulus_size);
	// the equation of the hyperboloid is x_0x_3-x_1x_2 = 0
	for (u = 0; u < regulus_size; u++) {
		Int_vec_zero(M, 8);
		if (u == 0) {

			if (f_opposite) {
				// create the infinity component, which is
				// [0,1,0,0]
				// [0,0,0,1]
				M[0 * 4 + 1] = 1;
				M[1 * 4 + 3] = 1;
			}
			else {
				// create the infinity component, which is
				// [0,0,1,0]
				// [0,0,0,1]
				M[0 * 4 + 2] = 1;
				M[1 * 4 + 3] = 1;
			}
		}
		else {
			if (f_opposite) {
				// create
				// [1,a,0,0]
				// [0,0,1,a]
				a = u - 1;
				M[0 * 4 + 0] = 1;
				M[0 * 4 + 1] = a;
				M[1 * 4 + 2] = 1;
				M[1 * 4 + 3] = a;
			}
			else {
				// create
				// [1,0,a,0]
				// [0,1,0,a]
				a = u - 1;
				M[0 * 4 + 0] = 1;
				M[1 * 4 + 1] = 1;
				M[0 * 4 + 2] = a;
				M[1 * 4 + 3] = a;
			}
		}
		
		if (f_v3) {
			cout << "grassmann::line_regulus_in_PG_3_q "
					"regulus element " << u << ":" << endl;
			Int_matrix_print(M, 2, 4);
		}
		regulus[u] = rank_lint_here(M, 0);

	} // next u
	if (f_vv) {
		cout << "grassmann::line_regulus_in_PG_3_q regulus:" << endl;
		Lint_vec_print(cout, regulus, regulus_size);
		cout << endl;
	}
	if (f_v) {
		cout << "grassmann::line_regulus_in_PG_3_q done" << endl;
	}
}

void grassmann::compute_dual_line_idx(int *&dual_line_idx,
		int *&self_dual_lines, int &nb_self_dual_lines,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 1);
	int *Basis;
	int nb_lines;
	int a, b;

	if (f_v) {
		cout << "grassmann::compute_dual_line_idx" << endl;
	}
	if (2 * k != n) {
		cout << "grassmann::compute_dual_line_idx need 2 * k == n" << endl;
		exit(1);
	}
	nb_lines = nCkq->as_int();
	Basis = NEW_int(n * n);
	dual_line_idx = NEW_int(nb_lines);
	self_dual_lines = NEW_int(nb_lines);

	nb_self_dual_lines = 0;
	for (a = 0; a < nb_lines; a++) {
		unrank_lint_here(Basis, a, 0/*verbose_level - 4*/);
		if (f_vv) {
			cout << "line " << a << " / " << nb_lines << ":" << endl;
			cout << "line is generated by" << endl;
			Int_matrix_print(Basis, k, n);
		}
		F->Linear_algebra->perp_standard(n, k, Basis, 0 /*verbose_level*/);
		if (f_vv) {
			cout << "after perp:" << endl;
			Int_matrix_print(Basis, n, n);
		}
		b = rank_lint_here(Basis + k * n, 0/*verbose_level - 4*/);
		if (f_vv) {
			cout << "line " << a << " / " << nb_lines
					<< " the dual is " << b << endl;
			cout << "dual line is generated by" << endl;
			Int_matrix_print(Basis + k * n, k, n);
		}
		dual_line_idx[a] = b;
		if (b == a) {
			self_dual_lines[nb_self_dual_lines++] = a;
		}
	}
	if (f_v) {
		cout << "grassmann::compute_dual_line_idx done" << endl;
	}
}

void grassmann::compute_dual_spread(
		int *spread, int *dual_spread, int spread_size,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 5);
	int *Basis;
	int i, a, b;

	if (f_v) {
		cout << "grassmann::compute_dual_spread" << endl;
	}
	if (2 * k != n) {
		cout << "grassmann::compute_dual_spread, need 2 * k == n" << endl;
		exit(1);
	}
	//Basis = NEW_int(n * n);
	Basis = M2;
	if (f_v) {
		cout << "grassmann::compute_dual_spread The spread is : ";
		Int_vec_print(cout, spread, spread_size);
		cout << endl;
	}
	for (i = 0; i < spread_size; i++) {
		a = spread[i];
		unrank_lint_here(Basis, a, 0/*verbose_level - 4*/);
		if (f_vv) {
			cout << i << "-th Line has rank " << a
					<< " and is generated by" << endl;
			Int_matrix_print(Basis, k, n);
		}
		F->Linear_algebra->perp_standard(n, k, Basis, 0 /*verbose_level*/);
		if (f_vv) {
			cout << "after perp:" << endl;
			Int_matrix_print(Basis, n, n);
		}
		b = rank_lint_here(Basis + k * n, 0/*verbose_level - 4*/);
		if (f_vv) {
			cout << i << "-th Line dual has rank " << b
					<< " and is generated by" << endl;
			Int_matrix_print(Basis + k * n, k, n);
		}
		dual_spread[i] = b;
	}
	if (f_v) {
		cout << "grassmann::compute_dual_spread The dual spread is : ";
		Int_vec_print(cout, dual_spread, spread_size);
		cout << endl;
	}
	
	//FREE_int(Basis);
	if (f_v) {
		cout << "grassmann::compute_dual_spread done" << endl;
	}
}


void grassmann::latex_matrix(ostream &ost, int *p)
{

	F->print_matrix_latex(ost, p, k, n);

}

void grassmann::latex_matrix_numerical(ostream &ost, int *p)
{

	F->print_matrix_numerical_latex(ost, p, k, n);

}

void grassmann::create_Schlaefli_graph(int *&Adj, int &sz, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "grassmann::create_Schlaefli_graph" << endl;
	}

	long int i, j, N;
	int rr;
	int *M1;
	int *M2;
	int *M;
	int v[2];
	int w[4];
	int *List;
	combinatorics::combinatorics_domain Combi;


	M1 = NEW_int(k * n);
	M2 = NEW_int(k * n);
	M = NEW_int(2 * k * n);

	N = Combi.generalized_binomial(n, k, q);

	List = NEW_int(N);
	sz = 0;

	for (i = 0; i < N; i++) {
		unrank_lint_here(M1, i, 0 /* verbose_level */);

		for (j = 0; j < q + 1; j++) {
			F->Linear_algebra->unrank_point_in_PG(v, 2, j);
			F->Linear_algebra->mult_vector_from_the_left(v, M1, w, k, n);
			if (F->evaluate_Fermat_cubic(w)) {
				break;
			}
		}
		if (j == q + 1) {
			List[sz++] = i;
		}
	}
	if (f_v) {
		cout << "create_graph::create_Schlaefli We found " << sz << " lines on the surface" << endl;
	}


	Adj = NEW_int(sz * sz);
	Int_vec_zero(Adj, sz * sz);

	for (i = 0; i < sz; i++) {
		unrank_lint_here(M1, List[i], 0 /* verbose_level */);

		for (j = i + 1; j < sz; j++) {
			unrank_lint_here(M2, List[j], 0 /* verbose_level */);

			Int_vec_copy(M1, M, k * n);
			Int_vec_copy(M2, M + k * n, k * n);

			rr = F->Linear_algebra->rank_of_rectangular_matrix(M, 2 * k, n, 0 /* verbose_level */);
			if (rr == 2 * k) {
				Adj[i * sz + j] = 1;
				Adj[j * sz + i] = 1;
			}
		}
	}


	FREE_int(List);
	FREE_int(M1);
	FREE_int(M2);
	FREE_int(M);
	if (f_v) {
		cout << "grassmann::create_Schlaefli_graph done" << endl;
	}

}

long int grassmann::make_special_element_zero(int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_v3 = (verbose_level >= 3);
	long int a;

	if (f_v) {
		cout << "grassmann::make_special_element_zero" << endl;
	}

	//int i;

	// make the element (I_k | 0).
	// Let a be its rank
	Int_vec_zero(M, k * n);
	make_identity_front(M, verbose_level);
#if 0
	for (i = 0; i < k; i++) {
		M[i * n + i] = 1;
	}
#endif
	if (f_v3) {
		cout << "grassmann::make_special_element_zero M:" << endl;
		Int_vec_print_integer_matrix_width(cout, M, k, n, n, F->log10_of_q + 1);
	}
	a = rank_lint(0/*verbose_level - 4*/);
	if (f_v3) {
		cout << "grassmann::make_special_element_zero a=" << a << endl;
	}
	return a;
}

long int grassmann::make_special_element_one(int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_v3 = (verbose_level >= 3);
	long int a;

	if (f_v) {
		cout << "grassmann::make_special_element_one" << endl;
	}

	//int i;

	// make the element (I_k | I_k).
	// Let a be its rank
	Int_vec_zero(M, k * n);
	make_identity_front(M, verbose_level);
	make_identity_back(M, verbose_level);
#if 0
	for (i = 0; i < k; i++) {
		M[i * n + i] = 1;
		M[i * n + k + i] = 1;
	}
#endif
	if (f_v3) {
		cout << "grassmann::make_special_element_one M:" << endl;
		Int_vec_print_integer_matrix_width(cout, M, k, n, n, F->log10_of_q + 1);
	}
	a = rank_lint(0/*verbose_level - 4*/);
	if (f_v3) {
		cout << "grassmann::make_special_element_one a=" << a << endl;
	}
	return a;
}

long int grassmann::make_special_element_infinity(int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_v3 = (verbose_level >= 3);
	long int a;

	if (f_v) {
		cout << "grassmann::make_special_element_infinity" << endl;
	}

	//int i;

	// make the element (I_k | I_k).
	// Let a be its rank
	Int_vec_zero(M, k * n);

	make_identity_back(M, verbose_level);

#if 0
	for (i = 0; i < k; i++) {
		M[i * n + k + i] = 1;
	}
#endif

	if (f_v3) {
		cout << "grassmann::make_special_element_infinity M:" << endl;
		Int_vec_print_integer_matrix_width(cout, M, k, n, n, F->log10_of_q + 1);
	}
	a = rank_lint(0/*verbose_level - 4*/);
	if (f_v3) {
		cout << "grassmann::make_special_element_infinity a=" << a << endl;
	}
	return a;
}


void grassmann::make_identity_front(int *M, int verbose_level)
{
	int i, j, a;

	for (i = 0; i < k; i++) {
		for (j = 0; j < k; j++) {
			if (i == j) {
				a = 1;
			}
			else {
				a = 0;
			}
			M[i * n + j] = a;
		}
	}
}

void grassmann::make_identity_back(int *M, int verbose_level)
{
	int i, j, a;

	for (i = 0; i < k; i++) {
		for (j = 0; j < k; j++) {
			if (i == j) {
				a = 1;
			}
			else {
				a = 0;
			}
			M[i * n + k + j] = a;
		}
	}
}

void grassmann::copy_matrix_back(int *A, int *M, int verbose_level)
{
	int i, j, a;

	for (i = 0; i < k; i++) {
		for (j = 0; j < k; j++) {
			a = A[i * k + j];
			M[i * n + k + j] = a;
		}
	}
}

void grassmann::extract_matrix_from_back(int *A, int *M, int verbose_level)
{
	int i, j, a;

	for (i = 0; i < k; i++) {
		for (j = 0; j < k; j++) {
			a = M[i * n + k + j];
			A[i * k + j] = a;
		}
	}
}

void grassmann::make_spread_from_spread_set(
		long int *Spread_set, int sz,
		long int *&Spread, int &spread_sz,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int *M;
	int *Spread_set_int;
	int h, h1, k2;

	if (f_v) {
		cout << "grassmann::make_spread_from_spread_set, sz = " << sz << endl;
	}

	spread_sz = sz + 1;
	k2 = k * k;
	Spread = NEW_lint(spread_sz);
	M = NEW_int(k * n);
	Spread_set_int = NEW_int(k2 * sz);
	for (h = 0; h < k2 * sz; h++) {
		Spread_set_int[h] = Spread_set[h];
	}

	for (h = 0; h < spread_sz; h++) {
		if (f_v) {
			cout << "grassmann::make_spread_from_spread_set, h = " << h << " / " << spread_sz << endl;
		}
		Int_vec_zero(M, k * n);
		if (h == 0) {
			make_identity_front(M, 0 /* verbose_level */);
			copy_matrix_back(Spread_set_int + h * k2, M, 0 /* verbose_level */);
		}
		else if (h == 1) {
			make_identity_back(M, 0 /* verbose_level */);
		}
		else {
			h1 = h - 1;
			make_identity_front(M, 0 /* verbose_level */);
			copy_matrix_back(Spread_set_int + h1 * k2, M, 0 /* verbose_level */);
		}
		if (f_v) {
			cout << "grassmann::make_spread_from_spread_set, before rank_lint_here" << endl;
		}
		Spread[h] = rank_lint_here(M, 0 /*verbose_level - 4*/);
	}
	FREE_int(Spread_set_int);
	FREE_int(M);

	if (f_v) {
		cout << "grassmann::make_spread_from_spread_set done" << endl;
	}
}


void grassmann::make_spread_set_from_spread(
		long int *Spread, int spread_sz,
		int *&Spread_set, int &sz,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int *M, *A;
	int h, h1, k2;

	if (f_v) {
		cout << "grassmann::make_spread_set_from_spread, spread_sz = " << spread_sz << endl;
	}

	sz = spread_sz - 1;
	k2 = k * k;
	Spread_set = NEW_int(sz * k2);
	M = NEW_int(k * n);
	A = NEW_int(k * k);

	for (h = 0; h < spread_sz; h++) {
		if (f_v) {
			cout << "grassmann::make_spread_set_from_spread, h = " << h << " / " << spread_sz << endl;
		}

		if (h == 1) {
			continue;
		}

		if (h > 1) {
			h1 = h - 1;
		}
		else {
			h1 = h;
		}

		unrank_lint_here(M, Spread[h], 0 /*verbose_level - 4*/);

		extract_matrix_from_back(A, M, 0 /*verbose_level - 4*/);

		Int_vec_copy(A, Spread_set + h1 * k2, k2);
	}
	FREE_int(A);
	FREE_int(M);

	if (f_v) {
		cout << "grassmann::make_spread_set_from_spread done" << endl;
	}
}


void grassmann::make_partition(long int *Spread, int spread_sz, long int *&Part, int &s, int verbose_level)
{
	int f_v = (verbose_level >= 1);


	if (f_v) {
		cout << "grassmann::make_partition" << endl;
	}
	int i;

	s = nb_points_covered(verbose_level);

	Part = NEW_lint(spread_sz * s);

	for (i = 0; i < spread_sz; i++) {
		unrank_lint(Spread[i], 0 /*verbose_level - 4*/);
		points_covered(Part + i * s, 0 /*verbose_level - 4*/);
	}

	if (f_v) {
		cout << "grassmann::make_partition done" << endl;
	}
}

void grassmann::make_spread_element(int *Spread_element, int *A, int verbose_level)
{
	int f_v = (verbose_level >= 1);


	if (f_v) {
		cout << "grassmann::make_spread_element" << endl;
	}
	int i, j;

	// make the k x n matrix ( I_k | Elt1 )
	Int_vec_zero(Spread_element, k * n);
	for (i = 0; i < k; i++) {
		Spread_element[i * n + i] = 1;
	}
	for (i = 0; i < k; i++) {
		for (j = 0; j < k; j++) {
			Spread_element[i * n + k + j] = A[i * k + j];
		}
	}
	if (f_v) {
		cout << "grassmann::make_spread_element done" << endl;
	}
}

}}}






