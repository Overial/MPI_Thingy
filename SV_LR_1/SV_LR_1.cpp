// IDM-23-04
// Afanasyev Vadim
// LR�1

#include <iostream>
#include <iomanip>
#include <chrono>
#include "mpi.h"

#include <thread>

using namespace std;
using namespace std::chrono;

#define TAG_SEND 0
#define TAG_RECEIVE 1

#define N1 1000
#define M1 1000
#define N2 1000
#define M2 1000

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

void SingleProcessMult()
{
	// ������� ������ �������
	for (int k = 0; k < M2; ++k)
	{
		// ������ ������ �������
		for (int i = 0; i < N1; ++i)
		{
			// ������� ������ �������
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

	int WorkerRowCount = 0;
	int WorkerMatrixSubset = 0;

	// ��������� MPI
	MPI_Init(NULL, NULL);

	// ������ ���� � ���������
	MPI_Comm_rank(MPI_COMM_WORLD, &Rank);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcessCount);

	WorkerProcessCount = ProcessCount - 1;

	// ������� �������
	if (Rank == 0)
	{
		srand(static_cast<unsigned int>(time(NULL)));

		steady_clock::time_point StartTime = high_resolution_clock::now();

		FillMatrices();

		// ��� ������� �� ����� ��������
		if (ProcessCount == 1)
		{
			steady_clock::time_point SingleStartTime = high_resolution_clock::now();

			SingleProcessMult();

			steady_clock::time_point SingleEndTime = high_resolution_clock::now();

			long long SingleTotalTime = duration_cast<microseconds>(SingleEndTime - SingleStartTime).count();

			cout << "SINGLE PROCESS MODE" << endl;
			// PrintMatrices();
			cout << "Total time: " << SingleTotalTime << endl;

			MPI_Finalize();

			return 0;
		}

		// �������� ������ ������� ��� ������� �������� ��������
		WorkerMatrixSubset = 0;

		// ���������� ����� ��� ������� �������� ��������
		WorkerRowCount = N1 / WorkerProcessCount;

		// ������� ������
		for (int i = 1; i <= WorkerProcessCount; ++i)
		{
			// ��������� ��� ������ 99% ���� ������
			if (i == WorkerProcessCount)
			{
				WorkerRowCount += N1 % WorkerProcessCount;
			}

			// ��������� �������� �������
			MPI_Send(&WorkerMatrixSubset, 1, MPI_INT, i, TAG_SEND, MPI_COMM_WORLD);

			// ��������� ���������� �����
			MPI_Send(&WorkerRowCount, 1, MPI_INT, i, TAG_SEND, MPI_COMM_WORLD);

			// ��������� �������� ������ �������
			MPI_Send(&Matrix1[WorkerMatrixSubset][0], WorkerRowCount * M1, MPI_INT, i, TAG_SEND, MPI_COMM_WORLD);

			// ��������� ������ �������
			MPI_Send(&Matrix2, N2 * M2, MPI_INT, i, TAG_SEND, MPI_COMM_WORLD);

			WorkerMatrixSubset += WorkerRowCount;
		}

		// ������ �������
		for (int i = 1; i <= WorkerProcessCount; ++i)
		{
			MPI_Recv(&WorkerMatrixSubset, 1, MPI_INT, i, TAG_RECEIVE, MPI_COMM_WORLD, &Status);

			MPI_Recv(&WorkerRowCount, 1, MPI_INT, i, TAG_RECEIVE, MPI_COMM_WORLD, &Status);

			MPI_Recv(&Result[WorkerMatrixSubset][0], WorkerRowCount * M2, MPI_INT, i, TAG_RECEIVE, MPI_COMM_WORLD, &Status);
		}

		steady_clock::time_point EndTime = high_resolution_clock::now();

		long long TotalTime = duration_cast<microseconds>(EndTime - StartTime).count();

		cout << "MPI MODE USING " << WorkerProcessCount << " WORKER PROCESSES" << endl;
		// PrintMatrices();
		cout << "Total time: " << TotalTime << endl;
	}

	// ������� ��������
	if (Rank > 0)
	{
		// �������� �������� �������
		MPI_Recv(&WorkerMatrixSubset, 1, MPI_INT, 0, TAG_SEND, MPI_COMM_WORLD, &Status);
		cout << "Received worker matrix subset: " << WorkerMatrixSubset << endl;

		// �������� ������
		MPI_Recv(&WorkerRowCount, 1, MPI_INT, 0, TAG_SEND, MPI_COMM_WORLD, &Status);
		cout << "Received worker row count: " << WorkerRowCount << endl;

		// �������� �������� �������
		MPI_Recv(&Matrix1, WorkerRowCount * M1, MPI_INT, 0, TAG_SEND, MPI_COMM_WORLD, &Status);

		// �������� ������ �������
		MPI_Recv(&Matrix2, N2 * M2, MPI_INT, 0, TAG_SEND, MPI_COMM_WORLD, &Status);

		// �������
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

		// ��������� ������
		MPI_Send(&WorkerMatrixSubset, 1, MPI_INT, 0, TAG_RECEIVE, MPI_COMM_WORLD);

		// ��������� ������
		MPI_Send(&WorkerRowCount, 1, MPI_INT, 0, TAG_RECEIVE, MPI_COMM_WORLD);

		// ��������� ���������
		MPI_Send(&Result, WorkerRowCount * M2, MPI_INT, 0, TAG_RECEIVE, MPI_COMM_WORLD);
	}

	// �� �����
	MPI_Finalize();

	return 0;
}
