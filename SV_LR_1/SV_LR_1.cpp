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
			cout << "| " << InMatrix[i][j] << " ";
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

int** Solve(int**& InMatrix1, int**& InMatrix2, int InRowCount1, int InColCount1, int InRowCount2, int InColCount2)
{
	int** Result = new int* [InRowCount1];
	for (int i = 0; i < InRowCount1; ++i)
	{
		Result[i] = new int[InColCount2];
	}

	Result[0][0] = 1;
	Result[0][1] = 2;
	Result[1][0] = 3;
	Result[1][1] = 4;

	cout << "Processing:" << endl << endl;
	for (int i = 0; i < InRowCount1; ++i)
	{
		for (int j = 0; j < InColCount2; ++j)
		{
			cout << "| ";
			int CurrentCol2 = 0;
			for (int j1 = 0; j1 < InColCount1; ++j1)
			{
				cout << InMatrix1[i][j1] << "*" << InMatrix2[CurrentCol2][j];
				if (j1 != InColCount1 - 1)
				{
					cout << " + ";
				}
				else
				{
					cout << " ";
				}

				++CurrentCol2;
			}
		}
		cout << "|" << endl;
	}
	cout << endl;

	cout << "Calculating cells:" << endl << endl;
	for (int i = 0; i < InRowCount1; ++i)
	{
		for (int j = 0; j < InColCount2; ++j)
		{
			cout << "| ";

			int Buffer = 0;
			int CurrentCol2 = 0;
			
			for (int j1 = 0; j1 < InColCount1; ++j1)
			{
				Buffer += InMatrix1[i][j1] * InMatrix2[CurrentCol2][j];
				++CurrentCol2;
			}
			
			Result[i][j] = Buffer;
			
			cout << Result[i][j] << " ";
		}
		cout << "|" << endl;
	}
	cout << endl;

	return Result;
}

int main()
{
	{
		MPI_Init(NULL, NULL);

		int Rank = 0;
		int Size = 0;

		MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
		MPI_Comm_size(MPI_COMM_WORLD, &Size);

		printf("Rank: %d Size: %d\n\n", Rank, Size);

		int RowCount1 = 2;
		int ColCount1 = 3;

		int** Matrix1 = InitMatrix(RowCount1, ColCount1);
		// FillMatrix(Matrix1, RowCount, ColCount);

		Matrix1[0][0] = 1;
		Matrix1[0][1] = 2;
		Matrix1[0][2] = 2;
		Matrix1[1][0] = 3;
		Matrix1[1][1] = 1;
		Matrix1[1][2] = 1;

		cout << "Matrix 1:" << endl;
		PrintMatrix(Matrix1, RowCount1, ColCount1);

		int RowCount2 = 3;
		int ColCount2 = 2;

		int** Matrix2 = InitMatrix(RowCount2, ColCount2);
		// FillMatrix(Matrix2, RowCount, ColCount);
		
		Matrix2[0][0] = 4;
		Matrix2[0][1] = 2;
		Matrix2[1][0] = 3;
		Matrix2[1][1] = 1;
		Matrix2[2][0] = 1;
		Matrix2[2][1] = 5;
		
		cout << "Matrix2:" << endl;
		PrintMatrix(Matrix2, RowCount2, ColCount2);

		// Expected result:
		// | 12 | 14 |
		// | 16 | 12 |

		int** Result = Solve(Matrix1, Matrix2, RowCount1, ColCount1, RowCount2, ColCount2);

		cout << "Result:" << endl;
		PrintMatrix(Result, RowCount1, ColCount2);

		DeleteMatrix(Matrix1, RowCount1);
		DeleteMatrix(Matrix2, RowCount2);

		MPI_Finalize();
	}

	_CrtDumpMemoryLeaks();

	return 0;
}
