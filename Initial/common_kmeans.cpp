#include "stdafx.h"
#include "Header.h"
using namespace std;

float euclid_dist(Point center, Point point)
{
	int i;
	float ans = 0.0;

	for (i = 0; i<center.data.size(); i++)
		ans += (center.data.at(i) - point.data.at(i)) *  (center.data.at(i) - point.data.at(i));

	return((float)sqrt(ans));
}

void kmeans_init_clusters(vector<Cluster>& clusters, vector<Point>& points, int k)
{
	for (int i = 0; i < k; i++) {
		Cluster cluster = { points.at(i),0 };
		clusters.push_back(cluster);
	}
}


void kmeans_reset_points(vector<Point>& points)
{
	for (int i = 0; i < points.size(); i++)
	{
		points.at(i).cluster_index = -1;
		points.at(i).dist_from_center = 9999;
	}
}


