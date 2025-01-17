/*
 * geometric_object_create.cpp
 *
 *  Created on: Nov 9, 2019
 *      Author: anton
 */





#include "foundations.h"

using namespace std;


namespace orbiter {
namespace layer1_foundations {
namespace geometry {


geometric_object_create::geometric_object_create()
{
	Descr = NULL;

	//std::string fname;
	nb_pts = 0;
	Pts = NULL;

	//std::string label_txt;
	//std::string label_tex;

}

geometric_object_create::~geometric_object_create()
{
	if (Pts) {
		FREE_lint(Pts);
	}
}

void geometric_object_create::init(geometric_object_description *Descr,
		projective_space *P, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	number_theory::number_theory_domain NT;


	if (f_v) {
		cout << "geometric_object_create::init" << endl;
	}
	Descr->P = P;
	geometric_object_create::Descr = Descr;

	field_theory::finite_field *F;

	F = P->F;


	if (Descr->f_hyperoval) {
		P->Arc_in_projective_space->create_hyperoval(
				Descr->f_translation,
				Descr->translation_exponent,
				Descr->f_Segre,
				Descr->f_Payne,
				Descr->f_Cherowitzo,
				Descr->f_OKeefe_Penttila,
				label_txt,
				label_tex,
				nb_pts, Pts,
			verbose_level);

		//F->export_magma(3, Pts, nb_pts, fname);
		//F->export_gap(3, Pts, nb_pts, fname);

	}
	else if (Descr->f_subiaco_oval) {
		P->Arc_in_projective_space->create_subiaco_oval(
				Descr->f_short,
				label_txt,
				label_tex,
			nb_pts, Pts,
			verbose_level);

		//F->export_magma(3, Pts, nb_pts, fname);
		//F->export_gap(3, Pts, nb_pts, fname);

	}
	else if (Descr->f_subiaco_hyperoval) {
		P->Arc_in_projective_space->create_subiaco_hyperoval(
				label_txt,
				label_tex,
			nb_pts, Pts,
			verbose_level);


		//F->export_magma(3, Pts, nb_pts, fname);
		//F->export_gap(3, Pts, nb_pts, fname);

	}
#if 0
	else if (Descr->f_adelaide_hyperoval) {

		finite_field *FQ;
		subfield_structure *S;

		FQ = NEW_OBJECT(finite_field);
		FQ->init_override_polynomial(Descr->Q, Descr->poly_Q, 0);

		S = NEW_OBJECT(subfield_structure);
		S->init(FQ, F, verbose_level);

		S->create_adelaide_hyperoval(
			fname, nb_pts, Pts,
			verbose_level);


		F->export_magma(3, Pts, nb_pts, fname);
		F->export_gap(3, Pts, nb_pts, fname);


		FREE_OBJECT(S);
		FREE_OBJECT(FQ);
	}
#endif
	else if (Descr->f_BLT_database) {

		knowledge_base K;

		K.retrieve_BLT_set_from_database(F,
				FALSE /* f_embedded */,
				Descr->BLT_database_k,
				label_txt,
				label_tex,
			nb_pts, Pts,
			verbose_level);
	}
	else if (Descr->f_BLT_database_embedded) {

		knowledge_base K;

		K.retrieve_BLT_set_from_database_embedded(F,
				Descr->BLT_database_embedded_k,
				label_txt,
				label_tex,
			nb_pts, Pts,
			verbose_level);
	}
#if 0
	else if (f_BLT_Linear) {
		create_BLT(f_BLT_in_PG /* f_embedded */, F,
			TRUE /* f_Linear */,
			FALSE /* f_Fisher */,
			FALSE /* f_Mondello */,
			FALSE /* f_FTWKB */,
			f_poly, poly,
			f_poly_Q, poly_Q,
			fname, nb_pts, Pts,
			verbose_level);
	}
	else if (f_BLT_Fisher) {
		create_BLT(f_BLT_in_PG /* f_embedded */, q,
			FALSE /* f_Linear */,
			TRUE /* f_Fisher */,
			FALSE /* f_Mondello */,
			FALSE /* f_FTWKB */,
			f_poly, poly,
			f_poly_Q, poly_Q,
			fname, nb_pts, Pts,
			verbose_level);
	}
	else if (f_BLT_Mondello) {
		create_BLT(f_BLT_in_PG /* f_embedded */, q,
			FALSE /* f_Linear */,
			FALSE /* f_Fisher */,
			TRUE /* f_Mondello */,
			FALSE /* f_FTWKB */,
			f_poly, poly,
			f_poly_Q, poly_Q,
			fname, nb_pts, Pts,
			verbose_level);
	}
	else if (f_BLT_FTWKB) {
		create_BLT(f_BLT_in_PG /* f_embedded */, q,
			FALSE /* f_Linear */,
			FALSE /* f_Fisher */,
			FALSE /* f_Mondello */,
			TRUE /* f_FTWKB */,
			f_poly, poly,
			f_poly_Q, poly_Q,
			fname, nb_pts, Pts,
			verbose_level);
	}
#endif
	else if (Descr->f_elliptic_quadric_ovoid) {
		P->create_elliptic_quadric_ovoid(
				label_txt,
				label_tex,
			nb_pts, Pts,
			verbose_level);
	}
	else if (Descr->f_ovoid_ST) {
		P->create_ovoid_ST(
				label_txt,
				label_tex,
			nb_pts, Pts,
			verbose_level);
	}


	else if (Descr->f_Baer_substructure) {

		P->create_Baer_substructure(
			Pts, nb_pts,
			label_txt,
			label_tex,
			verbose_level);

	}
	else if (Descr->f_orthogonal) {
		F->create_orthogonal(
				Descr->orthogonal_epsilon, P->n,
				label_txt,
				label_tex,
			nb_pts, Pts,
			verbose_level);
		// calls choose_anisotropic_form if necessary
	}
	else if (Descr->f_hermitian) {
		F->create_hermitian(P->n,
				label_txt,
				label_tex,
			nb_pts, Pts,
			verbose_level);
		// creates a hermitian
	}
	else if (Descr->f_cuspidal_cubic) {
		P->create_cuspidal_cubic(
				label_txt,
				label_tex,
			nb_pts, Pts,
			verbose_level);
	}
	else if (Descr->f_twisted_cubic) {
		P->create_twisted_cubic(
				label_txt,
				label_tex,
			nb_pts, Pts,
			verbose_level);
	}
	else if (Descr->f_elliptic_curve) {
		P->create_elliptic_curve(
				Descr->elliptic_curve_b, Descr->elliptic_curve_c,
				label_txt,
				label_tex,
			nb_pts, Pts,
			verbose_level);
	}

#if 0
	else if (Descr->f_ttp_code) {

		if (!Descr->f_Q) {
			cout << "please specify the field order "
					"using the option -Q <Q>" << endl;
			exit(1);
			}

		finite_field *FQ;

		FQ = NEW_OBJECT(finite_field);
		FQ->init_override_polynomial(Descr->Q, Descr->poly_Q, 0);
		FQ->create_ttp_code(F,
				Descr->f_ttp_construction_A, Descr->f_ttp_hyperoval, Descr->f_ttp_construction_B,
			fname, nb_pts, Pts,
			verbose_level);
		FREE_OBJECT(FQ);
	}
#endif
	else if (Descr->f_unital_XXq_YZq_ZYq) {


		P->create_unital_XXq_YZq_ZYq(
				label_txt,
				label_tex,
			nb_pts, Pts,
			verbose_level);

	}
#if 0
	else if (Descr->f_desarguesian_line_spread_in_PG_3_q) {

		if (!Descr->f_Q) {
			cout << "please specify the field order "
					"using the option -Q <Q>" << endl;
			exit(1);
			}

		finite_field *FQ;

		FQ = NEW_OBJECT(finite_field);
		FQ->init_override_polynomial(Descr->Q, Descr->poly_Q, 0);

		FQ->create_desarguesian_line_spread_in_PG_3_q(F,
				Descr->f_embedded_in_PG_4_q,
			fname, nb_pts, Pts,
			verbose_level);
		FREE_OBJECT(FQ);

	}
#endif

#if 0
	else if (Descr->f_Buekenhout_Metz) {

		finite_field *FQ;
		geometry_global Gg;

		FQ = NEW_OBJECT(finite_field);
		FQ->init_override_polynomial(Descr->Q, Descr->poly_Q, 0);

		Gg.create_Buekenhout_Metz(F, FQ,
				Descr->f_classical, Descr->f_Uab, Descr->parameter_a, Descr->parameter_b,
			fname, nb_pts, Pts,
			verbose_level);

	}
#endif

	else if (Descr->f_whole_space) {
		P->create_whole_space(
				label_txt,
				label_tex,
			nb_pts, Pts,
			verbose_level);
	}
	else if (Descr->f_hyperplane) {
		P->create_hyperplane(
				Descr->pt,
				label_txt,
				label_tex,
			nb_pts, Pts,
			verbose_level);
	}
	else if (Descr->f_segre_variety) {
		F->create_segre_variety(
				Descr->segre_variety_a, Descr->segre_variety_b,
				label_txt,
				label_tex,
			nb_pts, Pts,
			verbose_level);
	}
	else if (Descr->f_Maruta_Hamada_arc) {
		P->Arc_in_projective_space->create_Maruta_Hamada_arc(
				label_txt,
				label_tex,
			nb_pts, Pts,
			verbose_level);
	}
	else if (Descr->f_projective_variety) {


		ring_theory::homogeneous_polynomial_domain *HPD;


		HPD = orbiter_kernel_system::Orbiter->get_object_of_type_polynomial_ring(Descr->projective_variety_ring_label);

		HPD->create_projective_variety(
				Descr->variety_label_txt,
				Descr->variety_label_tex,
				Descr->variety_coeffs,
				label_txt,
				label_tex,
				nb_pts, Pts,
				verbose_level);

	}
	else if (Descr->f_intersection_of_zariski_open_sets) {

		ring_theory::homogeneous_polynomial_domain *HPD;


		HPD = orbiter_kernel_system::Orbiter->get_object_of_type_polynomial_ring(Descr->intersection_of_zariski_open_sets_ring_label);

		HPD->create_intersection_of_zariski_open_sets(
				Descr->variety_label_txt,
				Descr->variety_label_tex,
				Descr->Variety_coeffs,
				label_txt,
				label_tex,
				nb_pts, Pts,
				verbose_level);
	}
	else if (Descr->f_number_of_conditions_satisfied) {

		ring_theory::homogeneous_polynomial_domain *HPD;


		HPD = orbiter_kernel_system::Orbiter->get_object_of_type_polynomial_ring(Descr->number_of_conditions_satisfied_ring_label);

		HPD->number_of_conditions_satisfied(
				Descr->variety_label_txt,
				Descr->variety_label_tex,
				Descr->Variety_coeffs,
				Descr->number_of_conditions_satisfied_fname,
				label_txt,
				label_tex,
				nb_pts, Pts,
				verbose_level);
	}



	else if (Descr->f_projective_curve) {

		ring_theory::homogeneous_polynomial_domain *HPD;


		HPD = orbiter_kernel_system::Orbiter->get_object_of_type_polynomial_ring(Descr->projective_curve_ring_label);

		HPD->create_projective_curve(
				Descr->curve_label_txt,
				Descr->curve_label_tex,
				Descr->curve_coeffs,
				label_txt,
				label_tex,
				nb_pts, Pts,
				verbose_level);
	}

	else if (Descr->f_set) {

		Lint_vec_scan(Descr->set_text, Pts, nb_pts);

	}
	else {
		cout << "geometric_object_create::init nothing to create" << endl;
		exit(1);
	}


	if (f_v) {
		cout << "geometric_object_create::init created a set of size " << nb_pts << endl;
		Lint_vec_print_fully(cout, Pts, nb_pts);
		cout << endl;

		//lint_vec_print(cout, Pts, nb_pts);
		//cout << endl;
	}



	if (f_v) {
		cout << "geometric_object_create::init done" << endl;
	}
}



}}}



