/*
 * projective_space3.cpp
 *
 *  Created on: Jan 9, 2022
 *      Author: betten
 */




#include "foundations.h"


using namespace std;




namespace orbiter {
namespace layer1_foundations {
namespace geometry {


int projective_space::reverse_engineer_semilinear_map(
	int *Elt, int *Mtx, int &frobenius,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	//finite_field *F;
	int d = n + 1;
	int *v1, *v2, *v1_save;
	int *w1, *w2, *w1_save;
	int /*q,*/ h, hh, i, j, l, e, frobenius_inv, lambda, rk, c, cv;
	int *system;
	int *base_cols;
	number_theory::number_theory_domain NT;


	if (f_v) {
		cout << "projective_space::reverse_engineer_semilinear_map" << endl;
		cout << "verbose_level=" << verbose_level << endl;
		cout << "d=" << d << endl;
	}
	//F = P->F;
	//q = F->q;

	v1 = NEW_int(d);
	v2 = NEW_int(d);
	v1_save = NEW_int(d);
	w1 = NEW_int(d);
	w2 = NEW_int(d);
	w1_save = NEW_int(d);



	if (f_v) {
		cout << "projective_space::reverse_engineer_semilinear_map "
				"mapping unit vectors" << endl;
	}
	for (e = 0; e < d; e++) {
		// map the unit vector e_e
		// (with a one in position e and zeros elsewhere):
		for (h = 0; h < d; h++) {
			if (h == e) {
				v1[h] = 1;
			}
			else {
				v1[h] = 0;
			}
		}
		Int_vec_copy(v1, v1_save, d);
		i = rank_point(v1);
			// Now, the value of i should be equal to e.
		//j = element_image_of(i, Elt, 0);
		j = Elt[i];
		unrank_point(v2, j);
		if (f_v) {
			cout << "projective_space::reverse_engineer_semilinear_map "
					"unit vector " << e << " has rank " << i << " and maps to " << j << endl;
		}

#if 0
		if (f_vv) {
			print_from_to(d, i, j, v1_save, v2);
		}
#endif


		Int_vec_copy(v2, Mtx + e * d, d);
	}

	if (f_vv) {
		cout << "Mtx (before scaling):" << endl;
		Int_vec_print_integer_matrix_width(cout, Mtx, d, d, d, F->log10_of_q);
		cout << endl;
	}

	// map the vector (1,1,...,1):
	if (f_v) {
		cout << "projective_space::reverse_engineer_semilinear_map "
				"mapping the all-one vector"
				<< endl;
	}
	for (h = 0; h < d; h++) {
		v1[h] = 1;
	}
	Int_vec_copy(v1, v1_save, d);
	i = rank_point(v1);
	//j = element_image_of(i, Elt, 0);
	j = Elt[i];
	unrank_point(v2, j);
	if (f_v) {
		cout << "projective_space::reverse_engineer_semilinear_map "
				"the all one vector has rank " << i << " and maps to " << j << endl;
	}

#if 0
	if (f_vv) {
		print_from_to(d, i, j, v1_save, v2);
	}
#endif

	system = NEW_int(d * (d + 1));
	base_cols = NEW_int(d + 1);
	// coefficient matrix:
	for (i = 0; i < d; i++) {
		for (j = 0; j < d; j++) {
			system[i * (d + 1) + j] = Mtx[j * d + i];
		}
	}
	// RHS:
	for (i = 0; i < d; i++) {
		system[i * (d + 1) + d] = v2[i];
	}
	if (f_vv) {
		cout << "projective_space::reverse_engineer_semilinear_map "
				"linear system:" << endl;
		Int_vec_print_integer_matrix_width(cout, system,
				d, d + 1, d + 1, F->log10_of_q);
		cout << endl;
	}
	rk = F->Linear_algebra->Gauss_simple(system, d, d + 1, base_cols, verbose_level - 4);
	if (rk != d) {
		cout << "projective_space::reverse_engineer_semilinear_map "
				"rk != d, fatal" << endl;
		exit(1);
	}
	if (f_vv) {
		cout << "projective_space::reverse_engineer_semilinear_map "
				"after Gauss_simple:" << endl;
		Int_vec_print_integer_matrix_width(cout, system,
				d, d + 1, d + 1, F->log10_of_q);
		cout << endl;
	}
	for (i = 0; i < d; i++) {
		c = system[i * (d + 1) + d];
		if (c == 0) {
			cout << "projective_space::reverse_engineer_semilinear_map "
					"the input matrix does not have full rank" << endl;
			exit(1);
		}
		for (j = 0; j < d; j++) {
			Mtx[i * d + j] = F->mult(c, Mtx[i * d + j]);
		}
	}

	if (f_vv) {
		cout << "Mtx (after scaling):" << endl;
		Int_vec_print_integer_matrix_width(cout, Mtx, d, d, d, F->log10_of_q);
		cout << endl;
	}



	frobenius = 0;
	if (F->q != F->p) {

		// figure out the frobenius:
		if (f_v) {
			cout << "projective_space::reverse_engineer_semilinear_map "
					"figuring out the frobenius" << endl;
		}


		// create the vector (1,p,0,...,0)

		for (h = 0; h < d; h++) {
			if (h == 0) {
				v1[h] = 1;
			}
			else if (h == 1) {
				v1[h] = F->p;
			}
			else {
				v1[h] = 0;
			}
		}
		Int_vec_copy(v1, v1_save, d);
		i = rank_point(v1);
		//j = element_image_of(i, Elt, 0);
		j = Elt[i];
		unrank_point(v2, j);


#if 0
		if (f_vv) {
			print_from_to(d, i, j, v1_save, v2);
		}
#endif


		// coefficient matrix:
		for (i = 0; i < d; i++) {
			for (j = 0; j < 2; j++) {
				system[i * 3 + j] = Mtx[j * d + i];
			}
		}
		// RHS:
		for (i = 0; i < d; i++) {
			system[i * 3 + 2] = v2[i];
		}
		rk = F->Linear_algebra->Gauss_simple(system,
				d, 3, base_cols, verbose_level - 4);
		if (rk != 2) {
			cout << "rk != 2, fatal" << endl;
			exit(1);
		}
		if (f_vv) {
			cout << "after Gauss_simple:" << endl;
			Int_vec_print_integer_matrix_width(cout,
					system, 2, 3, 3, F->log10_of_q);
			cout << endl;
		}

		c = system[0 * 3 + 2];
		if (c != 1) {
			cv = F->inverse(c);
			for (hh = 0; hh < 2; hh++) {
				system[hh * 3 + 2] = F->mult(cv, system[hh * 3 + 2]);
			}
		}
		if (f_vv) {
			cout << "after scaling the last column:" << endl;
			Int_vec_print_integer_matrix_width(cout,
					system, 2, 3, 3, F->log10_of_q);
			cout << endl;
		}
		lambda = system[1 * 3 + 2];
		if (f_vv) {
			cout << "lambda=" << lambda << endl;
		}


		l = F->log_alpha(lambda);
		if (f_vv) {
			cout << "l=" << l << endl;
		}
		for (i = 0; i < F->e; i++) {
			if (NT.i_power_j(F->p, i) == l) {
				frobenius = i;
				break;
			}
		}
		if (i == F->e) {
			cout << "projective_space::reverse_engineer_semilinear_map "
					"problem figuring out the Frobenius" << endl;
			exit(1);
		}

		frobenius_inv = (F->e - frobenius) % F->e;
		if (f_vv) {
			cout << "frobenius = " << frobenius << endl;
			cout << "frobenius_inv = " << frobenius_inv << endl;
		}
		for (hh = 0; hh < d * d; hh++) {
			Mtx[hh] = F->frobenius_power(Mtx[hh], frobenius_inv);
		}


	}
	else {
		frobenius = 0;
		frobenius_inv = 0;
	}


	if (f_v) {
		cout << "projective_space::reverse_engineer_semilinear_map "
				"we found the following map" << endl;
		cout << "Mtx:" << endl;
		Int_vec_print_integer_matrix_width(cout,
				Mtx, d, d, d, F->log10_of_q);
		cout << endl;
		cout << "frobenius = " << frobenius << endl;
		cout << "frobenius_inv = " << frobenius_inv << endl;
	}



	FREE_int(v1);
	FREE_int(v2);
	FREE_int(v1_save);
	FREE_int(w1);
	FREE_int(w2);
	FREE_int(w1_save);
	FREE_int(system);
	FREE_int(base_cols);

	if (f_v) {
		cout << "projective_space::reverse_engineer_semilinear_map done" << endl;
	}

	return TRUE;
}

void projective_space::create_elliptic_quadric_ovoid(
		std::string &label_txt,
		std::string &label_tex,
		int &nb_pts, long int *&Pts,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "projective_space::create_elliptic_quadric_ovoid" << endl;
	}
	int epsilon = -1;
	int c1 = 1, c2 = 0, c3 = 0;
	int i, j, d, h;
	int *v, *w;
	geometry_global Gg;

	if (n != 3) {
		cout << "projective_space::create_elliptic_quadric_ovoid n != 3" << endl;
		exit(1);
	}
	d = n + 1;

	nb_pts = Gg.nb_pts_Qepsilon(epsilon, n, q);

	v = NEW_int(n + 1);
	w = NEW_int(n + 1);
	Pts = NEW_lint(N_points);

	if (f_v) {
		cout << "i : point : projective rank" << endl;
	}
	F->Linear_algebra->choose_anisotropic_form(c1, c2, c3, verbose_level);
	for (i = 0; i < nb_pts; i++) {
		F->Orthogonal_indexing->Q_epsilon_unrank(v, 1, epsilon, n, c1, c2, c3, i, 0 /* verbose_level */);
		for (h = 0; h < d; h++) {
			w[h] = v[h];
		}
		j = rank_point(w);
		Pts[i] = j;
		if (f_v) {
			cout << setw(4) << i << " : ";
			Int_vec_print(cout, v, d);
			cout << " : " << setw(5) << j << endl;
		}
	}

#if 0
	cout << "list of points on the ovoid:" << endl;
	cout << nb_pts << endl;
	for (i = 0; i < nb_pts; i++) {
		cout << Pts[i] << " ";
		}
	cout << endl;
#endif

	char str[1000];
	char str2[1000];

	snprintf(str, sizeof(str), "_q%d", q);
	snprintf(str2, sizeof(str2), "\\_q%d", q);


	label_txt.assign("ovoid");
	label_txt.append(str);
	label_tex.assign("ovoid");
	label_tex.append(str2);

	//write_set_to_file(fname, L, N, verbose_level);

	FREE_int(v);
	FREE_int(w);
	//FREE_int(L);
	if (f_v) {
		cout << "projective_space::create_elliptic_quadric_ovoid done" << endl;
	}
}

void projective_space::create_ovoid_ST(
		std::string &label_txt,
		std::string &label_tex,
		int &nb_pts, long int *&Pts,
	int verbose_level)
// Suzuki Tits ovoid in PG(3,2^(2r+1)),
// following Heinz Lueneburg: Translation planes, 1980, Chapter IV
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "projective_space::create_ovoid_ST" << endl;
	}
	int i, d, x, y, z, r, sigma, sigma_plus_two;
	long int a;
	int v[4];
	number_theory::number_theory_domain NT;

	if (EVEN(F->e)) {
		cout << "projective_space::create_ovoid_ST need odd field degree" << endl;
		exit(1);
	}
	if (F->p != 2) {
		cout << "projective_space::create_ovoid_ST F->p != 2" << endl;
		exit(1);
	}
	if (n != 3) {
		cout << "projective_space::create_ovoid_ST need n == 3" << endl;
		exit(1);
	}

	r = (F->e - 1) >> 1;

	sigma = NT.i_power_j(2, r + 1);
	sigma_plus_two = sigma + 2;


	if (f_v) {
		cout << "projective_space::create_ovoid_ST r = " << r << endl;
		cout << "projective_space::create_ovoid_ST sigma = " << sigma << endl;
		cout << "projective_space::create_ovoid_ST sigma_plus_two = " << sigma_plus_two << endl;
	}

	d = n + 1;

	nb_pts = F->q * F->q + 1;

	Pts = NEW_lint(nb_pts);

	i = 0;
	Pts[i++] = 1; // (0,1,0,0)
	for (x = 0; x < F->q; x++) {
		for (y = 0; y < F->q; y++) {

			z = F->add3(F->mult(x, y), F->power(x, sigma_plus_two), F->power(y, sigma));

			v[0] = 1;
			v[1] = z;
			v[2] = x;
			v[3] = y;

			a = rank_point(v);
			Pts[i++] = a;
		}
	}

	if (i != nb_pts) {
		cout << "projective_space::create_ovoid_ST i != nb_pts" << endl;
	}
	if (f_v) {
		cout << "i : point : projective rank" << endl;
		for (i = 0; i < nb_pts; i++) {
			cout << setw(4) << i << " : ";
			a = Pts[i];
			unrank_point(v, a);
			Int_vec_print(cout, v, d);
			cout << " : " << setw(5) << a << endl;
		}
	}

	char str[1000];
	char str2[1000];

	snprintf(str, sizeof(str), "_q%d", q);
	snprintf(str2, sizeof(str2), "\\_q%d", q);


	label_txt.assign("ovoid_ST");
	label_txt.append(str);
	label_tex.assign("ovoid\\_ST");
	label_tex.append(str2);

	if (f_v) {
		cout << "projective_space::create_ovoid_ST done" << endl;
	}
}

void projective_space::create_cuspidal_cubic(
		std::string &label_txt,
		std::string &label_tex,
		int &nb_pts, long int *&Pts,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "projective_space::create_cuspidal_cubic" << endl;
	}
	//int n = 2;
	long int i, a, d, s, t;
	int *v;
	int v2[2];

	if (n != 2) {
		cout << "projective_space::create_cuspidal_cubic n != 2" << endl;
		exit(1);
	}
	d = n + 1;
	nb_pts = q + 1;

	v = NEW_int(d);
	Pts = NEW_lint(N_points);

	if (f_v) {
		cout << "i : point : projective rank" << endl;
	}
	for (i = 0; i < nb_pts; i++) {
		F->PG_element_unrank_modified(v2, 1, 2, i);
		s = v2[0];
		t = v2[1];
		v[0] = F->mult(F->power(s, 3), F->power(t, 0));
		v[1] = F->mult(F->power(s, 2), F->power(t, 1));
		v[2] = F->mult(F->power(s, 0), F->power(t, 3));
#if 0
		for (j = 0; j < d; j++) {
			v[j] = F->mult(F->power(s, n - j), F->power(t, j));
		}
#endif
		a = rank_point(v);
		Pts[i] = a;
		if (f_v) {
			cout << setw(4) << i << " : ";
			Int_vec_print(cout, v, d);
			cout << " : " << setw(5) << a << endl;
		}
	}

#if 0
	cout << "list of points on the cubic:" << endl;
	cout << N << endl;
	for (i = 0; i < N; i++) {
		cout << L[i] << " ";
		}
	cout << endl;
#endif

	char str[1000];
	char str2[1000];
	snprintf(str, sizeof(str), "cuspidal_cubic_%d", q);
	snprintf(str2, sizeof(str2), "cuspidal\\_cubic\\_%d", q);
	label_txt.assign(str);
	label_tex.assign(str2);
	//write_set_to_file(fname, L, N, verbose_level);


#if 0
	long int nCk;
	combinatorics_domain Combi;
	int k = 6;
	int rk;
	int idx[6];
	int *subsets;

	nCk = Combi.int_n_choose_k(nb_pts, k);
	subsets = NEW_int(nCk * k);
	for (rk = 0; rk < nCk; rk++) {
		Combi.unrank_k_subset(rk, idx, nb_pts, k);
		for (i = 0; i < k; i++) {
			subsets[rk * k + i] = Pts[idx[i]];
		}
	}


	string fname2;

	snprintf(str, sizeof(str), "cuspidal_cubic_%d_subsets_%d.txt", q, k);
	fname2.assign(str);

	{

		ofstream fp(fname2);

		for (rk = 0; rk < nCk; rk++) {
			fp << k;
			for (i = 0; i < k; i++) {
				fp << " " << subsets[rk * k + i];
			}
			fp << endl;
		}
		fp << -1 << endl;

	}

	file_io Fio;

	cout << "Written file " << fname2 << " of size " << Fio.file_size(fname2) << endl;

#endif



	FREE_int(v);
	//FREE_int(L);
	if (f_v) {
		cout << "projective_space::create_cuspidal_cubic done" << endl;
	}
}

void projective_space::create_twisted_cubic(
		std::string &label_txt,
		std::string &label_tex,
		int &nb_pts, long int *&Pts,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "projective_space::create_twisted_cubic" << endl;
	}
	//int n = 3;
	long int i, j, d, s, t;
	int *v;
	int v2[2];

	if (n != 3) {
		cout << "projective_space::create_twisted_cubic n != 3" << endl;
		exit(1);
	}
	d = n + 1;

	nb_pts = q + 1;

	v = NEW_int(n + 1);
	Pts = NEW_lint(N_points);

	if (f_v) {
		cout << "i : point : projective rank" << endl;
	}
	for (i = 0; i < nb_pts; i++) {
		F->PG_element_unrank_modified(v2, 1, 2, i);
		s = v2[0];
		t = v2[1];
		v[0] = F->mult(F->power(s, 3), F->power(t, 0));
		v[1] = F->mult(F->power(s, 2), F->power(t, 1));
		v[2] = F->mult(F->power(s, 1), F->power(t, 2));
		v[3] = F->mult(F->power(s, 0), F->power(t, 3));
		j = rank_point(v);
		Pts[i] = j;
		if (f_v) {
			cout << setw(4) << i << " : ";
			Int_vec_print(cout, v, d);
			cout << " : " << setw(5) << j << endl;
		}
	}

#if 0
	cout << "list of points on the twisted cubic:" << endl;
	cout << N << endl;
	for (i = 0; i < N; i++) {
		cout << L[i] << " ";
		}
	cout << endl;
#endif

	char str[1000];
	char str2[1000];
	snprintf(str, sizeof(str), "twisted_cubic_%d", q);
	snprintf(str2, sizeof(str2), "twisted\\_cubic\\_%d", q);
	label_txt.assign(str);
	label_tex.assign(str2);
	//write_set_to_file(fname, L, N, verbose_level);

	FREE_int(v);
	//FREE_int(L);
	if (f_v) {
		cout << "projective_space::create_twisted_cubic done" << endl;
	}
}

void projective_space::create_elliptic_curve(
	int elliptic_curve_b, int elliptic_curve_c,
	std::string &label_txt,
	std::string &label_tex,
	int &nb_pts, long int *&Pts,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "projective_space::create_elliptic_curve" << endl;
	}
	//int n = 2;
	long int i, a, d;
	int *v;
	number_theory::elliptic_curve *E;

	if (n != 2) {
		cout << "projective_space::create_elliptic_curve n != 2" << endl;
		exit(1);
	}
	d = n + 1;

	nb_pts = q + 1;

	E = NEW_OBJECT(number_theory::elliptic_curve);
	v = NEW_int(n + 1);
	Pts = NEW_lint(N_points);

	E->init(F, elliptic_curve_b, elliptic_curve_c,
			verbose_level);

	nb_pts = E->nb;

	if (f_v) {
		cout << "i : point : projective rank" << endl;
	}
	for (i = 0; i < nb_pts; i++) {
		F->PG_element_rank_modified_lint(E->T + i * d, 1, d, a);
		Pts[i] = a;
		if (f_v) {
			cout << setw(4) << i << " : ";
			Int_vec_print(cout, E->T + i * d, d);
			cout << " : " << setw(5) << a << endl;
		}
	}

#if 0
	cout << "list of points on the elliptic curve:" << endl;
	cout << N << endl;
	for (i = 0; i < N; i++) {
		cout << L[i] << " ";
		}
	cout << endl;
#endif

	char str[1000];
	char str2[1000];
	snprintf(str, sizeof(str), "elliptic_curve_b%d_c%d_q%d",
			elliptic_curve_b, elliptic_curve_c, q);
	snprintf(str2, sizeof(str2), "elliptic\\_curve\\_b%d\\_c%d\\_q%d",
			elliptic_curve_b, elliptic_curve_c, q);
	label_txt.assign(str);
	label_tex.assign(str2);
	//write_set_to_file(fname, L, N, verbose_level);


	FREE_OBJECT(E);
	FREE_int(v);
	//FREE_int(L);
	if (f_v) {
		cout << "projective_space::create_elliptic_curve done" << endl;
	}
}

void projective_space::create_unital_XXq_YZq_ZYq(
		std::string &label_txt,
		std::string &label_tex,
		int &nb_pts, long int *&Pts,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "projective_space::create_unital_XXq_YZq_ZYq" << endl;
	}
	//int n = 2;
	if (n != 2) {
		cout << "projective_space::create_unital_XXq_YZq_ZYq n != 2" << endl;
		exit(1);
	}
	int i, rk, d;
	int *v;

	d = n + 1;

	v = NEW_int(d);
	Pts = NEW_lint(N_points);


	create_unital_XXq_YZq_ZYq_brute_force(Pts, nb_pts, verbose_level - 1);


	if (f_v) {
		cout << "i : point : projective rank" << endl;
	}
	for (i = 0; i < nb_pts; i++) {
		rk = Pts[i];
		unrank_point(v, rk);
		if (f_v) {
			cout << setw(4) << i << " : ";
			Int_vec_print(cout, v, d);
			cout << " : " << setw(5) << rk << endl;
		}
	}


	char str[1000];
	char str2[1000];
	snprintf(str, sizeof(str), "unital_XXq_YZq_ZYq_Q%d", q);
	snprintf(str2, sizeof(str2), "unital\\_XXq\\_YZq\\_ZYq\\_Q%d", q);
	label_txt.assign(str);
	label_tex.assign(str2);

	FREE_int(v);
	if (f_v) {
		cout << "projective_space::create_unital_XXq_YZq_ZYq done" << endl;
	}
}

void projective_space::create_whole_space(
		std::string &label_txt,
		std::string &label_tex,
		int &nb_pts, long int *&Pts,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i; //, d;

	if (f_v) {
		cout << "projective_space::create_whole_space" << endl;
	}
	//d = n + 1;

	Pts = NEW_lint(N_points);
	nb_pts = N_points;
	for (i = 0; i < N_points; i++) {
		Pts[i] = i;
	}

	char str[1000];
	char str2[1000];
	snprintf(str, sizeof(str), "whole_space_PG_%d_%d", n, q);
	snprintf(str2, sizeof(str2), "whole\\_space\\_PG\\_%d\\_%d", n, q);
	label_txt.assign(str);
	label_tex.assign(str2);

	if (f_v) {
		cout << "projective_space::create_whole_space done" << endl;
	}
}

void projective_space::create_hyperplane(
	int pt,
	std::string &label_txt,
	std::string &label_tex,
	int &nb_pts, long int *&Pts,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	long int i, d, a;
	int *v1;
	int *v2;

	if (f_v) {
		cout << "projective_space::create_hyperplane pt=" << pt << endl;
	}
	d = n + 1;
	v1 = NEW_int(d);
	v2 = NEW_int(d);

	unrank_point(v1, pt);
	Pts = NEW_lint(N_points);
	nb_pts = 0;
	for (i = 0; i < N_points; i++) {
		unrank_point(v2, i);
		a = F->Linear_algebra->dot_product(d, v1, v2);
		if (a == 0) {
			Pts[nb_pts++] = i;
			if (f_v) {
				cout << setw(4) << nb_pts - 1 << " : ";
				Int_vec_print(cout, v2, d);
				cout << " : " << setw(5) << i << endl;
			}
		}
	}

	char str[1000];
	char str2[1000];
	snprintf(str, sizeof(str), "hyperplane_PG_%d_%d_pt%d", n, q, pt);
	snprintf(str2, sizeof(str2), "hyperplane\\_PG\\_%d\\_%d\\_pt%d", n, q, pt);
	label_txt.assign(str);
	label_tex.assign(str2);

	FREE_int(v1);
	FREE_int(v2);
	if (f_v) {
		cout << "projective_space::create_hyperplane done" << endl;
	}
}

void projective_space::create_Baer_substructure(
	long int *&Pts, int &nb_pts,
	std::string &label_txt,
	std::string &label_tex,
	int verbose_level)
// assumes we are in PG(n,Q) where Q = q^2
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "projective_space::create_Baer_substructure" << endl;
	}

	// projective space over the big field FQ = this

	number_theory::number_theory_domain NT;
	int Q = q;
	int q = NT.i_power_j(F->p, F->e >> 1);
	if (f_v) {
		cout << "projective_space::create_Baer_substructure Q=" << Q << " q=" << q << endl;
	}

	int sz;
	int *v;
	int d = n + 1;
	int i, j, a, b, index, f_is_in_subfield;

	if (f_v) {
		cout << "projective_space::create_Baer_substructure Q=" << Q << endl;
		cout << "projective_space::create_Baer_substructure q=" << q << endl;
	}

	index = (Q - 1) / (q - 1);

	if (f_v) {
		cout << "projective_space::create_Baer_substructure index=" << index << endl;
	}

	v = NEW_int(d);
	Pts = NEW_lint(N_points);

	sz = 0;
	for (i = 0; i < N_points; i++) {

		F->PG_element_unrank_modified(v, 1, d, i);

		for (j = 0; j < d; j++) {
			a = v[j];
			b = F->log_alpha(a);
			f_is_in_subfield = FALSE;

			if (a == 0 || (b % index) == 0) {
				f_is_in_subfield = TRUE;
			}
			if (!f_is_in_subfield) {
				break;
			}
		}
		if (j == d) {
			Pts[nb_pts++] = i;
		}
	}
	cout << "the Baer substructure "
			"PG(" << n << "," << q << ") inside "
			"PG(" << n << "," << Q << ") has size "
			<< sz << ":" << endl;
	for (i = 0; i < sz; i++) {
		cout << Pts[i] << " ";
	}
	cout << endl;



	char str[1000];

	//write_set_to_file(fname, S, sz, verbose_level);


	snprintf(str, sizeof(str), "PG_%d_%d", n, F->q);
	label_txt.assign("Baer_substructure_");
	label_txt.append(str);

	snprintf(str, sizeof(str), "PG(%d,%d)", n, F->q);
	label_tex.assign("Baer\\_substructure\\_");
	label_tex.append(str);

	FREE_int(v);
	//FREE_int(S);
	if (f_v) {
		cout << "projective_space::create_Baer_substructure done" << endl;
	}
}





}}}

