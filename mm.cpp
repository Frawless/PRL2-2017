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
#include <sstream>
#include <ctype.h>

using namespace std;

MPI_Status stat;

#define BUFF_SIZE 1
#define TAG 0
#define TAG_A 1
#define TAG_B 2

typedef struct{
	int processCount;
	int procID;
	int rows;
	int cols;
	int C;
	int matN;
} T_PROCESOR;

typedef struct{
	int **matrix;
} T_MATRIX;

bool isNumber(string s)
{
	char * p ;
	strtol(s.c_str(), &p, 10) ;

	return (*p == 0) ;
}

int loadInput(T_PROCESOR *mat, T_MATRIX *matrix, ifstream *file){
	string line;
	int rows = 0;
	int cols = 0;
	int tmpR = 0;
	int tmpC = 0;
	string number;
	string input;
	
	// Načítání souboru
	while(getline(*file,line))
	{
		cols = 0;
		input.append(line);
		input.append("\n");
		stringstream ss(line);
		while(ss >> number){
			cols++;
		}
		rows++;
	}
	
	// Alokace matice
	matrix->matrix = (int **)malloc(rows*sizeof(int*));
	for(int i = 0; i < rows; i++)
	{
		matrix->matrix[i] = (int*)malloc(rows*sizeof(int));
	}

	stringstream ss(input);
	// Plnění alokované matice
	while(getline(ss,line,'\n')){
//		cerr<<line<<endl;
		stringstream iss(line);
		while(iss >> number){
			if(!isNumber(number))
			{
				cerr<<"Špatný formát vstupní matice! Matice obsahuje nepodporované znaky!"<<endl;
				exit(EXIT_FAILURE);
			}
			matrix->matrix[tmpR][tmpC] = atoi(number.c_str());
//			cerr<<"TestX: "<<matrix[tmpR][tmpC]<<endl;
			tmpC++;
		}
		tmpC = 0;
		tmpR++;
	}
	
	if(rows != mat->rows)
		return rows;
	else
		return cols;
}

void printMatrix(T_MATRIX matrix, int rows, int cols)
{
	cerr<<"Vypisuji matici:"<<endl;
	for(int i = 0; i < rows; i++)
	{
		for(int j = 0; j < cols; j++)
			cerr<<matrix.matrix[i][j]<<" ";
		cerr<<endl;
	}
}

void disposeMatrix(T_MATRIX *matrix, int rows, int cols)
{
	for(int i = 0; i < rows; i++)
	{
		free((int*)matrix->matrix[i]);
	}
	free(matrix->matrix);
}


int main(int argc, char *argv[])
{
	T_PROCESOR* matrix = new T_PROCESOR;
	
	MPI_Init(&argc,&argv); /* inicializace MPI */
	MPI_Comm_size(MPI_COMM_WORLD,&matrix->processCount); /* zjíštění počtu procesorů*/
	MPI_Comm_rank(MPI_COMM_WORLD,&matrix->procID); /* zjištění ID procesoru */
	
	ifstream mat1("mat1");
	ifstream mat2("mat2");
	
	
	int prvekB = 0;
	int prvekA = 0;
	
	matrix->matN = atoi(argv[1]);

    string fLine;
    getline(mat1, fLine);
	matrix->rows = atoi(fLine.c_str());
	getline(mat2, fLine);
	matrix->cols = atoi(fLine.c_str());
		
	if(matrix->procID == 0){
		int tmpR;
		int tmpC;
		int valA;
		int valB;
		int outputMatrix[matrix->cols*matrix->cols];
		// Vytvoření vstupních matic
		T_MATRIX inputA, inputB;
		// Načtení vstupních matic
		tmpR = loadInput(matrix,&inputA,&mat1);
		tmpC = loadInput(matrix,&inputB,&mat2);	
		
		cerr<<"tmP: "<<tmpR<<":"<<tmpC<<endl;
		cerr<<"matN: "<<matrix->cols<<endl;
		
//		if(tmpR != tmpC){
//			cerr<<"Matice mají neplatnou velikost!"<<endl;
//			exit(EXIT_FAILURE);
//		}
		
//		cerr<<"Počet procesorů: "<<matrix->processCount<<endl;
		
		for(int send = 1; send < matrix->processCount; send++)
		{
			// Posílání prvnímu řádku mrížky
			if(send <= matrix->cols){
				for(int i = 0; i < matrix->matN; i++)
				{
					valB = inputB.matrix[i][(send-1)];
					prvekB++;
					MPI_Send(&valB, BUFF_SIZE, MPI_INT, send, TAG_B, MPI_COMM_WORLD);
				}
			}
			// Posílání prvnímu sloupci mrížky
			if(send % matrix->cols == 1 || (matrix->cols == 1) || (send == 1)){
				for(int i = 0; i < matrix->matN; i++)
				{
					
//					cerr<<"sA: "<<valA<<endl;
//					cerr<<"valA: "<<inputA.matrix[(send/matrix->cols)][i]<<endl;
					if(matrix->cols == 1)
						valA = inputA.matrix[(send-1)][i];
					else
						valA = inputA.matrix[(send/matrix->cols)][i];
					prvekA++;
					MPI_Send(&valA, BUFF_SIZE, MPI_INT, send, TAG_A, MPI_COMM_WORLD);

				}
			}			
		}
		
		cerr<<"odesláno A:"<<prvekA<<" a B:"<<prvekB<<endl;
		
		for(int recv = 1; recv < matrix->processCount;recv++)
		{
			MPI_Recv(&tmpC, BUFF_SIZE, MPI_INT, recv, TAG, MPI_COMM_WORLD, &stat);
			outputMatrix[recv-1] = tmpC; 
			cerr<<"recv-1: "<<recv-1<<endl;
		}
		cerr<<"Matice došla"<<endl;
		
		// Výpis matice
		cout<<matrix->rows<<":"<<matrix->cols<<endl;
		for(tmpR = 0; tmpR < matrix->processCount-1; tmpR++)
		{
			cout<<outputMatrix[tmpR];
			if((tmpR+1)%matrix->cols == 0)
				cout<<endl;
			else
				cout<<" ";
		}

		
//		cout<<"Počet procesorů: "<<matrix->processCount<<endl;
//		cout<<"Matice: "<<matrix->rows<<"x"<<matrix->cols<<endl;
		
		
		
//		printMatrix(inputA,matrix->rows,tmpC);
		
//		disposeMatrix(&inputA,matrix->rows,tmpC);
//		disposeMatrix(&inputB,matrix->rows,tmpC);
	}
	else{
		int valA;
		int valB;
//		cout<<"Není to první procesor"<<endl;
		// Posílání prvnímu řádku mrížky
		if(matrix->procID <= matrix->cols){
			for(int i = 0; i < matrix->matN; i++)
			{
				MPI_Recv(&valB, BUFF_SIZE, MPI_INT, 0, TAG_B, MPI_COMM_WORLD, &stat);
				MPI_Recv(&valA, BUFF_SIZE, MPI_INT, matrix->procID-1, TAG_A, MPI_COMM_WORLD, &stat);
				
				matrix->C += valA * valB;
				if((matrix->procID+1) % matrix->cols != 1 && matrix->cols != 1){
					MPI_Send(&valA, BUFF_SIZE, MPI_INT, matrix->procID+1, TAG_A, MPI_COMM_WORLD);
				}
				if((matrix->procID + matrix->cols) < matrix->processCount){
					MPI_Send(&valB, BUFF_SIZE, MPI_INT, matrix->procID+matrix->cols, TAG_B, MPI_COMM_WORLD);
				}
					
			}
			cerr<<"Posílám C: "<<matrix->procID<<endl;
			MPI_Send(&matrix->C, BUFF_SIZE, MPI_INT, 0, TAG, MPI_COMM_WORLD);
		}
		// Posílání prvnímu sloupci mrížky
		else if((matrix->procID % matrix->cols == 1 && matrix->procID != 1) || matrix->cols == 1){
			for(int i = 0; i < matrix->matN; i++)
			{
				MPI_Recv(&valB, BUFF_SIZE, MPI_INT, matrix->procID-matrix->cols, TAG_B, MPI_COMM_WORLD, &stat);
				MPI_Recv(&valA, BUFF_SIZE, MPI_INT, 0, TAG_A, MPI_COMM_WORLD, &stat);
				
				matrix->C += valA * valB;

				if(matrix->cols != 1)
					MPI_Send(&valA, BUFF_SIZE, MPI_INT, matrix->procID+1, TAG_A, MPI_COMM_WORLD);
				if(matrix->procID + matrix->cols < matrix->processCount){
					MPI_Send(&valB, BUFF_SIZE, MPI_INT, matrix->procID+matrix->cols, TAG_B, MPI_COMM_WORLD);

				}
			}
			MPI_Send(&matrix->C, BUFF_SIZE, MPI_INT, 0, TAG, MPI_COMM_WORLD);
		}
		else
		{
			for(int i = 0; i < matrix->matN; i++)
			{
				MPI_Recv(&valB, BUFF_SIZE, MPI_INT, matrix->procID-matrix->cols, TAG_B, MPI_COMM_WORLD, &stat);
				MPI_Recv(&valA, BUFF_SIZE, MPI_INT, matrix->procID-1, TAG_A, MPI_COMM_WORLD, &stat);
				
				matrix->C += valA * valB;
				if((matrix->procID+1) % matrix->cols != 1 && matrix->cols != 1){
					MPI_Send(&valA, BUFF_SIZE, MPI_INT, matrix->procID+1, TAG_A, MPI_COMM_WORLD);
				}
				if(matrix->procID + matrix->cols < matrix->processCount){
					MPI_Send(&valB, BUFF_SIZE, MPI_INT, matrix->procID+matrix->cols, TAG_B, MPI_COMM_WORLD);
				}
			}
			MPI_Send(&matrix->C, BUFF_SIZE, MPI_INT, 0, TAG, MPI_COMM_WORLD);			
		}
	}
   
	
	
	// Spuštění procesorů
	free(matrix);
	MPI_Finalize(); 
	return EXIT_SUCCESS;
 
 }
