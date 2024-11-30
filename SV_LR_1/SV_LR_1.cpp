// IDM-23-04
// Afanasyev Vadim
// LR№1

#include <iostream>
#include <iomanip>
#include "mpi.h"

using namespace std;

#define TAG_SEND 0
#define TAG_RECEIVE 1

#define N1 3
#define M1 4
#define N2 4
#define M2 3

int Matrix1[N1][M1];
int Matrix2[N2][M2];
int Result[N1][M2];

static void FillMatrices()
{
	for (int i = 0; i < N1; ++i)
	{
		for (int j = 0; j < M1; ++j)
		{
			Matrix1[i][j] = rand() % 10;
		}
	}

	for (int i = 0; i < N2; ++i)
	{
		for (int j = 0; j < M2; ++j)
		{
			Matrix2[i][j] = rand() % 10;
		}
	}
}

static void PrintMatrices()
{
	cout << "Matrix 1:" << endl;
	for (int i = 0; i < N1; ++i)
	{
		for (int j = 0; j < M1; ++j)
		{
			cout << setw(4) << Matrix1[i][j] << "|";
		}
		cout << endl;
	}

	cout << "Matrix 2:" << endl;
	for (int i = 0; i < N2; ++i)
	{
		for (int j = 0; j < M2; ++j)
		{
			cout << setw(4) << Matrix2[i][j] << "|";
		}
		cout << endl;
	}

	cout << "Multiplication result:" << endl;
	for (int i = 0; i < N1; ++i)
	{
		for (int j = 0; j < M2; ++j)
		{
			cout << setw(4) << Result[i][j] << "|";
		}
		cout << endl;
	}
}

static void SingleProcessMult()
{
	cout << "Processing:" << endl;
	for (int i = 0; i < N1; ++i)
	{
		for (int j = 0; j < M2; ++j)
		{
			cout << "| ";
			int CurrentCol2 = 0;
			for (int j1 = 0; j1 < N1; ++j1)
			{
				cout << Matrix1[i][j1] << "*" << Matrix2[CurrentCol2][j];
				if (j1 != M1 - 1)
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
	/*for (int i = 0; i < InRowCount1; ++i)
	{
		for (int j = 0; j < InColCount2; ++j)
		{
			int Buffer = 0;
			int CurrentCol2 = 0;
			
			for (int j1 = 0; j1 < InColCount1; ++j1)
			{
				Buffer += InMatrix1[i][j1] * InMatrix2[CurrentCol2][j];
				++CurrentCol2;
			}
			
			Result[i][j] = Buffer;
		}
	}*/

	// Столбцы второй матрицы
	for (int k = 0; k < M2; ++k)
	{
		// Строки первой матрицы
		for (int i = 0; i < N1; ++i)
		{
			// Столбцы первой матрицы
			for (int j = 0; j < M1; ++j)
			{
				Result[i][k] += Matrix1[i][j] * Matrix2[j][k];
			}
		}
	}
}

int main()
{
	int Rank = 0;
	int ProcessCount = 0;
	int WorkerProcessCount = 0;

	MPI_Status Status;

	double StartTime = 0.0;
	double EndTime = 0.0;
	double TotalTime = 0.0;

	int WorkerRowCount = 0;
	int WorkerMatrixSubset = 0;

	// Запускаем MPI
	MPI_Init(NULL, NULL);

	// Читаем инфу о процессах
	MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcessCount);

	WorkerProcessCount = ProcessCount - 1;

	// Главный процесс
	if (Rank == 0)
	{
		srand(static_cast<unsigned int>(time(NULL)));

		// Начало отсчета времени
		StartTime = MPI_Wtime();

		// FillMatrices();

		// Matrix 1
		Matrix1[0][0] = 1;
		Matrix1[0][1] = 3;
		Matrix1[0][2] = 5;
		Matrix1[0][3] = 4;
		Matrix1[1][0] = 4;
		Matrix1[1][1] = 2;
		Matrix1[1][2] = 1;
		Matrix1[1][3] = 7;
		Matrix1[2][0] = 9;
		Matrix1[2][1] = 8;
		Matrix1[2][2] = 6;
		Matrix1[2][3] = 1;

		// Matrix 2
		Matrix2[0][0] = 2;
		Matrix2[0][1] = 3;
		Matrix2[0][2] = 5;
		Matrix2[1][0] = 6;
		Matrix2[1][1] = 9;
		Matrix2[1][2] = 7;
		Matrix2[2][0] = 2;
		Matrix2[2][1] = 3;
		Matrix2[2][2] = 7;
		Matrix2[3][0] = 7;
		Matrix2[3][1] = 9;
		Matrix2[3][2] = 1;

		// Expected result:
		// | 58 | 81 | 65 |
		// | 71 | 96 | 48 |
		// | 85 | 126| 144|

		// При запуске на одном процессе
		if (ProcessCount == 1)
		{
			StartTime = MPI_Wtime();
			
			SingleProcessMult();
			
			EndTime = MPI_Wtime();
			TotalTime = EndTime - TotalTime;
			
			cout << "SINGLE PROCESS MODE" << endl;
			PrintMatrices();
			cout << "Total time: " << TotalTime << endl;
			
			return 0;
		}

		// Фрагмент первой матрицы для каждого рабочего процесса
		WorkerMatrixSubset = 0;

		// Количество строк для каждого рабочего процесса
		WorkerRowCount = N1 / WorkerProcessCount;

		// Раздача тасков
		for (int i = 1; i <= WorkerProcessCount; ++i)
		{
			if (i == WorkerProcessCount)
			{
				WorkerRowCount += N1 % WorkerProcessCount;
				cout << "DEBUG worker row count to send: " << WorkerRowCount << endl;
			}

			// Отправить фрагмент матрицы
			MPI_Send(&WorkerMatrixSubset, 1, MPI_INT, i, TAG_SEND, MPI_COMM_WORLD);

			// Отправить количество строк
			MPI_Send(&WorkerRowCount, 1, MPI_INT, i, TAG_SEND, MPI_COMM_WORLD);

			// Отправить фрагмент первой матрицы
			MPI_Send(&Matrix1[WorkerMatrixSubset][0], WorkerRowCount * M1, MPI_INT, i, TAG_SEND, MPI_COMM_WORLD);

			// Отправить вторую матрицу
			MPI_Send(&Matrix2, N2 * M2, MPI_INT, i, TAG_SEND, MPI_COMM_WORLD);

			WorkerMatrixSubset += WorkerRowCount;
		}

		// Разбор полетов
		for (int i = 1; i <= WorkerProcessCount; ++i)
		{
			MPI_Recv(&WorkerMatrixSubset, 1, MPI_INT, i, TAG_RECEIVE, MPI_COMM_WORLD, &Status);

			MPI_Recv(&WorkerRowCount, 1, MPI_INT, i, TAG_RECEIVE, MPI_COMM_WORLD, &Status);

			MPI_Recv(&Result[WorkerMatrixSubset][0], WorkerRowCount * M2, MPI_INT, i, TAG_RECEIVE, MPI_COMM_WORLD, &Status);
		}

		EndTime = MPI_Wtime();
		TotalTime = EndTime - StartTime;

		cout << "MPI MODE USING " << WorkerProcessCount << " WORKER PROCESSES" << endl;
		PrintMatrices();
		cout << "Total time: " << TotalTime << endl;
	}

	// Рабочие процессы
	if (Rank > 0)
	{
		// Получить фрагмент матрицы
		MPI_Recv(&WorkerMatrixSubset, 1, MPI_INT, 0, TAG_SEND, MPI_COMM_WORLD, &Status);
		cout << "Received subset: " << WorkerMatrixSubset << endl;

		// Получить строки
		MPI_Recv(&WorkerRowCount, 1, MPI_INT, 0, TAG_SEND, MPI_COMM_WORLD, &Status);
		cout << "Received row count: " << WorkerRowCount << endl;

		// Получить исходную матрицу
		MPI_Recv(&Matrix1, WorkerRowCount * M1, MPI_INT, 0, TAG_SEND, MPI_COMM_WORLD, &Status);

		// Получить вторую матрицу
		MPI_Recv(&Matrix2, N2 * M2, MPI_INT, 0, TAG_SEND, MPI_COMM_WORLD, &Status);

		// Расчеты
		for (int k = 0; k < M2; ++k)
		{
			for (int i = 0; i < WorkerRowCount; ++i)
			{
				for (int j = 0; j < M1; ++j)
				{
					Result[i][k] += Matrix1[i][j] * Matrix2[j][k];
				}
			}
		}

		// Отправить оффсет
		MPI_Send(&WorkerMatrixSubset, 1, MPI_INT, 0, TAG_RECEIVE, MPI_COMM_WORLD);

		// Отправить строки
		MPI_Send(&WorkerRowCount, 1, MPI_INT, 0, TAG_RECEIVE, MPI_COMM_WORLD);

		// Отправить результат
		MPI_Send(&Result, WorkerRowCount * M2, MPI_INT, 0, TAG_RECEIVE, MPI_COMM_WORLD);
	}

	// По домам
	MPI_Finalize();

	return 0;
}
