/*
 * interface_coding_theory.cpp
 *
 *  Created on: Apr 4, 2020
 *      Author: betten
 */




#include "orbiter.h"

using namespace std;


namespace orbiter {
namespace layer5_applications {
namespace user_interface {


interface_coding_theory::interface_coding_theory()
{
	f_make_macwilliams_system = FALSE;
	make_macwilliams_system_q = 0;
	make_macwilliams_system_n = 0;
	make_macwilliams_system_k = 0;

	f_table_of_bounds = FALSE;
	table_of_bounds_n_max = 0;
	table_of_bounds_q = 0;

	f_make_bounds_for_d_given_n_and_k_and_q = FALSE;
	make_bounds_n = 0;
	make_bounds_k = 0;
	make_bounds_q = 0;


	f_Hamming_space_distance_matrix = FALSE;
	Hamming_space_n = 0;
	Hamming_space_q = 0;

	f_introduce_errors = FALSE;
	introduce_errors_crc_options_description = NULL;

	f_check_errors = FALSE;
	check_errors_crc_options_description = NULL;

	f_extract_block = FALSE;
	extract_block_crc_options_description = NULL;

	f_random_noise_in_bitmap_file = FALSE;
	//std::string random_noise_in_bitmap_file_input;
	//std::string random_noise_in_bitmap_file_output;
	random_noise_in_bitmap_file_numerator = 0;
	random_noise_in_bitmap_file_denominator = 0;

	f_random_noise_of_burst_type_in_bitmap_file = FALSE;
	//std::string random_noise_of_burst_type_in_bitmap_file_input;
	//std::string random_noise_of_burst_type_in_bitmap_file_output;
	random_noise_of_burst_type_in_bitmap_file_numerator = 0;
	random_noise_of_burst_type_in_bitmap_file_denominator = 0;
	random_noise_of_burst_type_in_bitmap_file_burst_length = 0;

}


void interface_coding_theory::print_help(int argc,
		std::string *argv, int i, int verbose_level)
{
	data_structures::string_tools ST;

	if (ST.stringcmp(argv[i], "-make_macwilliams_system") == 0) {
		cout << "-make_macwilliams_system <int : q> <int : n> <int k>" << endl;
	}
	else if (ST.stringcmp(argv[i], "-table_of_bounds") == 0) {
		cout << "-table_of_bounds <int : n_max> <int : q> " << endl;
	}
	else if (ST.stringcmp(argv[i], "-make_bounds_for_d_given_n_and_k_and_q") == 0) {
		cout << "-make_bounds_for_d_given_n_and_k_and_q <int : n> <int k> <int : q> " << endl;
	}
	else if (ST.stringcmp(argv[i], "-Hamming_space_distance_matrix") == 0) {
		cout << "-Hamming_space_distance_matrix <int : n> <int : q>" << endl;
	}
	else if (ST.stringcmp(argv[i], "-introduce_errors") == 0) {
		cout << "-introduce_errors <description> -end" << endl;
	}
	else if (ST.stringcmp(argv[i], "-check_errors") == 0) {
		cout << "-check_errors <description> -end" << endl;
	}
	else if (ST.stringcmp(argv[i], "-extract_block") == 0) {
		cout << "-extract_block <description> -end" << endl;
	}
	else if (ST.stringcmp(argv[i], "-random_noise_in_bitmap_file") == 0) {
		cout << "-random_noise_in_bitmap_file <fname_in> <fname_out> <numerator> <denominator>" << endl;
	}
	else if (ST.stringcmp(argv[i], "-random_noise_of_burst_type_in_bitmap_file") == 0) {
		cout << "-random_noise_of_burst_type_in_bitmap_file <fname_in> <fname_out> <numerator> <denominator> <burst_length>" << endl;
	}
}

int interface_coding_theory::recognize_keyword(int argc,
		std::string *argv, int i, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	data_structures::string_tools ST;

	if (i >= argc) {
		return false;
	}
	if (f_v) {
		cout << "interface_coding_theory::recognize_keyword argv[i]="
				<< argv[i] << " i=" << i << " argc=" << argc << endl;
	}
	if (ST.stringcmp(argv[i], "-make_macwilliams_system") == 0) {
		return true;
	}
	else if (ST.stringcmp(argv[i], "-table_of_bounds") == 0) {
		return true;
	}
	else if (ST.stringcmp(argv[i], "-make_bounds_for_d_given_n_and_k_and_q") == 0) {
		return true;
	}
	else if (ST.stringcmp(argv[i], "-Hamming_space_distance_matrix") == 0) {
		return true;
	}
	else if (ST.stringcmp(argv[i], "-introduce_errors") == 0) {
		return true;
	}
	else if (ST.stringcmp(argv[i], "-check_errors") == 0) {
		return true;
	}
	else if (ST.stringcmp(argv[i], "-extract_block") == 0) {
		return true;
	}
	else if (ST.stringcmp(argv[i], "-random_noise_in_bitmap_file") == 0) {
		return true;
	}
	else if (ST.stringcmp(argv[i], "-random_noise_of_burst_type_in_bitmap_file") == 0) {
		return true;
	}
	if (f_v) {
		cout << "interface_coding_theory::recognize_keyword not recognizing" << endl;
	}
	return false;
}

void interface_coding_theory::read_arguments(int argc,
		std::string *argv, int &i, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	data_structures::string_tools ST;

	if (f_v) {
		cout << "interface_coding_theory::read_arguments" << endl;
	}



	if (f_v) {
		cout << "interface_coding_theory::read_arguments "
				"the next argument is " << argv[i] << endl;
	}


	if (ST.stringcmp(argv[i], "-make_macwilliams_system") == 0) {
		f_make_macwilliams_system = TRUE;
		make_macwilliams_system_n = ST.strtoi(argv[++i]);
		make_macwilliams_system_k = ST.strtoi(argv[++i]);
		make_macwilliams_system_q = ST.strtoi(argv[++i]);
		if (f_v) {
			cout << "-make_macwilliams_system " << make_macwilliams_system_n << " " << make_macwilliams_system_k << " " << make_macwilliams_system_q << endl;
		}
	}
	else if (ST.stringcmp(argv[i], "-table_of_bounds") == 0) {
		f_table_of_bounds = TRUE;
		table_of_bounds_n_max = ST.strtoi(argv[++i]);
		table_of_bounds_q = ST.strtoi(argv[++i]);
		if (f_v) {
			cout << "-table_of_bounds " << table_of_bounds_n_max
					<< " " << table_of_bounds_q << endl;
		}
	}
	else if (ST.stringcmp(argv[i], "-make_bounds_for_d_given_n_and_k_and_q") == 0) {
		f_make_bounds_for_d_given_n_and_k_and_q = TRUE;
		make_bounds_n = ST.strtoi(argv[++i]);
		make_bounds_k = ST.strtoi(argv[++i]);
		make_bounds_q = ST.strtoi(argv[++i]);
		if (f_v) {
			cout << "-make_bounds_for_d_given_n_and_k_and_q "
					<< make_bounds_n << " " << make_bounds_k << " " << make_bounds_q << endl;
		}
	}
	else if (ST.stringcmp(argv[i], "-Hamming_space_distance_matrix") == 0) {
		f_Hamming_space_distance_matrix = TRUE;
		Hamming_space_n = ST.strtoi(argv[++i]);
		Hamming_space_q = ST.strtoi(argv[++i]);
		if (f_v) {
			cout << "-Hamming_space_distance_matrix " << Hamming_space_n << " " << Hamming_space_q << endl;
		}
	}
	else if (ST.stringcmp(argv[i], "-introduce_errors") == 0) {
		f_introduce_errors = TRUE;

		introduce_errors_crc_options_description = NEW_OBJECT(coding_theory::crc_options_description);
		if (f_v) {
			cout << "-introduce_errors " << endl;
		}
		introduce_errors_crc_options_description = NEW_OBJECT(coding_theory::crc_options_description);
		i += introduce_errors_crc_options_description->read_arguments(argc - (i + 1),
			argv + i + 1, verbose_level);

		if (f_v) {
			cout << "interface_coding_theory::read_arguments finished "
					"reading -introduce_errors" << endl;
			cout << "i = " << i << endl;
			cout << "argc = " << argc << endl;
			if (i < argc) {
				cout << "next argument is " << argv[i] << endl;
			}
			cout << "-introduce_errors " << endl;
			introduce_errors_crc_options_description->print();
		}
	}
	else if (ST.stringcmp(argv[i], "-check_errors") == 0) {
		f_check_errors = TRUE;
		check_errors_crc_options_description = NEW_OBJECT(coding_theory::crc_options_description);
		if (f_v) {
			cout << "-check_errors " << endl;
		}
		check_errors_crc_options_description = NEW_OBJECT(coding_theory::crc_options_description);
		i += check_errors_crc_options_description->read_arguments(argc - (i + 1),
			argv + i + 1, verbose_level);

		if (f_v) {
			cout << "interface_coding_theory::read_arguments finished "
					"reading -check_errors" << endl;
			cout << "i = " << i << endl;
			cout << "argc = " << argc << endl;
			if (i < argc) {
				cout << "next argument is " << argv[i] << endl;
			}
			cout << "-check_errors " << endl;
			check_errors_crc_options_description->print();
		}
	}
	else if (ST.stringcmp(argv[i], "-extract_block") == 0) {
		f_extract_block = TRUE;
		extract_block_crc_options_description = NEW_OBJECT(coding_theory::crc_options_description);
		if (f_v) {
			cout << "-extract_block " << endl;
		}
		extract_block_crc_options_description = NEW_OBJECT(coding_theory::crc_options_description);
		i += extract_block_crc_options_description->read_arguments(argc - (i + 1),
			argv + i + 1, verbose_level);

		if (f_v) {
			cout << "interface_coding_theory::read_arguments finished "
					"reading -extract_block" << endl;
			cout << "i = " << i << endl;
			cout << "argc = " << argc << endl;
			if (i < argc) {
				cout << "next argument is " << argv[i] << endl;
			}
			cout << "-extract_block " << endl;
			extract_block_crc_options_description->print();
		}
	}
	else if (ST.stringcmp(argv[i], "-random_noise_in_bitmap_file") == 0) {
		f_random_noise_in_bitmap_file = TRUE;
		random_noise_in_bitmap_file_input.assign(argv[++i]);
		random_noise_in_bitmap_file_output.assign(argv[++i]);
		random_noise_in_bitmap_file_numerator = ST.strtoi(argv[++i]);
		random_noise_in_bitmap_file_denominator = ST.strtoi(argv[++i]);
		if (f_v) {
			cout << "-random_noise_in_bitmap_file "
				<< " " << random_noise_in_bitmap_file_input
				<< " " << random_noise_in_bitmap_file_output
				<< " " << random_noise_in_bitmap_file_numerator
				<< " " << random_noise_in_bitmap_file_denominator
				<< endl;
		}
	}
	else if (ST.stringcmp(argv[i], "-random_noise_of_burst_type_in_bitmap_file") == 0) {
		f_random_noise_of_burst_type_in_bitmap_file = TRUE;
		random_noise_of_burst_type_in_bitmap_file_input.assign(argv[++i]);
		random_noise_of_burst_type_in_bitmap_file_output.assign(argv[++i]);
		random_noise_of_burst_type_in_bitmap_file_numerator = ST.strtoi(argv[++i]);
		random_noise_of_burst_type_in_bitmap_file_denominator = ST.strtoi(argv[++i]);
		random_noise_of_burst_type_in_bitmap_file_burst_length = ST.strtoi(argv[++i]);
		if (f_v) {
			cout << "-random_noise_of_burst_type_in_bitmap_file "
				<< " " << random_noise_of_burst_type_in_bitmap_file_input
				<< " " << random_noise_of_burst_type_in_bitmap_file_output
				<< " " << random_noise_of_burst_type_in_bitmap_file_numerator
				<< " " << random_noise_of_burst_type_in_bitmap_file_denominator
				<< " " << random_noise_of_burst_type_in_bitmap_file_burst_length
				<< endl;
		}
	}

	if (f_v) {
		cout << "interface_coding_theory::read_arguments done" << endl;
	}
}


void interface_coding_theory::print()
{
	if (f_make_macwilliams_system) {
		cout << "-make_macwilliams_system " << make_macwilliams_system_n << " " << make_macwilliams_system_k << " " << make_macwilliams_system_q << endl;
	}
	if (f_table_of_bounds) {
		cout << "-table_of_bounds " << table_of_bounds_n_max << " " << table_of_bounds_q << endl;
	}
	if (f_make_bounds_for_d_given_n_and_k_and_q) {
		cout << "-make_bounds_for_d_given_n_and_k_and_q " << make_bounds_n << " " << make_bounds_k << " " << make_bounds_q << endl;
	}
	if (f_Hamming_space_distance_matrix) {
		cout << "-Hamming_space_distance_matrix " << Hamming_space_n << " " << Hamming_space_q << endl;
	}
	if (f_introduce_errors) {
		cout << "-introduce_errors " << endl;
		introduce_errors_crc_options_description->print();
	}
	if (f_check_errors) {
		cout << "-check_errors " << endl;
		check_errors_crc_options_description->print();
	}
	if (f_extract_block) {
		cout << "-extract_block " << endl;
		extract_block_crc_options_description->print();
	}
	if (f_random_noise_in_bitmap_file) {
		cout << "-random_noise_in_bitmap_file "
			<< " " << random_noise_in_bitmap_file_input
			<< " " << random_noise_in_bitmap_file_output
			<< " " << random_noise_in_bitmap_file_numerator
			<< " " << random_noise_in_bitmap_file_denominator
			<< endl;
	}
	if (f_random_noise_of_burst_type_in_bitmap_file) {
		cout << "-random_noise_of_burst_type_in_bitmap_file "
			<< " " << random_noise_of_burst_type_in_bitmap_file_input
			<< " " << random_noise_of_burst_type_in_bitmap_file_output
			<< " " << random_noise_of_burst_type_in_bitmap_file_numerator
			<< " " << random_noise_of_burst_type_in_bitmap_file_denominator
			<< " " << random_noise_of_burst_type_in_bitmap_file_burst_length
			<< endl;
	}
}


void interface_coding_theory::worker(int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "interface_coding_theory::worker" << endl;
	}

	if (f_make_macwilliams_system) {

		coding_theory::coding_theory_domain Coding;

		int n, k, q;

		n = make_macwilliams_system_n;
		k = make_macwilliams_system_k;
		q = make_macwilliams_system_q;

		Coding.do_make_macwilliams_system(
				q,
				n,
				k,
				verbose_level);
	}
	else if (f_table_of_bounds) {

		coding_theory::coding_theory_domain Coding;

		Coding.make_table_of_bounds(table_of_bounds_n_max, table_of_bounds_q, verbose_level);
	}
	else if (f_make_bounds_for_d_given_n_and_k_and_q) {

		coding_theory::coding_theory_domain Coding;
		int d_GV;
		int d_singleton;
		int d_hamming;
		int d_plotkin;
		int d_griesmer;

		int n, k, q;

		n = make_bounds_n;
		k = make_bounds_k;
		q = make_bounds_q;

		d_GV = Coding.gilbert_varshamov_lower_bound_for_d(n, k, q, verbose_level);
		d_singleton = Coding.singleton_bound_for_d(n, k, q, verbose_level);
		d_hamming = Coding.hamming_bound_for_d(n, k, q, verbose_level);
		d_plotkin = Coding.plotkin_bound_for_d(n, k, q, verbose_level);
		d_griesmer = Coding.griesmer_bound_for_d(n, k, q, verbose_level);

		cout << "n = " << n << " k=" << k << " q=" << q << endl;

		cout << "d_GV = " << d_GV << endl;
		cout << "d_singleton = " << d_singleton << endl;
		cout << "d_hamming = " << d_hamming << endl;
		cout << "d_plotkin = " << d_plotkin << endl;
		cout << "d_griesmer = " << d_griesmer << endl;

	}
	else if (f_Hamming_space_distance_matrix) {

		coding_theory::coding_theory_domain Coding;

		Coding.make_Hamming_graph_and_write_file(Hamming_space_n, Hamming_space_q,
				FALSE /* f_projective*/, verbose_level);
	}

	else if (f_introduce_errors) {

		coding_theory::crc_codes Crc_codes;

		Crc_codes.introduce_errors(introduce_errors_crc_options_description,
				verbose_level);

	}

	else if (f_check_errors) {

		coding_theory::crc_codes Crc_codes;

		Crc_codes.check_errors(check_errors_crc_options_description,
				verbose_level);

	}

	else if (f_extract_block) {

		coding_theory::crc_codes Crc_codes;

		Crc_codes.extract_block(extract_block_crc_options_description,
				verbose_level);

	}
	else if (f_random_noise_in_bitmap_file) {

		cout << "f_random_noise_in_bitmap_file" << endl;

		graphics::graphical_output G;

		G.random_noise_in_bitmap_file(
				random_noise_in_bitmap_file_input,
				random_noise_in_bitmap_file_output,
				random_noise_in_bitmap_file_numerator,
				random_noise_in_bitmap_file_denominator,
				verbose_level);
	}
	else if (f_random_noise_of_burst_type_in_bitmap_file) {

		cout << "f_random_noise_of_burst_type_in_bitmap_file" << endl;

		graphics::graphical_output G;

		G.random_noise_in_bitmap_file_burst(
				random_noise_of_burst_type_in_bitmap_file_input,
				random_noise_of_burst_type_in_bitmap_file_output,
				random_noise_of_burst_type_in_bitmap_file_numerator,
				random_noise_of_burst_type_in_bitmap_file_denominator,
				random_noise_of_burst_type_in_bitmap_file_burst_length,
				verbose_level);
	}



	if (f_v) {
		cout << "interface_coding_theory::worker done" << endl;
	}
}



}}}


