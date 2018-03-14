#include "stdafx.h"
#include "Header.h"
using namespace std;


float kmeans_calc_quality(vector<Cluster>& clusters, vector<Point>& points)
{
	vector<vector<float>> dist_between_centers;
	dist_between_centers.assign(clusters.size(), vector<float>(clusters.size()));
	for (int c = 0; c < dist_between_centers.size(); c++)
	{
		for (int c2 = c + 1; c2 < dist_between_centers.size(); c2++)
		{
			float dist = euclid_dist(clusters.at(c).center, clusters.at(c2).center);
			dist_between_centers.at(c).at(c2) = dist;
			dist_between_centers.at(c2).at(c) = dist;

		}
	}

	float q = 0;
	int divider = 0;
	for (int i = 0; i < dist_between_centers.size(); i++)
	{
		for (int j = 0; j < dist_between_centers.at(i).size(); j++)
		{
			if (i == j)
				continue;
			q += (clusters.at(i).diameter / dist_between_centers.at(i).at(j));
			divider++;
		}
	}
	return q / divider;
}

void kmeans_recalc_clusters_centers(vector<Cluster>& clusters, vector<Point>& points)
{
	vector<Point> newCenters(clusters.size());
	for (int i = 0; i < points.size(); i++)
	{
		//first time init with values
		if (points.at(0).data.size() > newCenters.at(points.at(i).cluster_index).data.size())
		{
			for (int p = 0; p < points.at(i).data.size(); p++)
			{
				newCenters.at(points.at(i).cluster_index).data.push_back(points.at(i).data.at(p));
			}
		}
		else
		{
			for (int p = 0; p < points.at(i).data.size(); p++)
			{
				newCenters.at(points.at(i).cluster_index).data.at(p) += points.at(i).data.at(p);
			}
		}
	}

	for (int i = 0; i < newCenters.size(); i++)
	{
		for (int p = 0; p < newCenters.at(i).data.size(); p++)
		{
			newCenters.at(i).data.at(p) = newCenters.at(i).data.at(p) / clusters.at(i).number_of_points;
		}
		clusters.at(i).center = newCenters.at(i);
	}
}

int  kmeans_select_cluster_for_points(vector<Cluster>& clusters, vector<Point>& points)
{
	int num_of_points_changed = 0;
	for (int c = 0; c < clusters.size(); c++)
	{
		for (int p = 0; p < points.size(); p++)
		{

			float dist = euclid_dist(clusters.at(c).center, points.at(p));
			if (points.at(p).cluster_index == -1 || (points.at(p).cluster_index != c &&  points.at(p).dist_from_center > dist))
			{
				if (points.at(p).cluster_index > -1)
				{
					clusters.at(points.at(p).cluster_index).number_of_points--;
				}

				points.at(p).cluster_index = c;
				points.at(p).dist_from_center = dist;

				//defined max diameter(2r) 
				if (clusters.at(c).diameter < dist * 2)
				{
					clusters.at(c).diameter = dist * 2;
				}
				clusters.at(c).number_of_points++;
				num_of_points_changed++;
			}
		}
	}
	return num_of_points_changed;
}

vector<Cluster> kmeans(vector<Point>& points, int max, int limit, float qm,float T,float dT)
{
	vector<Cluster> returnClusters;

	///Change K!!!!!!
	for (int k = 2; k < max; k++)
	{
		vector<Cluster> clusters;
		kmeans_reset_points(points);
		kmeans_init_clusters(clusters, points, k);
		for (int i = 0; i < limit; i++)
		{
			int num_of_points_changed = kmeans_select_cluster_for_points(clusters, points);
			if (num_of_points_changed == 0)
				break;
			kmeans_recalc_clusters_centers(clusters, points);
		}

		float q = kmeans_calc_quality(clusters, points);
		if (debug == true)
			cout << "k=" << k << ". q=" << q << "\r\n";		
	}
	return returnClusters;
}
