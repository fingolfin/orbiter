DISCRETA_CPP = g++
DISCRETA_CC = gcc
CPPFLAGS = -Wall -O3 -funroll-loops -I.. -std=c++14
# -frename-registers
LIB = 
LFLAGS = -lm

MODULES = \
	algorithms.o \
	bitmatrix.o \
	bitvector.o \
	classify_bitvectors.o \
	classify_using_canonical_forms.o \
	data_file.o \
	data_input_stream_description_element.o \
	data_input_stream_description.o \
	data_input_stream.o \
	data_structures_global.o \
	fancy_set.o \
	int_matrix.o \
	int_vec.o \
	int_vector.o \
	lint_vec.o \
	nauty_output.o \
	page_storage.o \
	partitionstack.o \
	set_builder_description.o \
	set_builder.o \
	set_of_sets_lint.o \
	set_of_sets.o \
	sorting.o \
	spreadsheet.o \
	string_tools.o \
	tally_lint.o \
	tally_vector_data.o \
	tally.o \
	vector_builder_description.o \
	vector_builder.o \
	vector_hashing.o 


all : $(MODULES)

%.o: %.cpp %.c; $(DISCRETA_CPP) $(CPPFLAGS) -c  -o $@ $<

clean:
	- rm *.o *.a *~
