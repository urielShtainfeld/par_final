#pragma once
#include <vector>
#include <math.h>
#include <omp.h>
#include <iostream>
#include "mpi.h"

using namespace std;

extern bool debug;


//structs
struct Point
{
	int pointNum;
	std::vector<float> data;
	int cluster_index;
	float dist_from_center;

} typedef point_t;

struct Cluster
{
	point_t center;
	int number_of_points;
	float diameter;
} typedef cluster_t;

//seq_kmeans
float euclid_dist(Point center, Point point);
void kmeans_init_clusters(vector<Cluster>& clusters, vector<Point>& points, int k);
void kmeans_reset_points(vector<Point>& points);
vector<Cluster> kmeans(vector<Point>& points, int max, int limit, float qm, float& final_quality);

//openMP
vector<Cluster> openMP_kmeans(vector<Point>& points, int max, int limit, float qm, float& final_quality);
float openMP_kmeans_calc_quality(vector<Cluster>& clusters, vector<Point>& points);
void openMP_kmeans_recalc_clusters_centers(vector<Cluster>& clusters, vector<Point>& points);
int  openMP_kmeans_select_cluster_for_points(vector<Cluster>& clusters, vector<Point>& points);

//openMP and MPI
vector<Cluster> omp_mpi_kmeans(vector<Point>& points, int max, int limit, float qm, float& final_quality, int myid, int num_of_computers);
void mpi_send_recv_input_to_all(vector<Point>& points, int& max, int& limit, float& qm, int& n, int myid, int& row_length);
//io
void read_from_file(char* fileName, vector<Point>& points, int& n, int& row_length, int& max, int& limit, float& qm);
void write_to_file(char* file_name, vector<Cluster>& clusters, float final_quality, double runtime);

//util
void debug_print_point(Point& point);
void debug_print_cluster(Cluster& cluster);
