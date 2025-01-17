/*
 * create_graph.cpp
 *
 *  Created on: Nov 28, 2019
 *      Author: betten
 */





#include "orbiter.h"

using namespace std;

namespace orbiter {
namespace layer5_applications {
namespace apps_graph_theory {


create_graph::create_graph()
{
	description = NULL;

	f_has_CG = FALSE;
	CG = NULL;

	N = 0;
	Adj = NULL;


}

create_graph::~create_graph()
{
	if (f_has_CG) {
		FREE_OBJECT(CG);
	}
}

void create_graph::init(
		create_graph_description *description,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "create_graph::init" << endl;
	}
	create_graph::description = description;

	f_has_CG = FALSE;

	if (description->f_load) {
		if (f_v) {
			cout << "create_graph::init f_load" << endl;
		}

		f_has_CG = TRUE;
		CG = NEW_OBJECT(graph_theory::colored_graph);
		if (f_v) {
			cout << "create_graph::init before CG->load, fname=" << description->fname << endl;
		}
		CG->load(description->fname, verbose_level);
		if (f_v) {
			cout << "create_graph::init after CG->load, fname=" << description->fname << endl;
		}
		f_has_CG = TRUE;
		N = CG->nb_points;
		if (f_v) {
			cout << "create_graph::init number of vertices = " << N << endl;
		}
		label.assign(description->fname);
		if (f_v) {
			cout << "create_graph::init label = " << label << endl;
		}

		data_structures::string_tools String;
		String.chop_off_extension(label);

		label_tex.assign("File\\_");
		label_tex.append(label);

	}

	else if (description->f_Cayley_graph) {
		if (f_v) {
			cout << "create_graph::init f_Cayley_graph" << endl;
		}

		if (f_v) {
			cout << "create_graph::init group=" << description->Cayley_graph_group << endl;
			cout << "create_graph::init generators=" << description->Cayley_graph_gens << endl;
		}

		apps_algebra::any_group *G;

		G = Get_object_of_type_any_group(description->Cayley_graph_group);


		groups::strong_generators *SG;

		SG = G->get_strong_generators();


		groups::sims *Sims;

		//G = LG->initial_strong_gens->create_sims(verbose_level);
		Sims = SG->create_sims(verbose_level);

		cout << "group order G = " << Sims->group_order_lint() << endl;
		cout << "group order coded element size = " << G->A_base->elt_size_in_int << endl;


		int *v;
		int sz;
		int nb_gens;

		Get_int_vector_from_label(description->Cayley_graph_gens,
				v, sz, verbose_level);

		nb_gens = sz / G->A_base->elt_size_in_int;

		cout << "number of generators = " << nb_gens << endl;

		cout << "generators: ";
		Int_vec_print(cout, v, sz);
		cout << endl;

		data_structures_groups::vector_ge *gens;

		gens = NEW_OBJECT(data_structures_groups::vector_ge);

		gens->init_from_data(G->A, v, nb_gens, G->A_base->elt_size_in_int, verbose_level);

		cout << "generators:" << endl;
		gens->print(cout);



		int *Elt1;
		int *Elt2;
		ring_theory::longinteger_object go;
		int i, h, j;

		Elt1 = NEW_int(G->A_base->elt_size_in_int);
		Elt2 = NEW_int(G->A_base->elt_size_in_int);
		Sims->group_order(go);


		N = go.as_lint();

		Adj = NEW_int(N * N);
		Int_vec_zero(Adj, N * N);

		for (i = 0; i < go.as_lint(); i++) {


			Sims->element_unrank_lint(i, Elt1);

			if (f_v) {
				cout << "Element " << setw(5) << i << " / "
						<< go.as_int() << ":" << endl;
				G->A->element_print(Elt1, cout);
				cout << endl;
				G->A->element_print_as_permutation(Elt1, cout);
				cout << endl;
			}
			for (h = 0; h < nb_gens; h++) {

				G->A->element_mult(Elt1, gens->ith(h), Elt2, 0 /*verbose_level*/);

				j = Sims->element_rank_lint(Elt2);

				Adj[i * N + j] = 1;
				Adj[j * N + i] = 1;

			}


		}
		FREE_int(Elt1);
		FREE_int(Elt2);


		f_has_CG = FALSE;
		if (f_v) {
			cout << "create_graph::init number of vertices = " << N << endl;
		}
		label.assign("Cayley_graph_");
		label.append(G->A->label);
		if (f_v) {
			cout << "create_graph::init label = " << label << endl;
		}

		data_structures::string_tools String;
		String.chop_off_extension(label);

		label_tex.assign("Cayley\\_graph\\_");
		label_tex.append(G->A->label_tex);

	}

	else if (description->f_load_csv_no_border) {
		if (f_v) {
			cout << "create_graph::init f_load_from_file_csv_no_border" << endl;
		}

		orbiter_kernel_system::file_io Fio;
		int *M;
		int m, n;

		Fio.int_matrix_read_csv_no_border(description->fname, M, m, n, verbose_level);
		N = n;
		Adj = M;

		label.assign(description->fname);

		data_structures::string_tools String;
		String.chop_off_extension(label);


		label_tex.assign("File\\_");
		label_tex.append(label);
	}

	else if (description->f_load_adjacency_matrix_from_csv_and_select_value) {
		if (f_v) {
			cout << "create_graph::init f_load_adjacency_matrix_from_csv_and_select_value" << endl;
		}

		orbiter_kernel_system::file_io Fio;
		int *M;
		int m, n;
		int i;

		Fio.int_matrix_read_csv(
				description->load_adjacency_matrix_from_csv_and_select_value_fname,
				M, m, n, verbose_level);

		if (m != n) {
			cout << "create_graph::init the matrix is not square" << endl;
			exit(1);
		}
		N = n;
		for (i = 0; i < N * N; i++) {
			if (M[i] == description->load_adjacency_matrix_from_csv_and_select_value_value) {
				M[i] = 1;
			}
			else {
				M[i] = 0;
			}
		}
		Adj = M;

		label.assign(description->fname);

		data_structures::string_tools String;
		String.chop_off_extension(label);


		label_tex.assign("File\\_");
		label_tex.append(label);
	}

	else if (description->f_load_dimacs) {
		if (f_v) {
			cout << "create_graph::init f_load_from_file_dimacs" << endl;
		}

		orbiter_kernel_system::file_io Fio;
		int nb_V;
		int i, j, h;
		std::vector<std::vector<int>> Edges;

		if (f_v) {
			cout << "create_graph::init before Fio.read_dimacs_graph_format" << endl;
		}
		Fio.read_dimacs_graph_format(description->fname,
				nb_V, Edges, verbose_level);
		if (f_v) {
			cout << "create_graph::init after Fio.read_dimacs_graph_format" << endl;
		}

		N = nb_V;
		if (f_v) {
			cout << "create_graph::init N=" << N << endl;
		}
		if (f_v) {
			cout << "create_graph::init nb_E=" << Edges.size() << endl;
		}
		Adj = NEW_int(nb_V * nb_V);
		Int_vec_zero(Adj, nb_V * nb_V);

		for (h = 0; h < Edges.size(); h++) {
			i = Edges[h][0];
			j = Edges[h][1];
			if (FALSE) {
				cout << "create_graph::init edge " << h << " is " << i << " to " << j << endl;
			}
			Adj[i * nb_V + j] = 1;
			Adj[j * nb_V + i] = 1;
		}

		label.assign(description->fname);

		data_structures::string_tools String;
		String.chop_off_extension_and_path(label);


		label_tex.assign("File\\_");
		label_tex.append(label);
	}
	else if (description->f_edge_list) {

		combinatorics::combinatorics_domain Combi;
		int h, i, j, a;

		int *Idx;
		int sz;

		Int_vec_scan(description->edge_list_text, Idx, sz);

		N = description->n;


		Adj = NEW_int(N * N);
		Int_vec_zero(Adj, N * N);
		for (h = 0; h < sz; h++) {
			a = Idx[h];
			Combi.k2ij(a, i, j, N);
			Adj[i * N + j] = 1;
			Adj[j * N + i] = 1;
		}
		FREE_int(Idx);
		char str[1000];
		snprintf(str, sizeof(str), "graph_v%d_e%d", description->n, sz);
		label.assign(str);
		snprintf(str, sizeof(str), "Graph\\_%d\\_%d", description->n, sz);
		label_tex.assign(str);
	}
	else if (description->f_edges_as_pairs) {
		int h, i, j;
		int *Idx;
		int sz, sz2;

		Int_vec_scan(description->edges_as_pairs_text, Idx, sz);

		N = description->n;


		Adj = NEW_int(N * N);
		Int_vec_zero(Adj, N * N);
		sz2 = sz >> 1;
		for (h = 0; h < sz2; h++) {
			i = Idx[2 * h + 0];
			j = Idx[2 * h + 1];
			Adj[i * N + j] = 1;
			Adj[j * N + i] = 1;
		}
		FREE_int(Idx);
		char str[1000];
		snprintf(str, sizeof(str), "graph_v%d_e%d", description->n, sz2);
		label.assign(str);
		snprintf(str, sizeof(str), "Graph\\_%d\\_%d", description->n, sz2);
		label_tex.assign(str);
	}
	else if (description->f_cycle) {

		if (f_v) {
			cout << "create_graph::init before create_cycle" << endl;
		}
		create_cycle(N, Adj, description->cycle_n,
				verbose_level);


		if (f_v) {
			cout << "create_graph::init after create_cycle" << endl;
		}
	}
	else if (description->f_inversion_graph) {

		if (f_v) {
			cout << "create_graph::init before create_inversion_graph" << endl;
		}
		create_inversion_graph(N, Adj, description->inversion_graph_text,
				verbose_level);


		if (f_v) {
			cout << "create_graph::init after create_cycle" << endl;
		}
	}
	else if (description->f_Hamming) {

		if (f_v) {
			cout << "create_graph::init before create_Hamming" << endl;
		}
		create_Hamming(N, Adj,
				description->Hamming_n,
				description->Hamming_q,
				verbose_level);


		if (f_v) {
			cout << "create_graph::init after create_Hamming" << endl;
		}
	}
	else if (description->f_Johnson) {

		if (f_v) {
			cout << "create_graph::init before create_Johnson" << endl;
		}
		create_Johnson(N, Adj,
				description->Johnson_n,
				description->Johnson_k, description->Johnson_s,
				verbose_level);


		if (f_v) {
			cout << "create_graph::init after create_Johnson" << endl;
		}
	}
	else if (description->f_Paley) {

		if (f_v) {
			cout << "create_graph::init before create_Paley" << endl;
		}
		create_Paley(N, Adj,
				description->Paley_q,
				verbose_level);


		if (f_v) {
			cout << "create_graph::init after create_Paley" << endl;
		}
	}
	else if (description->f_Sarnak) {

		if (f_v) {
			cout << "create_graph::init before create_Sarnak" << endl;
		}
		create_Sarnak(N, Adj,
				description->Sarnak_p,
				description->Sarnak_q,
				verbose_level);


		if (f_v) {
			cout << "create_graph::init after create_Sarnak" << endl;
		}
	}
	else if (description->f_Schlaefli) {

		if (f_v) {
			cout << "create_graph::init before create_Schlaefli" << endl;
		}
		create_Schlaefli(N, Adj,
				description->Schlaefli_q,
				verbose_level);
		if (f_v) {
			cout << "create_graph::init after create_Schlaefli" << endl;
		}
	}
	else if (description->f_Shrikhande) {

		if (f_v) {
			cout << "create_graph::init before create_Shrikhande" << endl;
		}
		create_Shrikhande(N, Adj, verbose_level);

		if (f_v) {
			cout << "create_graph::init after create_Shrikhande" << endl;
		}
	}
	else if (description->f_Winnie_Li) {

		if (f_v) {
			cout << "create_graph::init before create_Winnie_Li" << endl;
		}
		create_Winnie_Li(N, Adj,
				description->Winnie_Li_q,
				description->Winnie_Li_index,
				verbose_level);

		if (f_v) {
			cout << "create_graph::init after create_Winnie_Li" << endl;
		}
	}
	else if (description->f_Grassmann) {

		if (f_v) {
			cout << "create_graph::init before create_Grassmann" << endl;
		}
		create_Grassmann(N, Adj,
				description->Grassmann_n,
				description->Grassmann_k,
				description->Grassmann_q,
				description->Grassmann_r,
				verbose_level);

		if (f_v) {
			cout << "create_graph::init after create_Grassmann" << endl;
		}
	}
	else if (description->f_coll_orthogonal) {

		if (f_v) {
			cout << "create_graph::init before create_coll_orthogonal" << endl;
		}
		create_coll_orthogonal(N, Adj,
				description->coll_orthogonal_epsilon,
				description->coll_orthogonal_d,
				description->coll_orthogonal_q, verbose_level);

		if (f_v) {
			cout << "create_graph::init after create_coll_orthogonal" << endl;
		}
	}
	else if (description->f_trihedral_pair_disjointness_graph) {

		algebraic_geometry::surface_domain *Surf;
		field_theory::finite_field *F;

		F = NEW_OBJECT(field_theory::finite_field);
		Surf = NEW_OBJECT(algebraic_geometry::surface_domain);

		F->finite_field_init(5, FALSE /* f_without_tables */, 0);
		Surf->init(F, verbose_level);

		Surf->Schlaefli->make_trihedral_pair_disjointness_graph(Adj, verbose_level);
		N = 120;
		label.assign("trihedral_pair_disjointness");
		label_tex.assign("trihedral\\_pair\\_disjointness");

		FREE_OBJECT(Surf);
		FREE_OBJECT(F);
	}
	else if (description->f_non_attacking_queens_graph) {

		char str[1000];

		graph_theory::graph_theory_domain GT;

		int n;

		n = description->non_attacking_queens_graph_n;


		if (f_v) {
			cout << "create_graph::init before GT.make_non_attacking_queens_graph" << endl;
		}
		GT.make_non_attacking_queens_graph(Adj, N, n, verbose_level);
		if (f_v) {
			cout << "create_graph::init after GT.make_non_attacking_queens_graph" << endl;
		}



		snprintf(str, sizeof(str), "non_attacking_queens_graph_%d", n);
		label.assign(str);
		snprintf(str, sizeof(str), "non\\_attacking\\_queens\\_graph\\_%d", n);
		label_tex.assign(str);

	}
	else if (description->f_disjoint_sets_graph) {

		graph_theory::graph_theory_domain GT;


		if (f_v) {
			cout << "create_graph::init before GT.make_disjoint_sets_graph" << endl;
		}
		GT.make_disjoint_sets_graph(Adj, N,
				description->disjoint_sets_graph_fname,
				verbose_level);
		if (f_v) {
			cout << "create_graph::init after GT.make_disjoint_sets_graph" << endl;
		}

		string L;
		data_structures::string_tools String;

		L.assign(description->disjoint_sets_graph_fname);
		String.chop_off_extension(L);
		L.append("_disjoint_sets");

		label.assign(L);

		L.assign(description->disjoint_sets_graph_fname);
		String.chop_off_extension(L);
		L.append("\\_disjoint\\_sets");

		label_tex.assign(L);
	}
	else if (description->f_orbital_graph) {

		graph_theory::graph_theory_domain GT;

		apps_algebra::any_group *AG;

		AG = Get_object_of_type_any_group(description->orbital_graph_group);


		if (f_v) {
			cout << "create_graph::init before GT.make_orbital_graph" << endl;
		}
		make_orbital_graph(N, Adj,
				AG, description->orbital_graph_orbit_idx,
				verbose_level);
		if (f_v) {
			cout << "create_graph::init after GT.make_orbital_graph" << endl;
		}
		if (f_v) {
			cout << "create_graph::init label = " << label << endl;
			cout << "create_graph::init label_tex = " << label_tex << endl;
			cout << "create_graph::init done" << endl;
		}

	}
	else if (description->f_collinearity_graph) {



		int *v;
		int m, n;

		Get_matrix(description->collinearity_graph_matrix, v, m, n);


		make_collinearity_graph(N, Adj,
				v, m, n,
				verbose_level);



	}
	else if (description->f_chain_graph) {



		int *v1;
		int sz1;
		int *v2;
		int sz2;

		Get_int_vector_from_label(
				description->chain_graph_partition_1,
				v1, sz1, 0 /* verbose_level*/);
		Get_int_vector_from_label(
				description->chain_graph_partition_2,
				v2, sz2, 0 /* verbose_level*/);


		make_chain_graph(N, Adj,
				v1, sz1,
				v2, sz2,
				verbose_level);



	}



	if (description->f_subset) {
		if (f_v) {
			cout << "create_graph::init the graph has a subset" << endl;
		}
		CG = NEW_OBJECT(graph_theory::colored_graph);
		CG->init_adjacency_no_colors(N, Adj,
				description->subset_label,
				description->subset_label_tex,
				verbose_level);

		int *subset;
		int sz;

		Int_vec_scan(description->subset_text, subset, sz);

		CG->init_adjacency_two_colors(N,
				Adj, subset, sz,
				description->subset_label,
				description->subset_label_tex,
				verbose_level);

		f_has_CG = TRUE;

		label.append(description->subset_label);
		label_tex.append(description->subset_label_tex);

		FREE_int(subset);
		if (f_v) {
			cout << "create_graph::init created colored graph with two colors" << endl;
		}

	}
	else {

		if (!f_has_CG) {

			CG = NEW_OBJECT(graph_theory::colored_graph);
			if (f_v) {
				cout << "create_graph::init before CG->init_adjacency_no_colors" << endl;
			}
			CG->init_adjacency_no_colors(N, Adj, label, label_tex,
					verbose_level);
			if (f_v) {
				cout << "create_graph::init after CG->init_adjacency_no_colors" << endl;
			}

			f_has_CG = TRUE;

			if (f_v) {
				cout << "create_graph::init created colored graph with one color" << endl;
			}
		}

	}

	int i;

	for (i = 0; i < description->Modifications.size(); i++) {
		description->Modifications[i].apply(CG, verbose_level);
	}

	CG->label.assign(label);
	CG->label_tex.assign(label_tex);


	if (f_v) {
		cout << "create_graph::init label = " << label << endl;
		cout << "create_graph::init done" << endl;
	}
}


void create_graph::create_cycle(int &N, int *&Adj,
		int n, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "create_graph::create_cycle" << endl;
	}

	graph_theory::graph_theory_domain GT;


	if (f_v) {
		cout << "create_graph::create_cycle before GT.make_cycle_graph" << endl;
	}
	GT.make_cycle_graph(Adj, N, n, verbose_level);
	if (f_v) {
		cout << "create_graph::create_cycle after GT.make_cycle_graph" << endl;
	}

	char str[1000];
	snprintf(str, sizeof(str), "Cycle_%d", n);
	label.assign(str);
	snprintf(str, sizeof(str), "Cycle\\_%d", n);
	label_tex.assign(str);


	if (f_v) {
		cout << "create_graph::create_cycle done" << endl;
	}
}

void create_graph::create_inversion_graph(int &N, int *&Adj,
		std::string &perm_text, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "create_graph::create_inversion_graph" << endl;
	}

	int *perm;
	int n;

	Int_vec_scan(perm_text, perm, n);


	graph_theory::graph_theory_domain GT;


	if (f_v) {
		cout << "create_graph::create_inversion_graph before GT.make_inversion_graph" << endl;
	}
	GT.make_inversion_graph(Adj, N, perm, n, verbose_level);
	if (f_v) {
		cout << "create_graph::create_inversion_graph after GT.make_inversion_graph" << endl;
	}

	char str[1000];
	snprintf(str, sizeof(str), "Inversion_%d", n);
	label.assign(str);
	snprintf(str, sizeof(str), "Inversion\\_%d", n);
	label_tex.assign(str);


	FREE_int(perm);

	if (f_v) {
		cout << "create_graph::create_inversion_graph done" << endl;
	}
}




void create_graph::create_Hamming(int &N, int *&Adj,
		int n, int q, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "create_graph::create_Hamming" << endl;
	}

	graph_theory::graph_theory_domain GT;


	if (f_v) {
		cout << "create_graph::create_Hamming before GT.make_Hamming_graph" << endl;
	}
	GT.make_Hamming_graph(Adj, N, n, q, verbose_level);
	if (f_v) {
		cout << "create_graph::create_Hamming after GT.make_Hamming_graph" << endl;
	}

	char str[1000];
	snprintf(str, sizeof(str), "Hamming_%d_%d", n, q);
	label.assign(str);
	snprintf(str, sizeof(str), "Hamming\\_%d\\_%d", n, q);
	label_tex.assign(str);


	if (f_v) {
		cout << "create_graph::create_Hamming done" << endl;
	}
}


void create_graph::create_Johnson(int &N, int *&Adj,
		int n, int k, int s, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "create_graph::create_Johnson" << endl;
	}

	graph_theory::graph_theory_domain GT;


	if (f_v) {
		cout << "create_graph::create_Johnson before GT.make_Johnson_graph" << endl;
	}
	GT.make_Johnson_graph(Adj, N, n, k, s, verbose_level);
	if (f_v) {
		cout << "create_graph::create_Johnson after GT.make_Johnson_graph" << endl;
	}

	char str[1000];
	snprintf(str, sizeof(str), "Johnson_%d_%d_%d", n, k, s);
	label.assign(str);
	snprintf(str, sizeof(str), "Johnson\\_%d\\_%d\\_%d", n, k, s);
	label_tex.assign(str);


	if (f_v) {
		cout << "create_graph::create_Johnson done" << endl;
	}
}

void create_graph::create_Paley(int &N, int *&Adj,
		int q, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "create_graph::create_Paley" << endl;
	}


	graph_theory::graph_theory_domain GT;


	if (f_v) {
		cout << "create_graph::create_Paley before GT.make_Paley_graph" << endl;
	}
	GT.make_Paley_graph(Adj, N, q, verbose_level);
	if (f_v) {
		cout << "create_graph::create_Paley after GT.make_Paley_graph" << endl;
	}

	char str[1000];
	snprintf(str, sizeof(str), "Paley_%d", q);
	label.assign(str);
	snprintf(str, sizeof(str), "Paley\\_%d", q);
	label_tex.assign(str);


	if (f_v) {
		cout << "create_graph::create_Paley done" << endl;
	}
}

void create_graph::create_Sarnak(int &N, int *&Adj,
		int p, int q, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "create_graph::create_Sarnak" << endl;
	}


	int f_vv = (verbose_level >= 2);
	int i, l, f_special = FALSE;
	number_theory::number_theory_domain NT;



	l = NT.Legendre(p, q, 0);
	if (f_v) {
		cout << "create_graph::create_Sarnak Legendre(" << p << ", " << q << ")=" << l << endl;
	}


	field_theory::finite_field *F;
	actions::action *A;
	int f_semilinear = FALSE;
	int f_basis = TRUE;

	F = NEW_OBJECT(field_theory::finite_field);
	F->finite_field_init(q, FALSE /* f_without_tables */, 0);
	//F->init_override_polynomial(q, override_poly, verbose_level);

	A = NEW_OBJECT(actions::action);

	if (l == 1) {
		f_special = TRUE;

		if (f_v) {
			cout << "create_graph::create_Sarnak "
					"Creating projective special linear group:" << endl;
		}
		A->init_projective_special_group(2, F,
			f_semilinear,
			f_basis,
			verbose_level - 2);
	}
	else {
		data_structures_groups::vector_ge *nice_gens;

		if (f_v) {
			cout << "create_graph::create_Sarnak "
					"Creating projective linear group:" << endl;
		}
		A->init_projective_group(2, F,
			f_semilinear,
			f_basis, TRUE /* f_init_sims */,
			nice_gens,
			verbose_level - 2);
		FREE_OBJECT(nice_gens);
	}



	groups::sims *Sims;

	Sims = A->Sims;


	//longinteger_object go;
	long int goi;

	goi = Sims->group_order_lint();

	if (f_v) {
		cout << "create_graph::create_Sarnak "
				"found a group of order " << goi << endl;
	}




	int a0, a1, a2, a3;
	int sqrt_p;

	int *sqrt_mod_q;
	int I;
	int *A4;
	int nb_A4 = 0;
	int j;

	A4 = NEW_int((p + 1) * 4);
	sqrt_mod_q = NEW_int(q);
	for (i = 0; i < q; i++) {
		sqrt_mod_q[i] = -1;
	}
	for (i = 0; i < q; i++) {
		j = F->mult(i, i);
		sqrt_mod_q[j] = i;
	}
	if (f_v) {
		cout << "create_graph::create_Sarnak sqrt_mod_q:" << endl;
		Int_vec_print(cout, sqrt_mod_q, q);
		cout << endl;
	}

	sqrt_p = 0;
	for (i = 1; i < p; i++) {
		if (i * i > p) {
			sqrt_p = i - 1;
			break;
		}
	}
	if (f_v) {
		cout << "create_graph::create_Sarnak p=" << p << endl;
		cout << "create_graph::create_Sarnak sqrt_p = " << sqrt_p << endl;
	}


	for (I = 0; I < q; I++) {
		if (F->add(F->mult(I, I), 1) == 0) {
			break;
		}
	}
	if (I == q) {
		cout << "create_graph::create_Sarnak did not find I" << endl;
		exit(1);
	}
	if (f_v) {
		cout << "create_graph::create_Sarnak I=" << I << endl;
	}

	for (a0 = 1; a0 <= sqrt_p; a0++) {
		if (EVEN(a0)) {
			continue;
		}
		for (a1 = -sqrt_p; a1 <= sqrt_p; a1++) {
			if (ODD(a1)) {
				continue;
			}
			for (a2 = -sqrt_p; a2 <= sqrt_p; a2++) {
				if (ODD(a2)) {
					continue;
				}
				for (a3 = -sqrt_p; a3 <= sqrt_p; a3++) {
					if (ODD(a3)) {
						continue;
					}
					if (a0 * a0 + a1 * a1 + a2 * a2 + a3 * a3 == p) {
						if (f_v) {
							cout << "create_graph::create_Sarnak solution " << nb_A4 << " : " << a0
									<< ", " << a1 << ", " << a2 << ", "
									<< a3 << ", " << endl;
						}
						if (nb_A4 == p + 1) {
							cout << "create_graph::create_Sarnak too many solutions" << endl;
							exit(1);
						}
						A4[nb_A4 * 4 + 0] = a0;
						A4[nb_A4 * 4 + 1] = a1;
						A4[nb_A4 * 4 + 2] = a2;
						A4[nb_A4 * 4 + 3] = a3;
						nb_A4++;
					}
				}
			}
		}
	}

	if (f_v) {
		cout << "create_graph::create_Sarnak nb_A4=" << nb_A4 << endl;
	}
	if (nb_A4 != p + 1) {
		cout << "create_graph::create_Sarnak nb_A4 != p + 1" << endl;
		exit(1);
	}

	if (f_v) {
		Int_matrix_print(A4, nb_A4, 4);
	}

	data_structures_groups::vector_ge *gens;
	int *Elt1;
	int *Elt2;
	int *Elt3;
	int M4[4];
	int det; //, s, sv;

	Elt1 = NEW_int(A->elt_size_in_int);
	Elt2 = NEW_int(A->elt_size_in_int);
	Elt3 = NEW_int(A->elt_size_in_int);

	gens = NEW_OBJECT(data_structures_groups::vector_ge);
	gens->init(A, verbose_level - 2);
	gens->allocate(nb_A4, verbose_level - 2);

	if (f_v) {
		cout << "create_graph::create_Sarnak making connection set:" << endl;
	}
	for (i = 0; i < nb_A4; i++) {

		if (f_vv) {
			cout << "create_graph::create_Sarnak making generator " << i << ":" << endl;
		}
		a0 = A4[i * 4 + 0];
		a1 = A4[i * 4 + 1];
		a2 = A4[i * 4 + 2];
		a3 = A4[i * 4 + 3];
		while (a0 < 0) {
			a0 += q;
		}
		while (a1 < 0) {
			a1 += q;
		}
		while (a2 < 0) {
			a2 += q;
		}
		while (a3 < 0) {
			a3 += q;
		}
		a0 = a0 % q;
		a1 = a1 % q;
		a2 = a2 % q;
		a3 = a3 % q;
		if (f_vv) {
			cout << "create_graph::create_Sarnak making generator " << i << ": a0=" << a0
					<< " a1=" << a1 << " a2=" << a2
					<< " a3=" << a3 << endl;
		}
		M4[0] = F->add(a0, F->mult(I, a1));
		M4[1] = F->add(a2, F->mult(I, a3));
		M4[2] = F->add(F->negate(a2), F->mult(I, a3));
		M4[3] = F->add(a0, F->negate(F->mult(I, a1)));

		if (f_vv) {
			cout << "M4=";
			Int_vec_print(cout, M4, 4);
			cout << endl;
		}

		if (f_special) {
			det = F->add(F->mult(M4[0], M4[3]),
					F->negate(F->mult(M4[1], M4[2])));

			if (f_vv) {
				cout << "det=" << det << endl;
			}

#if 0
			s = sqrt_mod_q[det];
			if (s == -1) {
				cout << "create_graph::create_Sarnak determinant is not a square" << endl;
				exit(1);
			}
			sv = F->inverse(s);
			if (f_vv) {
				cout << "create_graph::create_Sarnak det=" << det << " sqrt=" << s
						<< " mutiplying by " << sv << endl;
			}
			for (j = 0; j < 4; j++) {
				M4[j] = F->mult(sv, M4[j]);
			}
			if (f_vv) {
				cout << "create_graph::create_Sarnak M4=";
				int_vec_print(cout, M4, 4);
				cout << endl;
			}
#endif
		}

		A->make_element(Elt1, M4, verbose_level - 1);

		if (f_v) {
			cout << "create_graph::create_Sarnak s_" << i << "=" << endl;
			A->element_print_quick(Elt1, cout);
		}

		A->element_move(Elt1, gens->ith(i), 0);
	}

	if (f_v) {
		cout << "create_graph::create_Sarnak before Sims->Cayley_graph" << endl;
	}
	Sims->Cayley_graph(Adj, N, gens, verbose_level);
	if (f_v) {
		cout << "create_graph::create_Sarnak after Sims->Cayley_graph" << endl;
	}


	if (f_v) {
		cout << "create_graph::create_Sarnak "
				"The adjacency matrix of a graph with " << goi
				<< " vertices has been computed" << endl;
		//int_matrix_print(Adj, goi, goi);
	}

	int k;
	k = 0;
	for (i = 0; i < N; i++) {
		if (Adj[0 * N + i]) {
			k++;
		}
	}
	if (f_v) {
		cout << "create_graph::create_Sarnak the graph is regular of degree " << k << endl;
	}


	//N = goi;

	char str[1000];
	snprintf(str, sizeof(str), "Sarnak_%d_%d", p, q);
	label.assign(str);
	snprintf(str, sizeof(str), "Sarnak\\_%d\\_%d", p, q);
	label_tex.assign(str);

	FREE_OBJECT(gens);
	FREE_OBJECT(A);
	FREE_int(A4);
	FREE_int(Elt1);
	FREE_int(Elt2);
	FREE_int(Elt3);
	FREE_OBJECT(F);

	if (f_v) {
		cout << "create_graph::create_Sarnak done" << endl;
	}
}


void create_graph::create_Schlaefli(int &N, int *&Adj,
		int q, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "create_graph::create_Schlaefli" << endl;
	}

	graph_theory::graph_theory_domain GT;


	if (f_v) {
		cout << "create_graph::create_Schlaefli before GT.make_Schlaefli_graph" << endl;
	}
	GT.make_Schlaefli_graph(Adj, N, q, verbose_level);
	if (f_v) {
		cout << "create_graph::create_Schlaefli after GT.make_Schlaefli_graph" << endl;
	}

	char str[1000];

	snprintf(str, sizeof(str), "Schlaefli_%d", q);
	label.assign(str);
	snprintf(str, sizeof(str), "Schlaefli\\_%d", q);
	label_tex.assign(str);


	if (f_v) {
		cout << "create_graph::create_Schlaefli done" << endl;
	}
}

void create_graph::create_Shrikhande(int &N, int *&Adj, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "create_graph::create_Shrikhande" << endl;
	}

	actions::action *A;
	data_structures_groups::vector_ge *gens_G;
	data_structures_groups::vector_ge *gens_S;
	int *v;
	int n = 8;
	int i, j;
	int nb_G, nb_S;
	long int goi;
	int f_no_base = FALSE;

	A = NEW_OBJECT(actions::action);
	A->init_symmetric_group(n, f_no_base, verbose_level);
	goi = A->group_order_lint();

	if (f_v) {
		cout << "create_graph::create_Shrikhande "
				"Created group Sym(" << n << ") of size " << goi << endl;
	}

	nb_G = 2;
	nb_S = 6;

	gens_G = NEW_OBJECT(data_structures_groups::vector_ge);
	gens_G->init(A, verbose_level - 2);
	gens_G->allocate(nb_G, verbose_level - 2);


	gens_S = NEW_OBJECT(data_structures_groups::vector_ge);
	gens_S->init(A, verbose_level - 2);
	gens_S->allocate(nb_S, verbose_level - 2);

	v = NEW_int(n);


	for (i = 0; i < nb_G; i++) {
		if (i == 0) {
			for (j = 0; j < 4; j++) {
				v[j] = (j + 1) % 4;
			}
			for (j = 0; j < 4; j++) {
				v[4 + j] = 4 + j;
			}
		}
		else {
			for (j = 0; j < 4; j++) {
				v[j] = j;
			}
			for (j = 0; j < 4; j++) {
				v[4 + j] = 4 + ((j + 1) % 4);
			}
		}
		A->make_element(gens_G->ith(i), v, 0 /* verbose_level */);
	}

	if (f_v) {
		cout << "create_graph::create_Shrikhande "
				"generators for G:" << endl;
		for (i = 0; i < nb_G; i++) {
			cout << "generator " << i << ":" << endl;
			A->element_print(gens_G->ith(i), cout);
		}
	}

	for (i = 0; i < nb_S; i++) {
		if (i == 0) {
			for (j = 0; j < 4; j++) {
				v[j] = (j + 1) % 4;
			}
			for (j = 0; j < 4; j++) {
				v[4 + j] = 4 + j;
			}
		}
		else if (i == 1) {
			for (j = 0; j < 4; j++) {
				v[j] = (4 + j - 1) % 4;
			}
			for (j = 0; j < 4; j++) {
				v[4 + j] = 4 + j;
			}
		}
		else if (i == 2) {
			for (j = 0; j < 4; j++) {
				v[j] = j;
			}
			for (j = 0; j < 4; j++) {
				v[4 + j] = 4 + ((j + 1) % 4);
			}
		}
		else if (i == 3) {
			for (j = 0; j < 4; j++) {
				v[j] = j;
			}
			for (j = 0; j < 4; j++) {
				v[4 + j] = 4 + ((4 + j - 1) % 4);
			}
		}
		else if (i == 4) {
			for (j = 0; j < 4; j++) {
				v[j] = (j + 1) % 4;
			}
			for (j = 0; j < 4; j++) {
				v[4 + j] = 4 + ((j + 1) % 4);
			}
		}
		else if (i == 5) {
			for (j = 0; j < 4; j++) {
				v[j] = (4 + j - 1) % 4;
			}
			for (j = 0; j < 4; j++) {
				v[4 + j] = 4 + ((4 + j - 1) % 4);
			}
		}
		A->make_element(gens_S->ith(i), v, 0 /* verbose_level */);
	}

	if (f_v) {
		cout << "create_graph::create_Shrikhande "
				"generators for S:" << endl;
		for (i = 0; i < nb_S; i++) {
			cout << "generator " << i << ":" << endl;
			A->element_print(gens_S->ith(i), cout);
		}
	}

	groups::sims *G;


	G = A->create_sims_from_generators_with_target_group_order_lint(
		gens_G, 16, verbose_level);



	if (f_v) {
		cout << "create_graph::create_Shrikhande "
				"before G->Cayley_graph" << endl;
	}
	G->Cayley_graph(Adj, N, gens_S, verbose_level);
	if (f_v) {
		cout << "create_graph::create_Shrikhande "
				"after G->Cayley_graph" << endl;
	}



	if (f_v) {
		cout << "create_graph::create_Shrikhande "
				"The adjacency matrix of a graph with " <<
				goi << " vertices has been computed" << endl;
		//int_matrix_print(Adj, goi, goi);
	}

	//N = goi;

	char str[1000];
	snprintf(str, sizeof(str), "Shrikhande");
	label.assign(str);
	snprintf(str, sizeof(str), "Shrikhande");
	label_tex.assign(str);


	FREE_int(v);
	FREE_OBJECT(gens_G);
	FREE_OBJECT(gens_S);

	if (f_v) {
		cout << "create_graph::create_Shrikhande done" << endl;
	}
}

void create_graph::create_Winnie_Li(int &N, int *&Adj,
		int q, int index, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "create_graph::create_Winnie_Li" << endl;
	}

	graph_theory::graph_theory_domain GT;


	if (f_v) {
		cout << "create_graph::create_Winnie_Li "
				"before Combi.make_Winnie_Li_graph" << endl;
	}
	GT.make_Winnie_Li_graph(Adj, N, q, index, verbose_level);
	if (f_v) {
		cout << "create_graph::create_Winnie_Li "
				"after Combi.make_Winnie_Li_graph" << endl;
	}


	char str[1000];
	snprintf(str, sizeof(str), "Winnie_Li_%d_%d", q, index);
	label.assign(str);
	snprintf(str, sizeof(str), "Winnie_Li\\_%d\\_%d", q, index);
	label_tex.assign(str);



	if (f_v) {
		cout << "create_graph::create_Winnie_Li done" << endl;
	}
}

void create_graph::create_Grassmann(int &N, int *&Adj,
		int n, int k, int q, int r, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "create_graph::create_Grassmann" << endl;
	}


	graph_theory::graph_theory_domain GT;


	if (f_v) {
		cout << "create_graph::create_Grassmann "
				"before GT.make_Grassmann_graph" << endl;
	}
	GT.make_Grassmann_graph(Adj, N, n, k, q, r, verbose_level);
	if (f_v) {
		cout << "create_graph::create_Grassmann "
				"after GT.make_Grassmann_graph" << endl;
	}


	char str[1000];
	snprintf(str, sizeof(str), "Grassmann_%d_%d_%d_%d", n, k, q, r);
	label.assign(str);
	snprintf(str, sizeof(str), "Grassmann\\_%d\\_%d\\_%d\\_%d", n, k, q, r);
	label_tex.assign(str);


	if (f_v) {
		cout << "create_graph::create_Grassmann done" << endl;
	}
}

void create_graph::create_coll_orthogonal(int &N, int *&Adj,
		int epsilon, int d, int q, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "create_graph::create_coll_orthogonal" << endl;
	}

	graph_theory::graph_theory_domain GT;


	if (f_v) {
		cout << "create_graph::create_coll_orthogonal before "
				"GT.make_orthogonal_collinearity_graph" << endl;
	}
	GT.make_orthogonal_collinearity_graph(Adj, N,
			epsilon, d, q, verbose_level);
	if (f_v) {
		cout << "create_graph::create_coll_orthogonal after "
				"GT.make_orthogonal_collinearity_graph" << endl;
	}


	char str[1000];
	snprintf(str, sizeof(str), "Coll_orthogonal_%d_%d_%d", epsilon, d, q);
	label.assign(str);
	snprintf(str, sizeof(str), "Coll_orthogonal\\_%d\\_%d\\_%d", epsilon, d, q);
	label_tex.assign(str);

	if (f_v) {
		cout << "create_graph::create_coll_orthogonal done" << endl;
	}
}

void create_graph::make_orbital_graph(int &N, int *&Adj,
		apps_algebra::any_group *AG, int orbit_idx, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "create_graph::make_orbital_graph" << endl;
	}

	poset_classification::poset_classification_control *Control;

	Control = NEW_OBJECT(poset_classification::poset_classification_control);

	poset_classification::poset_classification *PC;

	if (f_v) {
		cout << "create_graph::make_orbital_graph "
				"before AG->orbits_on_subsets" << endl;
	}
	AG->orbits_on_subsets(Control, PC, 2, verbose_level);
	if (f_v) {
		cout << "create_graph::make_orbital_graph "
				"after AG->orbits_on_subsets" << endl;
	}

	long int set[2];
	int size;

	PC->get_Poo()->get_set(2 /* level */, orbit_idx, set, size);

	if (f_v) {
		cout << "create_graph::make_orbital_graph set: ";
		Lint_vec_print(cout, set, 2);
		cout << endl;
	}

	orbits_schreier::orbit_of_sets *Orb;

	Orb = NEW_OBJECT(orbits_schreier::orbit_of_sets);

	if (f_v) {
		cout << "create_graph::make_orbital_graph before Orb->init" << endl;
	}
	Orb->init(AG->A_base, AG->A,
			set, 2, AG->Subgroup_gens->gens, verbose_level);
	if (f_v) {
		cout << "create_graph::make_orbital_graph after Orb->init" << endl;
	}

	int *M;
	int nb_points;
	int i, j, h;

	nb_points = AG->A->degree;

	M = NEW_int(nb_points * nb_points);
	Int_vec_zero(M, nb_points * nb_points);
	for (h = 0; h < Orb->used_length; h++) {
		i = Orb->Sets[h][0];
		j = Orb->Sets[h][1];
		M[i * nb_points + j] = 1;
		M[j * nb_points + i] = 1;
	}

	FREE_OBJECT(Orb);
	N = nb_points;
	Adj = M;

	char str[1000];

	if (f_v) {
		cout << "create_graph::make_orbital_graph AG->A->label = " << AG->A->label << endl;
		cout << "create_graph::make_orbital_graph AG->A->label_tex = " << AG->A->label_tex << endl;
	}

	snprintf(str, sizeof(str), "_Orbital_%d", orbit_idx);
	label.assign("Group_");
	label.append(AG->A->label);
	label.append(str);
	snprintf(str, sizeof(str), "Orbital\\_%d", orbit_idx);
	label_tex.assign("Group\\_");
	label_tex.append(AG->A->label_tex);
	label_tex.append(str);

	if (f_v) {
		cout << "create_graph::make_orbital_graph done" << endl;
	}
}

void create_graph::make_collinearity_graph(int &N, int *&Adj,
		int *Inc, int nb_rows, int nb_cols, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "create_graph::make_collinearity_graph" << endl;
	}

	N = nb_rows;
	Adj = NEW_int(N * N);
	Int_vec_zero(Adj, N * N);

	int j, i1, i2;

	for (j = 0; j < nb_cols; j++) {
		for (i1 = 0; i1 < nb_rows; i1++) {
			if (Inc[i1 * nb_cols + j] == 0) {
				continue;
			}
			for (i2 = i1 + 1; i2 < nb_rows; i2++) {
				if (Inc[i2 * nb_cols + j] == 0) {
					continue;
				}
				Adj[i1 * N + i2] = 1;
				Adj[i2 * N + i1] = 1;
			}
		}
	}

	label.assign("collinearity_graph");
	label_tex.assign("collinearity\\_graph");

	if (f_v) {
		cout << "create_graph::make_collinearity_graph done" << endl;
	}
}

void create_graph::make_chain_graph(int &N, int *&Adj,
		int *part1, int sz1,
		int *part2, int sz2,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "create_graph::make_chain_graph" << endl;
	}
	if (sz1 != sz2) {
		cout << "create_graph::make_chain_graph sz1 != sz2" << endl;
	}

	int i, j;
	int N1, N2;
	int *first1;
	int *first2;

	first1 = NEW_int(sz1 + 1);
	first2 = NEW_int(sz1 + 1);

	N1 = 0;
	first1[0] = 0;
	for (i = 0; i < sz1; i++) {
		N1 += part1[i];
		first1[i + 1] = first1[i] + part1[i];
	}
	N2 = 0;
	first2[0] = N1;
	for (i = 0; i < sz2; i++) {
		N2 += part2[i];
		first2[i + 1] = first2[i] + part2[i];
	}
	N = N1 + N2;

	Adj = NEW_int(N * N);
	Int_vec_zero(Adj, N * N);

	int I, J, ii, jj;

	for (I = 0; I < sz1; I++) {
		for (i = 0; i < part1[I]; i++) {
			ii = first1[I] + i;
			for (J = 0; J < sz2 - I; J++) {
				for (j = 0; j < part2[J]; j++) {
					jj = first2[J] + j;
					Adj[ii * N + jj] = 1;
					Adj[jj * N + ii] = 1;
				}
			}
		}
	}

	label.assign("chain_graph");
	label_tex.assign("chain\\_graph");

	if (f_v) {
		cout << "create_graph::make_chain_graph done" << endl;
	}
}


}}}
