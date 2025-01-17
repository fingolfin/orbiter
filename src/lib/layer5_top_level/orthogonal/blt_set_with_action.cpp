/*
 * blt_set_with_action.cpp
 *
 *  Created on: Apr 7, 2019
 *      Author: betten
 */




#include "orbiter.h"

using namespace std;

namespace orbiter {
namespace layer5_applications {
namespace orthogonal_geometry_applications {


blt_set_with_action::blt_set_with_action()
{
	A = NULL;
	Blt_set_domain = NULL;

	set = NULL;

	Aut_gens = NULL;
	Inv = NULL;
	A_on_points = NULL;
	Orbits_on_points = NULL;
}

blt_set_with_action::~blt_set_with_action()
{
	if (Inv) {
		FREE_OBJECT(Inv);
	}
	if (A_on_points) {
		FREE_OBJECT(A_on_points);
	}
	if (Orbits_on_points) {
		FREE_OBJECT(Orbits_on_points);
	}
}

void blt_set_with_action::init_set(
		actions::action *A,
		orthogonal_geometry::blt_set_domain *Blt_set_domain,
		long int *set,
		groups::strong_generators *Aut_gens, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "blt_set_with_action::init_set" << endl;
	}
	blt_set_with_action::A = A;
	blt_set_with_action::Blt_set_domain = Blt_set_domain;
	blt_set_with_action::set = set;
	blt_set_with_action::Aut_gens = Aut_gens;
	Inv = NEW_OBJECT(orthogonal_geometry::blt_set_invariants);
	Inv->init(Blt_set_domain, set, verbose_level);

	if (Aut_gens) {
		init_orbits_on_points(verbose_level);
	}

	if (f_v) {
		cout << "blt_set_with_action::init_set done" << endl;
	}
}


void blt_set_with_action::init_orbits_on_points(
		int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "blt_set_with_action::init_orbits_on_points" << endl;
	}

	if (f_v) {
		cout << "blt_set_with_action action "
				"on points:" << endl;
	}
	A_on_points = A->restricted_action(
			Inv->the_set_in_orthogonal,
			Blt_set_domain->target_size,
			0 /*verbose_level*/);
	if (f_v) {
		cout << "blt_set_with_action action "
				"on points done" << endl;
	}


	if (f_v) {
		cout << "computing orbits on points:" << endl;
	}
	Orbits_on_points = Aut_gens->orbits_on_points_schreier(
			A_on_points, 0 /*verbose_level*/);
	if (f_v) {
		cout << "We found " << Orbits_on_points->nb_orbits
				<< " orbits on points" << endl;
	}

	if (f_v) {
		cout << "blt_set_with_action::init_orbits_on_points done" << endl;
	}
}

void blt_set_with_action::print_automorphism_group(
	ostream &ost)
{
	if (Aut_gens) {
		ring_theory::longinteger_object go;

		Aut_gens->group_order(go);

		ost << "The automorphism group has order " << go << ".\\\\" << endl;
		ost << "\\bigskip" << endl;
		ost << "Orbits of the automorphism group on points "
				"of the BLT-set:\\\\" << endl;
		Orbits_on_points->print_and_list_orbits_sorted_by_length_tex(ost);
	}
	else {
		ost << "The automorphism group is not available.\\\\" << endl;

	}
}

void blt_set_with_action::report(std::ostream &ost, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "blt_set_with_action::report" << endl;
	}

	if (Aut_gens) {
		ost << "The stabilizer is generated by:\\\\" << endl;
		Aut_gens->print_generators_tex(ost);
	}

	Inv->latex(ost, verbose_level);

	if (f_v) {
		cout << "blt_set_with_action::report done" << endl;
	}
}



}}}

