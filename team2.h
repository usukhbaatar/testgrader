#include <array>
#include <vector>
#include <string>
using namespace std;

vector<int> init_B(int);
vector<pair<int, string>> move_B(
	vector<vector<int>>,
	vector<vector<int>>,
	vector<pair<char, pair<int, int>>>);