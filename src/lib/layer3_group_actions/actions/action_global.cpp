// action_global.cpp
//
// Anton Betten
// October 10, 2013

#include "layer1_foundations/foundations.h"
#include "group_actions.h"

using namespace std;


namespace orbiter {
namespace layer3_group_actions {
namespace actions {


void action_global::action_print_symmetry_group_type(ostream &ost,
		symmetry_group_type a)
{
	if (a == unknown_symmetry_group_t) {
		ost << "unknown_symmetry_group_t";
		}
	else if (a == matrix_group_t) {
		ost << "matrix_group_t";
		}
	else if (a == perm_group_t) {
		ost << "perm_group_t";
		}
	else if (a == wreath_product_t) {
		ost << "wreath_product_t";
		}
	else if (a == direct_product_t) {
		ost << "direct_product_t";
		}
	else if (a == permutation_representation_t) {
		ost << "permutation_representation_t";
		}
	else if (a == action_on_sets_t) {
		ost << "action_on_sets_t";
		}
	else if (a == action_on_set_partitions_t) {
		ost << "action_on_set_partitions_t";
		}
	else if (a == action_on_subgroups_t) {
		ost << "action_on_subgroups_t";
		}
	else if (a == action_on_pairs_t) {
		ost << "action_on_pairs_t";
		}
	else if (a == action_on_ordered_pairs_t) {
		ost << "action_on_ordered_pairs_t";
		}
	else if (a == base_change_t) {
		ost << "base_change_t";
		}
	else if (a == product_action_t) {
		ost << "product_action_t";
		}
	else if (a == action_by_right_multiplication_t) {
		ost << "action_by_right_multiplication_t";
		}
	else if (a == action_by_restriction_t) {
		ost << "action_by_restriction_t";
		}
	else if (a == action_by_conjugation_t) {
		ost << "action_by_conjugation_t";
		}
	else if (a == action_by_representation_t) {
		ost << "action_by_representation_t";
		}
	else if (a == action_by_subfield_structure_t) {
		ost << "action_by_subfield_structure_t";
		}
	else if (a == action_on_determinant_t) {
		ost << "action_on_determinant_t";
		}
	else if (a == action_on_galois_group_t) {
		ost << "action_on_galois_group_t";
		}
	else if (a == action_on_sign_t) {
		ost << "action_on_sign_t";
		}
	else if (a == action_on_grassmannian_t) {
		ost << "action_on_grassmannian_t";
		}
	else if (a == action_on_spread_set_t) {
		ost << "action_on_spread_set_t";
		}
	else if (a == action_on_cosets_t) {
		ost << "action_on_cosets_t";
		}
	else if (a == action_on_factor_space_t) {
		ost << "action_on_factor_space_t";
		}
	else if (a == action_on_wedge_product_t) {
		ost << "action_on_wedge_product_t";
		}
	else if (a == action_on_bricks_t) {
		ost << "action_on_bricks_t";
		}
	else if (a == action_on_andre_t) {
		ost << "action_on_andre_t";
		}
	else if (a == action_on_orthogonal_t) {
		ost << "action_on_orthogonal_t";
		}
	else if (a == action_on_orbits_t) {
		ost << "action_on_orbits_t";
		}
	else if (a == action_on_flags_t) {
		ost << "action_on_flags_t";
		}
	else if (a == action_on_homogeneous_polynomials_t) {
		ost << "action_on_homogeneous_polynomials_t";
		}
	else if (a == action_on_k_subsets_t) {
		ost << "action_on_k_subsets_t";
		}
	else if (a == action_on_interior_direct_product_t) {
		ost << "action_on_interior_direct_product_t";
		}
	else {
		ost << "unknown symmetry_group_type" << endl;
		}
}


void action_global::make_generators_stabilizer_of_two_components(
	action *A_PGL_n_q, action *A_PGL_k_q,
	int k,
	data_structures_groups::vector_ge *gens, int verbose_level)
// used in semifield.cpp
// does not include the swap
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int *Q;
	int *Elt1;
	int *Zero;
	int *Id;
	int *Center;
	int *minusId;
	int n, i, len;
	int *P;
	groups::matrix_group *Mtx;
	field_theory::finite_field *Fq;
	int minus_one, alpha;
	groups::strong_generators *gens_PGL_k;
	//vector_ge *gens_PGL_k;


	if (f_v) {
		cout << "action_global::make_generators_stabilizer_of_two_components" << endl;
	}
	n = 2 * k;

	Zero = NEW_int(k * k);
	Id = NEW_int(k * k);
	Center = NEW_int(k * k);
	minusId = NEW_int(k * k);
	Q = NEW_int(n * n + 1);
	Elt1 = NEW_int(A_PGL_n_q->elt_size_in_int);


	Mtx = A_PGL_k_q->G.matrix_grp;
	Fq = Mtx->GFq;
	minus_one = Fq->negate(1);
	alpha = Fq->primitive_root();

	Int_vec_zero(Zero, k * k);
	Int_vec_zero(Id, k * k);
	Int_vec_zero(Center, k * k);
	Int_vec_zero(minusId, k * k);
	for (i = 0; i < k; i++) {
		Id[i * k + i] = 1;
	}
	for (i = 0; i < k; i++) {
		Center[i * k + i] = alpha;
	}
	for (i = 0; i < k; i++) {
		minusId[i * k + i] = minus_one;
	}

	gens_PGL_k = A_PGL_k_q->Strong_gens;
	//gens_PGL_k = A_PGL_k_q->strong_generators;
	
	len = gens_PGL_k->gens->len;
	//len = gens_PGL_k->len;

	int *Data;
	int new_len, sz, idx, h;

	new_len = 2 * len + 2;
	sz = n * n;
	if (Mtx->f_semilinear) {
		sz++;
	}
	

	Data = NEW_int(new_len * sz);
	idx = 0;
	for (h = 0; h < 2 * len; h++) {

		P = gens_PGL_k->gens->ith(h / 2);
		//P = gens_PGL_k->ith(h / 2);

		if (EVEN(h)) {
			// Q := diag(P,Id)
			orbiter_kernel_system::Orbiter->Int_vec->matrix_make_block_matrix_2x2(Q, k, P, Zero, Zero, Id);
		}
		else {
			// Q := diag(Id,P)
			orbiter_kernel_system::Orbiter->Int_vec->matrix_make_block_matrix_2x2(Q, k, Id, Zero, Zero, P);
		}
		if (Mtx->f_semilinear) {
			Q[n * n] = P[k * k];
		}
		Int_vec_copy(Q, Data + idx * sz, sz);
		idx++;
	}

#if 0
	// Q := matrix(0,I,I,0):
	int_matrix_make_block_matrix_2x2(Q, k, Zero, Id, Id, Zero);
	if (Mtx->f_semilinear) {
		Q[n * n] = 0;
	}
	int_vec_copy(Q, Data + idx * sz, sz);
	idx++;
#endif

	// Q := matrix(Center,0,0,I):
	orbiter_kernel_system::Orbiter->Int_vec->matrix_make_block_matrix_2x2(Q, k, Center, Zero, Zero, Id);
	if (Mtx->f_semilinear) {
		Q[n * n] = 0;
	}
	Int_vec_copy(Q, Data + idx * sz, sz);
	idx++;

	// Q := matrix(I,0,0,Center):
	orbiter_kernel_system::Orbiter->Int_vec->matrix_make_block_matrix_2x2(Q, k, Id, Zero, Zero, Center);
	if (Mtx->f_semilinear) {
		Q[n * n] = 0;
	}
	Int_vec_copy(Q, Data + idx * sz, sz);
	idx++;


	if (idx != new_len) {
		cout << "action_global::make_generators_stabilizer_of_two_components "
				"idx != new_len" << endl;
		exit(1);
	}



	gens->init(A_PGL_n_q, verbose_level - 2);
	gens->allocate(new_len, verbose_level - 2);
	for (h = 0; h < new_len; h++) {
		A_PGL_n_q->make_element(Elt1, Data + h * sz, 0);
		if (f_vv) {
			cout << "action_global::make_generators_stabilizer_of_two_components "
					"after make_element generator " << h << " : " << endl;
			A_PGL_n_q->print_quick(cout, Elt1);
		}
		A_PGL_n_q->move(Elt1, gens->ith(h));
	}
	

	FREE_int(Data);

	FREE_int(Zero);
	FREE_int(Id);
	FREE_int(Center);
	FREE_int(minusId);
	FREE_int(Q);
	FREE_int(Elt1);
	if (f_v) {
		cout << "action_global::make_generators_stabilizer_of_two_components done" << endl;
	}
}


void action_global::make_generators_stabilizer_of_three_components(
	action *A_PGL_n_q, action *A_PGL_k_q,
	int k,
	data_structures_groups::vector_ge *gens,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int *Q;
	int *Elt1;
	int *Zero;
	int *Id;
	int *minusId;
	int n, i, len;
	int *P;
	groups::matrix_group *Mtx;
	field_theory::finite_field *Fq;
	int minus_one;
	groups::strong_generators *gens_PGL_k;

	if (f_v) {
		cout << "action_global::make_generators_stabilizer_of_three_components" << endl;
		cout << "A_PGL_n_q:" << endl;
		A_PGL_n_q->print_info();
		cout << "A_PGL_k_q:" << endl;
		A_PGL_k_q->print_info();
	}
	n = 2 * k;

	Zero = NEW_int(k * k);
	Id = NEW_int(k * k);
	minusId = NEW_int(k * k);
	Q = NEW_int(n * n + 1);
	Elt1 = NEW_int(A_PGL_n_q->elt_size_in_int);


	Mtx = A_PGL_k_q->G.matrix_grp;
	Fq = Mtx->GFq;
	minus_one = Fq->negate(1);


	Int_vec_zero(Zero, k * k);
	Int_vec_zero(Id, k * k);
	Int_vec_zero(minusId, k * k);
	for (i = 0; i < k; i++) {
		Id[i * k + i] = 1;
	}
	for (i = 0; i < k; i++) {
		minusId[i * k + i] = minus_one;
	}

	gens_PGL_k = A_PGL_k_q->Strong_gens;
	//gens_PGL_k = A_PGL_k_q->strong_generators;
	
	len = gens_PGL_k->gens->len;
	//len = gens_PGL_k->len;

	int *Data;
	int new_len, sz, idx, h;

	new_len = len + 2;
	sz = n * n;
	if (Mtx->f_semilinear) {
		sz++;
	}
	if (f_v) {
		cout << "action_global::make_generators_stabilizer_of_three_components sz = " << sz << endl;
	}
	Data = NEW_int(new_len * sz);
	

	if (f_v) {
		cout << "action_global::make_generators_stabilizer_of_three_components step 1" << endl;
	}
	idx = 0;
	for (h = 0; h < len; h++) {

		if (f_v) {
			cout << "action_global::make_generators_stabilizer_of_three_components step 1: " << h << " / " << len << endl;
		}
		P = gens_PGL_k->gens->ith(h);
		//P = gens_PGL_k->ith(h);

		// Q := diag(P,P)
		orbiter_kernel_system::Orbiter->Int_vec->matrix_make_block_matrix_2x2(Q, k, P, Zero, Zero, P);
		if (Mtx->f_semilinear) {
			Q[n * n] = P[k * k];
		}
		if (f_v) {
			cout << "action_global::make_generators_stabilizer_of_three_components Q=" << endl;
			Int_matrix_print(Q, n, n);
		}
		Int_vec_copy(Q, Data + idx * sz, sz);
		idx++;
	}

	if (f_v) {
		cout << "action_global::make_generators_stabilizer_of_three_components step 2" << endl;
	}
	// Q := matrix(0,I,I,0):
	orbiter_kernel_system::Orbiter->Int_vec->matrix_make_block_matrix_2x2(Q, k, Zero, Id, Id, Zero);
	if (Mtx->f_semilinear) {
		Q[n * n] = 0;
	}
	if (f_v) {
		cout << "action_global::make_generators_stabilizer_of_three_components Q=" << endl;
		Int_matrix_print(Q, n, n);
	}
	Int_vec_copy(Q, Data + idx * sz, sz);
	idx++;

	if (f_v) {
		cout << "action_global::make_generators_stabilizer_of_three_components step 3" << endl;
	}
	// Q := matrix(0,I,-I,-I):
	orbiter_kernel_system::Orbiter->Int_vec->matrix_make_block_matrix_2x2(Q, k, Zero, Id, minusId, minusId);
	if (Mtx->f_semilinear) {
		Q[n * n] = 0;
	}
	if (f_v) {
		cout << "action_global::make_generators_stabilizer_of_three_components Q=" << endl;
		Int_matrix_print(Q, n, n);
	}
	Int_vec_copy(Q, Data + idx * sz, sz);
	idx++;


	if (idx != new_len) {
		cout << "action_global::make_generators_stabilizer_of_three_components "
				"idx != new_len" << endl;
		exit(1);
	}

	if (f_v) {
		cout << "action_global::make_generators_stabilizer_of_three_components step 4" << endl;
	}


	gens->init(A_PGL_n_q, verbose_level - 2);
	gens->allocate(new_len, verbose_level - 2);
	for (h = 0; h < new_len; h++) {

		if (f_v) {
			cout << "action_global::make_generators_stabilizer_of_three_components step 4: " << h << " / " << new_len << endl;
		}

		if (f_v) {
			cout << "action_global::make_generators_stabilizer_of_three_components generator=" << endl;
			Int_matrix_print(Data + h * sz, n, n);
		}
		A_PGL_n_q->make_element(Elt1, Data + h * sz, 0);
		if (f_vv) {
			cout << "action_global::make_generators_stabilizer_of_three_components "
					"after make_element generator " << h << " : " << endl;
			A_PGL_n_q->print_quick(cout, Elt1);
		}
		A_PGL_n_q->move(Elt1, gens->ith(h));
	}
	

	FREE_int(Data);

	FREE_int(Zero);
	FREE_int(Id);
	FREE_int(minusId);
	FREE_int(Q);
	FREE_int(Elt1);
	if (f_v) {
		cout << "action_global::make_generators_stabilizer_of_three_components done" << endl;
	}
}

void action_global::compute_generators_GL_n_q(int *&Gens,
		int &nb_gens, int &elt_size, int n,
		field_theory::finite_field *F,
		data_structures_groups::vector_ge *&nice_gens,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	action *A;
	data_structures_groups::vector_ge *gens;
	int *Elt;
	int h, i, l, alpha;

	if (f_v) {
		cout << "action_global::compute_generators_GL_n_q" << endl;
	}
	A = NEW_OBJECT(action);

	A->init_projective_group(n, F,
			FALSE /* f_semilinear */,
			TRUE /* f_basis */, TRUE /* f_init_sims */,
			nice_gens,
			verbose_level - 2);

	gens = A->Strong_gens->gens;

	l = gens->len;
	nb_gens = l + 1;
	elt_size = n * n;
	Gens = NEW_int(nb_gens * elt_size);
	for (h = 0; h < nb_gens; h++) {
		if (h < l) {
			Elt = gens->ith(h);
			for (i = 0; i < n * n; i++) {
				Gens[h * elt_size + i] = Elt[i];
			}
		}
		else {
			for (i = 0; i < n * n; i++) {
				Gens[h * elt_size + i] = 0;
			}
			alpha = F->primitive_root();
			for (i = 0; i < n; i++) {
				Gens[h * elt_size + i * n + i] = alpha;
			}
		}
	}
	if (f_vv) {
		for (h = 0; h < nb_gens; h++) {
			cout << "Generator " << h << ":" << endl;
			Int_matrix_print(Gens + h * elt_size, n, n);
		}
		
	}
	FREE_OBJECT(A);
	if (f_v) {
		cout << "action_global::compute_generators_GL_n_q done" << endl;
	}
}



// callbacks for Schreier Sims:


	int f_generator_orthogonal_siegel = TRUE;
	int f_generator_orthogonal_reflection = TRUE;
	int f_generator_orthogonal_similarity = TRUE;
	int f_generator_orthogonal_semisimilarity = TRUE;


void action_global::set_orthogonal_group_type(int f_siegel,
		int f_reflection,
		int f_similarity,
		int f_semisimilarity)
{
	f_generator_orthogonal_siegel = f_siegel;
	f_generator_orthogonal_reflection = f_reflection;
	f_generator_orthogonal_similarity = f_similarity;
	f_generator_orthogonal_semisimilarity = f_semisimilarity;
}

int action_global::get_orthogonal_group_type_f_reflection()
{
	return f_generator_orthogonal_reflection;
}



#if 0
void test_matrix_group(int k, int q, int f_semilinear, int verbose_level)
{
	action A;
	finite_field *F;
	int f_basis = TRUE;
	vector_ge *nice_gens;

	F = NEW_OBJECT(finite_field);
	F->init(q, 0);
	A.init_projective_group(k, F, f_semilinear, f_basis, TRUE /* f_init_sims */,
			nice_gens,
			verbose_level);
	FREE_OBJECT(nice_gens);
	FREE_OBJECT(F);
}
#endif

void action_global::lift_generators(
		data_structures_groups::vector_ge *gens_in,
		data_structures_groups::vector_ge *&gens_out,
	action *Aq, field_theory::subfield_structure *S, int n,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int *EltQ;
	int *Eltq;
	int *Mtx;
	int nb_gens, m, t;


	if (f_v) {
		cout << "action_global::lift_generators" << endl;
	}

	nb_gens = gens_in->len;

	m = n / S->s;

	gens_out = NEW_OBJECT(data_structures_groups::vector_ge);

	Eltq = NEW_int(Aq->elt_size_in_int);
	Mtx = NEW_int(n * n);

	if (f_v) {
		cout << "action_global::lift_generators lifting generators" << endl;
	}
	gens_out->init(Aq, verbose_level - 2);
	gens_out->allocate(nb_gens, verbose_level - 2);
	for (t = 0; t < nb_gens; t++) {
		if (f_vv) {
			cout << "lift_generators " << t << " / " << nb_gens << endl;
			}
		EltQ = gens_in->ith(t);
		S->lift_matrix(EltQ, m, Mtx, 0 /* verbose_level */);
		if (f_vv) {
			cout << "action_global::lift_generators lifted matrix:" << endl;
			Int_matrix_print(Mtx, n, n);
			}
		Aq->make_element(Eltq, Mtx, 0 /*verbose_level - 4 */);
		if (f_vv) {
			cout << "action_global::lift_generators after make_element:" << endl;
			Aq->element_print_quick(Eltq, cout);
			}
		Aq->element_move(Eltq, gens_out->ith(t), 0);
		if (f_vv) {
			cout << "action_global::lift_generators " << t << " / "
					<< nb_gens << " done" << endl;
			}
		}
	FREE_int(Eltq);
	FREE_int(Mtx);
	if (f_v) {
		cout << "action_global::lift_generators done" << endl;
	}
}

void action_global::retract_generators(
		data_structures_groups::vector_ge *gens_in,
		data_structures_groups::vector_ge *&gens_out,
	action *AQ, field_theory::subfield_structure *S, int n,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int *EltQ;
	int *Eltq;
	int *Mtx;
	int nb_gens, m, t;


	if (f_v) {
		cout << "action_global::retract_generators" << endl;
	}

	nb_gens = gens_in->len;

	m = n / S->s;

	gens_out = NEW_OBJECT(data_structures_groups::vector_ge);

	EltQ = NEW_int(AQ->elt_size_in_int);
	Mtx = NEW_int(m * m);

	if (f_v) {
		cout << "action_global::retract_generators retracting generators" << endl;
	}
	gens_out->init(AQ, verbose_level - 2);
	gens_out->allocate(nb_gens, verbose_level - 2);
	for (t = 0; t < nb_gens; t++) {
		if (f_vv) {
			cout << "action_global::retract_generators " << t
					<< " / " << nb_gens << endl;
		}
		Eltq = gens_in->ith(t);
		S->retract_matrix(Eltq, n, Mtx, m, 0 /* verbose_level */);
		if (f_vv) {
			cout << "action_global::retract_generators retracted matrix:" << endl;
			Int_matrix_print(Mtx, m, m);
		}
		AQ->make_element(EltQ, Mtx, 0 /*verbose_level - 4*/);
		if (f_vv) {
			cout << "action_global::retract_generators after make_element:" << endl;
			AQ->element_print_quick(EltQ, cout);
		}
		AQ->element_move(EltQ, gens_out->ith(t), 0);
		if (f_vv) {
			cout << "action_global::retract_generators " << t
					<< " / " << nb_gens << " done" << endl;
		}
	}
	FREE_int(EltQ);
	FREE_int(Mtx);
	if (f_v) {
		cout << "action_global::retract_generators done" << endl;
	}
}

void action_global::lift_generators_to_subfield_structure(
	int n, int s, 
	field_theory::subfield_structure *S,
	action *Aq, action *AQ, 
	groups::strong_generators *&Strong_gens,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int q, Q, m;
	field_theory::finite_field *Fq;
	//finite_field *FQ;
	groups::sims *Sims;
	number_theory::number_theory_domain NT;

	if (f_v) {
		cout << "action_global::lift_generators_to_subfield_structure" << endl;
	}
	Fq = S->Fq;
	//FQ = S->FQ;
	q = Fq->q;
	Q = NT.i_power_j(q, s);
	m = n / s;
	if (m * s != n) {
		cout << "action_global::lift_generators_to_subfield_structure "
				"s must divide n" << endl;
		exit(1);
	}
	if (f_v) {
		cout << "action_global::lift_generators_to_subfield_structure "
				"creating subfield structure" << endl;
	}
	if (f_v) {
		cout << "n=" << n << endl;
		cout << "s=" << s << endl;
		cout << "m=" << m << endl;
		cout << "q=" << q << endl;
		cout << "Q=" << Q << endl;
	}

	ring_theory::longinteger_object order_GLmQ;
	ring_theory::longinteger_object target_go;
	ring_theory::longinteger_domain D;
	int r;

	AQ->group_order(order_GLmQ);
	

	if (f_v) {
		cout << "action_global::lift_generators_to_subfield_structure "
				"order of GL(m,Q) = " << order_GLmQ << endl;
	}
	D.integral_division_by_int(order_GLmQ, 
		q - 1, target_go, r);
	if (f_v) {
		cout << "action_global::lift_generators_to_subfield_structure "
				"target_go = " << target_go << endl;
	}



	data_structures_groups::vector_ge *gens;
	data_structures_groups::vector_ge *gens1;


	gens = AQ->Strong_gens->gens;


	if (f_v) {
		cout << "action_global::lift_generators_to_subfield_structure "
				"before lift_generators" << endl;
	}
	lift_generators(gens, gens1, Aq, S, n, verbose_level);

	if (f_v) {
		cout << "action_global::lift_generators_to_subfield_structure "
				"after lift_generators" << endl;
	}


	if (f_v) {
		cout << "action_global::lift_generators_to_subfield_structure "
				"creating lifted group:" << endl;
	}
	//Aq->group_order(target_go);
	Sims = Aq->create_sims_from_generators_with_target_group_order(
		gens1, 
		target_go, 
		0 /* verbose_level */);

#if 0
	Sims = A1->create_sims_from_generators_without_target_group_order(
		gens1, MINIMUM(2, verbose_level - 3));
#endif

	if (f_v) {
		cout << "action_global::lift_generators_to_subfield_structure "
				"creating lifted group done" << endl;
	}

	ring_theory::longinteger_object go;

	Sims->group_order(go);

	if (f_v) {
		cout << "go=" << go << endl;
	}


	Strong_gens = NEW_OBJECT(groups::strong_generators);

	Strong_gens->init_from_sims(Sims, 0 /* verbose_level */);
	if (f_vv) {
		cout << "action_global::lift_generators_to_subfield_structure "
				"strong generators are:" << endl;
		Strong_gens->print_generators(cout);
	}


	FREE_OBJECT(gens1);
	FREE_OBJECT(Sims);
	if (f_v) {
		cout << "action_global::lift_generators_to_subfield_structure done" << endl;
	}
}


void action_global::perm_print_cycles_sorted_by_length(ostream &ost,
		int degree, int *perm, int verbose_level)
{
	perm_print_cycles_sorted_by_length_offset(ost,
			degree, perm, 0, FALSE, TRUE, verbose_level);
}

void action_global::perm_print_cycles_sorted_by_length_offset(ostream &ost,
	int degree, int *perm, int offset,
	int f_do_it_anyway_even_for_big_degree,
	int f_print_cycles_of_length_one, int verbose_level)
{
	int nb_gens = 1;
	int i;
	data_structures_groups::vector_ge Gens;
	action *A;
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	int f_big = FALSE;
	int f_doit = TRUE;
	
	if (f_v) {
		cout << "action_global::perm_print_cycles_sorted_by_length, "
				"degree=" << degree << endl;
	}
	
	if (degree > 500) {
		f_big = TRUE;
	}
	A = NEW_OBJECT(action);
	int f_no_base = FALSE;
	
	A->init_permutation_group(degree, f_no_base, 0/*verbose_level*/);
	Gens.init(A, verbose_level - 2);
	Gens.allocate(nb_gens, verbose_level - 2);
	for (i = 0; i < nb_gens; i++) {
		Gens.copy_in(i, perm + i * degree);
	}
	if (f_vv) {
		Gens.print(cout);
	}
	
	groups::schreier S;
	
	S.init(A, verbose_level - 2);
	S.init_generators(Gens, verbose_level - 2);
	S.compute_all_point_orbits(verbose_level);
	if (f_v) {
		cout << "after S.compute_all_point_orbits, "
				"nb_orbits=" << S.nb_orbits << endl;
	}
	//S.print_orbit_lengths(cout);
	//S.print_orbit_length_distribution(ost);

	int j, f, l, length, F, L, h, a, b, m, orbit_idx;
	int *orbit_len_sorted;
	int *sorting_perm;
	int *sorting_perm_inv;
	int nb_types;
	int *type_first;
	int *type_len;
	data_structures::sorting Sorting;
	
	Sorting.int_vec_classify(S.nb_orbits, S.orbit_len, orbit_len_sorted,
		sorting_perm, sorting_perm_inv, 
		nb_types, type_first, type_len);

#if 0
	ost << "permutation of degree " << degree << " with "
			<< S.nb_orbits << " orbits: " << endl;
	for (i = 0; i < nb_types; i++) {
		f = type_first[i];
		l = type_len[i];
		length = orbit_len_sorted[f];
		if (l > 1) {
			ost << l << " \\times ";
			}
		ost << length;
		if (i < nb_types - 1)
			ost << ", ";
		}
	ost << endl;
	ost << "cycles in increasing length:" << endl;
#endif
	if (f_big) {
		for (i = 0; i < nb_types; i++) {
			f = type_first[i];
			l = type_len[i];
			length = orbit_len_sorted[f];
			ost << l << " cycles of length " << length << endl;
		}
	}
	if (f_big && !f_do_it_anyway_even_for_big_degree) {
		f_doit = FALSE;
	}
	if (f_doit) {
		for (i = 0; i < nb_types; i++) {
			f = type_first[i];
			l = type_len[i];
			length = orbit_len_sorted[f];
			if (length == 1 && !f_print_cycles_of_length_one) {
				continue;
			}
			for (j = 0; j < l; j++) {
				orbit_idx = sorting_perm_inv[f + j];
				//ost << "orbit " << orbit_idx << ": ";
				F = S.orbit_first[orbit_idx];
				L = S.orbit_len[orbit_idx];
				m = S.orbit[F];
				for (h = 1; h < L; h++) {
					if (S.orbit[F + h] < m)
						m = S.orbit[F + h];
				}
				// now m is the least lement in the orbit
				ost << "(";
				a = m;
				ost << (a + offset);
				while (TRUE) {
					b = perm[a];
					if (b == m)
						break;
					ost << ", " << (b + offset);
					a = b;
				}
				ost << ")";
				if (length > 20) {
					//ost << endl;
				}
			} // next j
			//ost << endl;
		} // next i
	} // if
	//ost << "done" << endl;

#if 0
	classify C;

	C.init(S.orbit_len, S.nb_orbits, FALSE, 0);
	ost << " cycle type: ";
	C.print_file(ost, TRUE /* f_backwards */);
#endif

	FREE_int(orbit_len_sorted);
	FREE_int(sorting_perm);
	FREE_int(sorting_perm_inv);
	FREE_int(type_first);
	FREE_int(type_len);
	
	FREE_OBJECT(A);
}



action *action_global::init_direct_product_group_and_restrict(
		groups::matrix_group *M1, groups::matrix_group *M2, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	action *A_direct_product;
	action *Adp;
	groups::direct_product *P;
	long int *points;
	int nb_points;
	int i;

	if (f_v) {
		cout << "action_global::init_direct_product_group_and_restrict" << endl;
		cout << "M1=" << M1->label << endl;
		cout << "M2=" << M2->label << endl;
	}
	A_direct_product = NEW_OBJECT(action);
	A_direct_product = init_direct_product_group(M1, M2, verbose_level);
	if (f_v) {
		cout << "action_global::init_direct_product_group_and_restrict "
				"after A_direct_product->init_direct_product_group" << endl;
	}

	P = A_direct_product->G.direct_product_group;
	nb_points = P->degree_of_product_action;
	points = NEW_lint(nb_points);
	for (i = 0; i < nb_points; i++) {
		points[i] = P->perm_offset_i[2] + i;
	}

	if (f_v) {
		cout << "action_global::init_direct_product_group_and_restrict "
				"before A_direct_product->restricted_action" << endl;
	}
	Adp = A_direct_product->restricted_action(points, nb_points,
			verbose_level);
	Adp->f_is_linear = FALSE;


	if (f_v) {
		cout << "action_global::init_direct_product_group_and_restrict "
				"after A_direct_product->restricted_action" << endl;
	}
	return Adp;
}

action *action_global::init_direct_product_group(
		groups::matrix_group *M1, groups::matrix_group *M2,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);
	groups::direct_product *P;
	action *A;

	if (f_v) {
		cout << "action_global::init_direct_product_group" << endl;
		cout << "M1=" << M1->label << endl;
		cout << "M2=" << M2->label << endl;
	}

	A = NEW_OBJECT(action);
	P = NEW_OBJECT(groups::direct_product);



	A->type_G = direct_product_t;
	A->G.direct_product_group = P;
	A->f_allocated = TRUE;

	if (f_v) {
		cout << "action_global::init_direct_product_group "
				"before P->init" << endl;
	}
	P->init(M1, M2, verbose_level);
	if (f_v) {
		cout << "action_global::init_direct_product_group "
				"after P->init" << endl;
	}

	A->f_is_linear = FALSE;
	A->dimension = 0;


	A->low_level_point_size = 0;
	if (f_v) {
		cout << "action_global::init_direct_product_group low_level_point_size="
			<< A->low_level_point_size<< endl;
	}

	A->label.assign(P->label);
	A->label_tex.assign(P->label_tex);


	if (f_v) {
		cout << "action_global::init_direct_product_group "
				"label=" << A->label << endl;
	}

	A->degree = P->degree_overall;
	A->make_element_size = P->make_element_size;

	A->ptr = NEW_OBJECT(action_pointer_table);
	A->ptr->init_function_pointers_direct_product_group();

	A->elt_size_in_int = P->elt_size_int;
	A->coded_elt_size_in_char = P->char_per_elt;
	A->allocate_element_data();




	A->degree = P->degree_overall;
	if (f_v) {
		cout << "action_global::init_direct_product_group "
				"degree=" << A->degree << endl;
	}

	A->Stabilizer_chain = NEW_OBJECT(stabilizer_chain_base_data);
	A->Stabilizer_chain->allocate_base_data(A, P->base_length, verbose_level);

	if (f_v) {
		cout << "action_global::init_direct_product_group "
				"base_len=" << A->base_len() << endl;
	}


	Lint_vec_copy(P->the_base, A->get_base(), A->base_len());
	Int_vec_copy(P->the_transversal_length,
			A->get_transversal_length(), A->base_len());

	int *gens_data;
	int gens_size;
	int gens_nb;

	if (f_v) {
		cout << "action_global::init_direct_product_group "
				"before W->make_strong_generators_data" << endl;
	}
	P->make_strong_generators_data(gens_data,
			gens_size, gens_nb, verbose_level - 1);
	if (f_v) {
		cout << "action_global::init_direct_product_group "
				"after W->make_strong_generators_data" << endl;
	}
	A->Strong_gens = NEW_OBJECT(groups::strong_generators);
	if (f_v) {
		cout << "action_global::init_direct_product_group "
				"before A->Strong_gens->init_from_data" << endl;
	}

	data_structures_groups::vector_ge *nice_gens;

	A->Strong_gens->init_from_data(A,
			gens_data, gens_nb, gens_size,
			A->get_transversal_length(),
			nice_gens,
			verbose_level - 1);
	if (f_v) {
		cout << "action_global::init_direct_product_group "
				"after A->Strong_gens->init_from_data" << endl;
	}
	FREE_OBJECT(nice_gens);
	A->f_has_strong_generators = TRUE;
	FREE_int(gens_data);

	groups::sims *S;

	S = NEW_OBJECT(groups::sims);

	S->init(A, verbose_level - 2);
	if (f_v) {
		cout << "action_global::init_direct_product_group "
				"before S->init_generators" << endl;
	}
	S->init_generators(*A->Strong_gens->gens, verbose_level);
	if (f_v) {
		cout << "action_global::init_direct_product_group "
				"after S->init_generators" << endl;
	}
	if (f_v) {
		cout << "action_global::init_direct_product_group "
				"before S->compute_base_orbits_known_length" << endl;
	}
	S->compute_base_orbits_known_length(A->get_transversal_length(), verbose_level);
	if (f_v) {
		cout << "action_global::init_direct_product_group "
				"after S->compute_base_orbits_known_length" << endl;
	}


	if (f_v) {
		cout << "action_global::init_direct_product_group "
				"before init_sims_only" << endl;
	}

	A->init_sims_only(S, verbose_level);

	if (f_v) {
		cout << "action_global::init_direct_product_group "
				"after init_sims_only" << endl;
	}

	A->compute_strong_generators_from_sims(0/*verbose_level - 2*/);

	if (f_v) {
		cout << "action_global::init_direct_product_group, finished setting up "
				<< A->label;
		cout << ", a permutation group of degree " << A->degree << " ";
		cout << "and of order ";
		A->print_group_order(cout);
		cout << endl;
		//cout << "make_element_size=" << make_element_size << endl;
		//cout << "base_len=" << base_len << endl;
		//cout << "f_semilinear=" << f_semilinear << endl;
	}
	return A;
}



void action_global::compute_decomposition_based_on_orbits(
		geometry::projective_space *P,
		groups::schreier *Sch1, groups::schreier *Sch2,
		geometry::incidence_structure *&Inc,
		data_structures::partitionstack *&Stack, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "action_global::compute_decomposition_based_on_orbits" << endl;
	}

	data_structures::partitionstack *S1;
	data_structures::partitionstack *S2;


	S1 = NEW_OBJECT(data_structures::partitionstack);
	S2 = NEW_OBJECT(data_structures::partitionstack);

	if (f_v) {
		cout << "action_global::compute_decomposition_based_on_orbits "
				"before S1->allocate" << endl;
	}
	S1->allocate(P->N_points, 0 /* verbose_level */);
	S2->allocate(P->N_lines, 0 /* verbose_level */);

	if (f_v) {
		cout << "action_global::compute_decomposition_based_on_orbits "
				"before Sch1->get_orbit_partition" << endl;
	}
	Sch1->get_orbit_partition(*S1, 0 /*verbose_level*/);
	if (f_v) {
		cout << "action_global::compute_decomposition_based_on_orbits "
				"before Sch2->get_orbit_partition" << endl;
	}
	Sch2->get_orbit_partition(*S2, 0 /*verbose_level*/);
	if (f_v) {
		cout << "action_global::compute_decomposition_based_on_orbits "
				"after Sch2->get_orbit_partition" << endl;
	}




	if (f_v) {
		cout << "action_global::compute_decomposition_based_on_orbits "
				"before P->compute_decomposition" << endl;
	}
	P->compute_decomposition(S1, S2, Inc, Stack, verbose_level);

	FREE_OBJECT(S1);
	FREE_OBJECT(S2);

	if (f_v) {
		cout << "action_global::compute_decomposition_based_on_orbits done" << endl;
	}
}


void action_global::compute_decomposition_based_on_orbit_length(
		geometry::projective_space *P,
		groups::schreier *Sch1, groups::schreier *Sch2,
		geometry::incidence_structure *&Inc,
		data_structures::partitionstack *&Stack, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "action_global::compute_decomposition_based_on_orbit_length" << endl;
	}

	int *L1, *L2;

	Sch1->get_orbit_length(L1, 0 /* verbose_level */);
	Sch2->get_orbit_length(L2, 0 /* verbose_level */);

	data_structures::tally T1, T2;

	T1.init(L1, Sch1->A->degree, FALSE, 0);

	T2.init(L2, Sch2->A->degree, FALSE, 0);



	if (f_v) {
		cout << "action_global::compute_decomposition_based_on_orbit_length "
				"before P->compute_decomposition" << endl;
	}
	P->compute_decomposition_based_on_tally(&T1, &T2, Inc, Stack, verbose_level);


	FREE_int(L1);
	FREE_int(L2);

	if (f_v) {
		cout << "action_global::compute_decomposition_based_on_orbit_length done" << endl;
	}
}




void action_global::orbits_on_equations(
		action *A,
		ring_theory::homogeneous_polynomial_domain *HPD,
	int *The_equations, int nb_equations, groups::strong_generators *gens,
	actions::action *&A_on_equations, groups::schreier *&Orb,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "action_global::orbits_on_equations" << endl;
	}

	A_on_equations = NEW_OBJECT(action);

	if (f_v) {
		cout << "action_global::orbits_on_equations "
				"creating the induced action on the equations:" << endl;
	}
	A_on_equations->induced_action_on_homogeneous_polynomials_given_by_equations(
		A,
		HPD,
		The_equations, nb_equations,
		FALSE /* f_induce_action */, NULL /* sims *old_G */,
		verbose_level);
	if (f_v) {
		cout << "action_global::orbits_on_equations "
				"The induced action on the equations has been created, "
				"degree = " << A_on_equations->degree << endl;
	}

	if (f_v) {
		cout << "action_global::orbits_on_equations "
				"computing orbits on the equations:" << endl;
	}
	Orb = gens->orbits_on_points_schreier(A_on_equations,
			verbose_level - 2);

	if (FALSE) {
		cout << "action_global::orbits_on_equations "
				"We found " << Orb->nb_orbits
				<< " orbits on the equations:" << endl;
		Orb->print_and_list_orbits_tex(cout);
	}

	if (f_v) {
		cout << "action_global::orbits_on_equations done" << endl;
	}
}





void callback_choose_random_generator_orthogonal(int iteration,
	int *Elt, void *data, int verbose_level)
{
	//verbose_level += 5;
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "action_global::callback_choose_random_generator_orthogonal "
				"iteration=" << iteration << endl;
		}

	groups::schreier_sims *ss = (groups::schreier_sims *) data;
	action *A = ss->GA;
	action *subaction = ss->KA;
	groups::matrix_group *M;
#if 0
	int f_siegel = TRUE;
	int f_reflection = TRUE;
	int f_similarity = TRUE;
	int f_semisimilarity = TRUE;
#endif

	induced_actions::action_on_orthogonal *AO;
	orthogonal_geometry::orthogonal *O;

	AO = A->G.AO;
	O = AO->O;

	M = subaction->G.matrix_grp;
	if (f_v) {
		cout << "action_global::callback_choose_random_generator_orthogonal "
				"iteration=" << iteration
				<< " before M->orthogonal_group_random_generator"
				<< endl;
	}
	M->orthogonal_group_random_generator(ss->GA, O,
		f_generator_orthogonal_siegel,
		f_generator_orthogonal_reflection,
		f_generator_orthogonal_similarity,
		f_generator_orthogonal_semisimilarity,
		Elt, verbose_level - 2);
	//M->GL_invert_internal(Elt, Elt + M->elt_size_int_half, 0);
	if (f_v) {
		cout << "action_global::callback_choose_random_generator_orthogonal "
				"iteration=" << iteration
				<< " after M->orthogonal_group_random_generator"
				<< endl;
	}

	if (f_v) {
		cout << "action_global::callback_choose_random_generator_orthogonal "
				"iteration=" << iteration << " done" << endl;
	}
}


}}}

