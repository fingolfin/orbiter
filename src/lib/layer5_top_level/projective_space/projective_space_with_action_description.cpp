/*
 * projective_space_with_action_description.cpp
 *
 *  Created on: Jan 5, 2021
 *      Author: betten
 */




#include "orbiter.h"

using namespace std;

namespace orbiter {
namespace layer5_applications {
namespace projective_geometry {




projective_space_with_action_description::projective_space_with_action_description()
{
	f_n = FALSE;
	n = 0;

	f_q = FALSE;
	q = 0;

	f_field = FALSE;
	//field_label;

	F = NULL;

	f_use_projectivity_subgroup = FALSE;

	f_override_verbose_level = FALSE;
	override_verbose_level = 0;

}

projective_space_with_action_description::~projective_space_with_action_description()
{
}


int projective_space_with_action_description::read_arguments(
	int argc, std::string *argv,
	int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int i = 0;
	data_structures::string_tools ST;

	if (f_v) {
		cout << "projective_space_with_action_description::read_arguments" << endl;
	}
#if 0
	n = ST.strtoi(argv[i++]);
	if (f_v) {
		cout << "n = " << n << endl;
	}
	input_q.assign(argv[i++]);
	if (f_v) {
		cout << "q = " << input_q << endl;
		cout << "projective_space_with_action_description::read_arguments done" << endl;
	}
#endif
	for (; i < argc; i++) {

		if (ST.stringcmp(argv[i], "-use_projectivity_subgroup") == 0) {
			f_use_projectivity_subgroup = TRUE;
			if (f_v) {
				cout << "-use_projectivity_subgroup" << endl;
			}
		}
		else if (ST.stringcmp(argv[i], "-n") == 0) {
			f_n = TRUE;
			n = ST.strtoi(argv[++i]);
			if (f_v) {
				cout << "-n " << n << endl;
			}
		}
		else if (ST.stringcmp(argv[i], "-q") == 0) {
			f_q = TRUE;
			q = ST.strtoi(argv[++i]);
			if (f_v) {
				cout << "-q " << q << endl;
			}
		}
		else if (ST.stringcmp(argv[i], "-field") == 0) {
			f_field = TRUE;
			field_label.assign(argv[++i]);
			if (f_v) {
				cout << "-field " << field_label << endl;
			}
		}
		else if (ST.stringcmp(argv[i], "-v") == 0) {
			f_override_verbose_level = TRUE;
			override_verbose_level = ST.strtoi(argv[++i]);
			if (f_v) {
				cout << "-v " << override_verbose_level << endl;
			}
		}


		else if (ST.stringcmp(argv[i], "-end") == 0) {
			if (f_v) {
				cout << "-end" << endl;
			}
			break;
		}

		else {
			cout << "projective_space_with_action_description::read_arguments "
					"unrecognized option " << argv[i] << endl;
			exit(1);
		}

		if (f_v) {
			cout << "projective_space_with_action_description::read_arguments looping, i=" << i << endl;
		}
	} // next i

	if (f_v) {
		cout << "projective_space_with_action_description::read_arguments done" << endl;
	}
	return i + 1;
}



void projective_space_with_action_description::print()
{
	if (f_n) {
		cout << " -n " << n << endl;
	}
	if (f_q) {
		cout << " -q " << q << endl;
	}
	if (f_field) {
		cout << " -field " << field_label << endl;
	}
	if (f_use_projectivity_subgroup) {
		cout << " -f_use_projectivity_subgroup" << endl;
	}
	if (f_override_verbose_level) {
		cout << "-verbose_level " << override_verbose_level << endl;
	}
}

}}}

