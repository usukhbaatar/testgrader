#include <vector>
#include <string>
#include <array>
#include <iostream>
using namespace std;

vector<int> init_A(int map_id) {
	vector<int> ret(4);
	for(int i = 0; i < 4; i++) {
		/**
		 * 4 танкаа сонгох ба ret[i] дахь танкны утга
		 * 1 -> normal tank
		 * 2 -> light tank
		 * 3 -> heavy tank
		 * */ 
		ret[i] = rand() % 3 + 1;
	}
	return ret;
}

vector<pair<int, string>> move_A(
	vector<vector<int>> M,
	vector<vector<int>> C,
	vector<pair<char, pair<int, int>>> B) {
	string cmp = "GF";
	string dir = "URDL";
	vector<pair<int, string>> ret;
	for(int i = 0; i < 4; i++) {
		
		string str = "";
		str.push_back(cmp[rand()%10 < 3]);
		if (str[0] == 'F') str.push_back(dir[rand() % 4]);
		else {
			int m = rand() % 3 + 1;
			char c = dir[rand() % 4];
			for (int j = 0; j < m; j++) {
				str.push_back(c);
			}
		}
		/**
		 * i -> танкны дугаар 
		 * str -> i дахь танкны нүүдэл. 
		 * Жишээ нь. 
		 * - str = FU дээшээ буудна, 
		 * - str = GRRR баруун тийшээ 3 явна. хэрвээ саад тулах эсвэл light tank биш бол баруун тийшээ нэг л удаа явах эсвэл байрандаа зогсоно
		 * - str = GU - дээшээ нэг явна.
		*/
		ret.push_back(make_pair(i, str));
	}
	return ret;
}
