/*
 * graphical_output.cpp
 *
 *  Created on: Oct 11, 2020
 *      Author: betten
 */

#include "foundations.h"
#include "EasyBMP.h"


using namespace std;


namespace orbiter {
namespace layer1_foundations {
namespace graphics {



static int do_create_points_on_quartic_compute_point_function(double t,
		double *pt, void *extra_data, int verbose_level);
static int do_create_points_on_parabola_compute_point_function(double t,
		double *pt, void *extra_data, int verbose_level);
static int do_create_points_smooth_curve_compute_point_function(double t,
		double *output, void *extra_data, int verbose_level);

std::vector<int> get_color_grayscale(int bit_depth, int max_value, int loopCount, int f_invert_colors, int verbose_level);
static std::vector<int> get_color(int bit_depth, int max_value, int loopCount, int f_invert_colors, int verbose_level);
static void fillBitmap(BMP &image, int i, int j, std::vector<int> color);

static void interface_povray_draw_frame(
	animate *Anim, int h, int nb_frames, int round,
	double clipping_radius,
	std::ostream &fp,
	int verbose_level);


graphical_output::graphical_output()
{

	smooth_curve_Polish = NULL;
	parabola_a = 0.;
	parabola_b = 0.;
	parabola_c = 0.;

}

graphical_output::~graphical_output()
{

}

void graphical_output::draw_layered_graph_from_file(std::string &fname,
		layered_graph_draw_options *Opt,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "graphical_output::draw_layered_graph_from_file fname=" << fname << endl;
	}
	graph_theory::layered_graph *LG;
	orbiter_kernel_system::file_io Fio;

	LG = NEW_OBJECT(graph_theory::layered_graph);
	if (Fio.file_size(fname) <= 0) {
		cout << "graphical_output::draw_layered_graph_from_file file " << fname << " does not exist" << endl;
		exit(1);
		}
	LG->read_file(fname, verbose_level - 1);

	if (f_v) {
		cout << "graphical_output::draw_layered_graph_from_file Layered graph read from file" << endl;
	}

	LG->print_nb_nodes_per_level();

	int data1;


	data1 = LG->data1;

	if (f_v) {
		cout << "graphical_output::draw_layered_graph_from_file data1=" << data1 << endl;
	}

	if (Opt->f_y_stretch) {
		LG->place_with_y_stretch(Opt->y_stretch, verbose_level - 1);
		}
	if (Opt->f_spanning_tree) {
		// create n e w x coordinates
		LG->create_spanning_tree(TRUE /* f_place_x */, verbose_level);
		}
#if 0
	if (Opt->f_numbering_on) {
		// create depth first ranks at each node:
		LG->create_spanning_tree(FALSE /* f_place_x */, verbose_level);
		}
#endif

	if (Opt->f_x_stretch) {
		LG->scale_x_coordinates(Opt->x_stretch, verbose_level);
	}


	string fname_out;
	data_structures::string_tools ST;

	fname_out.assign(fname);
	ST.chop_off_extension(fname_out);
	fname_out.append("_draw");

	//fname_out.append(".mp");

	if (Opt->f_paths_in_between) {

		if (f_v) {
			cout << "graphical_output::draw_layered_graph_from_file f_paths_in_between" << endl;
		}
		std::vector<std::vector<int> > All_Paths;

		if (f_v) {
			cout << "graphical_output::draw_layered_graph_from_file before LG->find_all_paths_between" << endl;
		}
		LG->find_all_paths_between(Opt->layer1, Opt->node1, Opt->layer2, Opt->node2,
				All_Paths,
				verbose_level - 2);
		if (f_v) {
			cout << "graphical_output::draw_layered_graph_from_file after LG->find_all_paths_between" << endl;
		}

		if (f_v) {
			cout << "graphical_output::draw_layered_graph_from_file before LG->remove_edges" << endl;
		}
		LG->remove_edges(Opt->layer1, Opt->node1, Opt->layer2, Opt->node2,
				All_Paths,
				verbose_level - 2);
		if (f_v) {
			cout << "graphical_output::draw_layered_graph_from_file after LG->remove_edges" << endl;
		}


	}

	string fname_full;

	fname_full.assign(fname_out);
	fname_full.append(".mp");

	LG->draw_with_options(fname_out, Opt, verbose_level - 10);

	int n;
	double avg;
	n = LG->nb_nodes();
	avg = LG->average_word_length();
	if (f_v) {
		cout << "graphical_output::draw_layered_graph_from_file "
				"number of nodes = " << n << endl;
		cout << "graphical_output::draw_layered_graph_from_file "
				"average word length = " << avg << endl;
	}


	if (f_v) {
		cout << "graphical_output::draw_layered_graph_from_file "
				"Written file " << fname_full << " of size " << Fio.file_size(fname_full) << endl;
	}

	FREE_OBJECT(LG);

	if (f_v) {
		cout << "graphical_output::draw_layered_graph_from_file done" << endl;
	}
}

void graphical_output::do_domino_portrait(int D, int s,
		std::string &photo_label,
		layered_graph_draw_options *Opt,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "graphical_output::do_domino_portrait "
				"D=" << D << " s=" << s
				<< " photo_label=" << photo_label << endl;
	}


	string fname;
	//seed_random_generator_with_system_time();

	srand(777);

	if (f_v) {
		cout << "graphical_output::do_domino_portrait" << endl;
	}


	combinatorics::domino_assignment *Domino_Assignment;

	Domino_Assignment = NEW_OBJECT(combinatorics::domino_assignment);

	Domino_Assignment->initialize_assignment(D, s, verbose_level - 1);

	Domino_Assignment->read_photo(photo_label, verbose_level - 1);


	if (f_v) {
		cout << "graphical_output::do_domino_portrait before stage 0" << endl;
	}
	Domino_Assignment->stage0(verbose_level - 1);
	if (f_v) {
		cout << "graphical_output::do_domino_portrait after stage 0" << endl;
	}


	if (f_v) {
		cout << "graphical_output::do_domino_portrait before stage 1" << endl;
	}
	Domino_Assignment->stage1(verbose_level - 1);
	if (f_v) {
		cout << "graphical_output::do_domino_portrait after stage 1" << endl;
	}


	if (f_v) {
		cout << "graphical_output::do_domino_portrait before stage 2" << endl;
	}
	Domino_Assignment->stage2(verbose_level - 1);
	if (f_v) {
		cout << "graphical_output::do_domino_portrait after stage 2" << endl;
	}



#if 0
	sorting Sorting;

	Sorting.Heapsort_general((void *)Assi_table_sort[hd], tot_dom,
			compare_assignment,
			swap_assignment,
			Assi_table[hd]);
	//Sorting.quicksort_array(tot_dom,
	//		(void **) Assi_table_sort[hd],
	//		compare_assignment,
	//		Assi_table[hd] /* void *data */);

	//quicksort_array(tot_dom,
	//		Assi_table_sort[hd],
	//		compare_assignment,
	//		Assi_table[hd] /* void *data */);
#endif

	//cout << "solution " << hd << " after sort" << endl;
	//print_assignment(hd);


	fname.assign(photo_label);

	char str[1000];
	snprintf(str, sizeof(str), "_solution_%d", 0 /* hd */);
	fname.append(str);


	if (f_v) {
		cout << "graphical_output::do_domino_portrait calling draw_domino" << endl;
	}

	int cost;

	cost = Domino_Assignment->cost_function();
	Domino_Assignment->draw_domino_matrix(fname,
			Domino_Assignment->tot_dom,
			TRUE /* f_has_cost */, cost,
			Opt,
			verbose_level - 1);

	if (f_v) {
		cout << "graphical_output::do_domino_portrait after draw_domino" << endl;
	}


	if (f_v) {
		cout << "graphical_output::do_domino_portrait calling prepare_latex" << endl;
	}
	Domino_Assignment->prepare_latex(photo_label, verbose_level);
	if (f_v) {
		cout << "graphical_output::do_domino_portrait after prepare_latex" << endl;
	}

	if (f_v) {
		cout << "graphical_output::do_domino_portrait nb_changes=" << Domino_Assignment->nb_changes << endl;
	}
	if (f_v) {
		cout << "graphical_output::do_domino_portrait before classify_changes_by_type" << endl;
	}

	Domino_Assignment->classify_changes_by_type(verbose_level);
	if (f_v) {
		cout << "graphical_output::do_domino_portrait after classify_changes_by_type" << endl;
	}

	int *Cost;
	int len;
	string fname_cost;
	orbiter_kernel_system::file_io Fio;
	data_structures::string_tools String;

	fname_cost.assign(photo_label);
	String.chop_off_extension(fname_cost);
	fname_cost.append("_cost.csv");

	if (f_v) {
		cout << "graphical_output::do_domino_portrait before get_cost_function" << endl;
	}
	Domino_Assignment->get_cost_function(Cost, len, verbose_level);
	if (f_v) {
		cout << "graphical_output::do_domino_portrait after get_cost_function" << endl;
		cout << "graphical_output::do_domino_portrait after get_cost_function len=" << len << endl;
	}

	Fio.int_vec_write_csv(Cost, len, fname_cost, "Cost");

	if (f_v) {
		cout << "Written file " << fname_cost << " of size " << Fio.file_size(fname_cost) << endl;
	}


	if (f_v) {
		cout << "graphical_output::do_domino_portrait done" << endl;
	}
}

void graphical_output::do_create_points_on_quartic(double desired_distance, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "graphical_output::do_create_points_on_quartic" << endl;
	}

	double amin, amid, amax;
	//double epsilon = 0.001;
	int N = 200;
	int i;

	//a0 = 16. / 25.;
	//b0 = 16. / 25.;

	amin = 0;
	amid = 16. / 25.;
	amax = 100;

	int nb;

	{
		parametric_curve C1;
		parametric_curve C2;

		C1.init(2 /* nb_dimensions */,
				desired_distance,
				amin, amid,
				do_create_points_on_quartic_compute_point_function,
				this /* extra_data */,
				100. /* boundary */,
				N,
				verbose_level);

		cout << "after parametric_curve::init, C1.Pts.size()=" << C1.Pts.size() << endl;


		C2.init(2 /* nb_dimensions */,
				desired_distance,
				amid, amax,
				do_create_points_on_quartic_compute_point_function,
				this /* extra_data */,
				100. /* boundary */,
				N,
				verbose_level);

		cout << "after parametric_curve::init, C2.Pts.size()=" << C2.Pts.size() << endl;


		for (i = 0; i < (int) C1.Pts.size(); i++) {
			cout << C1.Pts[i].t << " : " << C1.Pts[i].coords[0] << ", " << C1.Pts[i].coords[1] << endl;
		}

		double *Pts;
		int nb_pts;

		nb_pts = 4 * (C1.Pts.size() + C2.Pts.size());
		Pts = new double[nb_pts * 2];
		nb = 0;
		for (i = 0; i < (int) C1.Pts.size(); i++) {
			Pts[nb * 2 + 0] = C1.Pts[i].coords[0];
			Pts[nb * 2 + 1] = C1.Pts[i].coords[1];
			nb++;
		}
		for (i = 0; i < (int) C1.Pts.size(); i++) {
			Pts[nb * 2 + 0] = -1 * C1.Pts[i].coords[0];
			Pts[nb * 2 + 1] = C1.Pts[i].coords[1];
			nb++;
		}
		for (i = 0; i < (int) C1.Pts.size(); i++) {
			Pts[nb * 2 + 0] = C1.Pts[i].coords[0];
			Pts[nb * 2 + 1] = -1 * C1.Pts[i].coords[1];
			nb++;
		}
		for (i = 0; i < (int) C1.Pts.size(); i++) {
			Pts[nb * 2 + 0] = -1 * C1.Pts[i].coords[0];
			Pts[nb * 2 + 1] = -1 * C1.Pts[i].coords[1];
			nb++;
		}
		for (i = 0; i < (int) C2.Pts.size(); i++) {
			Pts[nb * 2 + 0] = C2.Pts[i].coords[0];
			Pts[nb * 2 + 1] = C2.Pts[i].coords[1];
			nb++;
		}
		for (i = 0; i < (int) C2.Pts.size(); i++) {
			Pts[nb * 2 + 0] = -1 * C2.Pts[i].coords[0];
			Pts[nb * 2 + 1] = C2.Pts[i].coords[1];
			nb++;
		}
		for (i = 0; i < (int) C2.Pts.size(); i++) {
			Pts[nb * 2 + 0] = C2.Pts[i].coords[0];
			Pts[nb * 2 + 1] = -1 * C2.Pts[i].coords[1];
			nb++;
		}
		for (i = 0; i < (int) C2.Pts.size(); i++) {
			Pts[nb * 2 + 0] = -1 * C2.Pts[i].coords[0];
			Pts[nb * 2 + 1] = -1 * C2.Pts[i].coords[1];
			nb++;
		}
		orbiter_kernel_system::file_io Fio;

		string fname;

		fname.assign("points.csv");

		Fio.double_matrix_write_csv(fname, Pts, nb, 2);

		cout << "created curve 1 with " << C1.Pts.size() << " many points" << endl;
		cout << "created curve 2 with " << C2.Pts.size() << " many points" << endl;
	}
	cout << "created 4  curves with " << nb << " many points" << endl;



	if (f_v) {
		cout << "graphical_output::do_create_points_on_quartic done" << endl;
	}
}

void graphical_output::do_create_points_on_parabola(
		double desired_distance, int N,
		double a, double b, double c,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "graphical_output::do_create_points_on_parabola" << endl;
	}

	double amin, amax;
	double boundary;
	int i;

	amin = -10;
	amax = 3.08;
	boundary = 10;

	int nb;

	graphical_output::parabola_a = a;
	graphical_output::parabola_b = b;
	graphical_output::parabola_c = c;

	{
		parametric_curve C;

		C.init(2 /* nb_dimensions */,
				desired_distance,
				amin, amax,
				do_create_points_on_parabola_compute_point_function,
				this /* extra_data */,
				boundary,
				N,
				verbose_level);

		cout << "after parametric_curve::init, C.Pts.size()=" << C.Pts.size() << endl;




		for (i = 0; i < (int) C.Pts.size(); i++) {
			cout << C.Pts[i].t << " : " << C.Pts[i].coords[0] << ", " << C.Pts[i].coords[1] << endl;
		}

		{
		double *Pts;
		int nb_pts;

		nb_pts = C.Pts.size();
		Pts = new double[nb_pts * 2];
		nb = 0;
		for (i = 0; i < (int) C.Pts.size(); i++) {
			Pts[nb * 2 + 0] = C.Pts[i].coords[0];
			Pts[nb * 2 + 1] = C.Pts[i].coords[1];
			nb++;
		}
		orbiter_kernel_system::file_io Fio;
		char str[1000];
		string fname;

		snprintf(str, 1000, "parabola_N%d_%lf_%lf_%lf_points.csv", N, a, b, c);
		fname.assign(str);

		Fio.double_matrix_write_csv(fname, Pts, nb, 2);

		cout << "created curve 1 with " << C.Pts.size() << " many points" << endl;
		cout << "Written file " << fname << " of size " << Fio.file_size(fname) << endl;

		delete [] Pts;
		}

		{
		double *Pts;
		int nb_pts;

		nb_pts = C.Pts.size();
		Pts = new double[nb_pts * 6];
		nb = 0;
		for (i = 0; i < (int) C.Pts.size(); i++) {
			Pts[nb * 6 + 0] = C.Pts[i].coords[0];
			Pts[nb * 6 + 1] = C.Pts[i].coords[1];
			Pts[nb * 6 + 2] = 0.;
			Pts[nb * 6 + 3] = 0.;
			Pts[nb * 6 + 4] = 0.;
			Pts[nb * 6 + 5] = 1.;
			nb++;
		}
		orbiter_kernel_system::file_io Fio;
		char str[1000];
		string fname;
		snprintf(str, 1000, "parabola_N%d_%lf_%lf_%lf_projection_from_center.csv", N, a, b, c);
		fname.assign(str);

		Fio.double_matrix_write_csv(fname, Pts, nb, 6);

		cout << "created family of lines 1 with " << C.Pts.size() << " many lines" << endl;
		cout << "Written file " << fname << " of size " << Fio.file_size(fname) << endl;

		delete [] Pts;
		}

		{
		double *Pts;
		int nb_pts;
		double x, y, H, f;
		double h = 1.;

		nb_pts = C.Pts.size();
		Pts = new double[nb_pts * 6];
		nb = 0;
		for (i = 0; i < (int) C.Pts.size(); i++) {
			x = C.Pts[i].coords[0];
			y = C.Pts[i].coords[1];
			Pts[nb * 6 + 0] = x;
			Pts[nb * 6 + 1] = y;
			Pts[nb * 6 + 2] = 0.;

			H = sqrt(h * h + x * x + y * y);
			f = h / H;

			Pts[nb * 6 + 3] = x * f;
			Pts[nb * 6 + 4] = y * f;
			Pts[nb * 6 + 5] = 1. - f;
			nb++;
		}
		orbiter_kernel_system::file_io Fio;

		char str[1000];
		string fname;
		snprintf(str, 1000, "parabola_N%d_%lf_%lf_%lf_projection_from_sphere.csv", N, a, b, c);
		fname.assign(str);


		Fio.double_matrix_write_csv(fname, Pts, nb, 6);

		cout << "created family of lines 1 with " << C.Pts.size() << " many lines" << endl;
		cout << "Written file " << fname << " of size " << Fio.file_size(fname) << endl;

		delete [] Pts;
		}

		{
		double *Pts;
		int nb_pts;
		double x, y, H, f;
		double h = 1.;

		nb_pts = C.Pts.size();
		Pts = new double[nb_pts * 3];
		nb = 0;
		for (i = 0; i < (int) C.Pts.size(); i++) {
			x = C.Pts[i].coords[0];
			y = C.Pts[i].coords[1];

			H = sqrt(h * h + x * x + y * y);
			f = h / H;

			Pts[nb * 3 + 0] = x * f;
			Pts[nb * 3 + 1] = y * f;
			Pts[nb * 3 + 2] = 1. - f;
			nb++;
		}
		orbiter_kernel_system::file_io Fio;

		char str[1000];
		string fname;
		snprintf(str, 1000, "parabola_N%d_%lf_%lf_%lf_points_projected.csv", N, a, b, c);
		fname.assign(str);


		Fio.double_matrix_write_csv(fname, Pts, nb, 3);

		cout << "created family of lines 1 with " << C.Pts.size() << " many lines" << endl;
		cout << "Written file " << fname << " of size " << Fio.file_size(fname) << endl;

		delete [] Pts;
		}


	}
	cout << "created curve with " << nb << " many points" << endl;



	if (f_v) {
		cout << "graphical_output::do_create_points_on_parabola done" << endl;
	}
}

void graphical_output::do_smooth_curve(std::string &curve_label,
		double desired_distance, int N,
		double t_min, double t_max, double boundary,
		function_polish_description *FP_descr, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	int nb_dimensions;

	if (f_v) {
		cout << "graphical_output::do_smooth_curve" << endl;
	}

	smooth_curve_Polish = NEW_OBJECT(function_polish);

	if (f_v) {
		cout << "graphical_output::do_smooth_curve before smooth_curve_Polish->init" << endl;
	}
	smooth_curve_Polish->init(FP_descr, verbose_level);
	if (f_v) {
		cout << "graphical_output::do_smooth_curve after smooth_curve_Polish->init" << endl;
	}
#if 0
	if (smooth_curve_Polish->Variables.size() != 1) {
		cout << "interface_projective::do_smooth_curve number of variables should be 1, is "
				<< smooth_curve_Polish->Variables.size() << endl;
		exit(1);
	}
#endif
	nb_dimensions = smooth_curve_Polish->Entry.size();
	if (f_v) {
		cout << "graphical_output::do_smooth_curve nb_dimensions = " << nb_dimensions << endl;
	}


	{
		parametric_curve C;

		C.init(nb_dimensions,
				desired_distance,
				t_min, t_max,
				do_create_points_smooth_curve_compute_point_function,
				this /* extra_data */,
				boundary,
				N,
				verbose_level);

		cout << "after parametric_curve::init, C.Pts.size()=" << C.Pts.size() << endl;

		{
		double *Pts;
		int nb_pts;
		int i, j, nb;

		nb_pts = C.Pts.size();
		Pts = new double[nb_pts * nb_dimensions];
		nb = 0;
		for (i = 0; i < (int) C.Pts.size(); i++) {
			if (C.Pts[i].f_is_valid) {
				for (j = 0; j < nb_dimensions; j++) {
					Pts[nb * nb_dimensions + j] = C.Pts[i].coords[j];
				}
				nb++;
			}
		}
		orbiter_kernel_system::file_io Fio;

		char str[1000];
		string fname;
		snprintf(str, 1000, "function_%s_N%d_points.csv", curve_label.c_str(), N);
		fname.assign(str);


		Fio.double_matrix_write_csv(fname, Pts, nb, nb_dimensions);

		cout << "created curve 1 with " << C.Pts.size() << " many points" << endl;
		cout << "Written file " << fname << " of size " << Fio.file_size(fname) << endl;

		delete [] Pts;
		}

		{
		double *Pts;
		int nb_pts;
		int i, j, nb, n;
		double d; // euclidean distance to the previous point
		numerics Num;

		nb_pts = C.Pts.size();
		n = 1 + nb_dimensions + 1;
		Pts = new double[nb_pts * n];
		nb = 0;
		for (i = 0; i < (int) C.Pts.size(); i++) {
			if (C.Pts[i].f_is_valid) {
				Pts[nb * n + 0] = C.Pts[i].t;
				for (j = 0; j < nb_dimensions; j++) {
					Pts[nb * n + 1 + j] = C.Pts[i].coords[j];
				}
				if (nb) {
					d = Num.distance_euclidean(Pts + (nb - 1) * n + 1, Pts + nb * n + 1, 3);
				}
				else {
					d = 0;
				}
				Pts[nb * n + 1 + 4 + 0] = d;
				nb++;
			}
		}
		orbiter_kernel_system::file_io Fio;

		char str[1000];
		string fname;
		snprintf(str, 1000, "function_%s_N%d_points_plus.csv", curve_label.c_str(), N);
		fname.assign(str);

		Fio.double_matrix_write_csv(fname, Pts, nb, n);

		cout << "created curve 1 with " << C.Pts.size() << " many points" << endl;
		cout << "Written file " << fname << " of size " << Fio.file_size(fname) << endl;

		delete [] Pts;
		}

	}

	if (f_v) {
		cout << "graphical_output::do_smooth_curve done" << endl;
	}
}


//

static int do_create_points_on_quartic_compute_point_function(double t,
		double *pt, void *extra_data, int verbose_level)
{
	double num, denom, b;
	double epsilon = 0.00001;

	num = 4. - 4. * t;
	denom = 4. - 25. * t * 0.25;
	if (ABS(denom) < epsilon) {
		return FALSE;
	}
	else {
		b = num / denom;
		if (b < 0) {
			return FALSE;
		}
		else {
			pt[0] = sqrt(t);
			pt[1] = sqrt(b);
		}
	}
	cout << "created point " << pt[0] << ", " << pt[1] << endl;
	return TRUE;
}

static int do_create_points_on_parabola_compute_point_function(double t,
		double *pt, void *extra_data, int verbose_level)
{
	graphical_output *I = (graphical_output *) extra_data;
	double a = I->parabola_a;
	double b = I->parabola_b;
	double c = I->parabola_c;

	pt[0] = t;
	pt[1] = a * t * t + b * t + c;
	//cout << "created point " << pt[0] << ", " << pt[1] << endl;
	return TRUE;
}


static int do_create_points_smooth_curve_compute_point_function(double t,
		double *output, void *extra_data, int verbose_level)
{
	int f_v = (verbose_level >= 1);
	graphical_output *I = (graphical_output *) extra_data;
	int ret = FALSE;
	double epsilon = 0.0001;
	double *input; // to store the input variable and all local variables during evaluate


	if (f_v) {
		cout << "do_create_points_smooth_curve_compute_point_function t = " << t << endl;
	}
	if (f_v) {
		cout << "do_create_points_smooth_curve_compute_point_function before evaluate" << endl;
	}
	input = new double[I->smooth_curve_Polish->Variables.size()];
	input[0] = t;
	I->smooth_curve_Polish->evaluate(
			input /* variable_values */,
			output,
			verbose_level);
	delete [] input;

	if (I->smooth_curve_Polish->Entry.size() == 4) {
		if (ABS(output[3]) < epsilon) {
			ret = FALSE;
		}
		else {
			double av = 1. / output[3];
			output[0] *= av;
			output[1] *= av;
			output[2] *= av;
			output[3] *= av;
			ret = TRUE;
		}
	}
	else {
		ret = TRUE;
	}
	if (f_v) {
		cout << "do_create_points_smooth_curve_compute_point_function after evaluate t = " << t << endl;
	}
	return ret;
}

void graphical_output::draw_bitmap(draw_bitmap_control *C, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "graphical_output::draw_bitmap" << endl;
	}
	orbiter_kernel_system::file_io Fio;


	if (C->f_input_csv_file) {

		Fio.int_matrix_read_csv(C->input_csv_file_name,
				C->M, C->m, C->n,
				verbose_level);

		if (C->f_secondary_input_csv_file) {


			int m, n;


			Fio.int_matrix_read_csv(C->secondary_input_csv_file_name,
					C->M2, m, n,
					verbose_level);
			if (m != C->m) {
				cout << "secondary matrix must have the same size as the primary input matrix" << endl;
				exit(1);
			}
			if (n != C->n) {
				cout << "secondary matrix must have the same size as the primary input matrix" << endl;
				exit(1);
			}
		}

	}
	else if (C->f_input_object) {

		Get_matrix(C->input_object_label,
				C->M, C->m, C->n);
	}

	if (f_v) {
		cout << "graphical_output::draw_bitmap drawing matrix of size " << C->m << " x " << C->n << endl;
	}

	int *Row_parts = NULL;
	int nb_row_parts = 0;
	int *Col_parts = NULL;
	int nb_col_parts = 0;


	if (C->f_partition) {

		Get_int_vector_from_label(C->part_row, Row_parts, nb_row_parts, 0 /* verbose_level*/);
		Get_int_vector_from_label(C->part_col, Col_parts, nb_col_parts, 0 /* verbose_level*/);

		if (f_v) {
			cout << "row_part: ";
			Int_vec_print(cout, Row_parts, nb_row_parts);
			cout << endl;
			cout << "col_part: ";
			Int_vec_print(cout, Col_parts, nb_col_parts);
			cout << endl;
		}
	}
	int i;
	int max_value;
	data_structures::string_tools ST;

	max_value = orbiter_kernel_system::Orbiter->Int_vec->maximum(C->M, C->m * C->n);
	if (f_v) {
		cout << "max_value=" << max_value << endl;
	}

	//max_value += 5;
	//cout << "max_value after adjustment=" << max_value << endl;


	string fname_out;

	if (C->f_input_csv_file) {
		fname_out.assign(C->input_csv_file_name);
	}
	else {
		fname_out.assign("bitmatrix.csv");

	}
	ST.replace_extension_with(fname_out, "_draw.bmp");

	//int bit_depth = 8;

	BMP image;

	//int bit_depth = 24;


	if (max_value > 10000) {
		cout << "graphical_output::draw_bitmap max_value > 10000" << endl;
		exit(1);
	}
	if (max_value == 0) {
		max_value = 1;
	}


	if (f_v) {
		cout << "graphical_output::draw_bitmap color palette:" << endl;
		for (i = 0; i <= max_value; i++) {
			std::vector<int> color;

			if (C->f_grayscale) {
				color = get_color_grayscale(C->bit_depth, max_value, i, C->f_invert_colors, 1);
			}
			else {
				color = get_color(C->bit_depth, max_value, i, C->f_invert_colors, 1);
			}
			cout << "color " << i << " : " << color[0] << "," << color[1] << "," << color[2] << endl;
		}
	}

	int width, height;
	//int *Table;
	geometry::geometry_global Gg;

	width = C->n;
	height = C->m;

	if (f_v) {
		cout << "width=" << width << endl;
	}

	if (C->f_box_width) {
		image.SetSize(width * C->box_width, height * C->box_width);
	}
	else {
		image.SetSize(width, height);
	}

	image.SetBitDepth(C->bit_depth);

	int j, d;
	int N, N100, cnt;
	int indent = 0;

	N = height * width;
	N100 = N / 100 + 1;

	if (f_v) {
		cout << "N100=" << N100 << endl;
	}

	cnt = 0;

	if (C->f_secondary_input_csv_file) {
		indent = C->box_width >> 2;
	}
	if (f_v) {
		cout << "indent=" << indent << endl;
	}

	std::vector<int> color_white;

	if (C->f_grayscale) {
		color_white = get_color_grayscale(C->bit_depth, max_value, 0, C->f_invert_colors, 0);
	}
	else {
		color_white = get_color(C->bit_depth, max_value, 0, C->f_invert_colors, 0);
	}

	if (f_v) {
		cout << "color_white=" << color_white[0] << "," << color_white[1] << "," << color_white[2] << endl;
	}



	for (i = 0; i < height; i++) {



		for (j = 0; j < width; j++, cnt++) {


			if ((cnt % N100) == 0) {
				cout << "we are at " << ((double) cnt / (double) N) * 100. << " %" << endl;
			}
			d = C->M[i * width + j];
			//std::vector<int> color = getColor(M[idx_x * width + idx_z]);
			std::vector<int> color;

			if (C->f_grayscale) {
				color = get_color_grayscale(C->bit_depth, max_value, d, C->f_invert_colors, 0);
			}
			else {
				color = get_color(C->bit_depth, max_value, d, C->f_invert_colors, 0);

			}



			// Here the pixel is set on the image.
			if (C->f_box_width) {
				int I, J, u, v;

				I = i * C->box_width;
				J = j * C->box_width;

				if (C->f_secondary_input_csv_file) {
					if (C->M2[i * width + j] == 0) {
						for (u = 0; u < C->box_width; u++) {
							for (v = 0; v < C->box_width; v++) {
								if (u < indent || u >= C->box_width - indent || v < indent || v >= C->box_width - indent) {
									fillBitmap(image, J + v, I + u, color_white);
								}
								else {
									fillBitmap(image, J + v, I + u, color);
								}
							}
						}
					}
					else {
						for (u = 0; u < C->box_width; u++) {
							for (v = 0; v < C->box_width; v++) {
								fillBitmap(image, J + v, I + u, color);
							}
						}
					}
				}
				else {
					for (u = 0; u < C->box_width; u++) {
						for (v = 0; v < C->box_width; v++) {
							fillBitmap(image, J + v, I + u, color);
						}
					}
				}

			}
			else {
				fillBitmap(image, j, i, color);
			}
		}
	}
	if (C->f_partition) {

		if (f_v) {
			cout << "drawing the partition" << endl;
		}
		int i0, j0;
		int h, t, I, J;
		std::vector<int> color;


		if (C->f_grayscale) {
			color = get_color_grayscale(C->bit_depth, max_value, max_value, C->f_invert_colors, 0);
		}
		else {
			color = get_color(C->bit_depth, max_value, 1, C->f_invert_colors, 0);

		}

		// row partition:
		i0 = 0;
		for (h = 0; h <= nb_row_parts; h++) {
			for (t = 0; t < C->part_width; t++) {
				if (C->f_box_width) {
					for (j = 0; j < width * C->box_width; j++) {
						I = i0 * C->box_width;
						if (h == nb_row_parts) {
							fillBitmap(image, j, I - 1 - t, color);
						}
						else {
							fillBitmap(image, j, I + t, color);
						}
					}
				}
			}
			if (h < nb_row_parts) {
				i0 += Row_parts[h];
			}
		}

		// col partition:
		j0 = 0;
		for (h = 0; h <= nb_col_parts; h++) {
			for (t = 0; t < C->part_width; t++) {
				if (C->f_box_width) {
					for (i = 0; i < height * C->box_width; i++) {
						J = j0 * C->box_width;
						if (h == nb_col_parts) {
							fillBitmap(image, J - 1 - t, i, color);
						}
						else {
							fillBitmap(image, J + t, i, color);
						}
					}
				}
			}
			if (h < nb_col_parts) {
				j0 += Col_parts[h];
			}
		}
	}

	if (f_v) {
		cout << "before writing the image to file as " << fname_out << endl;
	}

	image.WriteToFile(fname_out.c_str());

	if (f_v) {
		std::cout << "Written file " << fname_out << std::endl;
		{
			orbiter_kernel_system::file_io Fio;
			cout << "Written file " << fname_out << " of size " << Fio.file_size(fname_out) << endl;
		}
	}



	if (f_v) {
		cout << "graphical_output::draw_bitmap done" << endl;
	}

}

void graphical_output::random_noise_in_bitmap_file(
		std::string fname_input,
		std::string fname_output,
		int probability_numerator,
		int probability_denominator,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "graphical_output::random_noise_in_bitmap_file" << endl;
	}
	orbiter_kernel_system::file_io Fio;

	BMP image;
	int H, W, i, j, r, c;
	orbiter_kernel_system::os_interface Os;

	image.ReadFromFile(fname_input.c_str());

	H = image.TellHeight();
	W = image.TellWidth();

	cout << "image size H=" << H << " W=" << W << endl;


	for (i = 0; i < H; i++) {
		for (j = 0; j < W; j++) {
			RGBApixel pix, pix1;

			pix = image.GetPixel(j, i);
			cout << i << " : " << j << " : " << (int) pix.Blue << "," << (int) pix.Green << "," << (int) pix.Red << endl;


			r = Os.random_integer(probability_denominator);
			if (r < probability_numerator) {

				c = Os.random_integer(256);
				pix1.Blue = pix1.Green = pix1.Red = c;
				pix1.Alpha = pix.Alpha;
				image.SetPixel(j, i, pix1);
			}
		}
	}

	image.WriteToFile(fname_output.c_str());


	if (f_v) {
		cout << "graphical_output::random_noise_in_bitmap_file done" << endl;
	}
}

void graphical_output::random_noise_in_bitmap_file_burst(
		std::string fname_input,
		std::string fname_output,
		int probability_numerator,
		int probability_denominator,
		int burst_length_max,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "graphical_output::random_noise_in_bitmap_file_burst" << endl;
	}
	orbiter_kernel_system::file_io Fio;

	BMP image;
	int H, W, i, j, h, k, r, c, burst_length;
	orbiter_kernel_system::os_interface Os;

	image.ReadFromFile(fname_input.c_str());

	H = image.TellHeight();
	W = image.TellWidth();

	cout << "image size H=" << H << " W=" << W << endl;

	probability_denominator *= burst_length_max * 2;

	for (i = 0; i < H; i++) {
		for (j = 0; j < W; j++) {
			RGBApixel pix, pix1;

			pix = image.GetPixel(j, i);
			cout << i << " : " << j << " : " << (int) pix.Blue << "," << (int) pix.Green << "," << (int) pix.Red << endl;


			r = Os.random_integer(probability_denominator);
			if (r < probability_numerator) {

				burst_length = Os.random_integer(burst_length_max);
				c = Os.random_integer(256);

				for (h = 0; h < burst_length; h++) {
					for (k = 0; k < 5; k++) {
						pix1.Blue = pix1.Green = pix1.Red = c;
						pix1.Alpha = pix.Alpha;
						if (j + h < W && i + k < H) {
							image.SetPixel(j + h, i + k, pix1);
						}
					}
				}
			}
		}
	}

	image.WriteToFile(fname_output.c_str());


	if (f_v) {
		cout << "graphical_output::random_noise_in_bitmap_file_burst done" << endl;
	}
}

void graphical_output::draw_projective_curve(draw_projective_curve_description *Descr,
		layered_graph_draw_options *Opt, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "graphical_output::draw_projective_curve" << endl;
	}

	orbiter_kernel_system::os_interface Os;
	int t0 = Os.os_ticks();
	//int xmax = Opt->xin; //1500;
	//int ymax = Opt->yin; //1500;
	int i;


	if (Descr->f_animate) {

		for (i = 0; i <= Descr->animate_nb_of_steps; i++) {


			if (f_v) {
				cout << "animate step " << i << " / " << Descr->animate_nb_of_steps << ":" << endl;
				}
			mp_graphics G;

			char str[1000];
			string fname;

			snprintf(str, sizeof(str), "%s_%d_%d", Descr->fname.c_str(), Descr->number, i);
			fname.assign(str);

			G.init(fname, Opt, verbose_level);
			//G.setup(fname, 0, 0, ONE_MILLION, ONE_MILLION,
			//		xmax, ymax, Opt->f_embedded, Opt->f_sideways, Opt->scale, Opt->line_width, verbose_level - 1);
			//G.setup(fname2, 0, 0, ONE_MILLION, ONE_MILLION, xmax, ymax);


			//G.frame(0.05);


			draw_projective(G, Descr->number, i, Descr->animate_nb_of_steps, FALSE, 0, 0, FALSE, 0, FALSE, 0);
			G.finish(cout, TRUE);

		}
	}
	else if (Descr->f_animate_with_transition) {
		int frame;

		frame = 0;

		if (Descr->f_title_page) {

			for (i = 0; i < 4; i++, frame++) {
				mp_graphics G;

				char str[1000];
				string fname;

				snprintf(str, sizeof(str), "%s_%d_%d", Descr->fname.c_str(), Descr->number, frame);
				fname.assign(str);

				G.init(fname, Opt, verbose_level);
				//G.setup(fname, 0, 0, ONE_MILLION, ONE_MILLION,
				//		xmax, ymax, Opt->f_embedded, Opt->f_sideways, Opt->scale, Opt->line_width,
				//		verbose_level - 1);

				draw_projective(G, Descr->number, 0, Descr->animate_nb_of_steps, TRUE, i, Descr->animate_transition_nb_of_steps, TRUE, i, FALSE, 0);


				G.finish(cout, TRUE);


			}
		}

		for (i = 0; i <= Descr->animate_transition_nb_of_steps; i++, frame++) {

			if (f_v) {
				cout << "frame " << frame << " transition in step " << i << " / " << Descr->animate_transition_nb_of_steps << ":" << endl;
			}
			mp_graphics G;

			char str[1000];
			string fname;

			snprintf(str, sizeof(str), "%s_%d_%d", Descr->fname.c_str(), Descr->number, frame);
			fname.assign(str);

			G.init(fname, Opt, verbose_level);
			//G.setup(fname, 0, 0, ONE_MILLION, ONE_MILLION,
			//		xmax, ymax, Opt->f_embedded, Opt->f_sideways, Opt->scale, Opt->line_width,
			//		verbose_level - 1);
			//G.setup(fname2, 0, 0, ONE_MILLION, ONE_MILLION, xmax, ymax);

			//G.frame(0.05);


			draw_projective(G, Descr->number, 0, Descr->animate_nb_of_steps, TRUE, i, Descr->animate_transition_nb_of_steps, FALSE, 0, FALSE, 0);
			G.finish(cout, TRUE);

		}

		for (i = 0; i <= Descr->animate_nb_of_steps; i++, frame++) {


			if (f_v) {
				cout << "frame " << frame << " animate step " << i << " / " << Descr->animate_nb_of_steps << ":" << endl;
			}
			mp_graphics G;

			char str[1000];
			string fname;

			snprintf(str, sizeof(str), "%s_%d_%d", Descr->fname.c_str(), Descr->number, frame);
			fname.assign(str);

			G.init(fname, Opt, verbose_level);
			//G.setup(fname, 0, 0, ONE_MILLION, ONE_MILLION,
			//		xmax, ymax, Opt->f_embedded, Opt->f_sideways, Opt->scale, Opt->line_width,
			//		verbose_level - 1);
			//G.setup(fname2, 0, 0, ONE_MILLION, ONE_MILLION, xmax, ymax);

			//G.frame(0.05);


			draw_projective(G, Descr->number, i, Descr->animate_nb_of_steps, FALSE, 0, 0, FALSE, 0, FALSE, 0);
			G.finish(cout, TRUE);

		}

		for (i = 0; i <= Descr->animate_transition_nb_of_steps; i++, frame++) {

			if (f_v) {
				cout << "frame " << frame << " transition in step " << i << " / " << Descr->animate_transition_nb_of_steps << ":" << endl;
			}
			mp_graphics G;

			char str[1000];
			string fname;

			snprintf(str, sizeof(str), "%s_%d_%d", Descr->fname.c_str(), Descr->number, frame);
			fname.assign(str);

			G.init(fname, Opt, verbose_level);
			//G.setup(fname, 0, 0, ONE_MILLION, ONE_MILLION,
			//		xmax, ymax, Opt->f_embedded, Opt->f_sideways, Opt->scale, Opt->line_width,
			//		verbose_level - 1);
			//G.setup(fname2, 0, 0, ONE_MILLION, ONE_MILLION, xmax, ymax);

			//G.frame(0.05);


			draw_projective(G, Descr->number,
					Descr->animate_nb_of_steps, Descr->animate_nb_of_steps, TRUE,
					Descr->animate_transition_nb_of_steps - i, Descr->animate_transition_nb_of_steps,
					FALSE, 0, FALSE, 0);
			G.finish(cout, TRUE);

		}
		if (Descr->f_trailer_page) {

			for (i = 0; i <= 7; i++, frame++) {
				mp_graphics G;

				char str[1000];
				string fname;

				snprintf(str, sizeof(str), "%s_%d_%d", Descr->fname.c_str(), Descr->number, frame);
				fname.assign(str);

				G.init(fname, Opt, verbose_level);
				//G.setup(fname, 0, 0, ONE_MILLION, ONE_MILLION,
				//		xmax, ymax, Opt->f_embedded, Opt->f_sideways, Opt->scale, Opt->line_width,
				//		verbose_level - 1);

				draw_projective(G, Descr->number, 0,
						Descr->animate_nb_of_steps, TRUE, i, Descr->animate_transition_nb_of_steps,
						FALSE, 0, TRUE, i);


				G.finish(cout, TRUE);


			}
		}


		cout << "frame=" << frame << endl;
	}



	Os.time_check(cout, t0);
	cout << endl;
}



void graphical_output::draw_projective(mp_graphics &G,
		int number, int animate_step, int animate_nb_of_steps,
	int f_transition, int transition_step, int transition_nb_steps,
	int f_title_page, int title_page_step,
	int f_trailer_page, int trailer_page_step)
{
	double *Dx, *Dy;
	int *Px, *Py;
	double x_stretch = 1.;
	double y_stretch = 1.;
	double dx = ONE_MILLION * 50 * x_stretch;
	double dy = ONE_MILLION * 50 * y_stretch; // stretch factor
	//double x_labels_offset = -.5;
	//double y_labels_offset = -.5;
	//double x_tick_half_width = 0.1;
	//double y_tick_half_width = 0.1;
	int N = 30;
	int i;
	double x_min = -10;
	double x_max = 10;
	double t_min = -1.13;
	double t_max = 5;
	double Delta_t;
	double step;
	double y_min = 0;
	double y_max = 2;
	double x, y, t;
	//int subdivide_v = 4;
	//int subdivide_h = 4;
	int f_plot_grid = TRUE;
	int f_plot_curve = TRUE;
	//int x_mod = 1;
	//int y_mod = 1;
	//int x_tick_mod = 1;
	//int y_tick_mod = 1;
	double height = 3.;
	double R, R2, X, Y;
	int mirror;
	int f_projection_on = TRUE;
	double radius = 10.;
	int N_curve = 500;
	numerics Num;


	cout << "draw_projective number=" << number << " animate_step=" << animate_step << " animate_nb_of_steps=" << animate_nb_of_steps << endl;

	if (number == 1 || number == 3) {
		x_min = -10;
		x_max = 10;
		y_min = -10;
		y_max = 10;
		x_stretch = .7;
		y_stretch = .7;
		dx = ONE_MILLION * 50 * x_stretch;
		dy = ONE_MILLION * 50 * y_stretch; // stretch factor
		t_min = -1.119437527;
		t_max = 4;
#if 0
		x_mod = 100;
		y_mod = 100;
		x_tick_mod = 1;
		y_tick_mod = 2;
		subdivide_v = 1;
		subdivide_h = 1;
#endif
		f_plot_curve = TRUE;
#if 0
		x_labels_offset = -.5;
		y_labels_offset = -.5;
		x_tick_half_width = 0.2;
		y_tick_half_width = 0.1;
#endif
		f_plot_grid = TRUE;
		f_plot_curve = TRUE;
		height = 6;
		R = 20;
		f_projection_on = TRUE;
		radius = 10.;
		}
	else if (number == 2 || number == 4) {
		x_min = -10;
		x_max = 10;
		y_min = -10;
		y_max = 10;
		x_stretch = 0.25;
		y_stretch = 0.25;
		dx = ONE_MILLION * 50 * x_stretch;
		dy = ONE_MILLION * 50 * y_stretch; // stretch factor
		t_min = -1.119437527;
		t_max = 4;
#if 0
		x_mod = 100;
		y_mod = 100;
		x_tick_mod = 1;
		y_tick_mod = 2;
		subdivide_v = 1;
		subdivide_h = 1;
#endif
		f_plot_curve = TRUE;
#if 0
		x_labels_offset = -.5;
		y_labels_offset = -.5;
		x_tick_half_width = 0.2;
		y_tick_half_width = 0.1;
#endif
		f_plot_grid = TRUE;
		f_plot_curve = TRUE;
		height = 6;
		R = 20;
		f_projection_on = FALSE;
		radius = 10.;
		}
	else if (number == 5) {
		x_min = -10;
		x_max = 10;
		y_min = -10;
		y_max = 10;
		x_stretch = 0.25;
		y_stretch = 0.25;
		dx = ONE_MILLION * 50 * x_stretch;
		dy = ONE_MILLION * 50 * y_stretch; // stretch factor
		t_min = 0;
		t_max = 4;
#if 0
		x_mod = 100;
		y_mod = 100;
		x_tick_mod = 1;
		y_tick_mod = 2;
		subdivide_v = 1;
		subdivide_h = 1;
#endif
		f_plot_curve = TRUE;
#if 0
		x_labels_offset = -.5;
		y_labels_offset = -.5;
		x_tick_half_width = 0.2;
		y_tick_half_width = 0.1;
#endif
		f_plot_grid = TRUE;
		f_plot_curve = TRUE;
		height = 6;
		R = 20;
		f_projection_on = TRUE;
		radius = 10.;
		}
	else if (number == 7 || number == 8) {
		x_min = -10;
		x_max = 10;
		y_min = -10;
		y_max = 10;
		x_stretch = 0.25;
		y_stretch = 0.25;
		dx = ONE_MILLION * 50 * x_stretch;
		dy = ONE_MILLION * 50 * y_stretch; // stretch factor
		t_min = 0;
		t_max = 10;
#if 0
		x_mod = 100;
		y_mod = 100;
		x_tick_mod = 1;
		y_tick_mod = 2;
		subdivide_v = 1;
		subdivide_h = 1;
#endif
		f_plot_curve = TRUE;
#if 0
		x_labels_offset = -.5;
		y_labels_offset = -.5;
		x_tick_half_width = 0.2;
		y_tick_half_width = 0.1;
#endif
		f_plot_grid = TRUE;
		f_plot_curve = TRUE;
		height = 6;
		R = 20;
		if (number == 8) {
			f_projection_on = FALSE;
			}
		else {
			f_projection_on = TRUE;
			}
		radius = 10.;
		N_curve = 2000;
		}

	Delta_t = t_max - t_min;

	G.sl_thickness(100);
	//G.sf_color(1);
	//G.sf_interior(10);
	Px = new int[N];
	Py = new int[N];
	Dx = new double[N];
	Dy = new double[N];


	cout << "draw_projective dx=" << dx << " dy=" << dy << endl;

	double box_x_min = x_min * 1.2;
	double box_x_max = x_max * 1.2;
	double box_y_min = y_min * 1.2;
	double box_y_max = y_max * 1.2;

	// draw a black frame:
	Dx[0] = box_x_min;
	Dy[0] = box_y_min;
	Dx[1] = box_x_max;
	Dy[1] = box_y_min;
	Dx[2] = box_x_max;
	Dy[2] = box_y_max;
	Dx[3] = box_x_min;
	Dy[3] = box_y_max;
	for (i = 0; i < 4; i++) {
		//project_to_disc(f_projection_on, radius, height, Dx[i], Dy[i], Dx[i], Dy[i]);
		Px[i] = Dx[i] * dx;
		Py[i] = Dy[i] * dy;
		}
	G.polygon5(Px, Py, 0, 1, 2, 3, 0);


	if (f_title_page) {

		X = 0;
		Y = 9;
		for (i = 0; i < 11; i++) {
			Dx[i] = X;
			Dy[i] = Y;
			Y = Y - 1.8;
			}

		for (i = 0; i < 11; i++) {
			Px[i] = Dx[i] * dx;
			Py[i] = Dy[i] * dy;
			}

		string s;


		s.assign("Transforming a Parabola");
		G.aligned_text_array(Px, Py, 0, "", s);

		s.assign("into a Hyperbola");
		G.aligned_text_array(Px, Py, 1, "", s);
		if (title_page_step == 0) {
			return;
			}
		s.assign("Step 1: Move into");
		G.aligned_text_array(Px, Py, 4, "", s);

		s.assign("the projective plane");
		G.aligned_text_array(Px, Py, 5, "", s);
		if (title_page_step == 1) {
			return;
			}

		s.assign("Step 2: Transform the equation");
		G.aligned_text_array(Px, Py, 6, "", s);
		if (title_page_step == 2) {
			return;
			}
		s.assign("Step 3: Move back");
		G.aligned_text_array(Px, Py, 7, "", s);

		s.assign("in the affine plane");
		G.aligned_text_array(Px, Py, 8, "", s);
		if (title_page_step == 3) {
			return;
			}

		s.assign("Created by Anton Betten 2017");
		G.aligned_text_array(Px, Py, 10, "", s);
		return;

		}


	if (f_trailer_page) {

		string s;

		X = 0;
		Y = 9.5;
		for (i = 0; i < 18; i++) {
			Dx[i] = X;
			Dy[i] = Y;
			Y = Y - 1.4;
			}

		for (i = 0; i < 18; i++) {
			Px[i] = Dx[i] * dx;
			Py[i] = Dy[i] * dy;
			}

		s.assign("Thanks for watching!");
		G.aligned_text_array(Px, Py, 0, "", s);
		if (trailer_page_step == 0) {
			return;
			}

		s.assign("credits:");
		G.aligned_text_array(Px, Py, 2, "", s);
		if (trailer_page_step == 1) {
			return;
			}

		s.assign("Felix Klein:");
		G.aligned_text_array(Px, Py, 4, "", s);
		if (trailer_page_step == 2) {
			return;
			}

		s.assign("Introduction to");
		G.aligned_text_array(Px, Py, 5, "", s);
		s.assign("non-euclidean geometry");
		G.aligned_text_array(Px, Py, 6, "", s);
		s.assign("(in German) 1928");
		G.aligned_text_array(Px, Py, 7, "", s);
		if (trailer_page_step == 3) {
			return;
			}
		s.assign("Latex: Donald Knuth");
		G.aligned_text_array(Px, Py, 9, "", s);
		s.assign("Leslie Lamport");
		G.aligned_text_array(Px, Py, 10, "", s);
		if (trailer_page_step == 4) {
			return;
			}
		s.assign("Tikz: Till Tantau");
		G.aligned_text_array(Px, Py, 11, "", s);
		if (trailer_page_step == 5) {
			return;
			}
		s.assign("ImageMagick Studio LLC");
		G.aligned_text_array(Px, Py, 12, "", s);
		if (trailer_page_step == 6) {
			return;
			}
		s.assign("Created by Anton Betten 2017");
		G.aligned_text_array(Px, Py, 14, "", s);
		return;

		}

#if 1
	if (f_plot_grid) {

		int *f_DNE;

		f_DNE = NEW_int(N);


		G.sl_thickness(10);

		for (x = 0; x < R; x++) {

			for (mirror = 0; mirror < 2; mirror++) {
				R2 = sqrt(R * R - x * x);

				// vertical line:
				t_min = -R2;
				t_max = R2;

				Delta_t = t_max - t_min;
				step = Delta_t / (double) N;

				for (i = 0; i < N; i++) {
					f_DNE[i] = FALSE;
					t = t_min + i * step;


					if (mirror == 0) {
						X = x;
						Y = t;
						}
					else {
						X = -x;
						Y = t;
						}


					if (f_DNE[i] == FALSE) {
						Dx[i] = X;
						Dy[i] = Y;
						Num.project_to_disc(f_projection_on, f_transition, transition_step, transition_nb_steps, radius, height, Dx[i], Dy[i], Dx[i], Dy[i]);
						if (Dx[i] < box_x_min || Dx[i] > box_x_max || Dy[i] < box_y_min || Dy[i] > box_y_max) {
							f_DNE[i] = TRUE;
							}
						}
					}
				G.plot_curve(N, f_DNE, Dx, Dy, dx, dy);
				}
			}
		for (y = 0; y < R; y++) {

			for (mirror = 0; mirror < 2; mirror++) {
				R2 = sqrt(R * R - y * y);

				// horizontal line:
				t_min = -R2;
				t_max = R2;

				Delta_t = t_max - t_min;
				step = Delta_t / (double) N;

				for (i = 0; i < N; i++) {
					f_DNE[i] = FALSE;
					t = t_min + i * step;


					if (mirror == 0) {
						X = t;
						Y = y;
						}
					else {
						X = t;
						Y = -y;
						}


					if (f_DNE[i] == FALSE) {
						Dx[i] = X;
						Dy[i] = Y;
						Num.project_to_disc(f_projection_on, f_transition, transition_step, transition_nb_steps, radius, height, Dx[i], Dy[i], Dx[i], Dy[i]);
						if (Dx[i] < box_x_min || Dx[i] > box_x_max || Dy[i] < box_y_min || Dy[i] > box_y_max) {
							f_DNE[i] = TRUE;
							}
						}
					}
				G.plot_curve(N, f_DNE, Dx, Dy, dx, dy);
				}
			}

		FREE_int(f_DNE);
		}
#endif

	if (f_plot_curve) {


		G.sl_color(2);

		double omega;

		omega = -1 * animate_step * M_PI / (2 * animate_nb_of_steps);
		cout << "animate_step=" << animate_step << " omega=" << omega << endl;
		double cos_omega, sin_omega;

		cos_omega = cos(omega);
		sin_omega = sin(omega);
		cout << "sin_omega=" << sin_omega << " cos_omega=" << cos_omega << endl;

		N = N_curve;

		delete [] Px;
		delete [] Py;
		delete [] Dx;
		delete [] Dy;

		int *f_DNE;

		f_DNE = NEW_int(N);
		Px = new int[N];
		Py = new int[N];
		Dx = new double[N];
		Dy = new double[N];


		G.sl_thickness(100);

		// draw the function as a parametric curve:

		double s_min, s_max, s, Delta_s;
		int h;

		for (h = 0; h < 2; h++) {
			if (number == 1 || number == 2) {
				s_min = 0;
				}
			else if (number == 5) {
				s_min = -30;
				}
			else if (number == 7 || number == 8) {
				s_min = -30;
				}
			else {
				s_min = -1.119437527;
				}
			s_max = 30;

			//t_min = -R;
			//t_max = R;

			//Delta_t = t_max - t_min;
			Delta_s = s_max - s_min;
			step = Delta_s / (double) N;

			cout << "Delta_s=" << Delta_s << " step=" << step << endl;
			cout << "draw_projective dx=" << dx << " dy=" << dy << endl;

			for (i = 0; i < N; i++) {


				f_DNE[i] = FALSE;

				s = exp(s_min + i * step);
					// this allows us to get many very small values and many very big values as well.

#if 0
				if (f_projection_on) {
					if (s > 10) {
						s = 10 + exp(s - 10);
						}
					}
#endif
				t = s;
				//t = exp(s);
				//t = t_min + i * step;

				//cout << "i=" << i << " s=" << s << " t=" << t << endl;


				if (number == 1 || number == 2) {
					X = t;
					Y = t * t;
					}
				else if (number == 3 || number == 4) {
					X = t;
					Y = t * t * t + 5 * t + 7;
					if (Y < 0) {
						f_DNE[i] = TRUE;
						}
					else {
						Y = sqrt(Y);
						}
					}
				else if (number == 5) {
					double denom, x, y;
					x = t;
					y = t * t;
					denom = x * sin_omega + cos_omega;

					if (ABS(denom) < 0.0000000001) {
						f_DNE[i] = TRUE;
						}
					else {
						if (h == 0) {
							X = (x * cos_omega - sin_omega) / denom;
							}
						else {
							X = (-x * cos_omega - sin_omega) / denom;
							}
						Y = y / denom;
						}
					}
				else if (number == 7 || number == 8) {
					X = t;
					if (t < 0) {
						f_DNE[i] = TRUE;
						}
					else {
						Y = log(t);
						if (ABS(Y) < 0.0001) {
							f_DNE[i] = TRUE;
							}
						}
					}

#if 0
				if (!f_DNE[i]) {
					double z;

					z = sqrt(X * X + Y * Y);
					if (z > 2 * R) {
						f_DNE[i] = TRUE;
						//cout << endl;
						//cout << "x=" << x << " y=" << y << " is out of bounds" << endl;
						}
					}
#endif

#if 0
				cout << "i=" << i << " s=" << s << " t=" << t << " f_DNE[i]=" << f_DNE[i];
				if (f_DNE[i] == FALSE) {
					cout << " X=" << X << " Y=" << Y << endl;
					}
				else {
					cout << endl;
					}
#endif

				if (f_DNE[i] == FALSE) {
					//double z;

					Dx[i] = X;
					Dy[i] = Y;
#if 0
					if (animate_step == 8) {
						cout << "X=" << X << " Y=" << Y << endl;
						}
#endif
					Num.project_to_disc(f_projection_on, f_transition, transition_step, transition_nb_steps, radius, height, Dx[i], Dy[i], Dx[i], Dy[i]);

					//z = sqrt(Dx[i] * Dx[i] + Dy[i] * Dy[i]);
					if (Dx[i] < box_x_min || Dx[i] > box_x_max || Dy[i] < box_y_min || Dy[i] > box_y_max) {
						f_DNE[i] = TRUE;
						//cout << endl;
						//cout << "x=" << x << " y=" << y << " is out of bounds" << endl;
						}
					if (!f_DNE[i] && isnan(Dx[i])) {
						f_DNE[i] = TRUE;
						}
					if (!f_DNE[i] && isnan(Dy[i])) {
						f_DNE[i] = TRUE;
						}
					if (!f_DNE[i] && ABS(Dx[i]) < 0.0001) {
						f_DNE[i] = TRUE;
						}
					if (!f_DNE[i] && ABS(Dy[i]) < 0.0001) {
						f_DNE[i] = TRUE;
						}
					}
				cout << i << " : s=" << s << " : " << " : t=" << t << " : ";
				if (f_DNE[i]) {
					cout << "-";
					}
				else {
					cout << Dx[i] << ", " << Dy[i];
					}
				cout << endl;
				}

			if (FALSE) {
				cout << "before plot_curve:" << endl;
				for (i = 0; i < N; i++) {
					cout << i << " : ";
					if (f_DNE[i]) {
						cout << "-";
						}
					else {
						cout << Dx[i] << ", " << Dy[i];
						}
					cout << endl;
					}
				}

			G.plot_curve(N, f_DNE, Dx, Dy, dx, dy);
			} // next h


		FREE_int(f_DNE);
		}


}


std::vector<int> get_color_grayscale(int bit_depth, int max_value, int loopCount, int f_invert_colors, int verbose_level)
{
	//int f_v = (verbose_level >= 1);
	int r, g, b;

	int d;

	d = 255 / max_value;

	if (loopCount > max_value) {
		loopCount = max_value;
	}

	if (f_invert_colors) {
		loopCount = max_value - loopCount;
	}
	r = g = b = loopCount * d;

	return { r, g, b};
}


std::vector<int> get_color(int bit_depth, int max_value, int loopCount, int f_invert_colors, int verbose_level)
{
	int f_v = (verbose_level>= 1);
	int r, g, b;
#if 0
		Black	#000000	(0,0,0)
		 	White	#FFFFFF	(255,255,255)
		 	Red	#FF0000	(255,0,0)
		 	Lime	#00FF00	(0,255,0)
		 	Blue	#0000FF	(0,0,255)
		 	Yellow	#FFFF00	(255,255,0)
		 	Cyan / Aqua	#00FFFF	(0,255,255)
		 	Magenta / Fuchsia	#FF00FF	(255,0,255)
		 	Silver	#C0C0C0	(192,192,192)
		 	Gray	#808080	(128,128,128)
		 	Maroon	#800000	(128,0,0)
		 	Olive	#808000	(128,128,0)
		 	Green	#008000	(0,128,0)
		 	Purple	#800080	(128,0,128)
		 	Teal	#008080	(0,128,128)
		 	Navy	#000080	(0,0,128)
#endif

#if 0
			maroon,#800000,128,0,0
			dark red,#8B0000,139,0,0
			brown,#A52A2A,165,42,42
			firebrick,#B22222,178,34,34
			crimson,#DC143C,220,20,60
			red,#FF0000,255,0,0
			tomato,#FF6347,255,99,71
			coral,#FF7F50,255,127,80
			indian red,#CD5C5C,205,92,92
			light coral,#F08080,240,128,128
			dark salmon,#E9967A,233,150,122
			salmon,#FA8072,250,128,114
			light salmon,#FFA07A,255,160,122
			orange red,#FF4500,255,69,0
			dark orange,#FF8C00,255,140,0
			orange,#FFA500,255,165,0
			gold,#FFD700,255,215,0
			dark golden rod,#B8860B,184,134,11
			golden rod,#DAA520,218,165,32
			pale golden rod,#EEE8AA,238,232,170
			dark khaki,#BDB76B,189,183,107
			khaki,#F0E68C,240,230,140
			olive,#808000,128,128,0
			yellow,#FFFF00,255,255,0
			yellow green,#9ACD32,154,205,50
			dark olive green,#556B2F,85,107,47
			olive drab,#6B8E23,107,142,35
			lawn green,#7CFC00,124,252,0
			chartreuse,#7FFF00,127,255,0
			green yellow,#ADFF2F,173,255,47
			dark green,#006400,0,100,0
			green,#008000,0,128,0
			forest green,#228B22,34,139,34
			lime,#00FF00,0,255,0
			lime green,#32CD32,50,205,50
			light green,#90EE90,144,238,144
			pale green,#98FB98,152,251,152
			dark sea green,#8FBC8F,143,188,143
			medium spring green,#00FA9A,0,250,154
			spring green,#00FF7F,0,255,127
			sea green,#2E8B57,46,139,87
			medium aqua marine,#66CDAA,102,205,170
			medium sea green,#3CB371,60,179,113
			light sea green,#20B2AA,32,178,170
			dark slate gray,#2F4F4F,47,79,79
			teal,#008080,0,128,128
			dark cyan,#008B8B,0,139,139
			aqua,#00FFFF,0,255,255
			cyan,#00FFFF,0,255,255
			light cyan,#E0FFFF,224,255,255
			dark turquoise,#00CED1,0,206,209
			turquoise,#40E0D0,64,224,208
			medium turquoise,#48D1CC,72,209,204
			pale turquoise,#AFEEEE,175,238,238
			aqua marine,#7FFFD4,127,255,212
			powder blue,#B0E0E6,176,224,230
			cadet blue,#5F9EA0,95,158,160
			steel blue,#4682B4,70,130,180
			corn flower blue,#6495ED,100,149,237
			deep sky blue,#00BFFF,0,191,255
			dodger blue,#1E90FF,30,144,255
			light blue,#ADD8E6,173,216,230
			sky blue,#87CEEB,135,206,235
			light sky blue,#87CEFA,135,206,250
			midnight blue,#191970,25,25,112
			navy,#000080,0,0,128
			dark blue,#00008B,0,0,139
			medium blue,#0000CD,0,0,205
			blue,#0000FF,0,0,255
			royal blue,#4169E1,65,105,225
			blue violet,#8A2BE2,138,43,226
			indigo,#4B0082,75,0,130
			dark slate blue,#483D8B,72,61,139
			slate blue,#6A5ACD,106,90,205
			medium slate blue,#7B68EE,123,104,238
			medium purple,#9370DB,147,112,219
			dark magenta,#8B008B,139,0,139
			dark violet,#9400D3,148,0,211
			dark orchid,#9932CC,153,50,204
			medium orchid,#BA55D3,186,85,211
			purple,#800080,128,0,128
			thistle,#D8BFD8,216,191,216
			plum,#DDA0DD,221,160,221
			violet,#EE82EE,238,130,238
			magenta / fuchsia,#FF00FF,255,0,255
			orchid,#DA70D6,218,112,214
			medium violet red,#C71585,199,21,133
			pale violet red,#DB7093,219,112,147
			deep pink,#FF1493,255,20,147
			hot pink,#FF69B4,255,105,180
			light pink,#FFB6C1,255,182,193
			pink,#FFC0CB,255,192,203
			antique white,#FAEBD7,250,235,215
			beige,#F5F5DC,245,245,220
			bisque,#FFE4C4,255,228,196
			blanched almond,#FFEBCD,255,235,205
			wheat,#F5DEB3,245,222,179
			corn silk,#FFF8DC,255,248,220
			lemon chiffon,#FFFACD,255,250,205
			light golden rod yellow,#FAFAD2,250,250,210
			light yellow,#FFFFE0,255,255,224
			saddle brown,#8B4513,139,69,19
			sienna,#A0522D,160,82,45
			chocolate,#D2691E,210,105,30
			peru,#CD853F,205,133,63
			sandy brown,#F4A460,244,164,96
			burly wood,#DEB887,222,184,135
			tan,#D2B48C,210,180,140
			rosy brown,#BC8F8F,188,143,143
			moccasin,#FFE4B5,255,228,181
			navajo white,#FFDEAD,255,222,173
			peach puff,#FFDAB9,255,218,185
			misty rose,#FFE4E1,255,228,225
			lavender blush,#FFF0F5,255,240,245
			linen,#FAF0E6,250,240,230
			old lace,#FDF5E6,253,245,230
			papaya whip,#FFEFD5,255,239,213
			sea shell,#FFF5EE,255,245,238
			mint cream,#F5FFFA,245,255,250
			slate gray,#708090,112,128,144
			light slate gray,#778899,119,136,153
			light steel blue,#B0C4DE,176,196,222
			lavender,#E6E6FA,230,230,250
			floral white,#FFFAF0,255,250,240
			alice blue,#F0F8FF,240,248,255
			ghost white,#F8F8FF,248,248,255
			honeydew,#F0FFF0,240,255,240
			ivory,#FFFFF0,255,255,240
			azure,#F0FFFF,240,255,255
			snow,#FFFAFA,255,250,250
			black,#000000,0,0,0
			dim gray / dim grey,#696969,105,105,105
			gray / grey,#808080,128,128,128
			dark gray / dark grey,#A9A9A9,169,169,169
			silver,#C0C0C0,192,192,192
			light gray / light grey,#D3D3D3,211,211,211
			gainsboro,#DCDCDC,220,220,220
			white smoke,#F5F5F5,245,245,245
			white,#FFFFFF,255,255,255
#endif

	int table[] = {

			//https://www.rapidtables.com/web/color/RGB_Color.html

			// 16 predefined colors:
			255,255,255, // white
			0,0,0, // black
			255,0,0,
			0,255,0,
			0,0,255,
			255,255,0,
			0,255,255,
			255,0,255,
			192,192,192,
			128,128,128,
			128,0,0,
			128,128,0,
			0,128,0,
			128,0,128,
			0,128,128,
			0,0,128,

			// 137 more:
			128,0,0,
			139,0,0,
			165,42,42,
			178,34,34,
			220,20,60,
			255,0,0,
			255,99,71,
			255,127,80,
			205,92,92,
			240,128,128,
			233,150,122,
			250,128,114,
			255,160,122,
			255,69,0,
			255,140,0,
			255,165,0,
			255,215,0,
			184,134,11,
			218,165,32,
			238,232,170,
			189,183,107,
			240,230,140,
			128,128,0,
			255,255,0,
			154,205,50,
			85,107,47,
			107,142,35,
			124,252,0,
			127,255,0,
			173,255,47,
			0,100,0,
			0,128,0,
			34,139,34,
			0,255,0,
			50,205,50,
			144,238,144,
			152,251,152,
			143,188,143,
			0,250,154,
			0,255,127,
			46,139,87,
			102,205,170,
			60,179,113,
			32,178,170,
			47,79,79,
			0,128,128,
			0,139,139,
			0,255,255,
			0,255,255,
			224,255,255,
			0,206,209,
			64,224,208,
			72,209,204,
			175,238,238,
			127,255,212,
			176,224,230,
			95,158,160,
			70,130,180,
			100,149,237,
			0,191,255,
			30,144,255,
			173,216,230,
			135,206,235,
			135,206,250,
			25,25,112,
			0,0,128,
			0,0,139,
			0,0,205,
			0,0,255,
			65,105,225,
			138,43,226,
			75,0,130,
			72,61,139,
			106,90,205,
			123,104,238,
			147,112,219,
			139,0,139,
			148,0,211,
			153,50,204,
			186,85,211,
			128,0,128,
			216,191,216,
			221,160,221,
			238,130,238,
			255,0,255,
			218,112,214,
			199,21,133,
			219,112,147,
			255,20,147,
			255,105,180,
			255,182,193,
			255,192,203,
			250,235,215,
			245,245,220,
			255,228,196,
			255,235,205,
			245,222,179,
			255,248,220,
			255,250,205,
			250,250,210,
			255,255,224,
			139,69,19,
			160,82,45,
			210,105,30,
			205,133,63,
			244,164,96,
			222,184,135,
			210,180,140,
			188,143,143,
			255,228,181,
			255,222,173,
			255,218,185,
			255,228,225,
			255,240,245,
			250,240,230,
			253,245,230,
			255,239,213,
			255,245,238,
			245,255,250,
			112,128,144,
			119,136,153,
			176,196,222,
			230,230,250,
			255,250,240,
			240,248,255,
			248,248,255,
			240,255,240,
			255,255,240,
			//240,255,255,
			255,250,250,
			//0,0,0,
			105,105,105,
			//128,128,128,
			169,169,169,
			//192,192,192,
			211,211,211,
			220,220,220,
			245,245,245,

			//255,255,255,

			// 16 + 127 = 143
	};


	if (loopCount < 16 /*&& bit_depth == 8*/) {
		r = table[loopCount * 3 + 0];
		g = table[loopCount * 3 + 1];
		b = table[loopCount * 3 + 2];
	}
	else if (loopCount < 143 /*&& bit_depth == 8*/) {

		loopCount -= 16;

		number_theory::number_theory_domain NT;
		int idx;

		idx = NT.power_mod(57, loopCount, 127);

		idx += 16;

		r = table[idx * 3 + 0];
		g = table[idx * 3 + 1];
		b = table[idx * 3 + 2];
	}
	else {
		double a1, a2, x, y, z;
		int max_color;

		max_color = (1 << bit_depth) - 1;

		if (loopCount > max_value) {
			cout << "loopCount > max_value" << endl;
			cout << "loopCount=" << loopCount << endl;
			cout << "max_value=" << max_value << endl;
			exit(1);
		}

		if (loopCount < 153) {
			r = table[loopCount * 3 + 0];
			g = table[loopCount * 3 + 1];
			b = table[loopCount * 3 + 2];
			return { r, g, b};
		}
		loopCount -= 153;

		a1 = (double) loopCount / (double) max_value;


		if (f_invert_colors) {
			a2 = 1. - a1;
		}
		else {
			a2 = a1;
		}
		x = a2;
		y = a2 * a2;
		z = y * a2;
		r = x * max_color;
		g = y * max_color;
		b = z * max_color;
		if (f_v) {
			cout << loopCount << " : " << max_value << " : "
					<< a1 << " : " << a2 << " : " << x << "," << y << "," << z << " : " << r << "," << g << "," << b << endl;
		}
	}

	//cout << "color " << loopCount << " : " << r << "," << g << "," << b << endl;
	return { r, g, b};
}

void fillBitmap(BMP &image, int i, int j, std::vector<int> color)
{
	// The pixel is set using its image
	// location and stacks 3 variables (RGB) into the vector word.
	image(i, j)->Red = color[0];
	image(i, j)->Green = color[1];
	image(i, j)->Blue = color[2];
};

void graphical_output::tree_draw(tree_draw_options *Tree_draw_options, int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "graphical_output::tree_draw" << endl;
	}

	if (!Tree_draw_options->f_file) {
		cout << "graphical_output::tree_draw please use -file <fname>" << endl;
		exit(1);
	}
	tree T;
	orbiter_kernel_system::file_io Fio;
	std::string fname2;

	cout << "Trying to read file " << Tree_draw_options->file_name << " of size "
			<< Fio.file_size(Tree_draw_options->file_name) << endl;

	if (Fio.file_size(Tree_draw_options->file_name) <= 0) {
		cout << "treedraw.out the input file " << Tree_draw_options->file_name
				<< " does not exist" << endl;
		exit(1);
	}

	if (f_v) {
		cout << "graphical_output::tree_draw reading input file " << Tree_draw_options->file_name << endl;
	}
	T.init(Tree_draw_options,
			orbiter_kernel_system::Orbiter->draw_options->xin,
			orbiter_kernel_system::Orbiter->draw_options->yin,
			verbose_level);
	if (f_v) {
		cout << "graphical_output::tree_draw reading input file " << Tree_draw_options->file_name << " finished" << endl;
	}

#if 0
	if (/* T.nb_nodes > 200 ||*/ f_no_circletext) {
		f_circletext = FALSE;
		}
	if (f_on_circle) {
		T.root->place_on_circle(xmax, ymax, T.max_depth);
		}

	if (f_count_leaves) {
		T.f_count_leaves = TRUE;
		}
#endif

	data_structures::string_tools ST;

	fname2.assign(Tree_draw_options->file_name);
	ST.chop_off_extension(fname2);
	fname2.append("_draw");

	if (f_v) {
		cout << "graphical_output::tree_draw before T.draw" << endl;
	}
	T.draw(fname2,
			Tree_draw_options,
			orbiter_kernel_system::Orbiter->draw_options,
			verbose_level);
	if (f_v) {
		cout << "graphical_output::tree_draw after T.draw" << endl;
	}

#if 0
	if (f_graph) {
		cout << "treedraw.out drawing as graph" << endl;
		T.draw(fname_out,
			xmax, ymax, xmax_out, ymax_out, rad, f_circle, f_circletext,
			f_i, f_e, TRUE, draw_vertex_callback,
			f_embedded, f_sideways, f_on_circle,
			scale, line_width, verbose_level - 1);
		}
	else if (f_placeholder_labels) {
		T.draw(fname_out,
			xmax, ymax, xmax_out, ymax_out, rad, f_circle, f_circletext,
			f_i, f_e, TRUE, draw_vertex_callback_placeholders,
			f_embedded, f_sideways, f_on_circle,
			scale, line_width, verbose_level - 1);
		}
	else {
		T.draw(fname_out,
			xmax, ymax, xmax_out, ymax_out, rad, f_circle, f_circletext,
			f_i, f_e, TRUE, draw_vertex_callback_standard,
			f_embedded, f_sideways, f_on_circle,
			scale, line_width, verbose_level - 1);
		}
#endif
	if (f_v) {
		cout << "graphical_output::tree_draw done" << endl;
	}

}





void graphical_output::animate_povray(
		povray_job_description *Povray_job_description,
		int verbose_level)
{
	int f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "graphical_output::animate_povray" << endl;
	}
	animate *A;

	A = NEW_OBJECT(animate);

	A->init(Povray_job_description,
			NULL /* extra_data */,
			verbose_level);


	A->draw_frame_callback = interface_povray_draw_frame;







	//char fname_makefile[1000];


	//sprintf(fname_makefile, "makefile_animation");

	{
		ofstream fpm(A->fname_makefile);

		A->fpm = &fpm;

		fpm << "all:" << endl;

		if (Povray_job_description->f_rounds) {

			int *rounds;
			int nb_rounds;

			Int_vec_scan(Povray_job_description->rounds_as_string, rounds, nb_rounds);

			cout << "Doing the following " << nb_rounds << " rounds: ";
			Int_vec_print(cout, rounds, nb_rounds);
			cout << endl;

			int r, this_round;

			for (r = 0; r < nb_rounds; r++) {


				this_round = rounds[r];

				cout << "round " << r << " / " << nb_rounds
						<< " is " << this_round << endl;

				//round = first_round + r;

				A->animate_one_round(
						this_round,
						verbose_level);

			}
		}
		else {
			cout << "round " << Povray_job_description->round << endl;


			A->animate_one_round(
					Povray_job_description->round,
					verbose_level);

		}

		fpm << endl;
	}
	orbiter_kernel_system::file_io Fio;

	cout << "Written file " << A->fname_makefile << " of size "
			<< Fio.file_size(A->fname_makefile) << endl;



	FREE_OBJECT(A);
	A = NULL;

}


static void interface_povray_draw_frame(
	animate *Anim, int h, int nb_frames, int round,
	double clipping_radius,
	ostream &fp,
	int verbose_level)
{
	int i, j;



	Anim->Pov->union_start(fp);


	if (round == 0) {


		for (i = 0; i < (int) Anim->S->Drawables.size(); i++) {
			drawable_set_of_objects D;
			int f_group_is_animated = FALSE;

			if (FALSE) {
				cout << "drawable " << i << ":" << endl;
			}
			D = Anim->S->Drawables[i];

			for (j = 0; j < Anim->S->animated_groups.size(); j++) {
				if (Anim->S->animated_groups[j] == i) {
					break;
				}
			}
			if (j < Anim->S->animated_groups.size()) {
				f_group_is_animated = TRUE;
			}
			if (FALSE) {
				if (f_group_is_animated) {
					cout << "is animated" << endl;
				}
				else {
					cout << "is not animated" << endl;
				}
			}
			D.draw(Anim, fp, f_group_is_animated, h, verbose_level);
		}


	}

	//Anim->S->clipping_by_cylinder(0, 1.7 /* r */, fp);

	Anim->rotation(h, nb_frames, round, fp);
	Anim->union_end(
			h, nb_frames, round,
			clipping_radius,
			fp);

}




}}}


