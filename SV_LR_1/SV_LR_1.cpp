#include <iostream>
#include "mpi.h"

using namespace std;

int**& InitMatrix(int InRowCount, int InColCount)
{
	int** Matrix = new int* [InRowCount];
	for (int i = 0; i < InRowCount; ++i)
	{
		Matrix[i] = new int[InColCount];
	}

	return Matrix;
}

void FillMatrix(int**& InMatrix, int InRowCount, int InColCount)
{
	for (int i = 0; i < InRowCount; ++i)
	{
		for (int j = 0; j < InColCount; ++j)
		{
			InMatrix[i][j] = rand() % 10;
		}
	}
}

void PrintMatrix(int** InMatrix, int InRowCount, int InColCount)
{
	cout << endl;
	for (int i = 0; i < InRowCount; ++i)
	{
		for (int j = 0; j < InColCount; ++j)
		{
			// printf("Elem[%d][%d] = %d", i, j, InMatrix[i][j]);
			cout << "|" << InMatrix[i][j];
		}
		cout << "|" << endl;
	}
	cout << endl;
}

void DeleteMatrix(int**& InMatrix, int InRowCount)
{
	for (int i = 0; i < InRowCount; ++i)
	{
		delete[] InMatrix[i];
	}

	delete[] InMatrix;
}

void Solve(int**& InMatrix1, int**& InMatrix2, int InRowCount, int ColCount, int InRank)
{
	
}

int main()
{
	{
		MPI_Init(NULL, NULL);

		int Rank = 0;
		int Size = 0;

		MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
		MPI_Comm_size(MPI_COMM_WORLD, &Size);

		printf("Rank: %d Size: %d\n", Rank, Size);

		int RowCount = 2;
		int ColCount = 2;

		int** Matrix1 = InitMatrix(RowCount, ColCount);
		FillMatrix(Matrix1, RowCount, ColCount);
		PrintMatrix(Matrix1, RowCount, ColCount);

		int** Matrix2 = InitMatrix(RowCount, ColCount);
		FillMatrix(Matrix2, RowCount, ColCount);
		PrintMatrix(Matrix2, RowCount, ColCount);

		Solve(Matrix1, Matrix2, RowCount, ColCount, Rank);

		DeleteMatrix(Matrix1, RowCount);
		DeleteMatrix(Matrix2, RowCount);

		MPI_Finalize();
	}

	_CrtDumpMemoryLeaks();

	return 0;
}
