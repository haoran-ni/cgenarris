#CC=gcc
CFLAGS=-std=gnu99 -fopenmp -O3  -fPIC  
PYTHON_H=/home/ritwit/anaconda3/include/python3.7m/

swig:
	swig -python pygenarris.i

pygenarris:pygenarris.o read_input.o spg_generation.o lattice_generator.o algebra.o\
		   molecule_utils.o combinatorics.o check_structure.o crystal_utils.o\
		   molecule_placement.o randomgen.o swig pywrap spglib.o
	${CC} ${CFLAGS} -shared  *.o -o _pygenarris.so -lm 
	
pywrap: swig
	${CC} ${CFLAGS} -I${PYTHON_H} -c pygenarris.c pygenarris_wrap.c  -lpython3

cgenarris:  main.o read_input.o spg_generation.o lattice_generator.o algebra.o\
		   molecule_utils.o combinatorics.o check_structure.o crystal_utils.o\
		   molecule_placement.o randomgen.o spglib.o
	$(CC) ${CFLAGS} *.o -o cgenarris.x -lm 

pygenarris.o: pygenarris.c
	${CC} ${CFLAGS} -c pygenarris.c

main.o: main.c
	${CC} ${CFLAGS} -c main.c

read_input.o: read_input.c read_input.h
	${CC} ${CFLAGS} -c read_input.c

spg_generation.o: spg_generation.c read_input.o lattice_generator.o\
spg_generation.h spglib.h algebra.o
	${CC} ${CFLAGS} -c spg_generation.c -lm 

lattice_generator.o: lattice_generator.c lattice_generator.h randomgen.o
	${CC} ${CFLAGS} -c lattice_generator.c -lm

algebra.o: algebra.c algebra.h
	${CC} ${CFLAGS} -c algebra.c

molecule_utils.o: molecule_utils.c molecule_utils.h read_input.o algebra.o
	${CC} ${CFLAGS} -c molecule_utils.c

combinatorics.o: combinatorics.h combinatorics.c algebra.o spglib.h
	${CC} ${CFLAGS} -c combinatorics.c -lm

check_structure.o: check_structure.c check_structure.h algebra.o spg_generation.o
	${CC} ${CFLAGS} -c check_structure.c -lm

crystal_utils.o: crystal_utils.c crystal_utils.h spg_generation.o
	${CC} ${CFLAGS} -c crystal_utils.c -lm

molecule_placement.o: molecule_placement.c molecule_placement.h spg_generation.o read_input.o
	 ${CC} ${CFLAGS} -c molecule_placement.c -lm

randomgen.o: randomgen.h
	${CC} ${CFLAGS} -c randomgen.c -lm

spglib.o:
	${CC} ${CFLAGS} -c spglib_src/*.c -lm

clean: 
	rm -rf *.o *.x *pygenarris.so __pycache__ pygenarris.py *wrap.c *.pyc _pygenarris.* build spglib_src/*.o


