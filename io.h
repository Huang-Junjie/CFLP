#ifndef IO_H
#define IO_H

#include <iostream>
#include <fstream>
#include "cflp.h"
using namespace std;

//���ļ��ж�ȡһ���������ӷ���һ��problem��
bool readInstance(string path, problem &p) {
	ifstream file(path.c_str(), ios_base::in | ios_base::binary);
	if (!file) return false;
	int capacity;
	int openingCost;
	float demand;
	float assignmentCost;
	file >> p.facilityNum;
	file >> p.customerNum;
	for (int i = 0; i < p.facilityNum; i++) {
		file >> capacity;
		file >> openingCost;
		p.capacity.push_back(capacity);
		p.openingCost.push_back(openingCost);
	}
	for (int i = 0; i < p.customerNum; i++) {
		file >> demand;
		p.demand.push_back((int)demand);
	}
	p.assignmentCost.resize(p.customerNum);
	for (int i = 0; i < p.facilityNum; i++) {
		for (int j = 0; j < p.customerNum; j++) {
			file >> assignmentCost;
			p.assignmentCost[j].push_back((int)assignmentCost);
		}
	}
	file.close();
	return true;
}


//��һ������ʵ���Ľ�д���ļ���
void writeSolution(string path, solution &s) {
	ofstream file(path.c_str(), ios_base::out | ios_base::binary);
	if (!file) return;
	file << s.cost << endl;
	for (int i = 0; i < s.isOpen.size(); i++) {
		file << s.isOpen[i] << " ";
	}
	file << endl;
	for (int i = 0; i < s.assign.size(); i++) {
		file << s.assign[i] << " ";
	}
	file << endl;
	file.close();
}


//����������Ľ���ͻ���ʱ��д���ļ���
void wirteResultTable(string path, vector<int> &result, vector<float> time) {
	ofstream file(path.c_str(), ios_base::out | ios_base::binary);
	if (!file) return;
	file << ",Result,Time(s)" << endl;
	for (int i = 0; i < result.size() && i < time.size(); i++) {
		file << 'p' << i + 1 << ',' << result[i] << ',' << time[i] << endl;
	}
	file.close();
}

//��ӡ��
void printSolution(solution &s) {
	cout << s.cost << endl;
	for (int i = 0; i < s.isOpen.size(); i++) {
		cout << s.isOpen[i] << " ";
	}
	cout << endl;
	for (int i = 0; i < s.assign.size(); i++) {
		cout << s.assign[i] << " ";
	}
	cout << endl;
}

#endif