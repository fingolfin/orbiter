// incidence_structure.cpp
//
// Anton Betten
//
// June 20, 2010



#include "foundations.h"

using namespace std;


namespace orbiter {
namespace layer1_foundations {
namespace geometry {



// #############################################################################
// incidence_structure
// #############################################################################

incidence_structure::incidence_structure()
{
	//std::string label;

	nb_rows = nb_cols = 0;

	f_rowsums_constant = FALSE;
	f_colsums_constant = FALSE;
	r = k = 0;
	nb_lines_on_point = NULL;
	nb_points_on_line = NULL;
	max_r = min_r = max_k = min_k = 0;
	lines_on_point = NULL;
	points_on_line = NULL;

	realization_type = 0;

	M = NULL;
	O = NULL;
	H = NULL;
	P = NULL;

}


incidence_structure::~incidence_structure()
{
	if (M) {
		FREE_int(M);
	}
	if (O) {
		// do not destroy
	}
	if (H) {
		// do not destroy
	}
	if (nb_lines_on_point) {
		FREE_int(nb_lines_on_point);
	}
	if (nb_points_on_line) {
		FREE_int(nb_points_on_line);
	}
	if (lines_on_point) {
		FREE_int(lines_on_point);
	}
	if (points_on_line) {
		FREE_int(points_on_line);
	}
}

void incidence_structure::check_point_pairs(int verbose_level)
{
	int *Mtx;
	int i, j, nb;
	int *Lines;

	Lines = NEW_int(nb_cols);
	Mtx = NEW_int(nb_rows * nb_rows);
	for (i = 0; i < nb_rows; i++) {
		for (j = i; j < nb_rows; j++) {
			nb = lines_through_two_points(Lines, i, j, 0);
			Mtx[i * nb_rows + j] = nb;
			Mtx[j * nb_rows + i] = nb;
		}
	}
	cout << "nb of lines through two points:" << endl;
	Int_vec_print_integer_matrix_width(cout, Mtx, nb_rows, nb_rows, nb_rows, 1);
	
	FREE_int(Lines);
}

int incidence_structure::lines_through_two_points(int *lines,
		int p1, int p2, int verbose_level)
{
	int h1, h2, l1, l2, nb;
	
	nb = 0;
	for (h1 = 0; h1 < nb_lines_on_point[p1]; h1++) {
		l1 = lines_on_point[p1 * max_r + h1];
		for (h2 = 0; h2 < nb_lines_on_point[p2]; h2++) {
			l2 = lines_on_point[p2 * max_r + h2];
			if (l1 == l2) {
				lines[nb++] = l1;
				break;
			}
		}
	}
	return nb;
}

void incidence_structure::init_projective_space(projective_space *P, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i;

	if (f_v) {
		cout << "incidence_structure::init_projective_space" << endl;
	}
	realization_type = INCIDENCE_STRUCTURE_REALIZATION_BY_PROJECTIVE_SPACE;
	incidence_structure::P = P;
	nb_rows = P->N_points;
	nb_cols = P->N_lines;

	f_rowsums_constant = TRUE;
	f_colsums_constant = TRUE;
	r = P->r;
	k = P->k;
	nb_lines_on_point = NEW_int(nb_rows);
	nb_points_on_line = NEW_int(nb_cols);
	for (i = 0; i < nb_rows; i++) {
		nb_lines_on_point[i] = r;
	}
	for (i = 0; i < nb_cols; i++) {
		nb_points_on_line[i] = k;
	}
	//int *nb_lines_on_point;
	//int *nb_points_on_line;
	max_r = min_r = r;
	max_k = min_k = k;

	if (f_v) {
		cout << "incidence_structure::init_projective_space nb_rows=" << nb_rows << endl;
		cout << "incidence_structure::init_projective_space nb_cols=" << nb_cols << endl;
	}
}

void incidence_structure::init_hjelmslev(hjelmslev *H, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	//int f_vv = (verbose_level >= 2);
	//algebra_global Algebra;

	if (f_v) {
		cout << "incidence_structure::init_hjelmslev" << endl;
	}
	realization_type = INCIDENCE_STRUCTURE_REALIZATION_BY_HJELMSLEV;
	incidence_structure::H = H;
	f_rowsums_constant = TRUE;
	f_colsums_constant = TRUE;
	//max_r = min_r = r = O->alpha;
	//max_k = min_k = k = O->q + 1;
	nb_rows = H->R->nb_PHG_elements(H->k);
	nb_cols = H->number_of_submodules();
	if (f_v) {
		cout << "incidence_structure::init_hjelmslev nb_rows=" << nb_rows << endl;
		cout << "incidence_structure::init_hjelmslev nb_cols=" << nb_cols << endl;
	}
	int *Mtx;
	int *Inc_Mtx;
	int *v;
	int *base_cols;
	int mtx_rk;
	int i, j, h, k, n;

	k = H->k;
	n = H->n;
	Mtx = NEW_int((k + 1) * n);
	Inc_Mtx = NEW_int(nb_rows * nb_cols);
	v = NEW_int(k);
	base_cols = NEW_int(n);
	for (i = 0; i < nb_rows; i++) {
		for (j = 0; j < nb_cols; j++) {
			cout << "i=" << i << " j=" << j << endl;
			H->R->PHG_element_unrank(Mtx, 1, n, i);
			H->unrank_lint(Mtx + n, j, 0);
			Int_vec_print_integer_matrix_width(cout, Mtx, k + 1, n, n, 1);
			mtx_rk = H->R->Gauss_int(
				Mtx, TRUE, FALSE, base_cols, FALSE, NULL,
				k + 1, n, n, 2);
			cout << "after Gauss:" << endl;
			Int_vec_print_integer_matrix_width(cout, Mtx, k + 1, n, n, 1);
			cout << "the rank is " << mtx_rk << endl;

			for (h = 0; h < n; h++) {
				if (Mtx[k * n + h]) {
					break;
				}
			}
			if (h < n) {
				if (f_v) {
					cout << "the last row is nonzero, the point is "
							"not on the line" << endl;
				}
				Inc_Mtx[i * nb_cols + j] = 0;
			}
			else {
				if (f_v) {
					cout << "the last row is zero, the point is on "
							"the line" << endl;
				}
				Inc_Mtx[i * nb_cols + j] = 1;
			}

#if 0
			if (mtx_rk == H->k) {
				Inc_Mtx[i * nb_cols + j] = 1;
			}
			else {
				Inc_Mtx[i * nb_cols + j] = 0;
			}
#endif
		}
	}
	if (f_v) {
		cout << "incidence matrix" << endl;
		Int_vec_print_integer_matrix_width(cout,
				Inc_Mtx, nb_rows, nb_cols, nb_cols, 1);
	}

	init_by_matrix(nb_rows, nb_cols, Inc_Mtx, verbose_level);
	realization_type = INCIDENCE_STRUCTURE_REALIZATION_BY_HJELMSLEV;
	
	FREE_int(Mtx);
	FREE_int(Inc_Mtx);
	FREE_int(v);
	FREE_int(base_cols);
	if (f_v) {
		cout << "incidence_structure::init_hjelmslev done" << endl;
	}
}

void incidence_structure::init_orthogonal(
		orthogonal_geometry::orthogonal *O, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	//int f_vv = (verbose_level >= 2);
	int i;

	if (f_v) {
		cout << "incidence_structure::init_orthogonal" << endl;
	}
	realization_type = INCIDENCE_STRUCTURE_REALIZATION_BY_ORTHOGONAL;
	incidence_structure::O = O;
	f_rowsums_constant = TRUE;
	f_colsums_constant = TRUE;
	max_r = min_r = r = O->Hyperbolic_pair->alpha;
	max_k = min_k = k = O->q + 1;
	nb_rows = O->Hyperbolic_pair->nb_points;
	nb_cols = O->Hyperbolic_pair->nb_lines;
	nb_lines_on_point = NEW_int(nb_rows);
	nb_points_on_line = NEW_int(nb_cols);
	for (i = 0; i < nb_rows; i++) {
		nb_lines_on_point[i] = max_r;
	}
	for (i = 0; i < nb_cols; i++) {
		nb_points_on_line[i] = max_k;
	}
	if (f_v) {
		cout << "incidence_structure::init_orthogonal done" << endl;
	}
}

void incidence_structure::init_by_incidences(int m, int n,
		int nb_inc, int *X, int verbose_level)
{
	int *M;
	int h, i, a;

	M = NEW_int(m * n);
	for (i = 0; i < m * n; i++) {
		M[i] = 0;
	}
	for (h = 0; h < nb_inc; h++) {
		a = X[h];
		M[a] = 1;
	}
	init_by_matrix(m, n, M, verbose_level);
	FREE_int(M);
}

void incidence_structure::init_by_R_and_X(int m, int n,
		int *R, int *X, int max_r, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int *M;
	int i, j, h;

	if (f_v) {
		cout << "incidence_structure::init_by_R_and_X "
				"m=" << m << " n=" << n << endl;
	}
	M = NEW_int(m * n);
	for (i = 0; i < m * n; i++) {
		M[i] = 0;
	}
	for (i = 0; i < m; i++) {
		for (h = 0; h < R[i]; h++) {
			j = X[i * max_r + h];
			M[i * n + j] = 1;
		}
	}
	init_by_matrix(m, n, M, verbose_level - 1);
}

void incidence_structure::init_by_set_of_sets(
		data_structures::set_of_sets *SoS, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int *M;
	int i, j, h, m, n;

	if (f_v) {
		cout << "incidence_structure::init_by_set_of_sets" << endl;
	}
	m = SoS->nb_sets;
	n = SoS->underlying_set_size;
	M = NEW_int(m * n);
	Int_vec_zero(M, m * n);
	for (i = 0; i < m; i++) {
		for (h = 0; h < SoS->Set_size[i]; h++) {
			j = SoS->Sets[i][h];
			M[i * n + j] = 1;
		}
	}
	init_by_matrix(m, n, M, verbose_level - 1);
	if (f_v) {
		cout << "incidence_structure::init_by_set_of_sets done" << endl;
	}
}

void incidence_structure::init_by_matrix(
		int m, int n, int *M, int verbose_level)
{
	int i;
	int f_v = (verbose_level >= 1);
	//int f_vv = (verbose_level >= 2);

	if (f_v) {
		cout << "incidence_structure::init_by_matrix "
				"m=" << m << " n=" << n << endl;
	}
	realization_type = INCIDENCE_STRUCTURE_REALIZATION_BY_MATRIX;
	nb_rows = m;
	nb_cols = n;
	incidence_structure::M = NEW_int(m * n);
	nb_lines_on_point = NEW_int(nb_rows);
	nb_points_on_line = NEW_int(nb_cols);
	for (i = 0; i < m * n; i++) {
		incidence_structure::M[i] = M[i];
	}
	init_by_matrix2(verbose_level);
	if (f_v) {
		cout << "incidence_structure::init_by_matrix done" << endl;
	}
}

void incidence_structure::init_by_matrix_as_bitmatrix(
		int m, int n, data_structures::bitmatrix *Bitmatrix,
		int verbose_level)
{
	int i, j;
	int f_v = (verbose_level >= 1);
	//int f_vv = (verbose_level >= 2);

	if (f_v) {
		cout << "incidence_structure::init_by_matrix_as_bitvector "
				"m=" << m << " n=" << n << endl;
	}
	realization_type = INCIDENCE_STRUCTURE_REALIZATION_BY_MATRIX;
	nb_rows = m;
	nb_cols = n;
	M = NEW_int(m * n);
	nb_lines_on_point = NEW_int(nb_rows);
	nb_points_on_line = NEW_int(nb_cols);
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			M[i] = Bitmatrix->s_ij(i, j);
		}
	}
	init_by_matrix2(verbose_level);

	if (f_v) {
		cout << "incidence_structure::init_by_matrix_as_bitvector "
				"done" << endl;
	}
}

void incidence_structure::init_by_matrix2(int verbose_level)
{
	int i, j, h;
	int f_v = (verbose_level >= 1);
	int m = nb_rows;
	int n = nb_cols;

	if (f_v) {
		cout << "incidence_structure::init_by_matrix2" << endl;
	}
	for (i = 0; i < m; i++) {
		nb_lines_on_point[i] = 0;
		for (j = 0; j < n; j++) {
			if (M[i * n + j]) {
				nb_lines_on_point[i]++;
			}
		}
	}
	for (j = 0; j < n; j++) {
		nb_points_on_line[j] = 0;
		for (i = 0; i < m; i++) {
			if (M[i * n + j]) {
				nb_points_on_line[j]++;
			}
		}
	}
	min_r = nb_lines_on_point[0];
	max_r = nb_lines_on_point[0];
	for (i = 1; i < m; i++) {
		if (nb_lines_on_point[i] > max_r) {
			max_r = nb_lines_on_point[i];
		}
		if (nb_lines_on_point[i] < min_r) {
			min_r = nb_lines_on_point[i];
		}
	}

	if (f_v) {
		cout << "incidence_structure::init_by_matrix2 "
				"min_r=" << min_r << " max_r=" << max_r << endl;
	}

	min_k = nb_points_on_line[0];
	max_k = nb_points_on_line[0];
	for (j = 1; j < n; j++) {
		if (nb_points_on_line[j] > max_k) {
			max_k = nb_points_on_line[j];
		}
		if (nb_points_on_line[j] < min_k) {
			min_k = nb_points_on_line[j];
		}
	}

	if (f_v) {
		cout << "incidence_structure::init_by_matrix2 "
				"min_k=" << min_k << " max_k=" << max_k << endl;
	}

	if (max_r == min_r) {
		f_rowsums_constant = TRUE;
		r = max_r;
	}
	else {
		f_rowsums_constant = FALSE;
	}
	if (max_k == min_k) {
		f_colsums_constant = TRUE;
		k = max_k;
	}
	else {
		f_colsums_constant = FALSE;
	}
	lines_on_point = NEW_int(m * max_r);
	points_on_line = NEW_int(n * max_k);
	for (i = 0; i < m * max_r; i++) {
		lines_on_point[i] = 0;
	}
	for (i = 0; i < n * max_k; i++) {
		points_on_line[i] = 0;
	}
	for (i = 0; i < m; i++) {
		h = 0;
		for (j = 0; j < n; j++) {
			if (M[i * n + j]) {
				lines_on_point[i * max_r + h++] = j;
			}
		}
	}
	for (j = 0; j < n; j++) {
		h = 0;
		for (i = 0; i < m; i++) {
			if (M[i * n + j]) {
				points_on_line[j * max_k + h++] = i;
			}
		}
	}
#if 0
	if (f_vv) {
		cout << "lines_on_point:" << endl;
		print_integer_matrix_width(cout,
				lines_on_point, m, max_r, max_r, 4);
		cout << "points_on_line:" << endl;
		print_integer_matrix_width(cout,
				points_on_line, n, max_k, max_k, 4);
	}
#endif
	if (f_v) {
		cout << "incidence_structure::init_by_matrix2 done" << endl;
	}
}


int incidence_structure::nb_points()
{
	return nb_rows;
}

int incidence_structure::nb_lines()
{
	return nb_cols;
}

int incidence_structure::get_ij(int i, int j)
{
	if (realization_type == INCIDENCE_STRUCTURE_REALIZATION_BY_MATRIX || 
		realization_type == INCIDENCE_STRUCTURE_REALIZATION_BY_HJELMSLEV) {
		return M[i * nb_cols + j];
	}
	else if (realization_type ==
			INCIDENCE_STRUCTURE_REALIZATION_BY_ORTHOGONAL) {
		long int p1, p2, rk;
		int *v;
		int *base_cols;

		//cout << "incidence_structure::get_ij i=" << i << " j=" << j << endl;
		v = NEW_int(3 * O->n);
		base_cols = NEW_int(O->n);
		//cout << "before O->unrank_point(v, 1, i);" << endl;
		O->Hyperbolic_pair->unrank_point(v, 1, i, 0);
		//cout << "before O->unrank_line(v, p1, p2, j);" << endl;
		O->Hyperbolic_pair->unrank_line(p1, p2, j, 0 /* verbose_level */);
		//cout << "before O->unrank_point(v + 1 * O->n, 1, p1);" << endl;
		O->Hyperbolic_pair->unrank_point(v + 1 * O->n, 1, p1, 0);
		//cout << "before O->unrank_point(v + 2 * O->n, 1, p2);" << endl;
		O->Hyperbolic_pair->unrank_point(v + 2 * O->n, 1, p2, 0);
		rk = O->F->Linear_algebra->Gauss_simple(v, 3, O->n, base_cols, 0/* verbose_level*/);

		FREE_int(v);
		FREE_int(base_cols);

		if (rk == 2) {
			return TRUE;
		}
		if (rk == 3) {
			return FALSE;
		}
		cout << "incidence_structure::get_ij fatal: rk=" << rk << endl;
		exit(1);
	}
	cout << "incidence_structure::get_ij: unknown realization_type";
	exit(1);
}

int incidence_structure::get_lines_on_point(int *data, int i, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int r;

	if (f_v) {
		cout << "incidence_structure::get_lines_on_point" << endl;
	}
	if (i < 0 || i >= nb_rows) {
		cout << "incidence_structure::get_lines_on_point "
				"i=" << i << " is illegal" << endl;
		exit(1);
	}
	if (realization_type == INCIDENCE_STRUCTURE_REALIZATION_BY_MATRIX || 
		realization_type == INCIDENCE_STRUCTURE_REALIZATION_BY_HJELMSLEV) {
		int h;
		for (h = 0; h < nb_lines_on_point[i]; h++) {
			data[h] = lines_on_point[i * max_r + h];
		}
		r = nb_lines_on_point[i];
	}
	else if (realization_type ==
			INCIDENCE_STRUCTURE_REALIZATION_BY_ORTHOGONAL) {
		O->lines_on_point_by_line_rank_must_fit_into_int(i, data, 0/*verbose_level - 2*/);
		r = O->Hyperbolic_pair->alpha;
	}
	else if (realization_type ==
			INCIDENCE_STRUCTURE_REALIZATION_BY_PROJECTIVE_SPACE) {
		long int *Data;
		int h;

		Data = NEW_lint(P->r);
		P->create_lines_on_point(i, Data, verbose_level);
		for (h = 0; h < P->r; h++) {
			data[h] = Data[h];
		}
		FREE_lint(Data);
		r = P->r;
	}
	else {
		cout << "incidence_structure::get_lines_on_point "
			"fatal: unknown realization_type";
		exit(1);
	}
	if (f_v) {
		cout << "incidence_structure::get_lines_on_point pt = " << i << " : ";
		Int_vec_print(cout, data, r);
		cout << endl;
		cout << "incidence_structure::get_lines_on_point done" << endl;
	}
	return r;
}

int incidence_structure::get_points_on_line(int *data, int j, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int k;

	if (f_v) {
		cout << "incidence_structure::get_points_on_line" << endl;
	}
	if (realization_type == INCIDENCE_STRUCTURE_REALIZATION_BY_MATRIX || 
		realization_type == INCIDENCE_STRUCTURE_REALIZATION_BY_HJELMSLEV) {
		int h;
		for (h = 0; h < nb_points_on_line[j]; h++) {
			data[h] = points_on_line[j * max_k + h];
		}
		k = nb_points_on_line[j];
	}
	else if (realization_type ==
			INCIDENCE_STRUCTURE_REALIZATION_BY_ORTHOGONAL) {
		long int *Data;
		int h;

		Data = NEW_lint(nb_points_on_line[j]);
		O->points_on_line_by_line_rank(j, Data, 0/* verbose_level - 2*/);
		for (h = 0; h < nb_points_on_line[j]; h++) {
			data[h] = Data[h];
		}
		FREE_lint(Data);
		k = O->q + 1;
	}
	else if (realization_type ==
			INCIDENCE_STRUCTURE_REALIZATION_BY_PROJECTIVE_SPACE) {
		long int *Data;
		int h;

		Data = NEW_lint(P->k);
		P->create_points_on_line(j, Data, 0 /*verbose_level*/);
		for (h = 0; h < P->k; h++) {
			data[h] = Data[h];
		}
		FREE_lint(Data);
		k = P->k;
	}
	else {
		cout << "incidence_structure::get_points_on_line "
				"fatal: unknown realization_type";
		exit(1);
	}
	if (f_v) {
		cout << "incidence_structure::get_points_on_line "
				"line = " << j << " : ";
		Int_vec_print(cout, data, k);
		cout << endl;
		cout << "incidence_structure::get_points_on_line done" << endl;
	}
	return k;
}

int incidence_structure::get_nb_inc()
{
	if (f_rowsums_constant) {
		return nb_rows * r;
	}
	else {
		int i, j, nb = 0;
		
		for (i = 0; i < nb_rows; i++) {
			for (j = 0; j < nb_cols; j++) {
				if (get_ij(i, j)) {
					nb++;
				}
			}
		}
		return nb;
	}
}

void incidence_structure::save_inc_file(char *fname)
{
	int i, j, nb_inc;

	nb_inc = get_nb_inc();
	
	ofstream f(fname);
	f << nb_rows << " " << nb_cols << " " << nb_inc << endl;
	for (i = 0; i < nb_rows; i++) {
		for (j = 0; j < nb_cols; j++) {
			if (get_ij(i, j)) {
				f << i * nb_cols + j << " ";
			}
		}
	}
	f << "0" << endl; // ago not known
	f << "-1" << endl;
}

void incidence_structure::save_row_by_row_file(char *fname)
{
    int i, j; //, nb_inc;
    int w;
    number_theory::number_theory_domain NT;

	//nb_inc = get_nb_inc();
	
	if (!f_rowsums_constant) {
		cout << "incidence_structure::save_row_by_row_file "
				"rowsums are not constant" << endl;
		exit(1);
	}
	ofstream f(fname);
	w = NT.int_log10(nb_cols) + 1;
	f << nb_rows << " " << nb_cols << " " << r << endl;
	for (i = 0; i < nb_rows; i++) {
		for (j = 0; j < r; j++) {
			f << setw(w) << lines_on_point[i * r + j] << " ";
		}
		f << endl;
	}
	f << "-1" << endl;
}


void incidence_structure::print(ostream &ost)
{
	int i, j;

	if (nb_cols == 0) {
		cout << "incidence_structure::print nb_cols == 0" << endl;
		return;
	}
	for (i = 0; i < nb_rows; i++) {
		for (j = 0; j < nb_cols; j++) {
			if (get_ij(i, j)) {
				ost << "1";
			}
			else {
				ost << ".";
			}
		}
		ost << endl;
	}
}

void incidence_structure::compute_TDO_safe_first(
	data_structures::partitionstack &PStack,
	int depth, int &step, int &f_refine, int &f_refine_prev,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "incidence_structure::compute_TDO_safe_first" << endl;
	}
	step = 0;
	f_refine_prev = TRUE;
}

int incidence_structure::compute_TDO_safe_next(
	data_structures::partitionstack &PStack,
	int depth, int &step, int &f_refine, int &f_refine_prev,
	int verbose_level)
// returns TRUE when we are done, FALSE otherwise
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int f_vvv = (verbose_level >= 3);

	if (f_v) {
		cout << "incidence_structure::compute_TDO_safe_next" << endl;
	}
	if (EVEN(step)) {
		f_refine = refine_column_partition_safe(
				PStack, verbose_level - 3);
	}
	else {
		f_refine = refine_row_partition_safe(
				PStack, verbose_level - 3);
	}

	if (f_vv) {
		cout << "incidence_structure::compute_TDO_safe_next "
				"step=" << step << " after refine" << endl;
	}
	if (f_vvv) {
		if (EVEN(step)) {
			int f_list_incidences = FALSE;
			get_and_print_col_decomposition_scheme(PStack,
					f_list_incidences, FALSE, verbose_level);
			PStack.print_classes_points_and_lines(cout);
		}
		else {
			int f_list_incidences = FALSE;
			get_and_print_row_decomposition_scheme(PStack,
					f_list_incidences, FALSE, verbose_level);
			PStack.print_classes_points_and_lines(cout);
		}
	}
	step++;
	if (!f_refine_prev && !f_refine) {
		return TRUE;
	}
	f_refine_prev = f_refine;
	return FALSE;
}

void incidence_structure::compute_TDO_safe(data_structures::partitionstack &PStack,
	int depth, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	//int f_vv = (verbose_level >= 2);
	//int f_vvv = (verbose_level >= 3);
	int f_refine, f_refine_prev;
	int i;
	
	if (f_v) {
		cout << "incidence_structure::compute_TDO_safe" << endl;
	}
	
	f_refine_prev = TRUE;
	for (i = 0; i < depth; i++) {
		if (f_v) {
			cout << "incidence_structure::compute_TDO_safe i = " << i << endl;
		}


		if (EVEN(i)) {
			if (f_v) {
				cout << "incidence_structure::compute_TDO_safe before refine_column_partition_safe" << endl;
			}
			f_refine = refine_column_partition_safe(PStack, verbose_level - 2);
			if (f_v) {
				cout << "incidence_structure::compute_TDO_safe after refine_column_partition_safe" << endl;
			}
		}
		else {
			if (f_v) {
				cout << "incidence_structure::compute_TDO_safe before refine_row_partition_safe" << endl;
			}
			f_refine = refine_row_partition_safe(PStack, verbose_level - 2);
			if (f_v) {
				cout << "incidence_structure::compute_TDO_safe after refine_row_partition_safe" << endl;
			}
		}

		if (f_v) {
			cout << "incidence_structure::compute_TDO_safe "
					"i=" << i << " after refine" << endl;
			if (EVEN(i)) {
				int f_list_incidences = FALSE;
				get_and_print_col_decomposition_scheme(PStack,
						f_list_incidences, FALSE, verbose_level);
				PStack.print_classes_points_and_lines(cout);
			}
			else {
				int f_list_incidences = FALSE;
				get_and_print_row_decomposition_scheme(PStack,
						f_list_incidences, FALSE, verbose_level);
				PStack.print_classes_points_and_lines(cout);
			}
		}
		
		if (!f_refine_prev && !f_refine) {
			if (f_v) {
				cout << "incidence_structure::compute_TDO_safe "
						"no refinement, we are done" << endl;
			}
			goto done;
		}
		f_refine_prev = f_refine;
	}

done:
	if (f_v) {
		cout << "incidence_structure::compute_TDO_safe done" << endl;
	}
	
}

int incidence_structure::compute_TDO(
	data_structures::partitionstack &PStack, int ht0,
	int depth, int verbose_level)
{
	int h1, h2, ht1, remaining_depth = depth;
	int f_v = (verbose_level >= 1);
	//int f_vv = (verbose_level >= 2);
	//int f_vvv = (verbose_level >= 3);
	data_structures::algorithms Algo;

	ht1 = PStack.ht;
	h2 = 0;
	
	if (f_v) {
		cout << "incidence_structure::compute_TDO "
				"depth=" << depth << " ht=" << PStack.ht << endl;
		if (ht0 >= PStack.ht) {
			cout << "ht0 >= PStack.ht, fatal" << endl;
			exit(1);
		}
	}
	while (remaining_depth) {

		if (f_v) {
			cout << "incidence_structure::compute_TDO "
					"remaining_depth=" << remaining_depth << endl;
		}
		if (remaining_depth) {
			h1 = compute_TDO_step(PStack, ht0, verbose_level);
			h2 = Algo.hashing(h1, h2);
			ht0 = ht1;
			ht1 = PStack.ht;
			remaining_depth--;
		}
		else {
			break;
		}
		if (f_v) {
			cout << "incidence_structure::compute_TDO after "
					"compute_TDO_step ht0=" << ht0 << " ht1=" << ht1 << endl;
		}
		if (ht0 == ht1) {
			break;
		}
	}
	return h2;
}

int incidence_structure::compute_TDO_step(
		data_structures::partitionstack &PStack, int ht0, int verbose_level)
{
	int ht1, h1, h2;
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int f_vvv = (verbose_level >= 3);
	int f_is_row_class;
	data_structures::algorithms Algo;

	ht1 = PStack.ht;
	h2 = 0;
	if (f_v) {
		cout << "incidence_structure::compute_TDO_step "
				"ht=" << PStack.ht << " ht0=" << ht0 << endl;
		cout << "before PStack.is_row_class" << endl;
	}
	f_is_row_class = PStack.is_row_class(ht0);
	if (f_v) {
		cout << "after PStack.is_row_class" << endl;
	}
	if (f_is_row_class) {
		if (f_vv) {
			cout << "incidence_structure::compute_TDO_step before "
					"refine_column_partition ht0=" << ht0
					<< " ht1=" << ht1 << endl;
		}
		h1 = refine_column_partition(PStack, ht0, verbose_level - 3);
		//cout << "h1=" << h1 << endl;
		h2 = Algo.hashing(h1, h2);
		if (f_v) {
			cout << "incidence_structure::compute_TDO_step after "
					"refine_column_partition ht=" << PStack.ht << endl;
		}
		if (f_vv) {
			int f_list_incidences = FALSE;
			get_and_print_col_decomposition_scheme(PStack,
					f_list_incidences, FALSE, verbose_level);
		}
		if (f_vvv) {
			PStack.print_classes_points_and_lines(cout);
		}
	}
	else {
		if (f_vv) {
			cout << "incidence_structure::compute_TDO_step before "
					"refine_column_partition ht0=" << ht0
					<< " ht1=" << ht1 << endl;
		}
		h1 = refine_row_partition(PStack, ht0, verbose_level - 3);
		//cout << "h1=" << h1 << endl;
		h2 = Algo.hashing(h1, h2);
		//cout << "h2=" << h2 << endl;
		if (f_v) {
			cout << "incidence_structure::compute_TDO_step after "
					"refine_row_partition ht=" << PStack.ht << endl;
		}
		if (f_vv) {
			int f_list_incidences = FALSE;
			get_and_print_row_decomposition_scheme(
					PStack, f_list_incidences, FALSE, verbose_level);
		}
		if (f_vvv) {
			PStack.print_classes_points_and_lines(cout);
		}
	}
	return h2;
}


void incidence_structure::get_partition(data_structures::partitionstack &PStack,
	int *row_classes, int *row_class_idx, int &nb_row_classes, 
	int *col_classes, int *col_class_idx, int &nb_col_classes)
{
	int i, c;
	
	PStack.get_row_and_col_classes(
		row_classes, nb_row_classes,
		col_classes, nb_col_classes, 0 /*verbose_level*/);
	for (i = 0; i < PStack.ht; i++) {
		row_class_idx[i] = col_class_idx[i] = -1;
	}
	for (i = 0; i < nb_row_classes; i++) {
		c = row_classes[i];
		row_class_idx[c] = i;
	}
	for (i = 0; i < nb_col_classes; i++) {
		c = col_classes[i];
		col_class_idx[c] = i;
	}
}

int incidence_structure::refine_column_partition_safe(
		data_structures::partitionstack &PStack, int verbose_level)
{

	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 5);
	int i, j, c, h, I, ht, first, next, N;
	int *row_classes;
	int *row_class_idx;
	int nb_row_classes;
	int *col_classes;
	int *col_class_idx;
	int nb_col_classes;

	int *data;
	int *neighbors;

	if (f_v) {
		cout << "incidence_structure::refine_column_partition_safe" << endl;
	}
	if (f_v) {
		cout << "incidence_structure::refine_column_partition_safe PStack.ht" << PStack.ht << endl;
	}
	row_classes = NEW_int(PStack.ht);
	col_classes = NEW_int(PStack.ht);
	row_class_idx = NEW_int(PStack.ht);
	col_class_idx = NEW_int(PStack.ht);

	if (f_v) {
		cout << "incidence_structure::refine_column_partition_safe "
				"before get_partition" << endl;
	}
	get_partition(PStack,
			row_classes, row_class_idx, nb_row_classes,
			col_classes, col_class_idx, nb_col_classes);
	if (f_v) {
		cout << "incidence_structure::refine_column_partition_safe "
				"after get_partition" << endl;
	}
	
	N = nb_points() + nb_lines();
	if (f_v) {
		cout << "incidence_structure::refine_column_partition_safe nb_row_classes= " << nb_row_classes << endl;
	}
	data = NEW_int(N * nb_row_classes);
	Int_vec_zero(data, N * nb_row_classes);
	
	neighbors = NEW_int(max_k);

	for (j = 0; j < nb_lines(); j++) {
		get_points_on_line(neighbors, j, 0 /*verbose_level - 2*/);
		for (h = 0; h < nb_points_on_line[j]; h++) {
			i = neighbors[h];
			c = PStack.cellNumber[PStack.invPointList[i]];
			I = row_class_idx[c];
			if (I == -1) {
				cout << "incidence_structure::refine_column_partition_safe "
						"I == -1" << endl;
				exit(1);
			}
			data[(nb_points() + j) * nb_row_classes + I]++;
		}
	}
	if (f_vv) {
		cout << "incidence_structure::refine_column_partition_safe "
				"data:" << endl;
		Int_vec_print_integer_matrix_width(cout,
			data + nb_points() * nb_row_classes,
			nb_lines(), nb_row_classes, nb_row_classes, 3);
	}

	ht = PStack.ht;
	
	for (c = 0; c < ht; c++) {
		if (PStack.is_row_class(c)) {
			continue;
		}
			
		if (PStack.cellSize[c] == 1) {
			continue;
		}
		first = PStack.startCell[c];
		next = first + PStack.cellSize[c];

		PStack.radix_sort(first /* left */,
				   next - 1 /* right */,
				   data, nb_row_classes, 0 /*radix*/, FALSE);
	}

	FREE_int(data);
	FREE_int(neighbors);
	
	FREE_int(row_classes);
	FREE_int(col_classes);
	FREE_int(row_class_idx);
	FREE_int(col_class_idx);
	if (f_v) {
		cout << "incidence_structure::refine_column_partition_safe done" << endl;
	}
	if (PStack.ht == ht) {
		return FALSE;
	}
	else {
		return TRUE;
	}
}

int incidence_structure::refine_row_partition_safe(
		data_structures::partitionstack &PStack, int verbose_level)
{

	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 5);
	int i, j, c, h, J, ht, first, next;
	int *row_classes;
	int *row_class_idx;
	int nb_row_classes;
	int *col_classes;
	int *col_class_idx;
	int nb_col_classes;

	int *data;
	int *neighbors;

	if (f_v) {
		cout << "incidence_structure::refine_row_partition_safe" << endl;
	}
	row_classes = NEW_int(PStack.ht);
	col_classes = NEW_int(PStack.ht);
	row_class_idx = NEW_int(PStack.ht);
	col_class_idx = NEW_int(PStack.ht);

	if (f_v) {
		cout << "incidence_structure::refine_row_partition_safe before get_partition" << endl;
	}
	get_partition(PStack,
			row_classes, row_class_idx, nb_row_classes,
			col_classes, col_class_idx, nb_col_classes);
	if (f_v) {
		cout << "incidence_structure::refine_row_partition_safe after get_partition" << endl;
	}
	if (f_v) {
		cout << "incidence_structure::refine_row_partition_safe nb_col_classes=" << nb_col_classes << endl;
	}
	
	data = NEW_int(nb_points() * nb_col_classes);
	Int_vec_zero(data, nb_points() * nb_col_classes);

	
	neighbors = NEW_int(max_r);

	for (i = 0; i < nb_points(); i++) {
		get_lines_on_point(neighbors, i, 0 /*verbose_level - 2*/);
		for (h = 0; h < nb_lines_on_point[i]; h++) {
			j = neighbors[h] + nb_points();
			c = PStack.cellNumber[PStack.invPointList[j]];
			J = col_class_idx[c];
			if (J == -1) {
				cout << "incidence_structure::refine_row_partition_safe J == -1" << endl;
				exit(1);
			}
			data[i * nb_col_classes + J]++;
		}
	}
	if (f_vv) {
		cout << "incidence_structure::refine_row_partition_safe data:" << endl;
		Int_vec_print_integer_matrix_width(cout, data, nb_points(),
				nb_col_classes, nb_col_classes, 3);
	}

	ht = PStack.ht;
	
	for (c = 0; c < ht; c++) {
		if (PStack.is_col_class(c)) {
			continue;
		}
			
		if (PStack.cellSize[c] == 1) {
			continue;
		}
		first = PStack.startCell[c];
		next = first + PStack.cellSize[c];

		PStack.radix_sort(first /* left */,
				   next - 1 /* right */,
				   data, nb_col_classes, 0 /*radix*/, FALSE);
	}

	FREE_int(data);
	FREE_int(neighbors);
	
	FREE_int(row_classes);
	FREE_int(col_classes);
	FREE_int(row_class_idx);
	FREE_int(col_class_idx);
	if (f_v) {
		cout << "incidence_structure::refine_row_partition_safe done" << endl;
	}
	if (PStack.ht == ht) {
		return FALSE;
	}
	else {
		return TRUE;
	}
}

int incidence_structure::refine_column_partition(
		data_structures::partitionstack &PStack,
		int ht0, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int f_vvv = (verbose_level >= 3);
	int row_cell, f, l, i, j, x, y, u, c, cell;
	int N, first, next, ht1, depth, idx, nb;
	int *data;
	int *neighbors, h;
	
	N = nb_points() + nb_lines();
	ht1 = PStack.ht;
	depth = ht1 - ht0 + 1;
	if (f_v) {
		cout << "incidence_structure::refine_column_partition "
				"ht0=" << ht0 << " ht=" << PStack.ht
				<< " depth=" << depth << endl;
		cout << "N=" << N << endl;		
		cout << "depth=" << depth << endl;		
		cout << "max_r=" << max_r << endl;		
	}
	data = NEW_int(N * depth);
	Int_vec_zero(data, N * depth);
	
	neighbors = NEW_int(max_r);
	for (y = 0; y < nb_lines(); y++) {
		j = nb_points() + y;
		c = PStack.cellNumber[PStack.invPointList[j]];
		data[j * depth + 0] = c;
	}

	for (row_cell = ht0; row_cell < ht1; row_cell++) {
		idx = row_cell - ht0;
		f = PStack.startCell[row_cell];
		l = PStack.cellSize[row_cell];
		if (f_vvv) {
			cout << "incidence_structure::refine_column_partition "
					"idx=" << idx
				<< " row_cell=" << row_cell 
				<< " f=" << f << " l=" << l << endl;
		}
		if (!PStack.is_row_class(row_cell)) {
			cout << "row_cell is not a row cell" << endl;
			cout << "row_cell=" << row_cell << endl;
			cout << "ht0=" << ht0 << endl;
			cout << "ht1=" << ht1 << endl;
			PStack.print(cout);
			cout << endl;
			exit(1);
		}
	
	
		for (i = 0; i < l; i++) {
			x = PStack.pointList[f + i];
			//if (f_v) {cout << i << " : " << x << " : ";}
			if (f_vv) {
				cout << "before get_lines_on_point" << endl;
			}
			nb = get_lines_on_point(neighbors, x, verbose_level - 2);
			if (f_vv) {
				cout << "after get_lines_on_point nb=" << nb << endl;
			}
			
			//O.lines_on_point_by_line_rank(x,
			//neighbors, 0/*verbose_level - 2*/);
			//if (f_v) {int_vec_print(cout, neighbors,
			//O.alpha);cout << endl;}
			for (u = 0; u < nb; u++) {
				y = neighbors[u];
				j = nb_points() + y;
				data[j * depth + 1 + idx]++;
			}
		}
	}
#if 0
	if (f_vvv) {
		cout << "data:" << endl;
		for (y = 0; y < O.nb_lines; y++) {
			j = O.nb_points + y;
			cout << y << " : " << j << " : ";
			int_vec_print(cout, data + j * depth, depth);
			cout << endl;
		}
		cout << endl;
	}
#endif
		
	ht0 = PStack.ht;
	for (cell = 0; cell < ht0; cell++) {
		if (PStack.is_row_class(cell)) {
			continue;
		}
			
		if (PStack.cellSize[cell] == 1) {
			continue;
		}
		first = PStack.startCell[cell];
		next = first + PStack.cellSize[cell];

		PStack.radix_sort(first /* left */,
				   next - 1 /* right */,
				   data, depth, 0 /*radix*/, FALSE);
	}
	if (f_vv) {
		cout << "incidence_structure::refine_column_partition "
				"after sorting, with " << PStack.ht - ht0
				<< " n e w classes" << endl;
		PStack.print(cout);
		cout << endl;
	}

	if (f_vvv) {
		PStack.print_column_refinement_info(ht0, data, depth);
	}
		
	h = PStack.hash_column_refinement_info(ht0, data, depth, 0);
	
	FREE_int(data);
	FREE_int(neighbors);
	return h;
}

int incidence_structure::refine_row_partition(
		data_structures::partitionstack &PStack,
		int ht0, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int f_vvv = (verbose_level >= 3);
	int col_cell, f, l, i, j, x, y, u, c, cell;
	int N, first, next, ht1, depth, idx, nb;
	int *data;
	int *neighbors, nb_neighbors, h;
	
	N = nb_points() + nb_lines();
	ht1 = PStack.ht;
	depth = ht1 - ht0 + 1;
	if (f_v) {
		cout << "incidence_structure::refine_row_partition "
				"ht0=" << ht0 << " ht=" << PStack.ht
				<< " depth=" << depth << endl;
	}
	data = NEW_int(N * depth);
	Int_vec_zero(data, N * depth);
	
	nb_neighbors = max_k;
	neighbors = NEW_int(nb_neighbors);
	for (x = 0; x < nb_points(); x++) {
		i = x;
		c = PStack.cellNumber[PStack.invPointList[i]];
		data[i * depth + 0] = c;
	}

	for (col_cell = ht0; col_cell < ht1; col_cell++) {
		idx = col_cell - ht0;
		f = PStack.startCell[col_cell];
		l = PStack.cellSize[col_cell];
		if (f_vvv) {
			cout << "incidence_structure::refine_row_partition "
				"idx=" << idx
				<< " col_cell=" << col_cell 
				<< " f=" << f 
				<< " l=" << l << endl;
		}

		if (!PStack.is_col_class(col_cell)) {
			cout << "col_cell is not a col cell" << endl;
			cout << "ht0=" << ht0 << endl;
			cout << "ht1=" << ht1 << endl;
			PStack.print(cout);
			cout << endl;
			exit(1);
		}
	
	
	
		for (j = 0; j < l; j++) {
			y = PStack.pointList[f + j];
			//if (f_v) {cout << j << " : " << y << " : ";}
			
			//O.points_on_line_by_line_rank(y - O.nb_points,
			//neighbors, 0/* verbose_level - 2*/);
			nb = get_points_on_line(neighbors, y - nb_points(), verbose_level - 2);
	
			//if (f_v) {int_vec_print(cout, neighbors,
			//O.alpha);cout << endl;}
			for (u = 0; u < nb; u++) {
				x = neighbors[u];
				i = x;
				data[i * depth + 1 + idx]++;
			}
		}
	}
#if 0
	if (f_vvv) {
		cout << "data:" << endl;
		for (i = 0; i < O.nb_lines; i++) {
			cout << i << " : ";
			int_vec_print(cout, data + i * depth, depth);
			cout << endl;
		}
		cout << endl;
	}
#endif
		
	ht0 = PStack.ht;
	for (cell = 0; cell < ht0; cell++) {
		if (PStack.is_col_class(cell)) {
			continue;
		}
			
		if (PStack.cellSize[cell] == 1) {
			continue;
		}
		first = PStack.startCell[cell];
		next = first + PStack.cellSize[cell];

		PStack.radix_sort(first /* left */,
				   next - 1 /* right */,
				   data, depth, 0 /*radix*/, FALSE);
	}
	if (f_vv) {
		cout << "incidence_structure::refine_row_partition "
				"after sorting, with " << PStack.ht - ht0
				<< " n e w classes" << endl;
		PStack.print(cout);
		cout << endl;
	}

	if (f_vv) {
		PStack.print_row_refinement_info(ht0, data, depth);
	}

	h = PStack.hash_row_refinement_info(ht0, data, depth, 0);

	
	FREE_int(data);
	FREE_int(neighbors);
	return h;
}

void incidence_structure::print_row_tactical_decomposition_scheme_incidences_tex(
		data_structures::partitionstack &PStack,
	ostream &ost, int f_enter_math_mode, 
	int *row_classes, int *row_class_inv, int nb_row_classes,
	int *col_classes, int *col_class_inv, int nb_col_classes, 
	int f_local_coordinates, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int *incidences;
	int c1, c2, f1, f2, l1; //, l2;
	int i, j, rij;
	int u, v, x, a, b, c, J;
	int *row_scheme;
	int *S;
	data_structures::sorting Sorting;

	if (f_v) {
		cout << "incidence_structure::print_row_tactical_decomposition_scheme_incidences_tex" << endl;
	}

	row_scheme = NEW_int(nb_row_classes * nb_col_classes);
	
	get_row_decomposition_scheme(PStack, 
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes,
		row_scheme, verbose_level - 2);

	
	for (i = 0; i < nb_row_classes; i++) {
		c1 = row_classes[i];
		f1 = PStack.startCell[c1];
		l1 = PStack.cellSize[c1];
		
		for (j = 0; j < nb_col_classes; j++) {

			rij = row_scheme[i * nb_col_classes + j];
			
			if (rij == 0) {
				continue;
			}
			
			S = NEW_int(rij);
			get_incidences_by_row_scheme(PStack, 
				row_classes, row_class_inv, nb_row_classes,
				col_classes, col_class_inv, nb_col_classes, 
				i, j, 
				rij, incidences, verbose_level - 2);

			c2 = col_classes[j];
			f2 = PStack.startCell[c2];
			//l2 = PStack.cellSize[c2];

			ost << "\\subsubsection*{Row class " << i
					<< " (cell " << c1 << ") vs. col class "
					<< j << " (cell " << c2 << ")";
			if (f_local_coordinates) {
				ost << " (in local coordinates)";
			}
			ost << ", $r_{" << i << ", " << j << "}=" << rij << "$}" << endl;
			//ost << "f1=" << f1 << " l1=" << l1 << endl;
			//ost << "f2=" << f2 << " l2=" << l2 << endl;
			for (u = 0; u < l1; u++) {
				x = PStack.pointList[f1 + u];
				ost << setw(4) << u << " : $P_{" << setw(4) << x
						<< "}$ is incident with ";
				for (v = 0; v < rij; v++) {
					a = incidences[u * rij + v];
					if (f_local_coordinates) {
						b = nb_points() + a;
						c = PStack.cellNumber[PStack.invPointList[b]];
						J = col_class_inv[c];
						if (J != j) {
							cout << "incidence_structure::print_row_"
									"tactical_decomposition_scheme_"
									"incidences_tex J != j" << endl;
							cout << "j=" << j << endl;
							cout << "J=" << J << endl;
						}
						a = PStack.invPointList[b] - f2;
					}
					S[v] = a;
					//ost << a << " ";
				}
				Sorting.int_vec_heapsort(S, rij);
				ost << "$\\{";
				for (v = 0; v < rij; v++) {
					ost << "\\ell_{" << setw(4) << S[v] << "}";
					if (v < rij - 1) {
						ost << ", ";
					}
				}
				ost << "\\}$";
				
				ost << "\\\\" << endl;
			}
			
			FREE_int(incidences);
			FREE_int(S);
		} // next j
	} // next i

	FREE_int(row_scheme);
}

void incidence_structure::print_col_tactical_decomposition_scheme_incidences_tex(
		data_structures::partitionstack &PStack,
	ostream &ost, int f_enter_math_mode, 
	int *row_classes, int *row_class_inv, int nb_row_classes,
	int *col_classes, int *col_class_inv, int nb_col_classes, 
	int f_local_coordinates, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int *incidences;
	int c1, c2, f1, f2, /*l1,*/ l2;
	int i, j, kij;
	int u, v, y, a, b, c, I;
	int *col_scheme;
	int *S;
	data_structures::sorting Sorting;

	if (f_v) {
		cout << "incidence_structure::print_col_tactical_decomposition_scheme_incidences_tex" << endl;
	}

	col_scheme = NEW_int(nb_row_classes * nb_col_classes);
	
	get_col_decomposition_scheme(PStack, 
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes,
		col_scheme, verbose_level - 2);

	
	for (i = 0; i < nb_row_classes; i++) {
		c1 = row_classes[i];
		f1 = PStack.startCell[c1];
		//l1 = PStack.cellSize[c1];
		
		for (j = 0; j < nb_col_classes; j++) {

			kij = col_scheme[i * nb_col_classes + j];
			
			if (kij == 0) {
				continue;
			}
			
			S = NEW_int(kij);
			get_incidences_by_col_scheme(PStack, 
				row_classes, row_class_inv, nb_row_classes,
				col_classes, col_class_inv, nb_col_classes, 
				i, j, 
				kij, incidences, verbose_level - 2);

			c2 = col_classes[j];
			f2 = PStack.startCell[c2];
			l2 = PStack.cellSize[c2];

			ost << "\\subsubsection*{Row class " << i
					<< " (cell " << c1 << ") vs. col class "
					<< j << " (cell " << c2 << ")";
			if (f_local_coordinates) {
				ost << " (in local coordinates)";
			}
			ost << ", $k_{" << i << ", " << j << "}="
					<< kij << "$}" << endl;
			//ost << "f1=" << f1 << " l1=" << l1 << endl;
			//ost << "f2=" << f2 << " l2=" << l2 << endl;
			for (u = 0; u < l2; u++) {
				y = PStack.pointList[f2 + u] - nb_points();
				ost << setw(4) << u << " : $\\ell_{" << setw(4)
						<< y << "}$ is incident with ";
				for (v = 0; v < kij; v++) {
					a = incidences[u * kij + v];
					if (f_local_coordinates) {
						b = a;
						c = PStack.cellNumber[PStack.invPointList[b]];
						I = row_class_inv[c];
						if (I != i) {
							cout << "incidence_structure::print_col_"
									"tactical_decomposition_scheme_"
									"incidences_tex I != i" << endl;
							cout << "i=" << i << endl;
							cout << "I=" << I << endl;
						}
						a = PStack.invPointList[b] - f1;
					}
					S[v] = a;
					//ost << a << " ";
				}
				Sorting.int_vec_heapsort(S, kij);
				ost << "$\\{";
				for (v = 0; v < kij; v++) {
					ost << "P_{" << setw(4) << S[v] << "}";
					if (v < kij - 1) {
						ost << ", ";
					}
				}
				ost << "\\}$";
				
				ost << "\\\\" << endl;
			}
			
			FREE_int(incidences);
			FREE_int(S);
		} // next j
	} // next i

	FREE_int(col_scheme);
}

void incidence_structure::get_incidences_by_row_scheme(
		data_structures::partitionstack &PStack,
	int *row_classes, int *row_class_inv, int nb_row_classes,
	int *col_classes, int *col_class_inv, int nb_col_classes, 
	int row_class_idx, int col_class_idx, 
	int rij, int *&incidences, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int c1, c2, f1, /*f2,*/ l1, /*l2,*/ x, nb, u, y, c, i, j;
	int *sz;
	int *neighbors;
	
	if (f_v) {
		cout << "incidence_structure::get_incidences_by_row_scheme" << endl;
	}
	c1 = row_classes[row_class_idx];
	f1 = PStack.startCell[c1];
	l1 = PStack.cellSize[c1];
	c2 = col_classes[col_class_idx];
	//f2 = PStack.startCell[c2];
	//l2 = PStack.cellSize[c2];
	incidences = NEW_int(l1 * rij);
	neighbors = NEW_int(max_r);
	sz = NEW_int(l1);
	for (i = 0; i < l1; i++) {
		sz[i] = 0;
	}
	for (i = 0; i < l1; i++) {
		x = PStack.pointList[f1 + i];
		nb = get_lines_on_point(neighbors, x, verbose_level - 2);
		//O.lines_on_point_by_line_rank(x, neighbors, verbose_level - 2);
		for (u = 0; u < nb; u++) {
			y = neighbors[u];
			j = nb_points() + y;
			c = PStack.cellNumber[PStack.invPointList[j]];
			if (c == c2) {
				incidences[i * rij + sz[i]++] = y;
			}
		}
	} // next i
	
	for (i = 0; i < l1; i++) {
		if (sz[i] != rij) {
			cout << "sz[i] != rij" << endl;
			exit(1);
		}
	}

	FREE_int(sz);
	FREE_int(neighbors);
}

void incidence_structure::get_incidences_by_col_scheme(
	data_structures::partitionstack &PStack,
	int *row_classes, int *row_class_inv, int nb_row_classes,
	int *col_classes, int *col_class_inv, int nb_col_classes, 
	int row_class_idx, int col_class_idx, 
	int kij, int *&incidences, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int c1, c2, /*f1,*/ f2, /*l1,*/ l2, x, nb, u, y, c, i, j;
	int *sz;
	int *neighbors;
	
	if (f_v) {
		cout << "incidence_structure::get_incidences_by_col_scheme" << endl;
	}
	c1 = row_classes[row_class_idx];
	//f1 = PStack.startCell[c1];
	//l1 = PStack.cellSize[c1];
	c2 = col_classes[col_class_idx];
	f2 = PStack.startCell[c2];
	l2 = PStack.cellSize[c2];
	incidences = NEW_int(l2 * kij);
	neighbors = NEW_int(max_k);
	sz = NEW_int(l2);
	for (j = 0; j < l2; j++) {
		sz[j] = 0;
	}
	for (j = 0; j < l2; j++) {
		y = PStack.pointList[f2 + j] - nb_points();
		nb = get_points_on_line(neighbors, y, verbose_level - 2);
		for (u = 0; u < nb; u++) {
			x = neighbors[u];
			i = x;
			c = PStack.cellNumber[PStack.invPointList[i]];
			if (c == c1) {
				incidences[j * kij + sz[j]++] = x;
			}
		}
	} // next j
	
	for (j = 0; j < l2; j++) {
		if (sz[j] != kij) {
			cout << "sz[j] != kij" << endl;
			exit(1);
		}
	}

	FREE_int(sz);
	FREE_int(neighbors);
}


void incidence_structure::get_row_decomposition_scheme(
		data_structures::partitionstack &PStack,
	int *row_classes, int *row_class_inv, int nb_row_classes,
	int *col_classes, int *col_class_inv, int nb_col_classes, 
	int *row_scheme, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int I, J, i, j, c1, f1, l1, x, y, u, c, nb;
	int *neighbors;
	int *data0;
	int *data1;
	
	if (f_v) {
		cout << "incidence_structure::get_row_decomposition_scheme" << endl;
	}
	neighbors = NEW_int(max_r);
	data0 = NEW_int(nb_col_classes);
	data1 = NEW_int(nb_col_classes);
	Int_vec_zero(row_scheme, nb_row_classes * nb_col_classes);
	for (I = 0; I < nb_row_classes; I++) {
		c1 = row_classes[I];
		f1 = PStack.startCell[c1];
		l1 = PStack.cellSize[c1];
		Int_vec_zero(data0, nb_col_classes);
		for (i = 0; i < l1; i++) {
			x = PStack.pointList[f1 + i];
			Int_vec_zero(data1, nb_col_classes);
			nb = get_lines_on_point(neighbors, x, verbose_level - 2);
			for (u = 0; u < nb; u++) {
				y = neighbors[u];
				j = nb_points() + y;
				c = PStack.cellNumber[PStack.invPointList[j]];
				J = col_class_inv[c];
				data1[J]++;
			}
			if (i == 0) {
				Int_vec_copy(data1, data0, nb_col_classes);
			}
			else {
				for (J = 0; J < nb_col_classes; J++) {
					if (data0[J] != data1[J]) {
						cout << "incidence_structure::get_row_"
								"decomposition_scheme not row-tactical "
								"I=" << I << " i=" << i
								<< " J=" << J << endl;
					}
				}
			}
		} // next i

		Int_vec_copy(data0,
				row_scheme + I * nb_col_classes,
				nb_col_classes);
	}
	FREE_int(neighbors);
	FREE_int(data0);
	FREE_int(data1);
	if (f_v) {
		cout << "incidence_structure::get_row_decomposition_scheme done" << endl;
	}
}

void incidence_structure::get_row_decomposition_scheme_if_possible(
		data_structures::partitionstack &PStack,
	int *row_classes, int *row_class_inv, int nb_row_classes,
	int *col_classes, int *col_class_inv, int nb_col_classes, 
	int *row_scheme, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int I, J, i, j, c1, f1, l1, x, y, u, c, nb;
	int *neighbors;
	int *data0;
	int *data1;
	
	if (f_v) {
		cout << "incidence_structure::get_row_decomposition_scheme_if_possible" << endl;
	}
	neighbors = NEW_int(max_r);
	data0 = NEW_int(nb_col_classes);
	data1 = NEW_int(nb_col_classes);
	for (i = 0; i < nb_row_classes * nb_col_classes; i++) {
		row_scheme[i] = 0;
	}
	for (I = 0; I < nb_row_classes; I++) {
		c1 = row_classes[I];
		f1 = PStack.startCell[c1];
		l1 = PStack.cellSize[c1];
		for (j = 0; j < nb_col_classes; j++) {
			data0[j] = 0;
		}
		for (i = 0; i < l1; i++) {
			x = PStack.pointList[f1 + i];
			for (J = 0; J < nb_col_classes; J++) {
				data1[J] = 0;
			}
			nb = get_lines_on_point(neighbors, x, verbose_level - 2);
			//O.lines_on_point_by_line_rank(x, neighbors,
			//verbose_level - 2);
			for (u = 0; u < nb; u++) {
				y = neighbors[u];
				j = nb_points() + y;
				c = PStack.cellNumber[PStack.invPointList[j]];
				J = col_class_inv[c];
				data1[J]++;
			}
			if (i == 0) {
				for (J = 0; J < nb_col_classes; J++) {
					data0[J] = data1[J];
				}
			}
			else {
				for (J = 0; J < nb_col_classes; J++) {
					if (data0[J] != data1[J]) {
						data0[J] = -1;
						//cout << "not row-tactical I=" << I
						//<< " i=" << i << " J=" << J << endl;
					}
				}
			}
		} // next i
		for (J = 0; J < nb_col_classes; J++) {
			row_scheme[I * nb_col_classes + J] = data0[J];
		}
	}
	FREE_int(neighbors);
	FREE_int(data0);
	FREE_int(data1);
}

void incidence_structure::get_col_decomposition_scheme(
		data_structures::partitionstack &PStack,
	int *row_classes, int *row_class_inv, int nb_row_classes,
	int *col_classes, int *col_class_inv, int nb_col_classes, 
	int *col_scheme, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int I, J, i, j, c1, f1, l1, x, y, u, c, nb;
	int *neighbors;
	int *data0;
	int *data1;
	
	if (f_v) {
		cout << "incidence_structure::get_col_decomposition_scheme" << endl;
	}
	neighbors = NEW_int(max_k);
	data0 = NEW_int(nb_row_classes);
	data1 = NEW_int(nb_row_classes);
	for (i = 0; i < nb_row_classes * nb_col_classes; i++) {
		col_scheme[i] = 0;
	}
	for (J = 0; J < nb_col_classes; J++) {
		c1 = col_classes[J];
		f1 = PStack.startCell[c1];
		l1 = PStack.cellSize[c1];
		for (i = 0; i < nb_row_classes; i++) {
			data0[i] = 0;
		}
		for (j = 0; j < l1; j++) {
			y = PStack.pointList[f1 + j] - nb_points();
			for (I = 0; I < nb_row_classes; I++) {
				data1[I] = 0;
			}
			
			//O.points_on_line_by_line_rank(y, neighbors,
			//verbose_level - 2);
			nb = get_points_on_line(neighbors, y, verbose_level - 2);
	
			for (u = 0; u < nb; u++) {
				x = neighbors[u];
				c = PStack.cellNumber[PStack.invPointList[x]];
				I = row_class_inv[c];
				data1[I]++;
			}
			if (j == 0) {
				for (I = 0; I < nb_row_classes; I++) {
					data0[I] = data1[I];
				}
			}
			else {
				for (I = 0; I < nb_row_classes; I++) {
					if (data0[I] != data1[I]) {
						cout << "not col-tactical J=" << J
								<< " j=" << j << " I=" << I << endl;
					}
				}
			}
		} // next j
		for (I = 0; I < nb_row_classes; I++) {
			col_scheme[I * nb_col_classes + J] = data0[I];
		}
	}
	FREE_int(neighbors);
	FREE_int(data0);
	FREE_int(data1);
}

void incidence_structure::row_scheme_to_col_scheme(
		data_structures::partitionstack &PStack,
	int *row_classes, int *row_class_inv, int nb_row_classes,
	int *col_classes, int *col_class_inv, int nb_col_classes, 
	int *row_scheme, int *col_scheme, int verbose_level)
{
	int I, J, c1, l1, c2, l2, a, b, c;
	
	for (I = 0; I < nb_row_classes; I++) {
		c1 = row_classes[I];
		l1 = PStack.cellSize[c1];
		for (J = 0; J < nb_col_classes; J++) {
			c2 = col_classes[J];
			l2 = PStack.cellSize[c2];
			a = row_scheme[I * nb_col_classes + J];
			b = a * l1;
			if (b % l2) {
				cout << "incidence_structure::row_scheme_to_col_scheme: "
						"cannot be tactical" << endl;
				exit(1);
			}
			c = b / l2;
			col_scheme[I * nb_col_classes + J] = c;
		}
	}
}

void incidence_structure::get_and_print_row_decomposition_scheme(
		data_structures::partitionstack &PStack,
	int f_list_incidences, int f_local_coordinates, int verbose_level)
{
	int *row_classes, *row_class_inv, nb_row_classes;
	int *col_classes, *col_class_inv, nb_col_classes;
	int *row_scheme;
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "incidence_structure::get_and_print_row_decomposition_scheme "
				"computing col scheme" << endl;
	}
	PStack.allocate_and_get_decomposition(
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		0);
	
	row_scheme = NEW_int(nb_row_classes * nb_col_classes);

	get_row_decomposition_scheme(PStack, 
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		row_scheme, 0);

	//cout << *this << endl;
	
	if (f_v) {
		cout << "row_scheme:" << endl;
		PStack.print_decomposition_scheme(cout,
			row_classes, nb_row_classes,
			col_classes, nb_col_classes,
			row_scheme, -1, -1);
	}

	if (f_list_incidences) {
		cout << "incidences by row-scheme:" << endl;
		print_row_tactical_decomposition_scheme_incidences_tex(
			PStack, 
			cout, FALSE /* f_enter_math_mode */, 
			row_classes, row_class_inv, nb_row_classes,
			col_classes, col_class_inv, nb_col_classes, 
			f_local_coordinates, 0 /*verbose_level*/);
	}

	FREE_int(row_classes);
	FREE_int(row_class_inv);
	FREE_int(col_classes);
	FREE_int(col_class_inv);
	FREE_int(row_scheme);
}

void incidence_structure::get_and_print_col_decomposition_scheme(
		data_structures::partitionstack &PStack,
	int f_list_incidences, int f_local_coordinates, int verbose_level)
{
	int *row_classes, *row_class_inv, nb_row_classes;
	int *col_classes, *col_class_inv, nb_col_classes;
	int *col_scheme;
	int f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "incidence_structure::get_and_print_col_decomposition_scheme "
				"computing col scheme" << endl;
	}
	PStack.allocate_and_get_decomposition(
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		verbose_level);
	
	col_scheme = NEW_int(nb_row_classes * nb_col_classes);

	get_col_decomposition_scheme(PStack, 
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		col_scheme, 0 /*verbose_level*/);

	//cout << *this << endl;
	
	if (f_v) {
		cout << "col_scheme:" << endl;
		PStack.print_decomposition_scheme(cout, 
			row_classes, nb_row_classes,
			col_classes, nb_col_classes, 
			col_scheme, -1, -1);
	}

	if (f_list_incidences) {
		cout << "incidences by col-scheme:" << endl;
		print_col_tactical_decomposition_scheme_incidences_tex(
			PStack, 
			cout, FALSE /* f_enter_math_mode */, 
			row_classes, row_class_inv, nb_row_classes,
			col_classes, col_class_inv, nb_col_classes, 
			f_local_coordinates, 0 /*verbose_level*/);
	}

	FREE_int(row_classes);
	FREE_int(row_class_inv);
	FREE_int(col_classes);
	FREE_int(col_class_inv);
	FREE_int(col_scheme);
}

void incidence_structure::get_and_print_decomposition_schemes(
		data_structures::partitionstack &PStack)
{
	int *row_classes, *row_class_inv, nb_row_classes;
	int *col_classes, *col_class_inv, nb_col_classes;
	int *row_scheme, *col_scheme;
	int verbose_level = 0;
	int f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "incidence_structure::get_and_print_decomposition_schemes "
				"computing both schemes" << endl;
	}
	PStack.allocate_and_get_decomposition(
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		verbose_level);
	
	row_scheme = NEW_int(nb_row_classes * nb_col_classes);
	col_scheme = NEW_int(nb_row_classes * nb_col_classes);

	get_row_decomposition_scheme(PStack, 
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		row_scheme, verbose_level);

	row_scheme_to_col_scheme(PStack, 
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		row_scheme, col_scheme, verbose_level);
	
	//cout << *this << endl;
	
	cout << "row_scheme:" << endl;
	PStack.print_decomposition_scheme(cout, 
		row_classes, nb_row_classes,
		col_classes, nb_col_classes, 
		row_scheme, -1, -1);

	cout << "col_scheme:" << endl;
	PStack.print_decomposition_scheme(cout, 
		row_classes, nb_row_classes,
		col_classes, nb_col_classes, 
		col_scheme, -1, -1);

	FREE_int(row_classes);
	FREE_int(row_class_inv);
	FREE_int(col_classes);
	FREE_int(col_class_inv);
	FREE_int(row_scheme);
	FREE_int(col_scheme);
}

void incidence_structure::get_and_print_decomposition_schemes_tex(
		data_structures::partitionstack &PStack)
{
	int *row_classes, *row_class_inv, nb_row_classes;
	int *col_classes, *col_class_inv, nb_col_classes;
	int *row_scheme, *col_scheme;
	int verbose_level = 0;
	int f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "incidence_structure::get_and_print_decomposition_schemes_tex "
				"computing both schemes" << endl;
	}
	PStack.allocate_and_get_decomposition(
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		verbose_level);
	
	row_scheme = NEW_int(nb_row_classes * nb_col_classes);
	col_scheme = NEW_int(nb_row_classes * nb_col_classes);

	get_row_decomposition_scheme(PStack, 
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		row_scheme, verbose_level);

	row_scheme_to_col_scheme(PStack, 
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		row_scheme, col_scheme, verbose_level);
	
	//cout << *this << endl;
	
	cout << "row_scheme:" << endl;
	PStack.print_decomposition_scheme_tex(cout, 
		row_classes, nb_row_classes,
		col_classes, nb_col_classes, 
		row_scheme);

	cout << "col_scheme:" << endl;
	PStack.print_decomposition_scheme_tex(cout, 
		row_classes, nb_row_classes,
		col_classes, nb_col_classes, 
		col_scheme);
	cout << "tactical scheme:" << endl;
	PStack.print_tactical_decomposition_scheme_tex(cout, 
		row_classes, nb_row_classes,
		col_classes, nb_col_classes, 
		row_scheme, col_scheme, FALSE /* f_print_subscripts */);

	FREE_int(row_classes);
	FREE_int(row_class_inv);
	FREE_int(col_classes);
	FREE_int(col_class_inv);
	FREE_int(row_scheme);
	FREE_int(col_scheme);
}

void incidence_structure::get_and_print_tactical_decomposition_scheme_tex(
	std::ostream &ost, int f_enter_math, data_structures::partitionstack &PStack)
{
	int *row_classes, *row_class_inv, nb_row_classes;
	int *col_classes, *col_class_inv, nb_col_classes;
	int *row_scheme, *col_scheme;
	int verbose_level = 0;
	int f_v = (verbose_level >= 1);
	
	if (f_v) {
	cout << "incidence_structure::get_and_print_tactical_decomposition_scheme_tex "
			"computing both schemes" << endl;
	}
	PStack.allocate_and_get_decomposition(
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		verbose_level);
	
	row_scheme = NEW_int(nb_row_classes * nb_col_classes);
	col_scheme = NEW_int(nb_row_classes * nb_col_classes);

	get_row_decomposition_scheme(PStack, 
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		row_scheme, verbose_level);

	row_scheme_to_col_scheme(PStack, 
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		row_scheme, col_scheme, verbose_level);
	
	PStack.print_tactical_decomposition_scheme_tex_internal(
		ost, f_enter_math,
		row_classes, nb_row_classes,
		col_classes, nb_col_classes, 
		row_scheme, col_scheme, FALSE /* f_print_subscripts */);

	FREE_int(row_classes);
	FREE_int(row_class_inv);
	FREE_int(col_classes);
	FREE_int(col_class_inv);
	FREE_int(row_scheme);
	FREE_int(col_scheme);
}

void incidence_structure::get_scheme(
	int *&row_classes, int *&row_class_inv, int &nb_row_classes,
	int *&col_classes, int *&col_class_inv, int &nb_col_classes,
	int *&scheme, int f_row_scheme, data_structures::partitionstack &PStack)
{
	int verbose_level = 0;

	PStack.allocate_and_get_decomposition(
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		verbose_level);
	
	scheme = NEW_int(nb_row_classes * nb_col_classes);
	if (f_row_scheme) {
		get_row_decomposition_scheme(PStack, 
			row_classes, row_class_inv, nb_row_classes,
			col_classes, col_class_inv, nb_col_classes, 
			scheme, verbose_level);
	}
	else {
		get_col_decomposition_scheme(PStack, 
			row_classes, row_class_inv, nb_row_classes,
			col_classes, col_class_inv, nb_col_classes, 
			scheme, verbose_level);
	}
}

void incidence_structure::free_scheme(
	int *row_classes, int *row_class_inv, 
	int *col_classes, int *col_class_inv, 
	int *scheme)
{
	FREE_int(row_classes);
	FREE_int(row_class_inv);
	FREE_int(col_classes);
	FREE_int(col_class_inv);
	FREE_int(scheme);
}

void incidence_structure::get_and_print_row_tactical_decomposition_scheme_tex(
	std::ostream &ost, int f_enter_math, int f_print_subscripts,
	data_structures::partitionstack &PStack)
{
	int *row_classes, *row_class_inv, nb_row_classes;
	int *col_classes, *col_class_inv, nb_col_classes;
	int *row_scheme; //, *col_scheme;
	int verbose_level = 0;
	int f_v = FALSE;
	
	if (f_v) {
		cout << "incidence_structure::get_and_print_row_tactical_"
				"decomposition_scheme_tex computing row scheme" << endl;
	}
	PStack.allocate_and_get_decomposition(
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		verbose_level);
	
	row_scheme = NEW_int(nb_row_classes * nb_col_classes);
	//col_scheme = NEW_int(nb_row_classes * nb_col_classes);

	if (f_v) {
		cout << "incidence_structure::get_and_print_row_tactical_"
				"decomposition_scheme_tex before get_row_"
				"decomposition_scheme" << endl;
	}
	get_row_decomposition_scheme(PStack, 
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		row_scheme, verbose_level);

	
	if (f_v) {
		cout << "incidence_structure::get_and_print_row_tactical_"
				"decomposition_scheme_tex before PStack.print_row_"
				"tactical_decomposition_scheme_tex" << endl;
	}
	PStack.print_row_tactical_decomposition_scheme_tex(ost, f_enter_math, 
		row_classes, nb_row_classes,
		col_classes, nb_col_classes, 
		row_scheme, f_print_subscripts);

	FREE_int(row_classes);
	FREE_int(row_class_inv);
	FREE_int(col_classes);
	FREE_int(col_class_inv);
	FREE_int(row_scheme);
	//FREE_int(col_scheme);
}

void incidence_structure::get_and_print_column_tactical_decomposition_scheme_tex(
	std::ostream &ost, int f_enter_math, int f_print_subscripts,
	data_structures::partitionstack &PStack)
{
	int *row_classes, *row_class_inv, nb_row_classes;
	int *col_classes, *col_class_inv, nb_col_classes;
	int *col_scheme;
	int verbose_level = 0;
	int f_v = FALSE;
	
	if (f_v) {
		cout << "incidence_structure::get_and_print_column_"
				"tactical_decomposition_scheme_tex computing "
				"column scheme" << endl;
	}
	PStack.allocate_and_get_decomposition(
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		verbose_level);
	
	//row_scheme = NEW_int(nb_row_classes * nb_col_classes);
	col_scheme = NEW_int(nb_row_classes * nb_col_classes);

	get_col_decomposition_scheme(PStack, 
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		col_scheme, verbose_level);

	
	PStack.print_column_tactical_decomposition_scheme_tex(
		ost, f_enter_math,
		row_classes, nb_row_classes,
		col_classes, nb_col_classes, 
		col_scheme, f_print_subscripts);

	FREE_int(row_classes);
	FREE_int(row_class_inv);
	FREE_int(col_classes);
	FREE_int(col_class_inv);
	//FREE_int(row_scheme);
	FREE_int(col_scheme);
}

void incidence_structure::print_non_tactical_decomposition_scheme_tex(
	std::ostream &ost, int f_enter_math, data_structures::partitionstack &PStack)
{
	int *row_classes, *row_class_inv, nb_row_classes;
	int *col_classes, *col_class_inv, nb_col_classes;
	int *row_scheme;
	int verbose_level = 0;
	int f_v = FALSE;
	
	if (f_v) {
		cout << "incidence_structure::print_non_tactical_decomposition_scheme_tex" << endl;
	}
	PStack.allocate_and_get_decomposition(
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		verbose_level);
	
	row_scheme = NEW_int(nb_row_classes * nb_col_classes);

	get_row_decomposition_scheme_if_possible(PStack, 
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		row_scheme, verbose_level);

	
	PStack.print_row_tactical_decomposition_scheme_tex(ost, f_enter_math, 
		row_classes, nb_row_classes,
		col_classes, nb_col_classes, 
		row_scheme, FALSE /* f_print_subscripts */);

	FREE_int(row_classes);
	FREE_int(row_class_inv);
	FREE_int(col_classes);
	FREE_int(col_class_inv);
	FREE_int(row_scheme);
}

void incidence_structure::print_line(
	std::ostream &ost, data_structures::partitionstack &P,
	int row_cell, int i, int *col_classes, int nb_col_classes, 
	int width, int f_labeled)
{
	int f1, f2, e1, e2;
	int J, j, h, l, cell;
	int first_column_element = P.startCell[1];
	
	f1 = P.startCell[row_cell];
	e1 = P.pointList[f1 + i];
	if (f_labeled) {
		ost << setw((int) width) << e1;
	}
	for (J = 0; J <= nb_col_classes; J++) {
		ost << "|";
		if (J == nb_col_classes)
			break;
		cell = col_classes[J];
		f2 = P.startCell[cell];
		l = P.cellSize[cell];
		for (j = 0; j < l; j++) {
			e2 = P.pointList[f2 + j] - first_column_element;
			if (get_ij(e1, e2)) {
				if (f_labeled) {
					ost << setw((int) width) << e1 * nb_lines() + e2;
				}
				else {
					ost << "X";
					//ost << "1";
				}
			}
			else {
				if (f_labeled) {
					for (h = 0; h < width - 1; h++) {
						ost << " ";
					}
					ost << ".";
				}
				else {
					ost << ".";
					//ost << "0";
				}
			}
		}
	}
	//ost << endl;
}

void incidence_structure::print_column_labels(
	std::ostream &ost, data_structures::partitionstack &P,
	int *col_classes, int nb_col_classes, int width)
{
	int f2, e2;
	int J, j, h, l, cell;
	int first_column_element = P.startCell[1];
	
	for (h = 0; h < width; h++) {
		ost << " ";
	}
	for (J = 0; J <= nb_col_classes; J++) {
		ost << "|";
		if (J == nb_col_classes) {
			break;
		}
		cell = col_classes[J];
		f2 = P.startCell[cell];
		l = P.cellSize[cell];
		for (j = 0; j < l; j++) {
			e2 = P.pointList[f2 + j] - first_column_element;
			ost << setw((int) width) << e2 + first_column_element;
		}
	}
	ost << endl;
}

void incidence_structure::print_hline(
	std::ostream &ost, data_structures::partitionstack &P,
	int *col_classes, int nb_col_classes, int width, int f_labeled)
{
	int J, j, h, l, cell;
	
	if (f_labeled) {
		for (h = 0; h < width; h++) {
			ost << "-";
		}
	}
	else {
		//ost << "-";
	}
	for (J = 0; J <= nb_col_classes; J++) {
		ost << "+";
		if (J == nb_col_classes) {
			break;
		}
		cell = col_classes[J];
		l = P.cellSize[cell];
		for (j = 0; j < l; j++) {
			if (f_labeled) {
				for (h = 0; h < width; h++) {
					ost << "-";
				}
			}
			else {
				ost << "-";
			}
		}
	}
	ost << endl;
}

void incidence_structure::print_partitioned(
	std::ostream &ost, data_structures::partitionstack &P, int f_labeled)
{
	//int *A;
	int *row_classes;
	int nb_row_classes;
	int *col_classes;
	int nb_col_classes;
	int I, i, cell, l;
	int width;
	int mn;
	number_theory::number_theory_domain NT;
	
	mn = nb_points() * nb_lines();
	
	width = NT.int_log10(mn) + 1;
	
	//A = get_incidence_matrix();
		
	row_classes = NEW_int(nb_points() + nb_lines());
	col_classes = NEW_int(nb_points() + nb_lines());
	P.get_row_and_col_classes(row_classes, nb_row_classes, 
		col_classes, nb_col_classes, 0 /* verbose_level */);
	//ost << "nb_row_classes = " << nb_row_classes << endl;
	//ost << "nb_col_classes = " << nb_col_classes << endl;
	
	if (f_labeled) {
		print_column_labels(ost, P, 
			col_classes, nb_col_classes, width);
	}
	for (I = 0; I <= nb_row_classes; I++) {
		print_hline(ost, P, col_classes,
				nb_col_classes, width, f_labeled);
		cell = row_classes[I];
		if (I < nb_row_classes) {
			l = P.cellSize[cell];
			for (i = 0; i < l; i++) {
				print_line(ost, P, cell, i,
						col_classes, nb_col_classes, width, f_labeled);
				ost << endl;
			}
		}
	}
	FREE_int(row_classes);
	FREE_int(col_classes);
	//FREE_int(A);
}

void incidence_structure::point_collinearity_graph(
		int *Adj, int verbose_level)
// G[nb_points() * nb_points()]
{
	int f_v = (verbose_level >= 1);
	int i, j, h, l, u;
	
	if (f_v) {
		cout << "incidence_structure::point_collinearity_graph" << endl;
	}
	for (i = 0; i < nb_points(); i++) {
		for (j = 0; j < nb_points(); j++) {
			Adj[i * nb_points() + j] = 0;
		}
	}
	for (i = 0; i < nb_points(); i++) {
		for (h = 0; h < nb_lines_on_point[i]; h++) {
			l = lines_on_point[i * max_r + h];
			for (u = 0; u < nb_points_on_line[l]; u++) {
				j = points_on_line[l * max_k + u];
				if (j == i) {
					continue;
				}
				Adj[i * nb_points() + j] = 1;
				Adj[j * nb_points() + i] = 1;
			}
		}
	}
	if (f_v) {
		cout << "incidence_structure::point_collinearity_graph "
				"the graph is:" << endl;
		Int_vec_print_integer_matrix_width(cout, Adj,
				nb_points(), nb_points(), nb_points(), 1);
	}
}

void incidence_structure::line_intersection_graph(
		int *Adj, int verbose_level)
// G[nb_lines() * nb_lines()]
{
	int f_v = (verbose_level >= 1);
	int i, j, h, l, m, u;
	
	if (f_v) {
		cout << "incidence_structure::line_intersection_graph" << endl;
	}
	for (i = 0; i < nb_lines(); i++) {
		for (j = 0; j < nb_lines(); j++) {
			Adj[i * nb_lines() + j] = 0;
		}
	}
	for (l = 0; l < nb_lines(); l++) {
		for (u = 0; u < nb_points_on_line[l]; u++) {
			i = points_on_line[l * max_k + u];
			for (h = 0; h < nb_lines_on_point[i]; h++) {
				m = lines_on_point[i * max_r + h];
				if (l == m) {
					continue;
				}
				Adj[l * nb_lines() + m] = 1;
				Adj[m * nb_lines() + l] = 1;
			}
		}
	}
	if (f_v) {
		cout << "incidence_structure::line_intersection_graph "
				"the graph is:" << endl;
		Int_vec_print_integer_matrix_width(cout, Adj,
				nb_lines(), nb_lines(), nb_lines(), 1);
	}
}

void incidence_structure::latex_it(std::ostream &ost, data_structures::partitionstack &P)
{

	cout << "incidence_structure::latex_it" << endl;
	cout << "currently disabled" << endl;
	exit(1);

#if 0
	int nb_V, nb_B;
	int *Vi, *Bj;
	int *R;
	int *X;
	int f_v = TRUE;
	latex_interface L;

	rearrange(Vi, nb_V, Bj, nb_B, R, X, P);

	L.incma_latex(ost,
		nb_points(), nb_lines(), 
		nb_V, nb_B, Vi, Bj, 
		R, X, max_r);

	FREE_int(Vi);
	FREE_int(Bj);
	FREE_int(R);
	FREE_int(X);
#endif


}

void incidence_structure::rearrange(int *&Vi, int &nb_V, 
	int *&Bj, int &nb_B, int *&R, int *&X, data_structures::partitionstack &P)
{
	int *row_classes;
	int nb_row_classes;
	int *col_classes;
	int nb_col_classes;
	//int *Vi, *Bj;
	int *row_perm;
	int *row_perm_inv;
	int *col_perm;
	int *col_perm_inv;
	int i, j, ii, jj, c, a, h;
	//int *R;
	//int *X;
	data_structures::sorting Sorting;

	row_classes = NEW_int(nb_points() + nb_lines());
	col_classes = NEW_int(nb_points() + nb_lines());
	P.get_row_and_col_classes(row_classes, nb_row_classes, 
		col_classes, nb_col_classes, 0 /* verbose_level */);
	//ost << "nb_row_classes = " << nb_row_classes << endl;
	//ost << "nb_col_classes = " << nb_col_classes << endl;

	nb_V = nb_row_classes;
	nb_B = nb_col_classes;
	Vi = NEW_int(nb_row_classes);
	Bj = NEW_int(nb_col_classes);
	for (i = 0; i < nb_row_classes; i++) {
		c = row_classes[i];
		a = P.cellSize[c];
		Vi[i] = a;
	}
	for (i = 0; i < nb_col_classes; i++) {
		c = col_classes[i];
		a = P.cellSize[c];
		Bj[i] = a;
	}

	row_perm = NEW_int(nb_points());
	row_perm_inv = NEW_int(nb_points());
	col_perm = NEW_int(nb_lines());
	col_perm_inv = NEW_int(nb_lines());
	
	P.get_row_and_col_permutation(
		row_classes, nb_row_classes,
		col_classes, nb_col_classes, 
		row_perm, row_perm_inv, 
		col_perm, col_perm_inv);
	
#if 0
	cout << "row_perm:" << endl;
	int_vec_print(cout, row_perm, nb_points());
	cout << endl;
	cout << "col_perm:" << endl;
	int_vec_print(cout, col_perm, nb_lines());
	cout << endl;
#endif

	R = NEW_int(nb_points());
	X = NEW_int(nb_points() * max_r);

	for (i = 0; i < nb_points(); i++) {
		ii = row_perm_inv[i];
		R[i] = nb_lines_on_point[ii];
		for (h = 0; h < nb_lines_on_point[ii]; h++) {
			jj = lines_on_point[ii * max_r + h];
			j = col_perm[jj];
			X[i * max_r + h] = j;
		}
		Sorting.int_vec_heapsort(X + i * max_r, nb_lines_on_point[ii]);
	}
	


	FREE_int(row_classes);
	FREE_int(col_classes);
	//FREE_int(Vi);
	//FREE_int(Bj);
	FREE_int(row_perm);
	FREE_int(row_perm_inv);
	FREE_int(col_perm);
	FREE_int(col_perm_inv);
	//FREE_int(R);
	//FREE_int(X);
}



void incidence_structure::decomposition_print_tex(ostream &ost, 
		data_structures::partitionstack &PStack,
		int f_row_tactical, int f_col_tactical,
	int f_detailed, int f_local_coordinates, int verbose_level)
{
	int *row_classes, *row_class_inv, nb_row_classes;
	int *col_classes, *col_class_inv, nb_col_classes;
	int f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "incidence_structure::decomposition_print_tex get decomposition" << endl;
	}
	PStack.allocate_and_get_decomposition(
		row_classes, row_class_inv, nb_row_classes,
		col_classes, col_class_inv, nb_col_classes, 
		verbose_level);

	ost << "\\subsection*{Decomposition}" << endl;
	PStack.print_decomposition_tex(ost, row_classes, nb_row_classes, 
		col_classes, nb_col_classes);
	
	if (f_row_tactical) {
		int *row_scheme;
		row_scheme = NEW_int(nb_row_classes * nb_col_classes);
		get_row_decomposition_scheme(PStack, 
			row_classes, row_class_inv, nb_row_classes,
			col_classes, col_class_inv, nb_col_classes, 
			row_scheme, 0);
		if (f_v) {
			cout << "incidence_structure::decomposition_"
					"print_tex row_scheme:" << endl;
			cout << "row_scheme:" << endl;
			PStack.print_decomposition_scheme(cout, 
				row_classes, nb_row_classes,
				col_classes, nb_col_classes, 
				row_scheme, -1, -1);
		}

		if (f_detailed) {
			ost << "\\subsection*{Incidences by row-scheme}" << endl;
			print_row_tactical_decomposition_scheme_incidences_tex(
				PStack, 
				ost, FALSE /* f_enter_math_mode */, 
				row_classes, row_class_inv, nb_row_classes,
				col_classes, col_class_inv, nb_col_classes, 
				f_local_coordinates, 0 /*verbose_level*/);
		}
		FREE_int(row_scheme);
	}
	if (f_col_tactical) {
		int *col_scheme;
		col_scheme = NEW_int(nb_row_classes * nb_col_classes);
		get_col_decomposition_scheme(PStack, 
			row_classes, row_class_inv, nb_row_classes,
			col_classes, col_class_inv, nb_col_classes, 
			col_scheme, 0);
		if (f_v) {
			cout << "incidence_structure::decomposition_"
					"print_tex col_scheme:" << endl;
			cout << "col_scheme:" << endl;
			PStack.print_decomposition_scheme(cout, 
				row_classes, nb_row_classes,
				col_classes, nb_col_classes, 
				col_scheme, -1, -1);
		}

		if (f_detailed) {
			ost << "\\subsection*{Incidences by col-scheme}" << endl;
			print_col_tactical_decomposition_scheme_incidences_tex(
				PStack, 
				ost, FALSE /* f_enter_math_mode */, 
				row_classes, row_class_inv, nb_row_classes,
				col_classes, col_class_inv, nb_col_classes, 
				f_local_coordinates, 0 /*verbose_level*/);
		}
		FREE_int(col_scheme);
	}


	


	FREE_int(row_classes);
	FREE_int(row_class_inv);
	FREE_int(col_classes);
	FREE_int(col_class_inv);
}



void incidence_structure::do_tdo_high_level(
		data_structures::partitionstack &S,
	int f_tdo_steps, int f_tdo_depth, int tdo_depth, 
	int f_write_tdo_files, int f_pic, 
	int f_include_tdo_scheme, int f_include_tdo_extra,
	int f_write_tdo_class_files,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int TDO_depth;

	if (f_v) {
		cout << "incidence_structure::do_tdo_high_level" << endl;
	}

	if (f_tdo_depth) {
		TDO_depth = tdo_depth;
	}
	else {
		TDO_depth = nb_points() + nb_lines();
	}

	if (f_tdo_steps) {
		compute_tdo_stepwise(S, 
			TDO_depth, 
			f_write_tdo_files, 
			f_pic, 
			f_include_tdo_scheme, 
			f_include_tdo_extra, 
			verbose_level);
	}
	else {
		compute_tdo(S, 
			f_write_tdo_files, 
			f_pic, 
			f_include_tdo_scheme, 
			verbose_level);
	}
	if (f_write_tdo_class_files) {
		int *row_classes, *row_class_inv, nb_row_classes;
		int *col_classes, *col_class_inv, nb_col_classes;
		int i;

		S.allocate_and_get_decomposition(
			row_classes, row_class_inv, nb_row_classes,
			col_classes, col_class_inv, nb_col_classes, 
			verbose_level - 1);

		for (i = 0; i < nb_row_classes; i++) {
			string fname;
			fname.assign(label);
			char str[1000];
			snprintf(str, sizeof(str),  "_TDO_point_class_%d.txt", i);
			fname.append(str);
			S.write_cell_to_file_points_or_lines(
					row_classes[i], fname, verbose_level - 1);
		}
		for (i = 0; i < nb_col_classes; i++) {
			string fname;
			fname.assign(label);
			char str[1000];
			snprintf(str, sizeof(str), "_TDO_line_class_%d.txt", i);
			fname.append(str);
			S.write_cell_to_file_points_or_lines(
					col_classes[i], fname, verbose_level - 1);
		}
		FREE_int(row_classes);
		FREE_int(row_class_inv);
		FREE_int(col_classes);
		FREE_int(col_class_inv);
	}
}





void incidence_structure::compute_tdo(
		data_structures::partitionstack &S,
	int f_write_tdo_files, 
	int f_pic, 
	int f_include_tdo_scheme, 
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	string fname;
	int f_list_incidences = FALSE;
	orbiter_kernel_system::file_io Fio;

	if (f_v) {
		cout << "incidence_structure_compute_tdo" << endl;
	}
	
	int N;
	

	if (f_v) {
		cout << "incidence_structure_compute_tdo "
				"initial partition:" << endl;
		S.print(cout);
		cout << endl;
	}
	N = nb_points() + nb_lines();

	int TDO_depth = N;
	int f_labeled = TRUE;

	compute_TDO_safe(S, TDO_depth, verbose_level - 1);

	if (f_vv) {
		print_partitioned(cout, S, f_labeled);
	}

	if (f_v) {
		cout << "TDO:" << endl;
		if (TDO_depth < N) {
			if (EVEN(TDO_depth)) {
				get_and_print_row_decomposition_scheme(
						S, f_list_incidences, FALSE, verbose_level);
			}
			else {
				get_and_print_col_decomposition_scheme(
						S, f_list_incidences, FALSE, verbose_level);
			}
		}
		else {
			get_and_print_decomposition_schemes(S);
			S.print_classes_points_and_lines(cout);
		}
	}

	if (f_write_tdo_files) {
		fname.assign(label);
		fname.append("_tdo_scheme.tex");
		{
			ofstream fp(fname);

				//fp << "$$" << endl;
				get_and_print_tactical_decomposition_scheme_tex(
						fp, TRUE /* f_enter_math */, S);
				//fp << "$$" << endl;
		}
		if (f_v) {
			cout << "written file " << fname << " of size "
					<< Fio.file_size(fname) << endl;
		}

		fname.assign(label);
		fname.append("_tdo.tex");
		{
			ofstream fp(fname);

			if (f_include_tdo_scheme) {
				fp << "$\\begin{array}{c}" << endl;
				if (f_pic) {
					latex_it(fp, S);
					fp << "\\\\" << endl;
				}
				get_and_print_tactical_decomposition_scheme_tex(
					fp, FALSE /* f_enter_math */, S);
				fp << "\\end{array}$" << endl;
			}
			else {
				latex_it(fp, S);
			}
		}
		if (f_v) {
			cout << "written file " << fname << " of size "
					<< Fio.file_size(fname) << endl;
		}
	}

}

void incidence_structure::compute_tdo_stepwise(
		data_structures::partitionstack &S,
	int TDO_depth, 
	int f_write_tdo_files, 
	int f_pic, 
	int f_include_tdo_scheme, 
	int f_include_extra, 
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	string fname;
	string fname_pic;
	string fname_scheme;
	string fname_extra;
	int step, f_refine, f_refine_prev, f_done;
	int f_local_coordinates = FALSE;
	int f_list_incidences = FALSE;
	orbiter_kernel_system::file_io Fio;

	if (f_v) {
		cout << "incidence_structure::compute_tdo_stepwise" << endl;
	}
	

	compute_TDO_safe_first(S, 
		TDO_depth, step, f_refine, f_refine_prev, verbose_level);

	S.sort_cells();

	f_done = FALSE;
	while (TRUE) {
		if (f_v) {
			cout << "incidence_structure::compute_tdo_stepwise "
					"TDO step=" << step << " ht=" << S.ht << endl;
			if (step == 0) {
				print_non_tactical_decomposition_scheme_tex(
						cout, FALSE /* f_enter_math */, S);
			}
			else if (EVEN(step - 1)) {

				get_and_print_col_decomposition_scheme(
						S, f_list_incidences, FALSE, verbose_level);
				get_and_print_column_tactical_decomposition_scheme_tex(
					cout, TRUE /* f_enter_math */,
					FALSE /* f_print_subscripts */, S);
			}
			else {

				get_and_print_row_decomposition_scheme(
						S, f_list_incidences, FALSE, verbose_level);
				get_and_print_row_tactical_decomposition_scheme_tex(
					cout, TRUE /* f_enter_math */,
					FALSE /* f_print_subscripts */, S);
			}
			S.print_classes_points_and_lines(cout);
		}
		if (f_write_tdo_files) {

			char str[1000];

			snprintf(str, sizeof(str), "_tdo_step_%d.tex", step);
			fname.assign(label);
			fname.append(str);

			snprintf(str, sizeof(str), "_tdo_step_%d_pic.tex", step);
			fname_pic.assign(label);
			fname_pic.append(str);

			snprintf(str, sizeof(str), "_tdo_step_%d_scheme.tex", step);
			fname_scheme.assign(label);
			fname_scheme.append(str);

			snprintf(str, sizeof(str), "_tdo_step_%d_extra.tex", step);
			fname_extra.assign(label);
			fname_extra.append(str);

			{
				ofstream fp(fname);
				ofstream fp_pic(fname_pic);
				ofstream fp_scheme(fname_scheme);
				ofstream fp_extra(fname_extra);

				if (f_include_tdo_scheme) {
					fp << "\\subsection*{Step $" << step
							<< "$, Height $" << S.ht << "$}" << endl;
					//fp << "$ht=" << S.ht << "$\\\\" << endl;
					//fp << "\\bigskip" << endl;
					fp << "$\\begin{array}{c}" << endl;

					if (f_pic) {
						fp << "\\input " << fname_pic << endl;
						latex_it(fp_pic, S);
						fp << "\\\\" << endl;
					}

					fp << "\\input " << fname_scheme << endl;
					if (step == 0) {
						print_non_tactical_decomposition_scheme_tex(
							fp_scheme, FALSE /* f_enter_math */, S);
					}
					else if (EVEN(step - 1)) {
						get_and_print_column_tactical_decomposition_scheme_tex(
							fp_scheme, FALSE /* f_enter_math */,
							FALSE /* f_print_subscripts */, S);
					}
					else {
						get_and_print_row_tactical_decomposition_scheme_tex(
							fp_scheme, FALSE /* f_enter_math */,
							FALSE /* f_print_subscripts */, S);
					}
					fp << "\\end{array}$" << endl;

					if (f_include_extra) {
						if (step == 0) {
							decomposition_print_tex(fp_extra, S,
									FALSE, FALSE, FALSE,
									f_local_coordinates, verbose_level);
							fp << "\\input " << fname_extra << endl;
						}
						else if (EVEN(step - 1)) {
							decomposition_print_tex(fp_extra, S,
									FALSE, TRUE, TRUE,
									f_local_coordinates, verbose_level);
							fp << "\\input " << fname_extra << endl;
							//PStack.print_classes_points_and_lines(cout);
						}
						else {
							decomposition_print_tex(fp_extra, S,
									TRUE, FALSE, TRUE,
									f_local_coordinates, verbose_level);
							fp << "\\input " << fname_extra << endl;
						}
					}
				}
				else {
					latex_it(fp_pic, S);
				}
			}
			if (f_v) {
				cout << "written file " << fname << " of size "
						<< Fio.file_size(fname) << endl;
				cout << "written file " << fname_pic << " of size "
						<< Fio.file_size(fname_pic) << endl;
				cout << "written file " << fname_scheme << " of size "
						<< Fio.file_size(fname_scheme) << endl;
				cout << "written file " << fname_extra << " of size "
						<< Fio.file_size(fname_extra) << endl;
			}
		}
		if (f_done) {
			break;
		}

		f_done = compute_TDO_safe_next(S, 
			TDO_depth, step, f_refine, f_refine_prev, verbose_level);
		S.sort_cells();
	}



	if (f_vv) {
		int f_labeled = FALSE;
		
		print_partitioned(cout, S, f_labeled);
	}

	if (f_v) {
		cout << "TDO:" << endl;
		if (TDO_depth < nb_points() + nb_lines()) {
			if (EVEN(TDO_depth)) {
				get_and_print_row_decomposition_scheme(S,
						f_list_incidences, FALSE, verbose_level);
			}
			else {
				get_and_print_col_decomposition_scheme(S,
						f_list_incidences, FALSE, verbose_level);
			}
		}
		else {
			get_and_print_decomposition_schemes(S);
			S.print_classes_points_and_lines(cout);
		}
	}

	if (f_write_tdo_files) {
		fname.assign(label);
		fname.append("_tdo.tex");

		fname_pic.assign(label);
		fname_pic.append("_tdo_pic.tex");

		fname_scheme.assign(label);
		fname_scheme.append("_tdo_scheme.tex");

		{
			ofstream fp(fname);
			ofstream fp_pic(fname_pic);
			ofstream fp_scheme(fname_scheme);

			if (f_include_tdo_scheme) {
				fp << "\\subsection*{The TDO at Height $"
						<< S.ht << "$}" << endl;
				fp << "$\\begin{array}{c}" << endl;
				if (f_pic) {
					fp << "\\input " << fname_pic << endl;
					latex_it(fp_pic, S);
					fp << "\\\\" << endl;
				}
				fp << "\\input " << fname_scheme << endl;
				get_and_print_tactical_decomposition_scheme_tex(
					fp_scheme, FALSE /* f_enter_math */, S);
				fp << "\\end{array}$" << endl;
			}
			else {
				latex_it(fp_pic, S);
			}
		}
		if (f_v) {
			cout << "written file " << fname << " of size "
					<< Fio.file_size(fname) << endl;
			cout << "written file " << fname_pic << " of size "
					<< Fio.file_size(fname_pic) << endl;
			cout << "written file " << fname_scheme << " of size "
					<< Fio.file_size(fname_scheme) << endl;
		}
	}

}

void incidence_structure::init_partitionstack_trivial(
		data_structures::partitionstack *S,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int N;


	if (f_v) {
		cout << "incidence_structure::init_partitionstack_trivial" << endl;
	}
	N = nb_points() + nb_lines();
	
	S->allocate(N, 0);

	// split off the column class:
	S->subset_continguous(nb_points(), nb_lines());
	S->split_cell(0);

}

void incidence_structure::init_partitionstack(
		data_structures::partitionstack *S,
	int f_row_part, int nb_row_parts, int *row_parts,
	int f_col_part, int nb_col_parts, int *col_parts,
	int nb_distinguished_point_sets,
	int **distinguished_point_sets, int *distinguished_point_set_size,
	int nb_distinguished_line_sets,
	int **distinguished_line_sets, int *distinguished_line_set_size,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int f_v3 = (verbose_level >= 3);
	int N;
	int i, j, a, h;


	if (f_v) {
		cout << "incidence_structure::init_partitionstack" << endl;
	}
	N = nb_points() + nb_lines();
	
	S->allocate(N, 0);

	// split off the column class:
	S->subset_continguous(nb_points(), nb_lines());
	S->split_cell(0);



	if (f_row_part) {
		a = row_parts[0];
		for (i = 1; i < nb_row_parts; i++) {
			S->subset_continguous(a, nb_points() - a);
			S->split_cell(0);
			a += row_parts[i];
		}
	}
	if (f_col_part) {
		a = nb_points() + col_parts[0];
		for (i = 1; i < nb_col_parts; i++) {
			S->subset_continguous(a, nb_points() + nb_lines() - a);
			S->split_cell(0);
			a += col_parts[i];
		}
	}



	for (j = 0; j < nb_distinguished_point_sets; j++) {
		if (f_v) {
			cout << "splitting off " << j << "-th distinguished point "
					"set of size "
					<< distinguished_point_set_size[j] << endl;
		}
		if (f_v3) {
			cout << "which is the following set of size "
					<< distinguished_point_set_size[j] << ":" << endl;
			Int_vec_print(cout, distinguished_point_sets[j],
					distinguished_point_set_size[j]);
			cout << endl;
		}
		S->split_multiple_cells(distinguished_point_sets[j],
				distinguished_point_set_size[j], TRUE, verbose_level);
		if (f_vv) {
			cout << "incidence_structure::init_partitionstack "
					"partition:" << endl;
			S->print_classes_points_and_lines(cout);
		}
	}



	for (j = 0; j < nb_distinguished_line_sets; j++) {
		if (f_v) {
			cout << "splitting off " << j << "-th distinguished "
					"line set of size "
					<< distinguished_line_set_size[j] << endl;
		}
		if (f_v3) {
			cout << "which is the following set of size "
					<< distinguished_line_set_size[j] << ":" << endl;
			Int_vec_print(cout, distinguished_line_sets[j],
					distinguished_line_set_size[j]);
			cout << endl;
		}
		
		int *set;
		int set_sz;

		set_sz = distinguished_line_set_size[j];
		set = NEW_int(set_sz);
		for (h = 0; h < set_sz; h++) {
			set[h] = distinguished_line_sets[j][h] + nb_points();
		}


		if (f_vv) {
			cout << "incidence_structure::init_partitionstack "
					"After adding Inc->nb_points():" << endl;
			Int_vec_print(cout, set, set_sz);
			cout << endl;
		}


		S->split_multiple_cells(set, set_sz, TRUE, verbose_level);
		FREE_int(set);
		if (f_vv) {
			cout << "incidence_structure::init_partitionstack "
					"partition:" << endl;
			S->print_classes_points_and_lines(cout);
		}
	}

	if (f_vv) {
		cout << "incidence_structure::init_partitionstack "
				"we have arrived at the following partition:" << endl;
		S->print_classes_points_and_lines(cout);
	}
	if (f_v) {
		cout << "incidence_structure::init_partitionstack done" << endl;
	}
}

void incidence_structure::shrink_aut_generators(
	int nb_distinguished_point_sets, 
	int nb_distinguished_line_sets, 
	int Aut_counter, int *Aut, int *Base, int Base_length, 
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i, j, h;
	int m, n;
	int nb_rows, nb_cols, total;

	if (f_v) {
		cout << "incidence_structure::shrink_aut_generators" << endl;
	}
	m = nb_points();
	n = nb_lines();

	nb_rows = m;
	nb_cols = n;

	nb_rows += nb_distinguished_line_sets;
	nb_cols += nb_distinguished_point_sets;

	total = nb_rows + nb_cols;

	for (h = 0; h < Aut_counter; h++) {
		for (i = 0; i < m; i++) {
			Aut[h * (m + n) + i] = Aut[h * total + i];
		}
		for (j = 0; j < n; j++) {
			Aut[h * (m + n) + m + j] =
					Aut[h * total + nb_rows + j] -
						nb_distinguished_line_sets;
		}
		for (i = Base_length - 1; i >= 0; i--) {
			if (Base[i] > m) {
				if (Base[i] < nb_rows) {
					cout << "Base[i] > m && Base[i] < nb_rows" << endl;
					exit(1);
				}
				Base[i] -= nb_distinguished_line_sets;
			}
		}
	}

	if (f_v) {
		cout << "incidence_structure::shrink_aut_generators "
				"done" << endl;
	}
}

void incidence_structure::print_aut_generators(
	int Aut_counter, int *Aut,
	int Base_length, int *Base, int *Transversal_length)
{
	int m, n, i, j, h;
	int *AUT;
	combinatorics::combinatorics_domain Combi;
	
	m = nb_points();
	n = nb_lines();
	AUT = NEW_int(m + n);
	
	cout << "incidence_structure::print_aut_generators "
			"base_length = " << Base_length << endl;
	cout << "The base is : " << endl;
	for (i = Base_length - 1; i >= 0; i--) {
		cout << Base[i] << " ";
	}
	cout << endl;
	cout << "The transversal lengths are: " << endl;
	for (i = 0; i < Base_length; i++) {
		cout << Transversal_length[i] << " ";
	}
	cout << endl;
	cout << "We have found " << Aut_counter << " gens:" << endl;
	for (h = 0; h < Aut_counter; h++) {
		for (i = 0; i < m; i++) {
			AUT[i] = Aut[h * (m + n) + i];
		}
		for (j = 0; j < n; j++) {
			AUT[m + j] = Aut[h * (m + n) + m + j] - m;
		}
		cout << h << " : " ;
		cout << "generator " << h << ":" << endl;
		for (i = 0; i < m + n; i++)
			cout << AUT[i] << " ";
		cout << endl;
		Combi.perm_print_product_action(cout, AUT, m + n, m,
				0 /* offset */, FALSE /* f_cycle_length */);
		cout << endl;
		//for ( j = 0; j < m + n; j++ ){
			//cout << Aut[h * (m + n) + j] << " ";
		//}
		//cout << endl;
	}
	cout << endl;
	
	FREE_int(AUT);
}

void incidence_structure::compute_extended_collinearity_graph(
	int *&Adj, int &v, int *&partition, 
	int f_row_part, int nb_row_parts, int *row_parts,
	int f_col_part, int nb_col_parts, int *col_parts,
	int nb_distinguished_point_sets,
	int **distinguished_point_sets, int *distinguished_point_set_size,
	int nb_distinguished_line_sets,
	int **distinguished_line_sets, int *distinguished_line_set_size,
	int verbose_level)
// side effect: the distinguished sets will be sorted afterwards
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	//int f_v4 = (verbose_level >= 4);
	int m = nb_points();
	int n = nb_lines();
	int i, j, J, j0, l, u, k, h, h1, h2, i1, i2, idx, y, z;
	int v1, v2, v3;
	int my_nb_col_parts;
	int *my_col_parts;
	data_structures::sorting Sorting;

	if (f_v) {
		cout << "incidence_structure::compute_extended_collinearity_graph" << endl;
	}
	v = v1 = m;
	if (f_col_part) {
		v += nb_col_parts - 1;
	}
	v2 = v;
	v += nb_distinguished_point_sets;
	v3 = v;
	for (i = 0; i < nb_distinguished_line_sets; i++) {
		v += distinguished_line_set_size[i];
	}
	if (f_v) {
		cout << "incidence_structure::compute_extended_"
				"collinearity_graph v=" << v << endl;
	}
	
	for (i = 0; i < nb_distinguished_point_sets; i++) {
		Sorting.int_vec_heapsort(distinguished_point_sets[i],
				distinguished_point_set_size[i]);
	}
	for (i = 0; i < nb_distinguished_line_sets; i++) {
		Sorting.int_vec_heapsort(distinguished_line_sets[i],
				distinguished_line_set_size[i]);
	}
	partition = NEW_int(v);
	for (i = 0; i < v; i++) {
		partition[i] = 1;
	}
	Adj = NEW_int(v * v);
	for (i = 0; i < v * v; i++) {
		Adj[i] = 0;
	}


	if (f_col_part) {
		my_nb_col_parts = nb_col_parts;
		my_col_parts = NEW_int(my_nb_col_parts);
		for (i = 0; i < nb_col_parts; i++) {
			my_col_parts[i] = col_parts[i];
		}
	}
	else {
		my_nb_col_parts = 1;
		my_col_parts = NEW_int(my_nb_col_parts);
		my_col_parts[0] = n;
	}
	j0 = 0;
	for (J = 0; J < my_nb_col_parts; J++) {
		l = my_col_parts[J];
		for (u = 0; u < l; u++) {
			j = j0 + u;
			k = nb_points_on_line[j];
			// keep track of which column class block j belongs to
			// we do this for the all column classes but the first
			if (J) {
				for (h = 0; h < k; h++) {
					i = points_on_line[j * max_k + h];
					Adj[i * v + v1 + J - 1] = 1;
					Adj[(v1 + J - 1) * v + i] = 1;
				}
			}
			// record the collinearity information resulting from block j:
			for (h1 = 0; h1 < k; h1++) {
				i1 = points_on_line[j * max_k + h1];
				for (h2 = h1 + 1; h2 < k; h2++) {
					i2 = points_on_line[j * max_k + h2];
					Adj[i1 * v + i2] = 1;
					Adj[i2 * v + i1] = 1;
				}
			}

#if 0
			// keep track of whether block j is in a distinguished line set:
			for (h = 0; h < nb_distinguished_line_sets; h++) {
				if (int_vec_search(distinguished_line_sets[h],
						distinguished_line_set_size[h], j, idx)) {
					Adj[j * v + v3 + h] = 1;
					Adj[(v3 + h) * v + j] = 1;
				}
			}
#endif

		}
	}

	// record the distinguished line sets:
	z = v2;
	for (h = 0; h < nb_distinguished_line_sets; h++) {
		for (u = 0; u < distinguished_line_set_size[h]; u++) {
			j = distinguished_line_sets[h][u];
			k = nb_points_on_line[j];
			for (y = 0; y < k; y++) {
				i = points_on_line[j * max_k + y];
				Adj[i * v + z + u] = 1;
				Adj[(z + u) * v + i] = 1;
			}
		}
		z += distinguished_line_set_size[h];
	}


	// finally, we record the distinguished point sets:
	for (i = 0; i < m; i++) {
		for (h = 0; h < nb_distinguished_point_sets; h++) {
			if (Sorting.int_vec_search(distinguished_point_sets[h],
					distinguished_point_set_size[h], i, idx)) {
				Adj[i * v + v2 + h] = 1;
				Adj[(v2 + h) * v + i] = 1;
			}
		}
	}

	// initialize the partition:
	z = 0;
	if (f_row_part) {
		for (h = 0; h < nb_row_parts; h++) {
			partition[z + row_parts[h] - 1] = 0;
			z += row_parts[h];
		}
	}
	else {
		partition[m - 1] = 0;
	}
	z = v1;
	for (h = 0; h < my_nb_col_parts; h++) {
		partition[z + h] = 0;
	}
	z = v2;
	for (h = 0; h < nb_distinguished_point_sets; h++) {
		partition[z + h] = 0;
	}
	z = v3;
	for (h = 0; h < nb_distinguished_line_sets; h++) {
		l = distinguished_line_set_size[h];
		partition[z + l - 1] = 0;
		z += l;
	}
	if (f_vv) {
		cout << "incidence_structure::compute_extended_collinearity_graph "
				"Adj=" << endl;
		Int_vec_print_integer_matrix_width(cout, Adj, v, v, v, 1);
	}
	

	if (f_v) {
		cout << "incidence_structure::compute_extended_collinearity_graph done" << endl;
	}

	FREE_int(my_col_parts);
}

void incidence_structure::compute_extended_matrix(
	int *&M, int &nb_rows, int &nb_cols, int &total, int *&partition, 
	int f_row_part, int nb_row_parts, int *row_parts,
	int f_col_part, int nb_col_parts, int *col_parts,
	int nb_distinguished_point_sets,
	int **distinguished_point_sets, int *distinguished_point_set_size,
	int nb_distinguished_line_sets,
	int **distinguished_line_sets, int *distinguished_line_set_size,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int f_v4 = (verbose_level >= 4);
	int m = nb_points();
	int n = nb_lines();
	int i, j, h, a;

	if (f_v) {
		cout << "incidence_structure::compute_extended_matrix" << endl;
	}
	
	nb_rows = m;
	nb_cols = n;

	nb_rows += nb_distinguished_line_sets;
	nb_cols += nb_distinguished_point_sets;

	total = nb_rows + nb_cols;
	
	if (f_vv) {
		cout << "nb_distinguished_line_sets="
				<< nb_distinguished_line_sets << endl;
		cout << "nb_distinguished_point_sets="
				<< nb_distinguished_point_sets << endl;
		cout << "m=" << m << endl;
		cout << "n=" << n << endl;
		cout << "nb_rows=" << nb_rows << endl;
		cout << "nb_cols=" << nb_cols << endl;
		cout << "total=" << total << endl;
	}

	partition = NEW_int(total);
	M = NEW_int(nb_rows * nb_cols);
	for (i = 0; i < nb_rows * nb_cols; i++) {
		M[i] = 0;
	}
	for (i = 0; i < m; i++) {
		for (h = 0; h < nb_lines_on_point[i]; h++) {
			a = lines_on_point[i * max_r + h];
			M[i * nb_cols + a] = 1;
		}
	}


	for (j = 0; j < nb_distinguished_point_sets; j++) {
		if (f_v) {
			cout << "The " << j << "-th distinguished point set is:" << endl;
			Int_vec_print(cout, distinguished_point_sets[j],
					distinguished_point_set_size[j]);
			cout << endl;
		}
		for (i = 0; i < distinguished_point_set_size[j]; i++) {
			a = distinguished_point_sets[j][i];
			M[a * nb_cols + n + j] = 1;
		}
	}


	for (j = 0; j < nb_distinguished_line_sets; j++) {
		if (f_v) {
			cout << "The " << j << "-th distinguished line set is:" << endl;
			Int_vec_print(cout, distinguished_line_sets[j],
					distinguished_line_set_size[j]);
			cout << endl;
		}
		for (i = 0; i < distinguished_line_set_size[j]; i++) {
			a = distinguished_line_sets[j][i];
			M[(m + j) * nb_cols + a] = 1;
		}
	}


	if (f_v4) {
		cout << "incidence_structure::compute_extended_matrix "
				"The extended incidence matrix is:" << endl;
		Int_vec_print_integer_matrix_width(cout, M, nb_rows, nb_cols, nb_cols, 1);
	}


	for (i = 0; i < total; i++) {
		partition[i] = 1;
	}

	if (f_row_part) {
		int a;
		a = 0;
		for (i = 0; i < nb_row_parts; i++) {
			a += row_parts[i];
			partition[a - 1] = 0;
		}
	}
	else {
		partition[m - 1] = 0;
	}
	if (f_col_part) {
		int a;
		a = nb_rows;
		for (i = 0; i < nb_col_parts; i++) {
			a += col_parts[i];
			partition[a - 1] = 0;
		}
	}
	else {
		partition[nb_rows + n - 1] = 0;
	}
#if 0
	for (i = 0; i < PB.P.ht; i++) {
		j = PB.P.startCell[i] + PB.P.cellSize[i] - 1;
		if (PB.P.startCell[i] >= m) {
			j += nb_distinguished_line_sets;
		}
		partition[j] = 0;
	}
#endif

	for (i = 0; i < nb_distinguished_line_sets; i++) {
		partition[m + i] = 0;
	}

	for (j = 0; j < nb_distinguished_point_sets; j++) {
		partition[nb_rows + n + j] = 0;
	}
	if (f_v4) {
		cout << "incidence_structure::compute_extended_matrix "
				"The partition is:" << endl;
		Int_vec_print(cout, partition, total);
		cout << endl;
	}

	
	if (f_v) {
		cout << "incidence_structure::compute_extended_matrix done" << endl;
	}
}


data_structures::bitvector *incidence_structure::encode_as_bitvector()
{
	int i, j, a;
	data_structures::bitvector *B;

	B = NEW_OBJECT(data_structures::bitvector);
	B->allocate(nb_rows * nb_cols);

	for (i = 0; i < nb_rows; i++) {
		for (j = 0; j < nb_cols; j++) {
			if (M[i * nb_cols + j]) {
				a = i * nb_cols + j;
				//bitvector_set_bit(bitvec, a);
				B->m_i(a, 1);
			}
		}
	}
	return B;
}


incidence_structure *incidence_structure::apply_canonical_labeling(
		long int *canonical_labeling, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int *Incma_out;
	int i, j, ii, jj;

	if (f_v) {
		cout << "incidence_structure::apply_canonical_labeling" << endl;
	}

	if (f_vv) {
		cout << "incidence_structure::apply_canonical_labeling labeling:" << endl;
		Lint_vec_print(cout, canonical_labeling, nb_rows + nb_cols);
		cout << endl;
	}

	Incma_out = NEW_int(nb_rows * nb_cols);
	//Orbiter->Int_vec.zero(Incma_out, nb_rows * nb_cols);
	for (i = 0; i < nb_rows; i++) {
		ii = canonical_labeling[i];
		for (j = 0; j < nb_cols; j++) {
			jj = canonical_labeling[nb_rows + j] - nb_rows;
			//cout << "i=" << i << " j=" << j << " ii=" << ii
			//<< " jj=" << jj << endl;
			Incma_out[i * nb_cols + j] = M[ii * nb_cols + jj];
		}
	}


	incidence_structure *Inc_out;

	Inc_out = NEW_OBJECT(incidence_structure);

	Inc_out->init_by_matrix(nb_rows, nb_cols, Incma_out, verbose_level);

	FREE_int(Incma_out);
	if (f_v) {
		cout << "incidence_structure::apply_canonical_labeling done" << endl;
	}
	return Inc_out;
}

void incidence_structure::save_as_csv(std::string &fname_csv, int verbose_level)
{
	orbiter_kernel_system::file_io Fio;

	Fio.int_matrix_write_csv(fname_csv, M, nb_rows, nb_cols);
}

#if 0
void incidence_structure::save_as_Levi_graph(std::string &fname_bin,
		int f_point_labels, long int *point_labels,
		int verbose_level)
{
	file_io Fio;
	colored_graph *CG;

	CG = NEW_OBJECT(colored_graph);

	CG->create_Levi_graph_from_incidence_matrix(
			M, nb_rows, nb_cols,
			f_point_labels, point_labels,
			verbose_level);
	CG->save(fname_bin, verbose_level);

	FREE_OBJECT(CG);
}
#endif

void incidence_structure::init_large_set(
		long int *blocks,
		int N_points, int design_b, int design_k, int partition_class_size,
		int *&partition, int verbose_level)
{
	int f_v = (verbose_level >= 1);


	if (f_v) {
		cout << "incidence_structure::init_large_set" << endl;
	}

	int *block;
	int *Incma;
	int nb_classes;
	int nb_rows;
	int nb_cols;
	int N;
	int u, i, j, t, a;
	combinatorics::combinatorics_domain Combi;


	nb_classes = design_b / partition_class_size;
	nb_rows = N_points + nb_classes;
	nb_cols = design_b + 1;
	N = nb_rows + nb_cols;
	if (f_v) {
		cout << "incidence_structure::init_large_set nb_rows=" << nb_rows << " nb_cols=" << nb_cols << " N=" << N << endl;
	}

	Incma = NEW_int(nb_rows * nb_cols);
	Int_vec_zero(Incma, nb_rows * nb_cols);
	block = NEW_int(design_k);

	for (u = 0; u < nb_classes; u++) {
		for (j = 0; j < partition_class_size; j++) {
			a = blocks[u * partition_class_size + j];
			Combi.unrank_k_subset(a, block, N_points, design_k);
			for (t = 0; t < design_k; t++) {
				i = block[t];
				Incma[i * nb_cols + u * partition_class_size + j] = 1;
			}
			Incma[(N_points + u) * nb_cols + u * partition_class_size + j] = 1;
		}
		Incma[(N_points + u) * nb_cols + nb_cols - 1] = 1;
	}

	init_by_matrix(nb_rows, nb_cols, Incma, verbose_level);
	if (f_v) {
		Int_matrix_print(Incma, nb_rows, nb_cols);
	}


	partition = NEW_int(N);
	for (i = 0; i < N; i++) {
		partition[i] = 1;
		}
	partition[N_points - 1] = 0;
	partition[nb_rows - 1] = 0;
	partition[nb_rows + design_b - 1] = 0;
	partition[nb_rows + nb_cols - 1] = 0;


	if (f_v) {
		data_structures::tally T;

		T.init(partition, N, FALSE, 0);
		T.print_array_tex(cout, TRUE);
	}


	FREE_int(Incma);
	FREE_int(block);

	if (f_v) {
		cout << "incidence_structure::init_large_set done" << endl;
	}
}



}}}



