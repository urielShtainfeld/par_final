#include "stdafx.h"
#include "Header.h"

void debug_print_point(Point& point)
{
	cout << "P=" << point.pointNum << ". Cluster Index=" << point.cluster_index << ". Dist From Center=" << point.dist_from_center << " Data:\r\n";
	for (int i = 0; i < point.data.size(); i++)
	{
		cout << point.data.at(i) << " ";
	}
	cout << " \r\n\r\n";
}

void debug_print_cluster(Cluster& cluster)
{
	cout << "Diameter=" << cluster.diameter << ". Number Of Points = " << cluster.number_of_points << ". Center =";
	debug_print_point(cluster.center);
}
