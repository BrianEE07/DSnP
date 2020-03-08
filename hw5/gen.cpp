#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

int main() {
	int n = 100000;
	srand(4567);
	vector <string> vec;
	for (int i = 0;i < n; ++i) {
		string s = "";
		for (int j = 0;j < 5; ++j) {
			s += rand() % 26 + 'a';
		}
		vec.push_back(s);
		cout << "adta -s " << s << endl;
	}
	cout << "adtp" << endl;
	// random_shuffle(vec.begin(), vec.end());
	sort(vec.begin(), vec.end());
	for (int i = 0;i < vec.size(); ++i) {
		cout << "adtd -s " << vec[i] << endl;
	}
	cout << "usage" << endl;
	cout << "adtp" << endl;
	cout << "q -f" << endl;
}
