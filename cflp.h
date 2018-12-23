#ifndef CLFP_H
#define CLFP_H

#include <vector>
using namespace std;

struct solution {
	int cost;
	vector<bool> isOpen;
	vector<int> assign;
	vector<int> capacityLeft;
};


struct problem {
	int facilityNum;
	int customerNum;
	vector<int> capacity;
	vector<int> openingCost;
	vector<int> demand;
	//assignmentCost[i][j] is the cost of allocating all the demand of customer i to facility j.
	vector<vector<int>> assignmentCost;
};

#endif