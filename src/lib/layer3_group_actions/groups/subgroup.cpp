// subgroup.cpp
//
// Anton Betten
// April 29, 2017

#include "layer1_foundations/foundations.h"
#include "group_actions.h"

using namespace std;


namespace orbiter {
namespace layer3_group_actions {
namespace groups {



subgroup::subgroup()
{
	A = NULL;
	Elements = NULL;
	group_order = 0;
	gens = NULL;
	nb_gens = 0;
	Sub = NULL;
	SG = NULL;
	//null();
}

subgroup::~subgroup()
{
	if (Elements) {
		FREE_int(Elements);
		}
	if (gens) {
		FREE_int(gens);
		}
	if (Sub) {
		FREE_OBJECT(Sub);
		}
	if (SG) {
		FREE_OBJECT(SG);
		}
}

void subgroup::init_from_sims(sims *S, sims *Sub,
		strong_generators *SG, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int *Elt;
	long int i, rk;
	data_structures::sorting Sorting;

	if (f_v) {
		cout << "subgroup::init_from_sims" << endl;
	}
	A = S->A;
	subgroup::Sub = Sub;
	subgroup::SG = SG;
	group_order = SG->group_order_as_lint();
	if (f_v) {
		cout << "subgroup::init_from_sims "
				"group_order=" << group_order << endl;
	}
	Elt = NEW_int(A->elt_size_in_int);
	Elements = NEW_int(group_order);
	for (i = 0; i < group_order; i++) {
		Sub->element_unrank_lint(i, Elt);
		rk = S->element_rank_lint(Elt);
		Elements[i] = rk;
	}
	Sorting.int_vec_heapsort(Elements, group_order);

	if (f_v) {
		cout << "subgroup::init_from_sims done" << endl;
	}
}

void subgroup::init(int *Elements,
		int group_order, int *gens, int nb_gens)
{
	subgroup::Elements = NEW_int(group_order);
	subgroup::gens = NEW_int(nb_gens);
	subgroup::group_order = group_order;
	subgroup::nb_gens = nb_gens;
	Int_vec_copy(Elements, subgroup::Elements, group_order);
	Int_vec_copy(gens, subgroup::gens, nb_gens);
}

void subgroup::print()
{
	cout << "group of order " << group_order << " : ";
	Int_vec_print(cout, Elements, group_order);
	cout << " gens: ";
	Int_vec_print(cout, gens, nb_gens);
	cout << endl;
}

int subgroup::contains_this_element(int elt)
{
	int idx;
	data_structures::sorting Sorting;
	
	if (Sorting.int_vec_search(Elements, group_order, elt, idx)) {
		return TRUE;
		}
	else {
		return FALSE;
		}
}

void subgroup::report(ostream &ost)
{
	SG->print_generators_tex(ost);
}

}}}


