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
	for (int i = 0; i < InRowCount; ++i)
	{
		for (int j = 0; j < InColCount; ++j)
		{
			printf("Elem[%d][%d] = %d\n", i, j, InMatrix[i][j]);
		}
	}
}

void DeleteMatrix(int**& InMatrix, int InRowCount)
{
	for (int i = 0; i < InRowCount; ++i)
	{
		delete[] InMatrix[i];
	}

	delete[] InMatrix;
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

		int RowCount = 3;
		int ColCount = 4;

		int** Matrix = InitMatrix(RowCount, ColCount);
		FillMatrix(Matrix, RowCount, ColCount);
		PrintMatrix(Matrix, RowCount, ColCount);
		DeleteMatrix(Matrix, RowCount);

		MPI_Finalize();
	}

	_CrtDumpMemoryLeaks();

	return 0;
}
