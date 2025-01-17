/*
 * canonical_form_classifier.cpp
 *
 *  Created on: Apr 24, 2021
 *      Author: betten
 */




#include "orbiter.h"

using namespace std;

namespace orbiter {
namespace layer5_applications {
namespace projective_geometry {



canonical_form_classifier::canonical_form_classifier()
{
	Descr = NULL;
	Poly_ring = NULL;
	AonHPD = NULL;
	nb_objects_to_test = 0;
	CB = NULL;
	canonical_labeling_len = 0;
	alpha = NULL;
	gamma = NULL;

	SubC = NULL;

	Elt = NULL;
	eqn2 = NULL;

	//canonical_equation = NULL;
	//transporter_to_canonical_form = NULL;
	//longinteger_object go_eqn;

	CFS_table = NULL;
	counter = 0;
	Canonical_forms = NULL;
	Goi = NULL;

	Classification_of_quartic_curves = NULL;

	transversal = NULL;
	frequency = NULL;
	nb_types = 0;

}

canonical_form_classifier::~canonical_form_classifier()
{

}

void canonical_form_classifier::count_nb_objects_to_test(int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int cnt;

	if (f_v) {
		cout << "canonical_form_classifier::count_nb_objects_to_test" << endl;
	}

	nb_objects_to_test = 0;


	for (cnt = 0; cnt < Descr->nb_files; cnt++) {

		char str[1000];
		string fname;

		snprintf(str, sizeof(str), Descr->fname_mask.c_str(), cnt);
		fname.assign(str);

		data_structures::spreadsheet S;

		S.read_spreadsheet(fname, verbose_level);

		nb_objects_to_test += S.nb_rows - 1;

		if (f_v) {
			cout << "canonical_form_classifier::count_nb_objects_to_test "
					"file " << cnt << " / " << Descr->nb_files << " has  "
					<< S.nb_rows - 1 << " objects" << endl;
		}
	}

	if (f_v) {
		cout << "canonical_form_classifier::count_nb_objects_to_test "
				"nb_objects_to_test=" << nb_objects_to_test << endl;
	}
}


void canonical_form_classifier::classify(canonical_form_classifier_description *Descr,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);


	if (f_v) {
		cout << "canonical_form_classifier::classify";
		if (Descr->f_algorithm_nauty) {
			cout << " using nauty" << endl;
		}
		else if (Descr->f_algorithm_substructure) {
			cout << " using substructure" << endl;
		}
	}




	canonical_form_classifier::Descr = Descr;


	if (!Descr->f_degree) {
		cout << "canonical_form_classifier::classify "
				"please use -degree <d>  to specify the degree" << endl;
		exit(1);
	}
	if (!Descr->f_fname_base_out) {
		cout << "please use -fname_base_out" << endl;
		exit(1);
	}

	Poly_ring = NEW_OBJECT(ring_theory::homogeneous_polynomial_domain);
	Poly_ring->init(Descr->PA->F, Descr->PA->n + 1, Descr->degree, t_PART, verbose_level);


	AonHPD = NEW_OBJECT(induced_actions::action_on_homogeneous_polynomials);
	if (f_v) {
		cout << "canonical_form_classifier::classify "
				"before AonHPD->init" << endl;
	}
	AonHPD->init(Descr->PA->A, Poly_ring, verbose_level);
	if (f_v) {
		cout << "canonical_form_classifier::classify "
				"after AonHPD->init" << endl;
	}




	if (f_v) {
		cout << "canonical_form_classifier::classify "
				"before count_nb_objects_to_test" << endl;
	}

	count_nb_objects_to_test(verbose_level);


	if (f_v) {
		cout << "canonical_form_classifier::classify "
				"nb_objects_to_test=" << nb_objects_to_test << endl;
	}

	Elt = NEW_int(Descr->PA->A->elt_size_in_int);
	eqn2 = NEW_int(Poly_ring->get_nb_monomials());



	Canonical_forms = NEW_int(nb_objects_to_test * Poly_ring->get_nb_monomials());
	Goi = NEW_lint(nb_objects_to_test);

	if (Descr->f_algorithm_nauty) {
		if (f_v) {
			cout << "canonical_form_classifier::classify "
					"before classify_nauty" << endl;
		}
		classify_nauty(verbose_level);
		if (f_v) {
			cout << "canonical_form_classifier::classify "
					"after classify_nauty" << endl;
		}
	}
	else if (Descr->f_algorithm_substructure) {

		if (f_v) {
			cout << "canonical_form_classifier::classify "
					"before classify_with_substructure" << endl;
		}
		classify_with_substructure(verbose_level);
		if (f_v) {
			cout << "canonical_form_classifier::classify "
					"after classify_with_substructure" << endl;
		}
	}
	else {
		cout << "canonical_form_classifier::classify "
				"please select which algorithm to use" << endl;
		exit(1);
	}

	//FREE_int(eqn2);
	//FREE_int(Elt);
	//FREE_int(canonical_equation);
	//FREE_int(transporter_to_canonical_form);


	int i, j;

	cout << "canonical forms:" << endl;
	for (i = 0; i < nb_objects_to_test; i++) {
		cout << setw(2) << i << " : ";
		Int_vec_print(cout,
				Canonical_forms + i * Poly_ring->get_nb_monomials(),
				Poly_ring->get_nb_monomials());
		cout << " : " << Goi[i] << endl;
	}

	Classification_of_quartic_curves = NEW_OBJECT(data_structures::tally_vector_data);

	Classification_of_quartic_curves->init(Canonical_forms,
			nb_objects_to_test, Poly_ring->get_nb_monomials(),
			verbose_level);


	Classification_of_quartic_curves->get_transversal(
			transversal, frequency, nb_types, verbose_level);


	cout << "Classification of curves:" << endl;


	cout << "transversal:" << endl;
	Int_vec_print(cout, transversal, nb_types);
	cout << endl;

	//Classification_of_quartic_curves->print();

	for (i = 0; i < Classification_of_quartic_curves->nb_types; i++) {

		//h = int_vec_hash(Reps + i * data_set_sz, data_set_sz);

		cout << i << " : " << Classification_of_quartic_curves->Frequency[i] << " x ";
		Int_vec_print(cout,
				Classification_of_quartic_curves->Reps + i * Classification_of_quartic_curves->data_set_sz,
				Classification_of_quartic_curves->data_set_sz);
		cout << " : ";
		j = Classification_of_quartic_curves->sorting_perm_inv[Classification_of_quartic_curves->type_first[i]];
		cout << Goi[j] << " : ";
		Int_vec_print(cout,
				Classification_of_quartic_curves->sorting_perm_inv + Classification_of_quartic_curves->type_first[i],
				Classification_of_quartic_curves->Frequency[i]);
		cout << endl;
#if 0
		cout << "for elements ";
		int_vec_print(cout, sorting_perm_inv + type_first[i], Frequency[i]);
		cout << endl;
#endif
	}


	if (f_v) {
		cout << "canonical_form_classifier::classify "
				"before write_canonical_forms_csv" << endl;
	}
	write_canonical_forms_csv(
			Descr->fname_base_out,
			verbose_level);
	if (f_v) {
		cout << "canonical_form_classifier::classify "
				"after write_canonical_forms_csv" << endl;
	}


	if (f_v) {
		cout << "canonical_form_classifier::classify "
				"before generate_source_code" << endl;
	}

	generate_source_code(
			Descr->fname_base_out,
			Classification_of_quartic_curves,
			verbose_level);

	if (f_v) {
		cout << "canonical_form_classifier::classify "
				"after generate_source_code" << endl;
	}


	if (f_v) {
		cout << "canonical_form_classifier::classify done" << endl;
	}
}


void canonical_form_classifier::classify_nauty(int verbose_level)
{
	int f_v = (verbose_level >= 1);


	if (f_v) {
		cout << "canonical_form_classifier::classify_nauty" << endl;
	}


	CB = NEW_OBJECT(data_structures::classify_bitvectors);




	main_loop(verbose_level);

	if (f_v) {
		cout << "canonical_form_classifier::classify_nauty "
				"The number of isomorphism types is " << CB->nb_types << endl;
	}


}

void canonical_form_classifier::classify_with_substructure(int verbose_level)
{
	int f_v = (verbose_level >= 1);


	if (f_v) {
		cout << "canonical_form_classifier::classify_with_substructure, "
				"Descr->substructure_size=" << Descr->substructure_size << endl;
	}




	SubC = NEW_OBJECT(set_stabilizer::substructure_classifier);

	if (f_v) {
		cout << "canonical_form_classifier::classify_with_substructure "
				"before SubC->classify_substructures" << endl;
	}

	SubC->classify_substructures(
			Descr->fname_base_out,
			Descr->PA->A,
			Descr->PA->A,
			Descr->PA->A->Strong_gens,
			Descr->substructure_size,
			verbose_level - 3);

	if (f_v) {
		cout << "canonical_form_classifier::classify_with_substructure "
				"after SubC->classify_substructures" << endl;
		cout << "canonical_form_classifier::classify_with_substructure "
				"We found " << SubC->nb_orbits
				<< " orbits at level " << Descr->substructure_size << ":" << endl;
	}



	CFS_table = (canonical_form_substructure **) NEW_pvoid(nb_objects_to_test);




	if (f_v) {
		cout << "canonical_form_classifier::classify_with_substructure before main_loop" << endl;
	}
	main_loop(verbose_level);
	if (f_v) {
		cout << "canonical_form_classifier::classify_with_substructure after main_loop" << endl;
	}



	if (f_v) {
		cout << "canonical_form_classifier::classify_with_substructure done" << endl;
	}

}


void canonical_form_classifier::main_loop(int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int cnt;



	if (f_v) {
		cout << "canonical_form_classifier::main_loop" << endl;
	}

	string fname_case_out;


	counter = 0;

	for (cnt = 0; cnt < Descr->nb_files; cnt++) {
		char str[1000];
		string fname;
		int row;

		snprintf(str, sizeof(str), Descr->fname_mask.c_str(), cnt);
		fname.assign(str);

		data_structures::spreadsheet S;

		S.read_spreadsheet(fname, verbose_level);

		if (f_v) {
			cout << "canonical_form_classifier::classify_nauty "
					"S.nb_rows = " << S.nb_rows << endl;
			cout << "canonical_form_classifier::classify_nauty "
					"S.nb_cols = " << S.nb_cols << endl;
		}



		int idx_po, idx_so, idx_eqn, idx_pts, idx_bitangents;

		idx_po = S.find_column(Descr->column_label_po);
		idx_so = S.find_column(Descr->column_label_so);
		idx_eqn = S.find_column(Descr->column_label_eqn);
		idx_pts = S.find_column(Descr->column_label_pts);
		idx_bitangents = S.find_column(Descr->column_label_bitangents);

		for (row = 0; row < S.nb_rows - 1; row++, counter++) {

			if (f_v) {
				cout << "cnt = " << cnt << " / " << Descr->nb_files
						<< " row = " << row << " / " << S.nb_rows - 1 << endl;
			}

			snprintf(str, sizeof(str), "_cnt%d", counter);

			fname_case_out.assign(Descr->fname_base_out);
			fname_case_out.append(str);

			int t;
			int po, so;


			po = S.get_lint(row + 1, idx_po);
			so = S.get_lint(row + 1, idx_so);
			string eqn_txt;
			string pts_txt;
			string bitangents_txt;

			t = S.Table[(row + 1) * S.nb_cols + idx_eqn];
			if (S.tokens[t] == NULL) {
				cout << "canonical_form_classifier::classify_nauty token[t] == NULL" << endl;
			}
			eqn_txt.assign(S.tokens[t]);
			t = S.Table[(row + 1) * S.nb_cols + idx_pts];
			if (S.tokens[t] == NULL) {
				cout << "canonical_form_classifier::classify_nauty token[t] == NULL" << endl;
			}
			pts_txt.assign(S.tokens[t]);
			t = S.Table[(row + 1) * S.nb_cols + idx_bitangents];
			if (S.tokens[t] == NULL) {
				cout << "canonical_form_classifier::classify_nauty token[t] == NULL" << endl;
			}
			bitangents_txt.assign(S.tokens[t]);

			data_structures::string_tools ST;

			ST.remove_specific_character(eqn_txt, '\"');
			ST.remove_specific_character(pts_txt, '\"');
			ST.remove_specific_character(bitangents_txt, '\"');

			if (FALSE) {
				cout << "row = " << row << " eqn=" << eqn_txt
						<< " pts_txt=" << pts_txt << " =" << bitangents_txt << endl;
			}


			quartic_curve_object *Qco;

			Qco = NEW_OBJECT(quartic_curve_object);

			Qco->init(
					counter, po, so,
					eqn_txt,
					pts_txt, bitangents_txt,
					verbose_level);




			if (Descr->f_algorithm_nauty) {
				if (f_v) {
					cout << "canonical_form_classifier::main_loop "
							"before classify_curve_nauty" << endl;
				}


				int *canonical_equation;
				int *transporter_to_canonical_form;

				canonical_equation = NEW_int(Poly_ring->get_nb_monomials());
				transporter_to_canonical_form = NEW_int(Descr->PA->A->elt_size_in_int);


				classify_curve_nauty(
						Qco,
						canonical_equation,
						transporter_to_canonical_form,
						verbose_level);

				Int_vec_copy(canonical_equation,
						Canonical_forms + counter * Poly_ring->get_nb_monomials(),
						Poly_ring->get_nb_monomials());

				FREE_int(canonical_equation);
				FREE_int(transporter_to_canonical_form);

				if (f_v) {
					cout << "canonical_form_classifier::main_loop "
							"after classify_curve_nauty" << endl;
				}
			}
			else if (Descr->f_algorithm_substructure) {





				if (Qco->nb_pts >= Descr->substructure_size) {

					if (f_v) {
						cout << "canonical_form_classifier::main_loop "
								"before CFS->classify_curve_with_substructure" << endl;
					}

					ring_theory::longinteger_object go_eqn;

					canonical_form_substructure *CFS;

					CFS = NEW_OBJECT(canonical_form_substructure);

					CFS->classify_curve_with_substructure(
							this,
							fname_case_out,
							Qco,
							go_eqn,
							verbose_level);

					CFS_table[counter] = CFS;
					Int_vec_copy(CFS->canonical_equation,
							Canonical_forms + counter * Poly_ring->get_nb_monomials(),
							Poly_ring->get_nb_monomials());
					Goi[counter] = go_eqn.as_lint();

					if (f_v) {
						cout << "canonical_form_classifier::main_loop "
								"after CFS->classify_curve_with_substructure" << endl;
					}
				}
				else {


					if (f_v) {
						cout << "canonical_form_classifier::main_loop "
								"too small for substructure algorithm. Skipping" << endl;
					}

					CFS_table[counter] = NULL;
					Int_vec_zero(
							Canonical_forms + counter * Poly_ring->get_nb_monomials(),
							Poly_ring->get_nb_monomials());
					Goi[counter] = -1;

				}
			}
			else {
				cout << "canonical_form_classifier::main_loop "
						"please select which algorithm to use" << endl;
				exit(1);
			}

			FREE_OBJECT(Qco);

#if 0
			FREE_int(eqn);
			FREE_lint(pts);
			FREE_lint(bitangents);
#endif

		} // next row


	} // next cnt

	if (f_v) {
		cout << "canonical_form_classifier::main_loop done" << endl;
	}
}


void canonical_form_classifier::classify_curve_nauty(
		quartic_curve_object *Qco,
		int *canonical_equation,
		int *transporter_to_canonical_form,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "canonical_form_classifier::classify_curve_nauty" << endl;
	}

	canonical_form_nauty *C;
	ring_theory::longinteger_object go;

	groups::strong_generators *gens_stab_of_canonical_equation;






	C = NEW_OBJECT(canonical_form_nauty);

	C->quartic_curve(
			Descr->PA,
			Poly_ring,
			AonHPD,
			Qco,
			canonical_equation,
			transporter_to_canonical_form,
			gens_stab_of_canonical_equation,
			verbose_level);

	C->Stab_gens_quartic->group_order(go);

	FREE_OBJECT(gens_stab_of_canonical_equation);

	canonical_labeling_len = C->canonical_labeling_len;

	if (f_v) {
		cout << "canonical_form_classifier::classify_curve_nauty "
				"canonical_labeling_len=" << canonical_labeling_len << endl;
	}
	if (canonical_labeling_len == 0) {
		cout << "canonical_form_classifier::classify_curve_nauty "
				"canonical_labeling_len == 0, error" << endl;
		exit(1);
	}
	alpha = NEW_lint(canonical_labeling_len);
	gamma = NEW_int(canonical_labeling_len);


	if (CB->n == 0) {
		CB->init(nb_objects_to_test,
				C->Canonical_form->get_allocated_length(),
				verbose_level);
	}
	int f_found;
	int idx;

	CB->search_and_add_if_new(C->Canonical_form->get_data(),
			C /* void *extra_data */, f_found, idx, verbose_level);


	if (!f_found) {
		if (f_v) {
			cout << "canonical_form_classifier::classify_curve_nauty "
					"After search_and_add_if_new, "
					"cnt = " << Qco->cnt << " po = " << Qco->po << " so = " << Qco->so
					<< " The canonical form is new" << endl;
		}
	}
	else {
		if (f_v) {
			cout << "canonical_form_classifier::classify_curve_nauty "
					"After search_and_add_if_new, "
					"cnt = " << Qco->cnt << " po = " << Qco->po << " so = " << Qco->so
					<< " We found the canonical form at idx = " << idx << endl;
		}




		long int *alpha_inv;
		long int *beta_inv;
		int i, j;

		//long int *canonical_labeling;




		int idx1;
		int found_at = -1;

		if (f_v) {
			cout << "canonical_form_classifier::classify_curve_nauty "
					"starting loop over idx1" << endl;
		}

		for (idx1 = idx; idx1 >= 0; idx1--) {



			// test if entry at idx1 is equal to C.
			// if not, break

			if (f_v) {
				cout << "canonical_form_classifier::classify_curve_nauty "
						"before CB->compare_at idx1 = " << idx1 << endl;
			}
			if (CB->compare_at(C->Canonical_form->get_data(), idx1) != 0) {
				if (f_v) {
					cout << "canonical_form_classifier::classify_curve_nauty "
							"at idx1 = " << idx1 << " is not equal, break" << endl;
				}
				break;
			}
			if (f_v) {
				cout << "canonical_form_classifier::classify_curve_nauty "
						"canonical form at " << idx1 << " is equal" << endl;
			}


			canonical_form_nauty *C1;
			C1 = (canonical_form_nauty *) CB->Type_extra_data[idx1];

			alpha_inv = C1->canonical_labeling;
			if (f_v) {
				cout << "canonical_form_classifier::classify_curve_nauty "
						"alpha_inv = " << endl;
				Lint_vec_print(cout, alpha_inv, canonical_labeling_len);
				cout << endl;
			}

			beta_inv = C->canonical_labeling;
			if (f_v) {
				cout << "canonical_form_classifier::classify_curve_nauty "
						"beta_inv = " << endl;
				Lint_vec_print(cout, beta_inv, canonical_labeling_len);
				cout << endl;
			}

			// compute gamma = beta * alpha^-1


			if (f_v) {
				cout << "canonical_form_classifier::classify_curve_nauty "
						"computing alpha" << endl;
			}
			for (i = 0; i < canonical_labeling_len; i++) {
				j = alpha_inv[i];
				alpha[j] = i;
			}

			if (f_v) {
				cout << "canonical_form_classifier::classify_curve_nauty "
						"computing gamma" << endl;
			}
			for (i = 0; i < canonical_labeling_len; i++) {
				gamma[i] = beta_inv[alpha[i]];
			}
			if (f_v) {
				cout << "canonical_form_classifier::classify_curve_nauty "
						"gamma = " << endl;
				Int_vec_print(cout, gamma, canonical_labeling_len);
				cout << endl;
			}


			// gamma maps C1 to C.
			// So, in the contragredient action, it maps the equation of C to the equation of C1,
			// which is what we want.

			// turn gamma into a matrix


			int Mtx[10];
			//int Mtx_inv[10];
			int frobenius;

			if (f_v) {
				cout << "canonical_form_classifier::classify_curve_nauty "
						"before PA->P->reverse_engineer_semilinear_map" << endl;
			}
			Descr->PA->P->reverse_engineer_semilinear_map(
				gamma, Mtx, frobenius,
				verbose_level);
			if (f_v) {
				cout << "canonical_form_classifier::classify_curve_nauty "
						"after PA->P->reverse_engineer_semilinear_map" << endl;
			}

			Mtx[9] = frobenius;

			Descr->PA->A->make_element(Elt, Mtx, 0 /* verbose_level*/);

			if (f_v) {
				cout << "The isomorphism from C to C1 is given by:" << endl;
				Descr->PA->A->element_print(Elt, cout);
			}



			//int frobenius_inv;

			//frobenius_inv = NT.int_negate(Mtx[3 * 3], PA->F->e);


			//PA->F->matrix_inverse(Mtx, Mtx_inv, 3, 0 /* verbose_level*/);

			if (f_v) {
				cout << "canonical_form_classifier::classify_curve_nauty "
						"before substitute_semilinear" << endl;
			}
			Poly_ring->substitute_semilinear(C->Qco->eqn /* coeff_in */, eqn2 /* coeff_out */,
					Descr->PA->A->is_semilinear_matrix_group(), frobenius, Mtx, 0/*verbose_level*/);
			if (f_v) {
				cout << "canonical_form_classifier::classify_curve_nauty "
						"after substitute_semilinear" << endl;
			}

			Descr->PA->F->PG_element_normalize_from_front(eqn2, 1, Poly_ring->get_nb_monomials());


			if (f_v) {
				cout << "The mapped equation is:";
				Poly_ring->print_equation_simple(cout, eqn2);
				cout << endl;
			}




			int idx2;

			if (!C1->Orb->search_equation(eqn2 /*new_object */, idx2, TRUE)) {
				// need to map points and bitangents under gamma:
				if (f_v) {
					cout << "we found the canonical form but we did not find "
							"the equation at idx1=" << idx1 << endl;
				}


			}
			else {
				if (f_v) {
					cout << "After search_and_add_if_new, cnt = " << Qco->cnt
							<< " po = " << Qco->po
							<< " so = " << Qco->so
							<< " We found the canonical form and the equation "
									"at idx2 " << idx2 << ", idx1=" << idx1 << endl;
				}
				found_at = idx1;
				break;
			}


		}


		if (found_at == -1) {

			if (f_v) {
				cout << "canonical_form_classifier::classify_curve_nauty "
						"we found the canonical form but we did not find the equation" << endl;
			}


			quartic_curve_object *Qc2;

			Qc2 = NEW_OBJECT(quartic_curve_object);

			Qc2->init_image_of(Qco,
						Elt,
						Descr->PA->A,
						Descr->PA->A_on_lines,
						eqn2,
						verbose_level);

			canonical_form_nauty *C2;
			ring_theory::longinteger_object go;


			C2 = NEW_OBJECT(canonical_form_nauty);

			if (f_v) {
				cout << "canonical_form_classifier::classify_curve_nauty "
						"we will recompute the quartic curve from the canonical equation." << endl;
			}
			if (f_v) {
				cout << "canonical_form_classifier::classify_curve_nauty "
						"before C2->quartic_curve" << endl;
			}
			C2->quartic_curve(
					Descr->PA,
					Poly_ring,
					AonHPD,
					Qc2,
					canonical_equation,
					transporter_to_canonical_form,
					gens_stab_of_canonical_equation,
					verbose_level);
			if (f_v) {
				cout << "canonical_form_classifier::classify_curve_nauty "
						"after C2->quartic_curve" << endl;
			}

			if (f_v) {
				cout << "canonical_form_classifier::classify_curve_nauty "
						"After search_and_add_if_new, adding at " << idx << endl;
			}
			CB->add_at_idx(C2->Canonical_form->get_data(), C2 /* void *extra_data */, idx, 0 /* verbose_level*/);


		} // if (found_at == -1)
		else {
			if (f_v) {
				cout << "canonical_form_classifier::classify_curve_nauty "
						"we found the equation at index " << found_at << endl;
			}

		}

	} // if f_found

	FREE_lint(alpha);
	FREE_int(gamma);

	if (f_v) {
		cout << "canonical_form_classifier::classify_curve_nauty done" << endl;
	}

}


void canonical_form_classifier::write_canonical_forms_csv(
		std::string &fname_base,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);
	//int f_vv = (verbose_level >= 2);
	std::string fname;
	int i, j;
	data_structures::sorting Sorting;

	//int nb_orbits;
	int nb_monomials;

	actions::action *A;
	actions::action *A_on_lines;

	if (f_v) {
		cout << "canonical_form_classifier::write_canonical_forms_csv" << endl;
	}
	fname.assign(fname_base);
	fname.append("_canonical_form.csv");


	//nb_orbits = Classification_of_quartic_curves->nb_types;
	nb_monomials = Poly_ring->get_nb_monomials();


	A = Descr->PA->A;
	A_on_lines = Descr->PA->A_on_lines;


	{
		ofstream ost(fname.c_str());

		ost << "ROW,CNT,PO,SO,SourceRow,Eqn,Pts,Bitangents,Transporter,CanEqn,CanPts,CanLines,AutTl,AutGens,Ago" << endl;
		for (i = 0; i < nb_objects_to_test; i++) {

			if (f_v) {
				cout << "canonical_form_classifier::write_canonical_forms_csv "
						"i=" << i << " / " << nb_objects_to_test << endl;
			}

			if (CFS_table[i] == NULL) {
				continue;
			}

			ost << i;
			ost << ",";
			ost << CFS_table[i]->Qco->cnt;
			ost << ",";
			ost << CFS_table[i]->Qco->po;
			ost << ",";
			ost << CFS_table[i]->Qco->so;
			ost << ",";

			{
				string str;
				Int_vec_create_string_with_quotes(str,
						CFS_table[i]->Qco->eqn, nb_monomials);
				ost << str;
			}
			ost << ",";
			{
				string str;
				Lint_vec_create_string_with_quotes(str,
						CFS_table[i]->Qco->pts,
						CFS_table[i]->Qco->nb_pts);
				ost << str;
			}
			ost << ",";
			{
				string str;
				Lint_vec_create_string_with_quotes(str,
						CFS_table[i]->Qco->bitangents,
						CFS_table[i]->Qco->nb_bitangents);
				ost << str;
			}
			ost << ",";
			{
				string str;
				Int_vec_create_string_with_quotes(str,
						CFS_table[i]->transporter_to_canonical_form,
						A->make_element_size);
				ost << str;
			}
			ost << ",";
			{
				string str;
				Int_vec_create_string_with_quotes(str,
						CFS_table[i]->canonical_equation,
						nb_monomials);
				ost << str;
			}
			ost << ",";


			long int *Pts_orig;
			long int *Pts_canonical;

			if (f_v) {
				cout << "canonical_form_classifier::write_canonical_forms_csv "
						"i=" << i << " / " << nb_objects_to_test << " mapping points" << endl;
			}
			Pts_orig = CFS_table[i]->Qco->pts;
			Pts_canonical = NEW_lint(CFS_table[i]->Qco->nb_pts);
			for (j = 0; j < CFS_table[i]->Qco->nb_pts; j++) {
				Pts_canonical[j] = A->element_image_of(Pts_orig[j],
						CFS_table[i]->transporter_to_canonical_form, 0 /* verbose_level */);
			}
			Sorting.lint_vec_heapsort(Pts_canonical, CFS_table[i]->Qco->nb_pts);


			{
				string str;
				orbiter_kernel_system::Orbiter->Lint_vec->create_string_with_quotes(str,
						Pts_canonical, CFS_table[i]->Qco->nb_pts);
				ost << str;
			}
			ost << ",";


			if (f_v) {
				cout << "canonical_form_classifier::write_canonical_forms_csv "
						"i=" << i << " / " << nb_objects_to_test
						<< " mapping bitangents" << endl;
			}
			long int *bitangents_orig;
			long int *bitangents_canonical;

			bitangents_orig = CFS_table[i]->Qco->bitangents;
			bitangents_canonical = NEW_lint(CFS_table[i]->Qco->nb_bitangents);

			if (f_v) {
				cout << "canonical_form_classifier::write_canonical_forms_csv "
						"i=" << i << " / " << nb_objects_to_test << " bitangents_orig:" << endl;
				Lint_vec_print(cout, bitangents_orig, CFS_table[i]->Qco->nb_bitangents);
				cout << endl;
			}

			for (j = 0; j < CFS_table[i]->Qco->nb_bitangents; j++) {
				bitangents_canonical[j] = A_on_lines->element_image_of(
						bitangents_orig[j],
						CFS_table[i]->transporter_to_canonical_form,
						0 /* verbose_level */);
			}

			//Sorting.lint_vec_heapsort(bitangents_canonical, CFS_table[i]->nb_bitangents);

			{
				string str;
				orbiter_kernel_system::Orbiter->Lint_vec->create_string_with_quotes(str,
						bitangents_canonical,
						CFS_table[i]->Qco->nb_bitangents);
				ost << str;
			}
			ost << ",";

			groups::strong_generators *gens;

			gens = CFS_table[i]->gens_stab_of_canonical_equation;

			{
				string str;
				Int_vec_create_string_with_quotes(str,
						gens->tl, A->base_len());
				ost << str;
			}
			ost << ",";

			{
				string str;

				gens->get_gens_data_as_string_with_quotes(str,
						0 /*verbose_level*/);
				ost << str;
			}
			ost << ",";
			ring_theory::longinteger_object go;

			gens->group_order(go);
			ost << go << endl;


		}
		ost << "END" << endl;
	}


	orbiter_kernel_system::file_io Fio;

	cout << "written file " << fname << " of size "
			<< Fio.file_size(fname.c_str()) << endl;
	if (f_v) {
		cout << "canonical_form_classifier::write_canonical_forms_csv done" << endl;
	}
}



void canonical_form_classifier::generate_source_code(
		std::string &fname_base,
		data_structures::tally_vector_data *Classification_of_quartic_curves,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int f_vv = (verbose_level >= 2);
	std::string fname;
	int orbit_index;
	int i, j;

	int nb_orbits;
	int nb_monomials;

	actions::action *A;
	actions::action *A_on_lines;

	if (f_v) {
		cout << "canonical_form_classifier::generate_source_code" << endl;
	}
	fname.assign(fname_base);
	fname.append(".cpp");


	nb_orbits = Classification_of_quartic_curves->nb_types;
	nb_monomials = Poly_ring->get_nb_monomials();


	A = Descr->PA->A;
	A_on_lines = Descr->PA->A_on_lines;

	{
		ofstream f(fname.c_str());

		f << "static int " << fname_base.c_str() << "_nb_reps = "
				<< nb_orbits << ";" << endl;
		f << "static int " << fname_base.c_str() << "_size = "
				<< nb_monomials << ";" << endl;



		if (f_v) {
			cout << "canonical_form_classifier::generate_source_code "
					"preparing reps" << endl;
		}
		f << "// the equations:" << endl;
		f << "static int " << fname_base.c_str() << "_reps[] = {" << endl;
		for (orbit_index = 0;
				orbit_index < nb_orbits;
				orbit_index++) {


			int *equation;

			if (f_v) {
				cout << "canonical_form_classifier::generate_source_code "
						"orbit_index = " << orbit_index << endl;
			}

			int idx;

			idx = Classification_of_quartic_curves->sorting_perm_inv[Classification_of_quartic_curves->type_first[orbit_index]];

			canonical_form_substructure *CFS = CFS_table[idx];


			if (CFS) {
				//equation = Classification_of_quartic_curves->Reps + orbit_index * Classification_of_quartic_curves->data_set_sz;
				equation = CFS->canonical_equation;

				f << "\t";
				for (i = 0; i < nb_monomials; i++) {
					f << equation[i];
					f << ", ";
				}
				f << endl;
			}
			else {
				f << "\t";
				for (i = 0; i < nb_monomials; i++) {
					f << 0;
					f << ", ";
				}
				f << "// problem" << endl;

			}

		}
		f << "};" << endl;



		if (f_v) {
			cout << "canonical_form_classifier::generate_source_code "
					"preparing stab_order" << endl;
		}
		f << "// the stabilizer orders:" << endl;
		f << "static const char *" << fname_base.c_str() << "_stab_order[] = {" << endl;
		for (orbit_index = 0;
				orbit_index < nb_orbits;
				orbit_index++) {

			ring_theory::longinteger_object ago;

			int idx;

			idx = Classification_of_quartic_curves->sorting_perm_inv[Classification_of_quartic_curves->type_first[orbit_index]];


			ago.create(Goi[idx], __FILE__, __LINE__);

			f << "\t\"";

			ago.print_not_scientific(f);
			f << "\"," << endl;

		}
		f << "};" << endl;





		if (f_v) {
			cout << "canonical_form_classifier::generate_source_code "
					"preparing Bitangents" << endl;
		}
		f << "// the 28 bitangents:" << endl;
		f << "static long int " << fname_base.c_str() << "_Bitangents[] = { " << endl;


		for (orbit_index = 0;
				orbit_index < nb_orbits;
				orbit_index++) {


			if (f_v) {
				cout << "canonical_form_classifier::generate_source_code "
						"orbit_index = " << orbit_index << endl;
			}

			int idx;

			idx = Classification_of_quartic_curves->sorting_perm_inv[Classification_of_quartic_curves->type_first[orbit_index]];

			canonical_form_substructure *CFS = CFS_table[idx];


			if (CFS) {
				long int *bitangents_orig;
				long int *bitangents_canonical;

				bitangents_orig = CFS->Qco->bitangents;
				bitangents_canonical = NEW_lint(CFS->Qco->nb_bitangents);
				for (j = 0; j < CFS->Qco->nb_bitangents; j++) {
					bitangents_canonical[j] = A_on_lines->element_image_of(bitangents_orig[j],
							CFS->transporter_to_canonical_form, 0 /* verbose_level */);
				}




				f << "\t";
				for (j = 0; j < 28; j++) {
					f << bitangents_canonical[j];
					f << ", ";
				}
				f << endl;
			}
			else {
				f << "\t";
				for (j = 0; j < 28; j++) {
					f << 0;
					f << ", ";
				}
				f << "// problem" << endl;

			}

		}
		f << "};" << endl;

		f << "static int " << fname_base.c_str() << "_make_element_size = "
				<< A->make_element_size << ";" << endl;

		{
			int *stab_gens_first;
			int *stab_gens_len;
			int fst;



			stab_gens_first = NEW_int(nb_orbits);
			stab_gens_len = NEW_int(nb_orbits);
			fst = 0;
			for (orbit_index = 0;
					orbit_index < nb_orbits;
					orbit_index++) {


				groups::strong_generators *gens;

				int idx;

				idx = Classification_of_quartic_curves->sorting_perm_inv[Classification_of_quartic_curves->type_first[orbit_index]];

				canonical_form_substructure *CFS = CFS_table[idx];
				//gens = CFS->Gens_stabilizer_canonical_form;
				if (CFS) {
					gens = CFS->gens_stab_of_canonical_equation;


					stab_gens_first[orbit_index] = fst;
					stab_gens_len[orbit_index] = gens->gens->len;
					fst += stab_gens_len[orbit_index];
				}
				else {
					stab_gens_first[orbit_index] = fst;
					stab_gens_len[orbit_index] = 0;
					fst += 0;

				}
			}


			if (f_v) {
				cout << "canonical_form_classifier::generate_source_code "
						"preparing stab_gens_fst" << endl;
			}
			f << "static int " << fname_base.c_str() << "_stab_gens_fst[] = { " << endl << "\t";
			for (orbit_index = 0;
					orbit_index < nb_orbits;
					orbit_index++) {
				f << stab_gens_first[orbit_index];
				if (orbit_index < nb_orbits - 1) {
					f << ", ";
				}
				if (((orbit_index + 1) % 10) == 0) {
					f << endl << "\t";
				}
			}
			f << "};" << endl;

			if (f_v) {
				cout << "canonical_form_classifier::generate_source_code "
						"preparing stab_gens_len" << endl;
			}
			f << "static int " << fname_base.c_str() << "_stab_gens_len[] = { " << endl << "\t";
			for (orbit_index = 0;
					orbit_index < nb_orbits;
					orbit_index++) {
				f << stab_gens_len[orbit_index];
				if (orbit_index < nb_orbits - 1) {
					f << ", ";
				}
				if (((orbit_index + 1) % 10) == 0) {
					f << endl << "\t";
				}
			}
			f << "};" << endl;


			if (f_v) {
				cout << "canonical_form_classifier::generate_source_code "
						"preparing stab_gens" << endl;
			}
			f << "static int " << fname_base.c_str() << "_stab_gens[] = {" << endl;
			for (orbit_index = 0;
					orbit_index < nb_orbits;
					orbit_index++) {
				int j;

				for (j = 0; j < stab_gens_len[orbit_index]; j++) {
					if (f_vv) {
						cout << "canonical_form_classifier::generate_source_code "
								"before extract_strong_generators_in_order "
								"generator " << j << " / "
								<< stab_gens_len[orbit_index] << endl;
					}
					f << "\t";

					groups::strong_generators *gens;

					int idx;

					idx = Classification_of_quartic_curves->sorting_perm_inv[Classification_of_quartic_curves->type_first[orbit_index]];

					canonical_form_substructure *CFS = CFS_table[idx];
					//gens = CFS->Gens_stabilizer_canonical_form;
					if (CFS) {
						gens = CFS->gens_stab_of_canonical_equation;


						A->element_print_for_make_element(gens->gens->ith(j), f);
						f << endl;
					}
					else {
						f << "// problem" << endl;
					}
				}
			}
			f << "};" << endl;


			FREE_int(stab_gens_first);
			FREE_int(stab_gens_len);
		}
	}

	orbiter_kernel_system::file_io Fio;

	cout << "written file " << fname << " of size "
			<< Fio.file_size(fname.c_str()) << endl;
	if (f_v) {
		cout << "canonical_form_classifier::generate_source_code done" << endl;
	}
}




void canonical_form_classifier::report(std::string &fname_base, int verbose_level)
{

	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "canonical_form_classifier::report" << endl;
	}

	string label;
	string fname;


	label.assign(fname_base);
	label.append("_canonical");

	fname.assign(label);
	fname.append(".tex");


	{
		ofstream ost(fname);
		orbiter_kernel_system::latex_interface L;

		L.head_easy(ost);


		report2(ost, fname_base, verbose_level);

		L.foot(ost);
	}
	orbiter_kernel_system::file_io Fio;

	cout << "Written file " << fname << " of size "
			<< Fio.file_size(fname) << endl;


	if (f_v) {
		cout << "canonical_form_classifier::report done" << endl;
	}

}

void canonical_form_classifier::report2(std::ostream &ost, std::string &fname_base, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	std::string label;
	std::string fname;

	if (f_v) {
		cout << "canonical_form_classifier::report2" << endl;
	}

	label.assign(fname_base);
	label.append("_canonical");

	fname.assign(label);
	fname.append("_data.csv");

	int i, j;
	//int nb_cols = 5 + SubC->nb_orbits + 15;
	int nb_cols = 21;

	//long int *Table;

	data_structures::spreadsheet S;

	S.init_empty_table(nb_objects_to_test + 1, nb_cols);

	//Table = NEW_lint(nb_objects_to_test * nb_cols);

	S.fill_entry_with_text(0, 0, "Line");
	S.fill_entry_with_text(0, 1, "CNT");
	S.fill_entry_with_text(0, 2, "PO");
	S.fill_entry_with_text(0, 3, "SO");
	S.fill_entry_with_text(0, 4, "nb_pts");
	S.fill_entry_with_text(0, 5, "nb_sub_orbs");
	S.fill_entry_with_text(0, 6, "frequencies");
	S.fill_entry_with_text(0, 7, "nb_types");
	S.fill_entry_with_text(0, 8, "selected_type");
	S.fill_entry_with_text(0, 9, "selected_orbit");
	S.fill_entry_with_text(0, 10, "selected_frequency");
	S.fill_entry_with_text(0, 11, "go_min");
	S.fill_entry_with_text(0, 12, "set_stabilizer_order");
	S.fill_entry_with_text(0, 13, "reduced_set_size");
	S.fill_entry_with_text(0, 14, "nb_interesting_subsets");
	S.fill_entry_with_text(0, 15, "nb_interesting_subsets_reduced");
	S.fill_entry_with_text(0, 16, "nb_interesting_subsets_rr");
	S.fill_entry_with_text(0, 17, "nb_orbits");
	S.fill_entry_with_text(0, 18, "nb_interesting_orbits");
	S.fill_entry_with_text(0, 19, "nb_interesting_points");
	S.fill_entry_with_text(0, 20, "orbit_length_under_set_stab");
	S.fill_entry_with_text(0, 21, "stab_of_eqn");

	j = 1;
	for (i = 0; i < nb_objects_to_test; i++, j++) {

		cout << "i=" << i << endl;


		S.set_entry_lint(j, 0, i);

		if (CFS_table[i]) {

			S.set_entry_lint(j, 1, CFS_table[i]->Qco->cnt);
			S.set_entry_lint(j, 2, CFS_table[i]->Qco->po);
			S.set_entry_lint(j, 3, CFS_table[i]->Qco->so);
			S.set_entry_lint(j, 4, CFS_table[i]->Qco->nb_pts);
			S.set_entry_lint(j, 5, SubC->nb_orbits);

			//cout << "i=" << i << " getting orbit_frequencies" << endl;

			string str;

			Int_vec_create_string_with_quotes(str, CFS_table[i]->SubSt->orbit_frequencies, SubC->nb_orbits);

			S.fill_entry_with_text(j, 6, str);

			//cout << "i=" << i << " getting orbit_frequencies part 3" << endl;

			S.set_entry_lint(j, 7, CFS_table[i]->SubSt->nb_types);
			S.set_entry_lint(j, 8, CFS_table[i]->SubSt->selected_type);
			S.set_entry_lint(j, 9, CFS_table[i]->SubSt->selected_orbit);
			S.set_entry_lint(j, 10, CFS_table[i]->SubSt->selected_frequency);
			S.set_entry_lint(j, 11, CFS_table[i]->SubSt->gens->group_order_as_lint());
			S.set_entry_lint(j, 12, CFS_table[i]->Gens_stabilizer_original_set->group_order_as_lint());
			S.set_entry_lint(j, 13, CFS_table[i]->CS->Stab_orbits->reduced_set_size);
			S.set_entry_lint(j, 14, CFS_table[i]->SubSt->nb_interesting_subsets);
			S.set_entry_lint(j, 15, CFS_table[i]->CS->Stab_orbits->nb_interesting_subsets_reduced);
			S.set_entry_lint(j, 16, CFS_table[i]->CS->nb_interesting_subsets_rr);
			S.set_entry_lint(j, 17, CFS_table[i]->CS->Stab_orbits->nb_orbits);
			S.set_entry_lint(j, 18, CFS_table[i]->CS->Stab_orbits->nb_interesting_orbits);
			S.set_entry_lint(j, 19, CFS_table[i]->CS->Stab_orbits->nb_interesting_points);
			S.set_entry_lint(j, 20, CFS_table[i]->Orb->used_length);
			S.set_entry_lint(j, 21, CFS_table[i]->gens_stab_of_canonical_equation->group_order_as_lint());

		}
		else {
			//Orbiter->Lint_vec.zero(Table + i * nb_cols, nb_cols);
			//Table[i * nb_cols + 0] = i;
			S.set_entry_lint(j, 1, 0);
			S.set_entry_lint(j, 2, 0);
			S.set_entry_lint(j, 3, 0);
			S.set_entry_lint(j, 4, 0);
			S.set_entry_lint(j, 5, 0);
			S.fill_entry_with_text(j, 6, "");

			int h;

			for (h = 7; h <= 21; h++) {
				S.set_entry_lint(j, h, 0);
			}
		}

	}
	if (f_v) {
		cout << "canonical_form_classifier::report2 finished collecting Table" << endl;
	}

#if 1
	orbiter_kernel_system::file_io Fio;

	S.save(fname, 0 /* verbose_level*/);

	//Fio.lint_matrix_write_csv(fname, Table, nb_objects_to_test, nb_cols);

	cout << "Written file " << fname << " of size "
			<< Fio.file_size(fname) << endl;
#endif

	if (f_v) {
		cout << "canonical_form_classifier::report2 done" << endl;
	}

}


}}}


