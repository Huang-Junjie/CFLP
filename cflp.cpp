#include <string>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <utility>
#include "cflp.h"
#include "io.h"


//根据当前问题，当前解，寻找为某个顾客分配需求代价最小的设施的下标及代价
pair<int, int> findLeastCostFacility(problem &p, solution &s, int &cus) {
	int mincost = INT_MAX;
	int minindex = -1;
	for (int i = 0; i < p.facilityNum; i++) {
		if (s.capacityLeft[i] < p.demand[cus]) continue;
		if (p.assignmentCost[cus][i] < mincost) {
			mincost = p.assignmentCost[cus][i];
			minindex = i;
		}
	}
	if (!s.isOpen[minindex]) mincost += p.openingCost[minindex];
	return make_pair(minindex, mincost);
}


//贪心法获得一个问题的初始解
void getInitalSolution(problem &p, solution &s) {
	s.isOpen.resize(p.facilityNum);
	s.assign.resize(p.customerNum);
	s.capacityLeft.resize(p.facilityNum);
	s.cost = 0;
	for (bool i : s.isOpen) i = false;
	for (int i = 0; i < p.facilityNum; i++) s.capacityLeft[i] = p.capacity[i];
	//贪心法得到初始解
	pair<int, int> leastAssign;
	for (int i = 0; i < p.customerNum; i++) {
		leastAssign = findLeastCostFacility(p, s, i);
		s.isOpen[leastAssign.first] = true;
		s.assign[i] = leastAssign.first;
		s.capacityLeft[leastAssign.first] -= p.demand[i];
		s.cost += leastAssign.second;
	}
}


//随机将1名顾客分配给另一个设施
void exchangeOp(problem &p, solution &s) {
	int cus = rand() % p.customerNum;
	int fac = rand() % p.facilityNum;
	while (true) {
		if (fac == s.assign[cus]) {
			fac = rand() % p.facilityNum;
			continue;
		}
		if (s.capacityLeft[fac] < p.demand[cus]) {
			//4/5概率选另外一个设施，1/5概率选另外一名顾客
			if (rand() % 5) fac = rand() % p.facilityNum;
			else cus = rand() % p.customerNum;
			continue;
		}
		s.capacityLeft[s.assign[cus]] += p.demand[cus];
		s.cost -= p.assignmentCost[cus][s.assign[cus]];
		if (s.capacityLeft[s.assign[cus]] == p.capacity[s.assign[cus]]) {
			s.isOpen[s.assign[cus]] = false;
			s.cost -= p.openingCost[s.assign[cus]];
		}
		if (!s.isOpen[fac]) {
			s.isOpen[fac] = true;
			s.cost += p.openingCost[fac];
		}
		s.assign[cus] = fac;
		s.capacityLeft[fac] -= p.demand[cus];
		s.cost += p.assignmentCost[cus][fac];
		return;
	}
}


//随机交换2名顾客分配的设施
void swapOp(problem &p, solution &s) {
	int cus1 = rand() % p.customerNum;
	int cus2 = rand() % p.customerNum;
	int exdemand;
	while (true) {
		if (cus1 == cus2) {
			cus2 = rand() % p.customerNum;
			continue;
		}
		//令cus2为需求大的顾客
		if (p.demand[cus1] > p.demand[cus2]) {
			int temp = cus1;
			cus1 = cus2;
			cus2 = temp;
		}
		exdemand = p.demand[cus2] - p.demand[cus1];
		if (s.capacityLeft[s.assign[cus1]] - exdemand < 0) {
			if (rand() % 2) cus1 = rand() % p.customerNum;
			else cus2 = rand() % p.customerNum;
			continue;
		}
		int fac1 = s.assign[cus1];
		int fac2 = s.assign[cus2];
		s.capacityLeft[fac1] -= exdemand;
		s.capacityLeft[fac2] += exdemand;
		s.cost -= p.assignmentCost[cus1][fac1];
		s.cost += p.assignmentCost[cus1][fac2];
		s.cost -= p.assignmentCost[cus2][fac2];
		s.cost += p.assignmentCost[cus2][fac1];
		s.assign[cus1] = fac2;
		s.assign[cus2] = fac1;
		return;
	}
}


//交换2名顾客分配的设施,无法交换则直接返回
void swapCus(problem &p, solution &s, int cus1, int cus2) {
	if (p.demand[cus1] > p.demand[cus2]) {
		int temp = cus1;
		cus1 = cus2;
		cus2 = temp;
	}
	int exdemand = p.demand[cus2] - p.demand[cus1];
	if (s.capacityLeft[s.assign[cus1]] - exdemand < 0) {
		return;
	}
	int fac1 = s.assign[cus1];
	int fac2 = s.assign[cus2];
	s.capacityLeft[fac1] -= exdemand;
	s.capacityLeft[fac2] += exdemand;
	s.cost -= p.assignmentCost[cus1][fac1];
	s.cost += p.assignmentCost[cus1][fac2];
	s.cost -= p.assignmentCost[cus2][fac2];
	s.cost += p.assignmentCost[cus2][fac1];
	s.assign[cus1] = fac2;
	s.assign[cus2] = fac1;
}


//随机选2名顾客，将这两名顾客间的所有顾客分配的设施反转
void inversionOp(problem &p, solution &s) {
	int cus1 = rand() % p.customerNum;
	int cus2 = rand() % p.customerNum;
	if (cus1 < cus2) {
		while (cus1 < cus2) {
			swapCus(p, s, cus1, cus2);
			cus1++;
			cus2--;
		}
		return;
	}
	if (cus1 > cus2) {
		while (true) {
			swapCus(p, s, cus1, cus2);
			cus1 = (cus1 + 1) % p.customerNum;
			if (cus1 == cus2) return;
			cus2 = (cus2 - 1 + p.customerNum) % p.customerNum;
			if (cus1 == cus2) return;
		}
	}
}


/*  将解s按概率改变为其一个邻域解
*	邻域操作：
*		1. 随机将1名顾客分配给另一个设施
*		2. 随机交换2名顾客分配的设施
*		3. 随机选2名顾客，将这两名顾客间的所有顾客分配的设施反转
*/
void getNewSolution(problem &p, solution &s) {
	int op = rand() % 3;
	if (op == 0) exchangeOp(p, s);
	else if (op == 1) swapOp(p, s);
	else inversionOp(p, s);
}


//贪心法
void greedy() {
	vector<int> result;
	vector<float> runtime;
	for (int i = 1; i < 72; i++) {
		cout << "---------------------------" << endl;
		cout << "P" << i << ":" << endl;
		srand((unsigned int)(time(NULL)));
		long startTime = clock();
		problem pro;
		solution sol;
		readInstance("Instances/p" + to_string(i), pro);
		getInitalSolution(pro, sol);
		//初始化一个未开始分配的解
		solution emptySol;
		emptySol.isOpen.resize(pro.facilityNum);
		emptySol.assign.resize(pro.customerNum);
		emptySol.capacityLeft.resize(pro.facilityNum);
		emptySol.cost = 0;
		for (bool i : emptySol.isOpen) i = false;
		for (int i = 0; i < pro.facilityNum; i++) emptySol.capacityLeft[i] = pro.capacity[i];
		//初始化分配顺序
		vector<int> order(pro.customerNum);
		for (int i = 0; i < order.size(); i++) order[i] = i;

		int index;
		int temp;
		pair<int, int> leastAssign;
		solution newSol;
		//打乱顺序使用贪心法，取最优值
		for (int j = 0; j < 1000; j++) {
			//打乱分配顺序
			for (int i = 0; i < order.size() - 1; i++) {
				index = rand() % (order.size() - i) + i;
				temp = order[i];
				order[i] = order[index];
				order[index] = temp;
			}
			//使用贪心法进行分配
			newSol = emptySol;
			for (int i = 0; i < pro.customerNum; i++) {
				leastAssign = findLeastCostFacility(pro, newSol, order[i]);
				newSol.isOpen[leastAssign.first] = true;
				newSol.assign[order[i]] = leastAssign.first;
				newSol.capacityLeft[leastAssign.first] -= pro.demand[order[i]];
				newSol.cost += leastAssign.second;
			}
			if (newSol.cost < sol.cost) sol = newSol;
			cout << "p" << i << ": times: " << j << endl;
			printSolution(sol);
		}
		result.push_back(sol.cost);
		runtime.push_back((clock() - startTime) / CLK_TCK);
		writeSolution("Results/Greedy/p" + to_string(i) + ".txt", sol);
	}
	wirteResultTable("Results/GreedyTable.csv", result, runtime);
}





//局部搜索法
void localSearch() {
	vector<int> result;
	vector<float> runtime;
	for (int i = 1; i < 72; i++) {
		cout << "---------------------------" << endl;
		cout << "P" << i << ":" << endl;
		srand((unsigned int)(time(NULL)));
		long startTime = clock();
		problem pro;
		solution sol;
		readInstance("Instances/p" + to_string(i), pro);
		getInitalSolution(pro, sol);
		solution newSol(sol);
		for (int j = 0; j < 100000; j++) {
			//寻找邻域解
			getNewSolution(pro, newSol);
			//若新解cost小，则直接接受
			if (newSol.cost < sol.cost) {
				sol = newSol;
			}
			else newSol = sol;
			if (j % 1000 == 0) {
				cout << "p" << i << ": times: " << j << endl;
				printSolution(sol);
			}
		}
		result.push_back(sol.cost);
		runtime.push_back((clock() - startTime) / CLK_TCK);
		writeSolution("Results/LS/p" + to_string(i) + ".txt", sol);

	}
	wirteResultTable("Results/LSTable.csv", result, runtime);
}


//模拟退火法
void sa() {
	vector<int> result;
	vector<float> runtime;
	double Tmax = 500;
	double Tmin = 0.1;
	int times = 1000;
	double rate = 0.99;
	for (int i = 1; i < 72; i++) {
		cout << "---------------------------" << endl;
		cout << "P" << i << ":" << endl;
		srand((unsigned int)(time(NULL)));
		long startTime = clock();
		problem pro;
		solution sol;
		readInstance("Instances/p" + to_string(i), pro);
		getInitalSolution(pro, sol);
		solution newSol(sol);
		solution bestSol(sol);
		double T = Tmax;
		while (T > Tmin) {
			for (int j = 0; j < times; j++) {
				//寻找邻域解
				getNewSolution(pro, newSol);
				//若新解cost小，则直接接受
				//若新解cost大，则以exp(-(newcost - cost)/ T)概率接受
				if (newSol.cost < sol.cost ||
					rand() / (RAND_MAX + 1.0) < exp((sol.cost - newSol.cost) / T)) {
					sol = newSol;
				}
				else newSol = sol;
				if (sol.cost < bestSol.cost) bestSol = sol;
			}
			T *= rate;
			cout << "p" << i << ": T: " << T << endl;
			printSolution(sol);
		}
		result.push_back(bestSol.cost);
		runtime.push_back((clock() - startTime) / CLK_TCK);
		writeSolution("Results/SA/p" + to_string(i) + ".txt", bestSol);
	}
	wirteResultTable("Results/SATable.csv", result, runtime);
}


int main() {
	/*greedy();
	localSearch();*/
	sa();
}
