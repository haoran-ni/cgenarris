// test example for the rigid-body optimizer

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../crystal.h"
#include "../cocrystal.h"
#include "rigid_press.h"

void run_example(char *xdir, int natoms_per_mol,int  Z, int cell_type);

void read_vector(int size, char *path, float *vec)
{
    FILE *file_ptr = fopen(path, "r");

    for(int i=0 ; i<size ; i++)
    { fscanf(file_ptr, "%f", vec+i); }

    fclose(file_ptr);
}

void read_crystal(char *path, crystal *xtl)
{
    char line[1000], dummy[1000];
    FILE *file_ptr = fopen(path, "r");

    int i=0, j=0;
    while(!feof(file_ptr))
    {
        char *out = fgets(line, 1000, file_ptr);
        if(out == NULL)
        { break; }

        switch(line[0])
        {
            case 'l':
            sscanf(line, "%s %f %f %f", dummy, xtl->lattice_vectors[j], xtl->lattice_vectors[j]+1, xtl->lattice_vectors[j]+2);
            j++;
            break;

            case 'a':
            sscanf(line, "%s %f %f %f %s", dummy, xtl->Xcord+i, xtl->Ycord+i, xtl->Zcord+i, xtl->atoms+2*i);
            i++;
            break;

            case '#':
            break;

            default:
            printf("ERROR: parsing line (%s)\n", line);
            exit(1);
        }
    }
    fclose(file_ptr);
}

void print_crystal(crystal* xtal)
{
     int N = xtal->num_atoms_in_molecule;
     int m = xtal->Z;

     printf("#Z = %d \n", xtal->Z);
     printf("#napm = %d \n", xtal->num_atoms_in_molecule);

    for(int i = 0; i < 3; i++)
    {
        printf("lattice_vector %12f %12f %12f \n",
            xtal->lattice_vectors[i][0], xtal->lattice_vectors[i][1],
            xtal->lattice_vectors[i][2]);
    }

    for(int i = 0; i < N*m; i++)
    {
        printf("atom %12f %12f %12f  %c \n", xtal->Xcord[i],
            xtal->Ycord[i],  xtal->Zcord[i],  xtal->atoms[2*i]);
    }
}


void run_example(char *xdir, int natoms_per_mol, int  Z, int cell_type)
{
    crystal xtl;
    float *cutmat;

    printf("Running example: %s\n", xdir);

    xtl.num_atoms_in_molecule = natoms_per_mol;
    xtl.Z = Z;
    int num_atoms = xtl.Z*xtl.num_atoms_in_molecule;
    xtl.Xcord = (float*)malloc(sizeof(float)*num_atoms);
    xtl.Ycord = (float*)malloc(sizeof(float)*num_atoms);
    xtl.Zcord = (float*)malloc(sizeof(float)*num_atoms);
    xtl.atoms = (char*)malloc(sizeof(char)*(2*num_atoms+1));
    cutmat = (float*)malloc(sizeof(float)*num_atoms*num_atoms);
    for(int i=0 ; i<=2*num_atoms ; i++)
    { xtl.atoms[i] = ' '; }

    char geo_path[1000];
    strcpy(geo_path, xdir);
    strcat(geo_path, "/geometry.in");
    read_crystal(geo_path, &xtl);
    char cutmat_path[1000];
    strcpy(cutmat_path, xdir);
    strcat(cutmat_path, "/cutoff_matrix.txt");
    read_vector(num_atoms*num_atoms, cutmat_path, cutmat);
    //print_crystal(&xtl);

    Opt_settings set;
    set.cell_family = cell_type;
    set.max_iteration = 400;
    Opt_status status = optimize_crystal(&xtl, cutmat, set);

    if(status == SUCCESS)
    { print_crystal(&xtl); }
    else if(status  == ITER_LIMIT)
    { printf("Optimization failed: Max iterations reached\n"); }
    else
    { printf("Optimization failed\n"); }

    free(xtl.Xcord);
    free(xtl.Ycord);
    free(xtl.Zcord);
    free(xtl.atoms);
    free(cutmat);

}

int main(void)
{
  run_example("sample_structures/Example1", 12, 2, TRICLINIC);
  //run_example("sample_structures/fast_opt", 30, 2, MONOCLINIC);
  //run_example("sample_structures/failed_1", 30, 2, MONOCLINIC);
  //run_example("sample_structures/Example2", 12, 4, TRICLINIC);
  //run_example("sample_structures/Example5", 30, 4, MONOCLINIC);
  // run_example("sample_structures/Example4", 30, 2, TRICLINIC);
    return 0;
}
