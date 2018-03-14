#include "stdafx.h"
#include "Header.h"
#include <fstream>
#include <iostream>
using namespace std;

void read_from_file(char* file_name, vector<Point>& points, int& n, int& row_length, int& max, int& limit, float& qm)
{
	ifstream inFile(file_name);
	inFile >> n >> row_length >> max >> limit >> qm;
	for (int i = 0; i < n; i++)
	{

		int name;
		inFile >> name;
		vector<float> data;
		//read value by value;
		for (int j = 0; j < row_length; j++)
		{
			float value = 0;
			inFile >> value;
			data.push_back(value);
		}

		point_t p = { name ,data,-1,99999 };
		points.push_back(p);
	}
	inFile.close();
}

void write_to_file(char* file_name, vector<Cluster>& clusters, float final_quality, double runtime)
{
	ofstream out_file(file_name);
	out_file << "K=" << clusters.size() << ". Quality=" << final_quality << ". runtime=" << runtime << ".\r\n";
	for (int c = 0; c < clusters.size(); c++)
	{
		out_file << "C" << c + 1 << " : ";
		for (int p = 0; p < clusters.at(c).center.data.size(); p++)
		{
			out_file << clusters.at(c).center.data.at(p) << " ";
		}
		out_file << "\r\n";
	}
	out_file.close();

}
