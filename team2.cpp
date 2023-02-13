#include <vector>
#include <string>
using namespace std;

vector<int> init_B() {
	vector<int> ret(4);
	for(int i = 0; i < 4; i++) {
		ret[i] = rand() % 3 + 1;
	}
	return ret;
}

vector<pair<int, string>> move_B() {
	string cmp = "GF";
	string dir = "URDL";
	vector<pair<int, string>> ret;
	for(int i = 0; i < 4; i++) {
		int x = i;
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
		ret.push_back(make_pair(x, str));
	}
	return ret;
}
