#include "stdafx.h"
#include "Header.h"

using namespace std;


float openMP_kmeans_calc_quality(vector<Cluster>& clusters, vector<Point>& points)
{
	vector<vector<float>> dist_between_centers;
	dist_between_centers.assign(clusters.size(), vector<float>(clusters.size()));
	int c = 0, c2 = 0;
	float dist = 0;


	for (c = 0; c < dist_between_centers.size(); c++)
	{
#pragma omp parallel for \
	private(c2,dist) firstprivate(c) shared(clusters,points) schedule(static) ordered
		for (c2 = c + 1; c2 < dist_between_centers.size(); c2++)
		{
			dist = euclid_dist(clusters.at(c).center, clusters.at(c2).center);
			dist_between_centers.at(c).at(c2) = dist;
			dist_between_centers.at(c2).at(c) = dist;

		}
	}

	float q = 0;
	int divider = 0;
	int i = 0, j = 0;

#pragma omp parallel for \
	private(c2,dist) firstprivate(i,j) shared(clusters,points,dist_between_centers) schedule(static) ordered reduction(+:q),reduction(+:divider)
	for (i = 0; i < dist_between_centers.size(); i++)
	{
		for (j = 0; j < dist_between_centers.at(i).size(); j++)
		{
			if (i == j)
				continue;
			q += (clusters.at(i).diameter / dist_between_centers.at(i).at(j));
			divider++;
		}
	}

	//free
	dist_between_centers = vector<vector<float>>();

	return q / divider;
}

void openMP_kmeans_recalc_clusters_centers(vector<Cluster>& clusters, vector<Point>& points)
{
	vector<Point> newCenters(clusters.size());
	int i = 0, p = 0;
	for (int i = 0; i < points.size(); i++)
	{
		//first time init with values
		if (points.at(0).data.size() > newCenters.at(points.at(i).cluster_index).data.size())
		{
			for (p = 0; p < points.at(i).data.size(); p++)
			{
				newCenters.at(points.at(i).cluster_index).data.push_back(points.at(i).data.at(p));
			}
		}
		else
		{
#pragma omp parallel for \
	private(p) firstprivate(i) shared(clusters,points,newCenters)
			for (p = 0; p < points.at(i).data.size(); p++)
			{
				newCenters.at(points.at(i).cluster_index).data.at(p) += points.at(i).data.at(p);
			}
		}
	}

	for (i = 0; i < newCenters.size(); i++)
	{
#pragma omp parallel for \
	private(p) shared(clusters,points,newCenters)  schedule(static)
		for (p = 0; p < newCenters.at(i).data.size(); p++)
		{
			newCenters.at(i).data.at(p) = newCenters.at(i).data.at(p) / clusters.at(i).number_of_points;
		}
		clusters.at(i).center = newCenters.at(i);
	}
}

int  openMP_kmeans_select_cluster_for_points(vector<Cluster>& clusters, vector<Point>& points)
{
	int num_of_points_changed = 0;
	int c, p = 0;
	float dist = 0;
	Point *point;

	for (c = 0; c < clusters.size(); c++)
	{
#pragma omp parallel for \
	private(dist,point,p) \
	shared(c,clusters,points)
		for (p = 0; p < points.size(); p++)
		{
			point = &points.at(p);
			float dist = euclid_dist(clusters.at(c).center, *point);
			if ((*point).cluster_index == -1 || ((*point).cluster_index != c && (*point).dist_from_center > dist))
			{
				if ((*point).cluster_index > -1)
				{
#pragma omp critical
					clusters.at((*point).cluster_index).number_of_points--;
				}

				(*point).cluster_index = c;
				(*point).dist_from_center = dist;

				//defined max diameter(2r) 
				if (clusters.at(c).diameter < dist * 2)
				{
#pragma omp critical
					clusters.at(c).diameter = dist * 2;
				}
#pragma omp critical
				{
					clusters.at(c).number_of_points++;
				}
				num_of_points_changed++;
			}
		}
	}
	return num_of_points_changed;
}


vector<Cluster> openMP_kmeans(vector<Point>& points, int max, int limit, float qm, float& final_quality)
{
	vector<Cluster> returnClusters;
	final_quality = 9999;//max

	for (int k = 2; k < max; k++)
	{
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
		if (debug == true)
			cout << "k=" << k << ". q=" << q << "\r\n";
		if (q < qm) {
			final_quality = q;
			return clusters;
		}
		else if (q < final_quality)
		{
			returnClusters = clusters;
			final_quality = q;
		}

	}
	return returnClusters;
}
