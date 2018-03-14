#include "stdafx.h"
#include "Header.h"

using namespace std;

void mpi_recv_clusters(vector<Cluster>& clusters, int& k, float& q, int row_length)
{
	MPI_Status status;
	MPI_Recv(&k, 1, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status);
	MPI_Recv(&q, 1, MPI_FLOAT, status.MPI_SOURCE, 3, MPI_COMM_WORLD, &status);
	clusters.resize(k);

	for (int c = 0; c < clusters.size(); c++)
	{
		MPI_Recv(&clusters.at(c).diameter, 1, MPI_FLOAT, status.MPI_SOURCE, 4, MPI_COMM_WORLD, &status);
		MPI_Recv(&clusters.at(c).number_of_points, 1, MPI_INT, status.MPI_SOURCE, 5, MPI_COMM_WORLD, &status);
		MPI_Recv(&clusters.at(c).center.cluster_index, 1, MPI_INT, status.MPI_SOURCE, 6, MPI_COMM_WORLD, &status);


		clusters.at(c).center.dist_from_center = 0;
		clusters.at(c).center.pointNum = 0;

		clusters.at(c).center.data.resize(row_length);

		MPI_Recv(&clusters.at(c).center.data[0], row_length, MPI_FLOAT, status.MPI_SOURCE, 7, MPI_COMM_WORLD, &status);
	}


}

void mpi_send_clusters(vector<Cluster>& clusters, int k, float q, int target)
{
	MPI_Send(&k, 1, MPI_INT, target, 2, MPI_COMM_WORLD);
	MPI_Send(&q, 1, MPI_FLOAT, target, 3, MPI_COMM_WORLD);

	for (int c = 0; c < clusters.size(); c++)
	{
		MPI_Send(&clusters.at(c).diameter, 1, MPI_FLOAT, target, 4, MPI_COMM_WORLD);
		MPI_Send(&clusters.at(c).number_of_points, 1, MPI_INT, target, 5, MPI_COMM_WORLD);
		MPI_Send(&clusters.at(c).center.cluster_index, 1, MPI_INT, target, 6, MPI_COMM_WORLD);
		MPI_Send(&clusters.at(c).center.data[0], clusters.at(c).center.data.size(), MPI_FLOAT, target, 7, MPI_COMM_WORLD);
	}


}

void mpi_send_recv_input_to_all(vector<Point>& points, int& max, int& limit, float& qm, int& n, int myid, int& row_length)
{

	MPI_Bcast(&max, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&limit, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&qm, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&n, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&row_length, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

	if (myid != 0)
	{
		points.resize(n);
	}

	for (int p = 0; p < points.size(); p++)
	{
		MPI_Bcast(&points.at(p).pointNum, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&points.at(p).cluster_index, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&points.at(p).dist_from_center, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

		if (myid != 0)
		{
			points.at(p).data.resize(row_length);
		}

		MPI_Bcast(&points.at(p).data[0], points.at(p).data.size(), MPI_FLOAT, 0, MPI_COMM_WORLD);
		/*if (debug == true)
		{
		cout << "Process " << myid << ": \r\n";
		debug_print_point(points.at(p));
		}*/
	}
}

vector<Cluster> omp_mpi_kmeans(vector<Point>& points, int max, int limit, float qm, float& final_quality, int myid, int num_of_computers)
{

	vector<Cluster> returnClusters;
	final_quality = 9999;//max
	int k = 2;

	if (myid == 0)
	{
		while (k < max)
		{
			int loopSize = 0;
			for (int comp = 1; comp < num_of_computers && k < max; comp++, k++, loopSize++)
			{
				MPI_Send(&k, 1, MPI_INT, comp, 1, MPI_COMM_WORLD);
			}
			bool need_to_break_from_while = false;

			for (int comp = 0; comp < loopSize; comp++)
			{
				vector<Cluster> clusters;
				int recvK = 0;
				float recvQ = 0;
				mpi_recv_clusters(clusters, recvK, recvQ, points.at(0).data.size());
				cout << "MAIN :" << " recvK=" << recvK << ". recvQ=" << recvQ << "\r\n";

				if (recvQ < qm) {
					final_quality = recvQ;
					returnClusters = clusters;
					need_to_break_from_while = true;
					break;
				}
				else if (recvQ < final_quality)
				{
					returnClusters = clusters;
					final_quality = recvQ;
				}
			}
			if (need_to_break_from_while == true)
				break;

		}

		//tell all computers to exit
		k = -1;
		for (int comp = 0; comp < num_of_computers; comp++)
		{
			MPI_Request request;
			MPI_Isend(&k, 1, MPI_INT, comp, 1, MPI_COMM_WORLD, &request);
		}
	}
	else
	{
		while (true)
		{
			MPI_Status status;
			MPI_Recv(&k, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
			if (k < 2)
				break;
			vector<Cluster> clusters;
			kmeans_reset_points(points);
			kmeans_init_clusters(clusters, points, k);
			for (int i = 0; i < limit; i++)
			{
				int num_of_points_changed = openMP_kmeans_select_cluster_for_points(clusters, points);
				if (num_of_points_changed == 0)
					break;
				openMP_kmeans_recalc_clusters_centers(clusters, points);
			}

			float q = openMP_kmeans_calc_quality(clusters, points);
			//if (debug == true)
			//	cout << "process=" << myid << " k=" << k << ". q=" << q << "\r\n";

			mpi_send_clusters(clusters, k, q, 0);
		}

	}

	return returnClusters;
}
