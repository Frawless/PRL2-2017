/************************************************
*		Projekt: Projekt do předmětu PRL    	*
*		Mesh-Multiplication						*
*		Autor: Bc. Jakub Stejskal <xstejs24>	*
*		Nazev souboru: mm.cpp					*
*		Datum: 16. 4. 2017						*		
*		Verze: 1.0								*
*************************************************/

#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string> 

using namespace std;

MPI_Status stat;

typedef struct{
	int processCount;
	int procID;
	int rows;
	int cols;
	int *matrix;
} T_MATRIX;

void loadMatrix(T_MATRIX *matrix);

int main(int argc, char *argv[])
{
	T_MATRIX* matrix = new T_MATRIX;
	
	MPI_Init(&argc,&argv); /* inicializace MPI */
	MPI_Comm_size(MPI_COMM_WORLD,&matrix->processCount); /* zjíštění počtu procesorů*/
	MPI_Comm_rank(MPI_COMM_WORLD,&matrix->procID); /* zjištění ID procesoru */
	
	ifstream mat1("mat1");
	ifstream mat2("mat2");

    string fLine;
    getline(mat1, fLine);
	matrix->rows = atoi(fLine.c_str());
	getline(mat2, fLine);
	matrix->cols = atoi(fLine.c_str());
	
	if(matrix->procID == 0){
		cout<<"Počet procesorů: "<<matrix->processCount<<endl;
		cout<<"Matice: "<<matrix->rows<<"x"<<matrix->cols<<endl;
	}
	else{
		cout<<"Není to první procesor"<<endl;
	}
   
	
	
	// Spuštění procesorů
	free(matrix);
	MPI_Finalize(); 
	return EXIT_SUCCESS;
 
 }
