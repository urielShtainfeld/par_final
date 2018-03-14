#include "stdafx.h"
#include "Header.h"
#include <ctime>
using namespace std;

char* input_file_path = "D:\\ParallelProcessing\\Projects\\final\\Sales_Transactions_Dataset_Weekly.txt";
//char* output_file_path = "D:\\ParallelProcessing\\Projects\\final\\output.txt";
//char* output_file_path = "D:\\ParallelProcessing\\Projects\\final\\outputOMP.txt";
char* output_file_path = "D:\\ParallelProcessing\\Projects\\final\\outputOMP_MPI.txt";

bool debug = true;
void main(int argc, char *argv[])
{

	MPI_Init(&argc, &argv);

	int n = 0, row_length = 0, max = 0, limit = 0;
	float qm = 0, final_quality = 0;
	vector<Point> points;

	int myid = 0, num_of_computers = 0;
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &num_of_computers);

	if (myid == 0)
	{
		read_from_file(input_file_path, points, n, row_length, max, limit, qm);
	}

	mpi_send_recv_input_to_all(points, max, limit, qm, n, myid, row_length);
	MPI_Barrier(MPI_COMM_WORLD);
	/*if (debug == true)
	{
	cout << "Process " << myid << ": max=" << max << ": limit=" << limit << ": qm=" << qm << "\r\n";
	}*/
	vector<Cluster> clusters;
	clock_t start_time = clock();
	//clusters = kmeans(points, max, limit, qm, final_quality);
	//clusters = openMP_kmeans(points, max, limit, qm, final_quality);
	clusters = omp_mpi_kmeans(points, max, limit, qm, final_quality, myid, num_of_computers);
	clock_t end_time = clock();

	if (myid == 0)
	{
		write_to_file(output_file_path, clusters, final_quality, double(end_time - start_time) / CLOCKS_PER_SEC);
	}

	MPI_Finalize();
	return;
}


