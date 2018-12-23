#include <string>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include "cflp.h"
#include "io.h"


//���һ������ĳ�ʼ��
void getInitalSolution(problem &p, solution &s) {
	int mincost;
	int minindex;
	int cost;
	s.isOpen.resize(p.facilityNum);
	s.assign.resize(p.customerNum);
	s.capacityLeft.resize(p.facilityNum);
	s.cost = 0;
	for (bool i : s.isOpen) i = false;
	for (int i = 0; i < p.facilityNum; i++) s.capacityLeft[i] = p.capacity[i];
	//̰�ķ��õ���ʼ��
	for (int i = 0; i < p.customerNum; i++) {
		mincost = INT_MAX;
		minindex = -1;
		for (int j = 0; j < p.facilityNum; j++) {
			if (s.capacityLeft[j] < p.demand[i]) continue;
			cost = s.isOpen[j] ? 0 : p.openingCost[j];
			cost += p.assignmentCost[i][j];
			if (cost < mincost) {
				mincost = cost;
				minindex = j;
			}
		}
		s.isOpen[minindex] = true;
		s.assign[i] = minindex;
		s.capacityLeft[minindex] -= p.demand[i];
		s.cost += mincost;
	}
}



//�����1���˿ͷ������һ����ʩ
void exchangeOp(problem &p, solution &s) {
	int cus = rand() % p.customerNum;
	int fac = rand() % p.facilityNum;
	while (true) {
		if (fac == s.assign[cus]) {
			fac = rand() % p.facilityNum;
			continue;
		}
		if (s.capacityLeft[fac] < p.demand[cus]) {
			//4/5����ѡ����һ����ʩ��1/5����ѡ����һ���˿�
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


//�������2���˿ͷ������ʩ
void swapOp(problem &p, solution &s) {
	int cus1 = rand() % p.customerNum;
	int cus2 = rand() % p.customerNum;
	int exdemand;
	while (true) {
		if (cus1 == cus2) {
			cus2 = rand() % p.customerNum;
			continue;
		}
		//��cus2Ϊ�����Ĺ˿�
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


//����2���˿ͷ������ʩ,�޷�������ֱ�ӷ���
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


//���ѡ2���˿ͣ����������˿ͼ�����й˿ͷ������ʩ��ת
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


/*  ����s�����ʸı�Ϊ��һ�������
*	���������
*		1. �����1���˿ͷ������һ����ʩ
*		2. �������2���˿ͷ������ʩ
*		3. ���ѡ2���˿ͣ����������˿ͼ�����й˿ͷ������ʩ��ת
*/
void getNewSolution(problem &p, solution &s) {
	int op = rand() % 3;
	if (op == 0) exchangeOp(p, s);
	else if (op == 1) swapOp(p, s);
	else inversionOp(p, s);
}


int main()
{
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
		for (int i = 0; i < 100000; i++) {
			//Ѱ�������
			getNewSolution(pro, newSol);
			//���½�costС����ֱ�ӽ���
			if (newSol.cost < sol.cost) {
				sol = newSol;
			}
			else newSol = sol;
			if (i % 1000 == 0) {
				cout << "times: " << i << endl;
				printSolution(sol);
			}
		}

		result.push_back(sol.cost);
		runtime.push_back((clock() - startTime) / CLK_TCK);
		writeSolution("Results/LS/p" + to_string(i) + ".txt", sol);

	}
	wirteResultTable("Results/LSTable.csv", result, runtime);
}
