#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <array>
#include "team1.h"
#include "team2.h"

using namespace std;

const int num_tanks = 4;
const int tank_score = 200;
const int flag_score = 9999;

int rounds;

int score_A = 16, score_B = 16;
int diffA = 0, diffB = 0;

int ID, H, W;
int BID = 0;
int id1, id2;
bool initFailed = false;

vector<vector<int>> map;
vector<vector<int>> color;
vector<vector<int>> grid;

struct tank {
	int id;
	int type;
	int pos_x, pos_y;
	int health;
	int speed;
	int damage;	
	int supply;
	bool alive;
};

struct bullet {
	int bullet_id;
	int id;
	int pos_x, pos_y;
	int damage;
	char direction;
};

struct flag {
	int pos_x, pos_y;
	int health = 125;
	bool alive = 1;
} flag_A, flag_B;

vector<tank> tanks_A;
vector<pair<int, int> > grid_pos_A(4);
vector<tank> tanks_B;
vector<pair<int, int> > grid_pos_B(4);
vector<bullet> bullets;
vector<pair<int, int> > grid_pos_bullet;
vector<int> damage(4), speed(4), health(4);
vector<vector<int>> occ;
vector<pair<int, string>> m_A, m_B;


int is_valid(bool valid_A, bool valid_B) {
	if(!valid_A && !valid_B) {
		return 0;
	}
	else if(!valid_A) {
		return -1;
	}
	else if(!valid_B) {
		return 1;
	}
	return 100;
}

vector<vector<int>> rot_map(vector<vector<int>> B) {
	for(int i = 0; i < H; i++) {
		reverse(B[i].begin(), B[i].end());
		for(int j = 0; j < W; j++) {
			if(abs(B[i][j]) > 9) {
				B[i][j] = -B[i][j];
			}
		}
	}
	reverse(B.begin(),B.end());

	return B;
}

char rot_char(char c) {
	char ret;
	if (c == 'U') return 'D';
	if (c == 'D') return 'U';
	if (c == 'L') return 'R';
	if (c == 'R') return 'L';
	return c;
}

vector<pair<int, string>> rot_move(vector<pair<int, string>> B) {
	for(int i = 0; i < 4; i++) {
		B[i].first *= (-1);
		for(int j = 1; j < B[i].second.size(); j++) {
			B[i].second[j] = rot_char(B[i].second[j]);
		}
	}
	return B;
}

vector<tank> rot_tank(vector<tank> B) {
	int sz = (int)B.size();
	for(int i = 0; i < sz; i++) {
		B[i].id = -B[i].id;
 		B[i].pos_x = H - B[i].pos_x - 2;
		B[i].pos_y = W - B[i].pos_y - 2;
	}
	return B;
}

vector<bullet> rot_bullets(vector<bullet> B) {
	int sz = (int)B.size();
	for(int i = 0; i < sz; i++) {
		char tmp = B[i].direction;
		switch(tmp) {
			case 'U':
				tmp = 'D';
				break;
			case 'D':
				tmp = 'U';
				break;
			case 'L':
				tmp = 'R';
				break;
			default:
				tmp = 'L';
		}
		B[i].pos_x = H - B[i].pos_x;
		B[i].pos_y = W - B[i].pos_y;
	}
	return B;
}

// 0 -> 10  1 -> 11 ..... etc
// 0 -> -10 1 -> -11 ....... etc
bool invalid_move_A[num_tanks], invalid_move_B[num_tanks];

void change_tag(int id, int p) {
	if(p == 1) {
		invalid_move_A[id] = 1;
	}
	else {
		invalid_move_B[id] = 1;
	}
}

bool validate(vector<pair<int, string>> &m1, int p) {
	sort(m1.begin(), m1.end());
	vector<int> vis(num_tanks, 0);
	int sz = (int)m1.size();

	for(int i = 0; i < sz; i++) {
		int idx = m1[i].first - 10;
		if(idx < 0 || idx > 3 || vis[idx]){
			return 0;
		}

		vis[idx] = 1;
		string &str = m1[i].second;
		if(p > 0 && !tanks_A[idx].alive) {
			str = "G";
			continue;
		}
		if(p < 0 && !tanks_B[idx].alive) {
			str = "G";
			continue;
		}
		if(str.empty()) {
			str = "G";
			change_tag(idx, p);
			continue;
		}
		if(str[0] != 'G' && str[0] != 'F') {
			str = "G";
			change_tag(idx, p);
			continue;
		}
		if(str[0] == 'F') {
			int num = 0;

			if(p == -1) {
				num = tanks_B[idx].supply;
			}
			else {
				num = tanks_A[idx].supply;
			}

			if(	num == 0 || str.size() != 2 || !(str[1] == 'U' || str[1] == 'D'  || str[1] == 'L' || str[1] == 'R')) {
				str = "G";
				change_tag(idx, p);
			} 
		}
		else {
			if((int)str.size() == 1) {
				continue;
			}
			char ch = str[1];
			bool fl = 1;
			if (!(ch == 'U' || ch == 'D' || ch == 'L' || ch == 'R')) {
				str = "G";
				change_tag(idx, p);
				continue;
			}
			for(int i = 1; i < (int)str.size(); i++) {
				fl &= (str[i] == ch);
				str[i] = ch;
			}

			int max_speed = 0;

			if(p == -1) {
				max_speed = speed[tanks_B[idx].type]; //!!!!!!!!!!!!!
				max_speed = tanks_B[i].speed;
			}
			else {
				max_speed = speed[tanks_A[idx].type];
				max_speed = tanks_A[i].speed;
			}

			while((int)str.size() - 1 > max_speed) {
				str.pop_back();
				fl = 0;
			}
			if(!fl) {
				change_tag(idx, p);
			}
			// cout << max_speed << " " << str << endl;
		}
	}
	return 1;
}

void deny_tanks(vector<bool> &stat, int p, vector<pair<int, string>> &m) {
	if(p == 1) {
		for(int ind = 0; ind < 4; ind++) {
			if(stat[ind]) {
				continue;
			}
			if(m[ind].second[0] == 'F' || m[ind].second.size() == 1) {
				stat[ind] = 1;
				continue;
			}
			char ch = m[ind].second[1];
			int change_x[2], change_y[2];
			//finding the change of coordinate
			if(ch == 'U') {
				change_x[0] = -1;
				change_y[0] = 0;
				change_x[1] = -1;
				change_y[1] = 1;
			}
			if(ch == 'D') {
				change_x[0] = 2;
				change_y[0] = 0;
				change_x[1] = 2;
				change_y[1] = 1;
			}
			if(ch == 'L') {
				change_x[0] = 0;
				change_y[0] = -1;
				change_x[1] = 1;
				change_y[1] = -1;
			}
			if(ch == 'R') {
				change_x[0] = 0;
				change_y[0] = 2;
				change_x[1] = 1;
				change_y[1] = 2;
			}
			//checking if we need to deny the tank
			if(!tanks_A[ind].alive) {
				stat[ind] = 1;
				continue;
			}
			if(grid_pos_A[ind].first % 6 != 0 || grid_pos_A[ind].second % 6 != 0) {
				continue;
			}
			if(tanks_A[ind].pos_x + change_x[0] < 0 || tanks_A[ind].pos_x + change_x[0] >= H) {
				stat[ind] = 1;
				continue;
			}
			if(tanks_A[ind].pos_y + change_y[0] < 0 || tanks_A[ind].pos_y + change_y[0] >= W) {
				stat[ind] = 1;
				continue;
			}
			if(tanks_A[ind].pos_x + change_x[1] < 0 || tanks_A[ind].pos_x + change_x[1] >= H) {
				stat[ind] = 1;
				continue;
			}
			if(tanks_A[ind].pos_y + change_y[1] < 0 || tanks_A[ind].pos_y + change_y[1] >= W) {
				stat[ind] = 1;
				continue;
			}
		} 
	} else {
		for(int ind = 0; ind < 4; ind++) {
			if (stat[ind]) {
				continue;
			}
			if(m[ind].second[0] == 'F' || m[ind].second.size() == 1) {
				stat[ind] = 1;
				continue;
			}
			char ch = m[ind].second[1];
			int change_x[2], change_y[2];
			//finding the cfhange of coordinate
			if(ch == 'U') {
				change_x[0] = -1;
				change_y[0] = 0;
				change_x[1] = -1;
				change_y[1] = 1;
			}
			if(ch == 'D') {
				change_x[0] = 2;
				change_y[0] = 0;
				change_x[1] = 2;
				change_y[1] = 1;
			}
			if(ch == 'L') {
				change_x[0] = 0;
				change_y[0] = -1;
				change_x[1] = 1;
				change_y[1] = -1;
			}
			if(ch == 'R') {
				change_x[0] = 0;
				change_y[0] = 2;
				change_x[1] = 1;
				change_y[1] = 2;
			}
			//checking if we need to deny the tank
			if(!tanks_B[ind].alive) {
				stat[ind] = 1;
				continue;
			}
			if(grid_pos_B[ind].first % 6 != 0 || grid_pos_B[ind].second % 6 != 0) {
				continue;
			}
			if(tanks_B[ind].pos_x + change_x[0] < 0 || tanks_B[ind].pos_x + change_x[0] >= H) {
				stat[ind] = 1;
				continue;
			}
			if(tanks_B[ind].pos_y + change_y[0] < 0 || tanks_B[ind].pos_y + change_y[0] >= W) {
				stat[ind] = 1;
				continue;
			}

			if(tanks_B[ind].pos_x + change_x[1] < 0 || tanks_B[ind].pos_x + change_x[1] >= H) {
				stat[ind] = 1;
				continue;
			}
			if(tanks_B[ind].pos_y + change_y[1] < 0 || tanks_B[ind].pos_y + change_y[1] >= W) {
				stat[ind] = 1;
				continue;
			}
		}
	}
}

void fill(int x0, int y0, int x1, int y1, int ff) {
	for(int i = x0; i <= x1; ++i) {
		for(int j = y0; j <= y1; ++j) {
			grid[i][j] = ff;
		}
	}
}

void hitting_bullets(vector<bool> &is) {
	int change_x[4] = {-1, -1, 0, 0};
	int change_y[4] = {-1, 0, -1, 0};
	for(int j = 0; j < bullets.size(); j++) {
		for(int i = 0; i < 4; i++) {
			int xx = grid_pos_bullet[j].first + change_x[i];
			int yy = grid_pos_bullet[j].second + change_y[i];
			if(xx < 0 || xx >= 6 * H) {
				is[j] = 1;
				continue;
			}
			if(yy < 0 || yy >= 6 * W) {
				is[j] = 1;
				continue;
			}
			if(grid[xx][yy] == 3 || grid[xx][yy] == 0 || grid[xx][yy] == bullets[j].id) {
				continue;
			}
			is[j] = 1;
		}
	}
}

void eliminating_bullets(vector<bool> is, vector<pair<int, string>> m1, vector<pair<int, string>> m2) {
	int change_x[4] = {-1, -1, 0, 0};
	int change_y[4] = {-1, 0, -1, 0};
	for(int j = bullets.size() - 1; j >= 0; j--) {
		if(!is[j]) {
			continue;
		}
		int ind = bullets[j].id;
		if(ind < 0) {
			ind = ind * (-1) - 10;
			tanks_B[ind].supply++;
		} else {
			ind = ind - 10;
			tanks_A[ind].supply++;
		}
		vector<bool> vis(5, false);
		for(int i = 0; i < 4; i++) {
			int xx = grid_pos_bullet[j].first + change_x[i];
			int yy = grid_pos_bullet[j].second + change_y[i];
			if(xx < 0 || xx >= 6 * H) {
				continue;
			}
			if(yy < 0 || yy >= 6 * W) {
				continue;
			}
			if(grid[xx][yy] == 3 || grid[xx][yy] == 0 || grid[xx][yy] == bullets[j].id) {
				continue;
			}
			if(grid[xx][yy] == 1) {
				//fill
				fill(xx / 6 * 6, yy / 6 * 6, xx / 6 * 6 + 5, yy / 6 * 6 + 5, 0);
				map[xx / 6][yy / 6] = 0;
				occ[xx / 6][yy / 6]--;
				continue;
			}
			if(grid[xx][yy] == 2) {
				continue;
			}
			if((grid[xx][yy] ^ bullets[j].id) > 0) {
				continue;
			}
			if(bullets[j].id > 0) {
				if(grid[xx][yy] == -100) {
					if(vis[4]) {
						continue;
					}
					vis[4] = 1;
					flag_B.health -= bullets[j].damage;
					if(flag_B.health <= 0) {
						flag_B.alive = 0;
						continue;
					}
				} else {
					if(vis[grid[xx][yy] * (-1) - 10]) {
						continue;
					}
					vis[grid[xx][yy] * (-1) - 10] = 1;
					tanks_B[grid[xx][yy] * (-1) - 10].health -= bullets[j].damage;
					if (tanks_B[grid[xx][yy] * (-1) - 10].health > 0) {
						continue;
					}
					score_A += tank_score;
					diffA += tank_score;
					tanks_B[grid[xx][yy] * (-1) - 10].alive = false;
					map[tanks_B[grid[xx][yy] * (-1) - 10].pos_x][tanks_B[grid[xx][yy] * (-1) - 10].pos_y] = 0;
					map[tanks_B[grid[xx][yy] * (-1) - 10].pos_x][tanks_B[grid[xx][yy] * (-1) - 10].pos_y + 1] = 0;
					map[tanks_B[grid[xx][yy] * (-1) - 10].pos_x + 1][tanks_B[grid[xx][yy] * (-1) - 10].pos_y] = 0;
					map[tanks_B[grid[xx][yy] * (-1) - 10].pos_x + 1][tanks_B[grid[xx][yy] * (-1) - 10].pos_y + 1] = 0;
					occ[tanks_B[grid[xx][yy] * (-1) - 10].pos_x][tanks_B[grid[xx][yy] * (-1) - 10].pos_y]--;
					occ[tanks_B[grid[xx][yy] * (-1) - 10].pos_x][tanks_B[grid[xx][yy] * (-1) - 10].pos_y + 1]--;
					occ[tanks_B[grid[xx][yy] * (-1) - 10].pos_x + 1][tanks_B[grid[xx][yy] * (-1) - 10].pos_y]--;
					occ[tanks_B[grid[xx][yy] * (-1) - 10].pos_x + 1][tanks_B[grid[xx][yy] * (-1) - 10].pos_y + 1]--;
					if(grid_pos_B[grid[xx][yy] * (-1) - 10].first % 6 == 0 && grid_pos_B[grid[xx][yy] * (-1) - 10].second % 6 == 0) {
						fill(grid_pos_B[grid[xx][yy] * (-1) - 10].first, grid_pos_B[grid[xx][yy] * (-1) - 10].second,
						 grid_pos_B[grid[xx][yy] * (-1) - 10].first + 11, grid_pos_B[grid[xx][yy] * (-1) - 10].second + 11, 0);
						continue;
					}
					char ch = m2[grid[xx][yy] * (-1) - 10].second[1];
					if(ch == 'U') {
						occ[tanks_B[grid[xx][yy] * (-1) - 10].pos_x - 1][tanks_B[grid[xx][yy] * (-1) - 10].pos_y]--;
						occ[tanks_B[grid[xx][yy] * (-1) - 10].pos_x - 1][tanks_B[grid[xx][yy] * (-1) - 10].pos_y + 1]--;
					}
					if(ch == 'D') {
						occ[tanks_B[grid[xx][yy] * (-1) - 10].pos_x + 2][tanks_B[grid[xx][yy] * (-1) - 10].pos_y]--;
						occ[tanks_B[grid[xx][yy] * (-1) - 10].pos_x + 2][tanks_B[grid[xx][yy] * (-1) - 10].pos_y + 1]--;
					}
					if(ch == 'R') {
						occ[tanks_B[grid[xx][yy] * (-1) - 10].pos_x][tanks_B[grid[xx][yy] * (-1) - 10].pos_y + 2]--;
						occ[tanks_B[grid[xx][yy] * (-1) - 10].pos_x + 1][tanks_B[grid[xx][yy] * (-1) - 10].pos_y + 2]--;
					}
					if(ch == 'L') {
						occ[tanks_B[grid[xx][yy] * (-1) - 10].pos_x][tanks_B[grid[xx][yy] * (-1) - 10].pos_y - 1]--;
						occ[tanks_B[grid[xx][yy] * (-1) - 10].pos_x + 1][tanks_B[grid[xx][yy] * (-1) - 10].pos_y - 1]--;
					}
					fill(grid_pos_B[grid[xx][yy] * (-1) - 10].first, grid_pos_B[grid[xx][yy] * (-1) - 10].second,
						 grid_pos_B[grid[xx][yy] * (-1) - 10].first + 11, grid_pos_B[grid[xx][yy] * (-1) - 10].second + 11, 0);
				}
			} else {
				if(grid[xx][yy] == 100) {
					if(vis[4]) {
						continue;
					}
					vis[4] = 1;
					flag_A.health -= bullets[j].damage;
					if (flag_A.health <= 0) {
						flag_A.alive = 0;
						continue;
					}
				} else {
					if(vis[grid[xx][yy] - 10]) {
						continue;
					}
					vis[grid[xx][yy] - 10] = 1;
					tanks_A[grid[xx][yy] - 10].health -= bullets[j].damage;
					if (tanks_A[grid[xx][yy] - 10].health > 0) {
						continue;
					}
					score_B += tank_score;
					diffB += tank_score;
					tanks_A[grid[xx][yy] - 10].alive = false;
					map[tanks_A[grid[xx][yy] - 10].pos_x][tanks_A[grid[xx][yy] - 10].pos_y] = 0;
					map[tanks_A[grid[xx][yy] - 10].pos_x][tanks_A[grid[xx][yy] - 10].pos_y + 1] = 0;
					map[tanks_A[grid[xx][yy] - 10].pos_x + 1][tanks_A[grid[xx][yy] - 10].pos_y] = 0;
					map[tanks_A[grid[xx][yy] - 10].pos_x + 1][tanks_A[grid[xx][yy] - 10].pos_y + 1] = 0;
					occ[tanks_A[grid[xx][yy] - 10].pos_x][tanks_A[grid[xx][yy] - 10].pos_y]--;
					occ[tanks_A[grid[xx][yy] - 10].pos_x][tanks_A[grid[xx][yy] - 10].pos_y + 1]--;
					occ[tanks_A[grid[xx][yy] - 10].pos_x + 1][tanks_A[grid[xx][yy] - 10].pos_y]--;
					occ[tanks_A[grid[xx][yy] - 10].pos_x + 1][tanks_A[grid[xx][yy] - 10].pos_y + 1]--;
					if(grid_pos_A[grid[xx][yy] - 10].first % 6 == 0 && grid_pos_A[grid[xx][yy] - 10].second % 6 == 0) {
						fill(grid_pos_A[grid[xx][yy] - 10].first, grid_pos_A[grid[xx][yy] - 10].second,
							 grid_pos_A[grid[xx][yy] - 10].first + 11, grid_pos_A[grid[xx][yy] - 10].second + 11, 0);
						continue;
					}
					char ch = m2[grid[xx][yy] - 10].second[1];
					if(ch == 'U') {
						occ[tanks_A[grid[xx][yy] - 10].pos_x - 1][tanks_A[grid[xx][yy] - 10].pos_y]--;
						occ[tanks_A[grid[xx][yy] - 10].pos_x - 1][tanks_A[grid[xx][yy] - 10].pos_y + 1]--;
					}
					if(ch == 'D') {
						occ[tanks_A[grid[xx][yy] - 10].pos_x + 2][tanks_A[grid[xx][yy] - 10].pos_y]--;
						occ[tanks_A[grid[xx][yy] - 10].pos_x + 2][tanks_A[grid[xx][yy] - 10].pos_y + 1]--;
					}
					if(ch == 'R') {
						occ[tanks_A[grid[xx][yy] - 10].pos_x][tanks_A[grid[xx][yy] - 10].pos_y + 2]--;
						occ[tanks_A[grid[xx][yy] - 10].pos_x + 1][tanks_A[grid[xx][yy] - 10].pos_y + 2]--;
					}
					if(ch == 'L') {
						occ[tanks_A[grid[xx][yy] - 10].pos_x][tanks_A[grid[xx][yy] - 10].pos_y - 1]--;
						occ[tanks_A[grid[xx][yy] - 10].pos_x + 1][tanks_A[grid[xx][yy] - 10].pos_y - 1]--;
					}
					fill(grid_pos_A[grid[xx][yy] - 10].first, grid_pos_A[grid[xx][yy] - 10].second,
						 grid_pos_A[grid[xx][yy] - 10].first + 11, grid_pos_A[grid[xx][yy] - 10].second + 11, 0);
				}
			}
		}
	}
	for(int j = bullets.size() - 1; j >= 0; --j) {
		if(!is[j]) {
			continue;
		}
		bullets.erase(bullets.begin() + j);
		grid_pos_bullet.erase(grid_pos_bullet.begin() + j);
	}
}

void move_tanks(vector<tank> tanks, vector<bool> wont_move, vector<bool> deny, vector<pair<int, string> > m, int doil) {
	for(int i = 0; i < 4; i++) {
		if(deny[i] || wont_move[i] || doil % (6 / (m[i].second.size() - 1))) {
			continue;
		}
		if(m[i].first > 0) {
			if(m[i].second[1] == 'U') {
				fill(grid_pos_A[i].first + 11, grid_pos_A[i].second, grid_pos_A[i].first + 11, grid_pos_A[i].second + 11, 0);
				fill(grid_pos_A[i].first - 1, grid_pos_A[i].second, grid_pos_A[i].first - 1, grid_pos_A[i].second + 11, tanks_A[i].id);
				grid_pos_A[i].first--;
				if(grid_pos_A[i].first % 6 == 0) {
					tanks_A[i].pos_x = grid_pos_A[i].first / 6;
					score_A += 2;
					diffA += 2;
					if(color[tanks_A[i].pos_x][tanks_A[i].pos_y] == 1) {
							score_A--;
					}
					if(color[tanks_A[i].pos_x][tanks_A[i].pos_y] == -1) {
							score_B--;
					}
					if(color[tanks_A[i].pos_x][tanks_A[i].pos_y + 1] == 1) {
							score_A--;
					}
					if(color[tanks_A[i].pos_x][tanks_A[i].pos_y + 1] == -1) {
							score_B--;
					}
					color[tanks_A[i].pos_x][tanks_A[i].pos_y] = 1;
					color[tanks_A[i].pos_x][tanks_A[i].pos_y + 1] = 1;
					map[tanks_A[i].pos_x][tanks_A[i].pos_y] = i + 10;
					map[tanks_A[i].pos_x][tanks_A[i].pos_y + 1] = i + 10;
					map[tanks_A[i].pos_x + 2][tanks_A[i].pos_y] = 0;
					map[tanks_A[i].pos_x + 2][tanks_A[i].pos_y + 1] = 0;
					occ[tanks_A[i].pos_x + 2][tanks_A[i].pos_y]--;
					occ[tanks_A[i].pos_x + 2][tanks_A[i].pos_y + 1]--;
				}
				if(grid_pos_A[i].first % 6 == 5) {
					occ[tanks_A[i].pos_x - 1][tanks_A[i].pos_y]++;
					occ[tanks_A[i].pos_x - 1][tanks_A[i].pos_y + 1]++;
				}
			}
			if(m[i].second[1] == 'D') {
				fill(grid_pos_A[i].first, grid_pos_A[i].second, grid_pos_A[i].first, grid_pos_A[i].second + 11, 0);
				fill(grid_pos_A[i].first + 12, grid_pos_A[i].second, grid_pos_A[i].first + 12, grid_pos_A[i].second + 11, tanks_A[i].id);
				grid_pos_A[i].first++;
				if(grid_pos_A[i].first % 6 == 0) {
					tanks_A[i].pos_x = grid_pos_A[i].first / 6;
					score_A += 2;
					diffA += 2;
					if(color[tanks_A[i].pos_x + 1][tanks_A[i].pos_y] == 1) {
							score_A--;
					}
					if(color[tanks_A[i].pos_x + 1][tanks_A[i].pos_y] == -1) {
							score_B--;
					}
					if(color[tanks_A[i].pos_x + 1][tanks_A[i].pos_y + 1] == 1) {
							score_A--;
					}
					if(color[tanks_A[i].pos_x + 1][tanks_A[i].pos_y + 1] == -1) {
							score_B--;
					}
					color[tanks_A[i].pos_x + 1][tanks_A[i].pos_y] = 1;
					color[tanks_A[i].pos_x + 1][tanks_A[i].pos_y + 1] = 1;
					map[tanks_A[i].pos_x + 1][tanks_A[i].pos_y] = i + 10;
					map[tanks_A[i].pos_x + 1][tanks_A[i].pos_y + 1] = i + 10;
					map[tanks_A[i].pos_x - 1][tanks_A[i].pos_y] = 0;
					map[tanks_A[i].pos_x - 1][tanks_A[i].pos_y + 1] = 0;
					occ[tanks_A[i].pos_x - 1][tanks_A[i].pos_y]--;
					occ[tanks_A[i].pos_x - 1][tanks_A[i].pos_y + 1]--;
				}
				if(grid_pos_A[i].first % 6 == 1) {
					occ[tanks_A[i].pos_x + 2][tanks_A[i].pos_y]++;
					occ[tanks_A[i].pos_x + 2][tanks_A[i].pos_y + 1]++;
				}
			}
			if(m[i].second[1] == 'R') {
				fill(grid_pos_A[i].first, grid_pos_A[i].second, grid_pos_A[i].first + 11, grid_pos_A[i].second, 0);
				fill(grid_pos_A[i].first, grid_pos_A[i].second + 12, grid_pos_A[i].first + 11, grid_pos_A[i].second + 12, tanks_A[i].id);
				grid_pos_A[i].second++;
				if(grid_pos_A[i].second % 6 == 0) {
					tanks_A[i].pos_y = grid_pos_A[i].second / 6;
					score_A += 2;
					diffA += 2;
					if(color[tanks_A[i].pos_x][tanks_A[i].pos_y + 1] == 1) {
							score_A--;
					}
					if(color[tanks_A[i].pos_x][tanks_A[i].pos_y + 1] == -1) {
							score_B--;
					}
					if(color[tanks_A[i].pos_x + 1][tanks_A[i].pos_y + 1] == 1) {
							score_A--;
					}
					if(color[tanks_A[i].pos_x + 1][tanks_A[i].pos_y + 1] == -1) {
							score_B--;
					}
					color[tanks_A[i].pos_x][tanks_A[i].pos_y + 1] = 1;
					color[tanks_A[i].pos_x + 1][tanks_A[i].pos_y + 1] = 1;
					map[tanks_A[i].pos_x][tanks_A[i].pos_y + 1] = i + 10;
					map[tanks_A[i].pos_x + 1][tanks_A[i].pos_y + 1] = i + 10;
					map[tanks_A[i].pos_x][tanks_A[i].pos_y - 1] = 0;
					map[tanks_A[i].pos_x + 1][tanks_A[i].pos_y - 1] = 0;
					occ[tanks_A[i].pos_x][tanks_A[i].pos_y - 1]--;
					occ[tanks_A[i].pos_x + 1][tanks_A[i].pos_y - 1]--;
				}
				if(grid_pos_A[i].second % 6 == 1) {
					occ[tanks_A[i].pos_x][tanks_A[i].pos_y + 2]++;
					occ[tanks_A[i].pos_x + 1][tanks_A[i].pos_y + 2]++;
				}
			}
			if(m[i].second[1] == 'L') {
				fill(grid_pos_A[i].first, grid_pos_A[i].second + 11, grid_pos_A[i].first + 11, grid_pos_A[i].second + 11, 0);
				fill(grid_pos_A[i].first, grid_pos_A[i].second - 1, grid_pos_A[i].first + 11, grid_pos_A[i].second - 1, tanks_A[i].id);
				grid_pos_A[i].second--;
				if(grid_pos_A[i].second % 6 == 0) {
					tanks_A[i].pos_y = grid_pos_A[i].second / 6;
					score_A += 2;
					diffA += 2;
					if(color[tanks_A[i].pos_x][tanks_A[i].pos_y] == 1) {
						score_A--;
					}
					if(color[tanks_A[i].pos_x][tanks_A[i].pos_y] == -1) {
							score_B--;
					}
					if(color[tanks_A[i].pos_x + 1][tanks_A[i].pos_y] == 1) {
							score_A--;
					}
					if(color[tanks_A[i].pos_x + 1][tanks_A[i].pos_y] == -1) {
							score_B--;
					}
					color[tanks_A[i].pos_x][tanks_A[i].pos_y] = 1;
					color[tanks_A[i].pos_x + 1][tanks_A[i].pos_y] = 1;
					map[tanks_A[i].pos_x][tanks_A[i].pos_y] = i + 10;
					map[tanks_A[i].pos_x + 1][tanks_A[i].pos_y] = i + 10;
					map[tanks_A[i].pos_x][tanks_A[i].pos_y + 2] = 0;
					map[tanks_A[i].pos_x + 1][tanks_A[i].pos_y + 2] = 0;
					occ[tanks_A[i].pos_x][tanks_A[i].pos_y + 2]--;
					occ[tanks_A[i].pos_x + 1][tanks_A[i].pos_y + 2]--;
				}
				if(grid_pos_A[i].second % 6 == 5) {
					occ[tanks_A[i].pos_x][tanks_A[i].pos_y - 1]++;
					occ[tanks_A[i].pos_x + 1][tanks_A[i].pos_y - 1]++;
				}
			}
		} else {
			if(m[i].second[1] == 'U') {
				fill(grid_pos_B[i].first + 11, grid_pos_B[i].second, grid_pos_B[i].first + 11, grid_pos_B[i].second + 11, 0);
				fill(grid_pos_B[i].first - 1, grid_pos_B[i].second, grid_pos_B[i].first - 1, grid_pos_B[i].second + 11, tanks_B[i].id);
				grid_pos_B[i].first--;
				if(grid_pos_B[i].first % 6 == 0) {
					tanks_B[i].pos_x = grid_pos_B[i].first / 6;
					score_B += 2;
					diffB += 2;
					if(color[tanks_B[i].pos_x][tanks_B[i].pos_y] == 1) {
							score_A--;
					}
					if(color[tanks_B[i].pos_x][tanks_B[i].pos_y] == -1) {
							score_B--;
					}
					if(color[tanks_B[i].pos_x][tanks_B[i].pos_y + 1] == 1) {
							score_A--;
					}
					if(color[tanks_B[i].pos_x][tanks_B[i].pos_y + 1] == -1) {
							score_B--;
					}
					color[tanks_B[i].pos_x][tanks_B[i].pos_y] = -1;
					color[tanks_B[i].pos_x][tanks_B[i].pos_y + 1] = -1;
					map[tanks_B[i].pos_x][tanks_B[i].pos_y] = -i - 10;
					map[tanks_B[i].pos_x][tanks_B[i].pos_y + 1] = -i - 10;
					map[tanks_B[i].pos_x + 2][tanks_B[i].pos_y] = 0;
					map[tanks_B[i].pos_x + 2][tanks_B[i].pos_y + 1] = 0;
					occ[tanks_B[i].pos_x + 2][tanks_B[i].pos_y]--;
					occ[tanks_B[i].pos_x + 2][tanks_B[i].pos_y + 1]--;
				}
				if(grid_pos_B[i].first % 6 == 5) {
					occ[tanks_B[i].pos_x - 1][tanks_B[i].pos_y]++;
					occ[tanks_B[i].pos_x - 1][tanks_B[i].pos_y + 1]++;
				}
			}
			if(m[i].second[1] == 'D') {
				fill(grid_pos_B[i].first, grid_pos_B[i].second, grid_pos_B[i].first, grid_pos_B[i].second + 11, 0);
				fill(grid_pos_B[i].first + 12, grid_pos_B[i].second, grid_pos_B[i].first + 12, grid_pos_B[i].second + 11, tanks_B[i].id);
				grid_pos_B[i].first++;
				if(grid_pos_B[i].first % 6 == 0) {
					tanks_B[i].pos_x = grid_pos_B[i].first / 6;
					score_B += 2;
					diffB += 2;
					if(color[tanks_B[i].pos_x + 1][tanks_B[i].pos_y] == 1) {
						score_A--;
					}
					if(color[tanks_B[i].pos_x + 1][tanks_B[i].pos_y] == -1) {
						score_B--;
					}
					if(color[tanks_B[i].pos_x + 1][tanks_B[i].pos_y + 1] == 1) {
						score_A--;
					}
					if(color[tanks_B[i].pos_x + 1][tanks_B[i].pos_y + 1] == -1) {
						score_B--;
					}
					color[tanks_B[i].pos_x + 1][tanks_B[i].pos_y] = -1;
					color[tanks_B[i].pos_x + 1][tanks_B[i].pos_y + 1] = -1;
					map[tanks_B[i].pos_x + 1][tanks_B[i].pos_y] = -i - 10;
					map[tanks_B[i].pos_x + 1][tanks_B[i].pos_y + 1] = -i - 10;
					map[tanks_B[i].pos_x - 1][tanks_B[i].pos_y] = 0;
					map[tanks_B[i].pos_x - 1][tanks_B[i].pos_y + 1] = 0;
					occ[tanks_B[i].pos_x - 1][tanks_B[i].pos_y]--;
					occ[tanks_B[i].pos_x - 1][tanks_B[i].pos_y + 1]--;
				}
				if(grid_pos_B[i].first % 6 == 1) {
					occ[tanks_B[i].pos_x + 2][tanks_B[i].pos_y]++;
					occ[tanks_B[i].pos_x + 2][tanks_B[i].pos_y + 1]++;
				}
			}
			if(m[i].second[1] == 'R') {
				fill(grid_pos_B[i].first, grid_pos_B[i].second, grid_pos_B[i].first + 11, grid_pos_B[i].second, 0);
				fill(grid_pos_B[i].first, grid_pos_B[i].second + 12, grid_pos_B[i].first + 11, grid_pos_B[i].second + 12, tanks_B[i].id);
				grid_pos_B[i].second++;
				if(grid_pos_B[i].second % 6 == 0) {
					tanks_B[i].pos_y = grid_pos_B[i].second / 6;
					score_B += 2;
					diffB += 2;
					if(color[tanks_B[i].pos_x][tanks_B[i].pos_y + 1] == 1) {
							score_A--;
					}
					if(color[tanks_B[i].pos_x][tanks_B[i].pos_y + 1] == -1) {
							score_B--;
					}
					if(color[tanks_B[i].pos_x + 1][tanks_B[i].pos_y + 1] == 1) {
							score_A--;
					}
					if(color[tanks_B[i].pos_x + 1][tanks_B[i].pos_y + 1] == -1) {
							score_B--;
					}
					color[tanks_B[i].pos_x][tanks_B[i].pos_y + 1] = -1;
					color[tanks_B[i].pos_x + 1][tanks_B[i].pos_y + 1] = -1;
					map[tanks_B[i].pos_x][tanks_B[i].pos_y + 1] = -i - 10;
					map[tanks_B[i].pos_x + 1][tanks_B[i].pos_y + 1] = -i - 10;
					map[tanks_B[i].pos_x][tanks_B[i].pos_y - 1] = 0;
					map[tanks_B[i].pos_x + 1][tanks_B[i].pos_y - 1] = 0;
					occ[tanks_B[i].pos_x][tanks_B[i].pos_y - 1]--;
					occ[tanks_B[i].pos_x + 1][tanks_B[i].pos_y - 1]--;
				}
				if(grid_pos_B[i].second % 6 == 1) {
					occ[tanks_B[i].pos_x][tanks_B[i].pos_y + 2]++;
					occ[tanks_B[i].pos_x + 1][tanks_B[i].pos_y + 2]++;
				}
			}
			if(m[i].second[1] == 'L') {
				fill(grid_pos_B[i].first, grid_pos_B[i].second + 11, grid_pos_B[i].first + 11, grid_pos_B[i].second + 11, 0);
				fill(grid_pos_B[i].first, grid_pos_B[i].second - 1, grid_pos_B[i].first + 11, grid_pos_B[i].second - 1, tanks_B[i].id);
				grid_pos_B[i].second--;
				if(grid_pos_B[i].second % 6 == 0) {
					tanks_B[i].pos_y = grid_pos_B[i].second / 6;
					score_B += 2;
					diffB += 2;
					if(color[tanks_B[i].pos_x][tanks_B[i].pos_y] == 1) {
						score_A--;
					}
					if(color[tanks_B[i].pos_x][tanks_B[i].pos_y] == -1) {
						score_B--;
					}
					if(color[tanks_B[i].pos_x + 1][tanks_B[i].pos_y] == 1) {
						score_A--;
					}
					if(color[tanks_B[i].pos_x + 1][tanks_B[i].pos_y] == -1) {
						score_B--;
					}
					color[tanks_B[i].pos_x][tanks_B[i].pos_y] = -1;
					color[tanks_B[i].pos_x + 1][tanks_B[i].pos_y] = -1;
					map[tanks_B[i].pos_x][tanks_B[i].pos_y] = -i - 10;
					map[tanks_B[i].pos_x + 1][tanks_B[i].pos_y] = -i - 10;
					map[tanks_B[i].pos_x][tanks_B[i].pos_y + 2] = 0;
					map[tanks_B[i].pos_x + 1][tanks_B[i].pos_y + 2] = 0;
					occ[tanks_B[i].pos_x][tanks_B[i].pos_y + 2]--;
					occ[tanks_B[i].pos_x + 1][tanks_B[i].pos_y + 2]--;
				}
				if(grid_pos_B[i].second % 6 == 5) {
					occ[tanks_B[i].pos_x][tanks_B[i].pos_y - 1]++;
					occ[tanks_B[i].pos_x + 1][tanks_B[i].pos_y - 1]++;
				}
			}
		}
	}
}

void move_bullet(int doil) {
	for(int i = 0; i < bullets.size(); i++) {
		bullet &it = bullets[i];
		if(it.direction == 'U') {
			it.pos_x -= (doil % 6 == 0);
			--grid_pos_bullet[i].first;
		}
		if(it.direction == 'D') {
			it.pos_x += (doil % 6 == 0);
			++grid_pos_bullet[i].first;
		}
		if(it.direction == 'L') {
			it.pos_y -= (doil % 6 == 0);
			--grid_pos_bullet[i].second;
		}
		if(it.direction == 'R') {
			it.pos_y += (doil % 6 == 0);
			++grid_pos_bullet[i].second;
		}
	}
}

void move(vector<pair<int, string>> &m1, vector<pair<int, string>> &m2) {
	// change map
	// change color
	// change score
	// extract map
	vector<bool> does_hit(16);

	vector<bool> deny_A(4, false);
	vector<bool> deny_B(4, false);

	vector<bool> wont_move_A(4, false);
	vector<bool> wont_move_B(4, false);

	for(int i = 0; i < 4; i++) {
		if(tanks_A[i].alive && m1[i].second[0] == 'F') {
			char ch = m1[i].second[1];
			tanks_A[i].supply--;
			bullets.push_back({ BID++,
								m1[i].first,
								tanks_A[i].pos_x + (ch == 'D' ? 2 : (ch == 'R' || ch == 'L')),
							   	tanks_A[i].pos_y + (ch == 'R' ? 2 : (ch == 'U' || ch == 'D')),
								tanks_A[i].damage,
								ch});
			grid_pos_bullet.push_back(make_pair(bullets.back().pos_x * 6, bullets.back().pos_y * 6));
		}
		if(tanks_B[i].alive && m2[i].second[0] == 'F') {
			char ch = m2[i].second[1];
			tanks_B[i].supply--;
			bullets.push_back({ BID++,
								m2[i].first,
								tanks_B[i].pos_x + (ch == 'D' ? 2 : (ch == 'R' || ch == 'L')),
							   	tanks_B[i].pos_y + (ch == 'R' ? 2 : (ch == 'U' || ch == 'D')),
							   	tanks_B[i].damage,
							   	ch});
			grid_pos_bullet.push_back(make_pair(bullets.back().pos_x * 6, bullets.back().pos_y * 6));
		}
	}

	hitting_bullets(does_hit);
	eliminating_bullets(does_hit, m1, m2);

	for(int doil = 1; doil <= 36; doil++) {
		does_hit = vector<bool>(16, false);
		move_bullet(doil);
		deny_tanks(deny_A, 1, m1);
		deny_tanks(deny_B, -1, m2);
		//add to occupy
		for(int ind = 0; ind < 4; ind++) {
			if(deny_A[ind]) {
				continue;
			}
			if(grid_pos_A[ind].first % 6 != 0 || grid_pos_A[ind].second % 6 != 0) {
				continue;
			}
			char ch = m1[ind].second[1];
			int change_x[2], change_y[2];
			//finding the change of coordinate
			if(ch == 'U') {
				change_x[0] = -1;
				change_y[0] = 0;
				change_x[1] = -1;
				change_y[1] = 1;
			}
			if (ch == 'D') {
				change_x[0] = 2;
				change_y[0] = 0;
				change_x[1] = 2;
				change_y[1] = 1;
			}
			if (ch == 'L') {
				change_x[0] = 0;
				change_y[0] = -1;
				change_x[1] = 1;
				change_y[1] = -1;
			}
			if (ch == 'R') {
				change_x[0] = 0;
				change_y[0] = 2;
				change_x[1] = 1;
				change_y[1] = 2;
			}
			occ[tanks_A[ind].pos_x+change_x[0]][tanks_A[ind].pos_y+change_y[0]]++;
			occ[tanks_A[ind].pos_x+change_x[1]][tanks_A[ind].pos_y+change_y[1]]++;
		}
		for(int ind = 0; ind < 4; ind++) {
			if(deny_B[ind]) {
				continue;
			}
			if(grid_pos_B[ind].first % 6 != 0 || grid_pos_B[ind].second % 6 != 0) {
				continue;
			}
			char ch = m2[ind].second[1];
			int change_x[2], change_y[2];
			//finding the change of coordinate
			if(ch == 'U') {
				change_x[0] = -1;
				change_y[0] = 0;
				change_x[1] = -1;
				change_y[1] = 1;
			}
			if (ch == 'D') {
				change_x[0] = 2;
				change_y[0] = 0;
				change_x[1] = 2;
				change_y[1] = 1;
			}
			if (ch == 'L') {
				change_x[0] = 0;
				change_y[0] = -1;
				change_x[1] = 1;
				change_y[1] = -1;
			}
			if (ch == 'R') {
				change_x[0] = 0;
				change_y[0] = 2;
				change_x[1] = 1;
				change_y[1] = 2;
			}
			occ[tanks_B[ind].pos_x+change_x[0]][tanks_B[ind].pos_y+change_y[0]]++;
			occ[tanks_B[ind].pos_x+change_x[1]][tanks_B[ind].pos_y+change_y[1]]++;
		}
		for(int ind = 0; ind < 4; ind++) {
			if(deny_A[ind]) {
				continue;
			}
			if(grid_pos_A[ind].first % 6 != 0 || grid_pos_A[ind].second % 6 != 0) {
				continue;
			}
			char ch = m1[ind].second[1];
			int change_x[2], change_y[2];
			int Change_x[2], Change_y[2];
			//finding the change of coordinate
			if(ch == 'U') {
				change_x[0] = -1;
				change_y[0] = 0;
				change_x[1] = -1;
				change_y[1] = 1;

				Change_x[0] = 0;
				Change_x[1] = 0;
				Change_y[0] = 0;
				Change_y[1] = 1;
			}
			if (ch == 'D') {
				change_x[0] = 2;
				change_y[0] = 0;
				change_x[1] = 2;
				change_y[1] = 1;

				Change_x[0] = 1;
				Change_x[1] = 1;
				Change_y[0] = 0;
				Change_y[1] = 1;
			}
			if (ch == 'L') {
				change_x[0] = 0;
				change_y[0] = -1;
				change_x[1] = 1;
				change_y[1] = -1;

				Change_x[0] = 0;
				Change_x[1] = 1;
				Change_y[0] = 0;
				Change_y[1] = 0;
			}
			if (ch == 'R') {
				change_x[0] = 0;
				change_y[0] = 2;
				change_x[1] = 1;
				change_y[1] = 2;

				Change_x[0] = 0;
				Change_x[1] = 1;
				Change_y[0] = 1;
				Change_y[1] = 1;
			}
			if(occ[tanks_A[ind].pos_x + change_x[0]][tanks_A[ind].pos_y + change_y[0]] >= 3) {
				wont_move_A[ind] = 1;
				continue;
			}
			if(occ[tanks_A[ind].pos_x + change_x[1]][tanks_A[ind].pos_y + change_y[1]] >= 3) {
				wont_move_A[ind] = 1;
				continue;
			}
			if(occ[tanks_A[ind].pos_x + change_x[0]][tanks_A[ind].pos_y + change_y[0]] == 1 && occ[tanks_A[ind].pos_x + change_x[1]][tanks_A[ind].pos_y + change_y[1]] == 1) {
				continue;
			}
			int change_X[2], change_Y[2];
			if(occ[tanks_A[ind].pos_x + change_x[0]][tanks_A[ind].pos_y + change_y[0]] == 2) {
				if(occ[tanks_A[ind].pos_x + Change_x[0]][tanks_A[ind].pos_y + Change_y[0]] != 2) {
					wont_move_A[ind] = 1;
					continue;
				}
				int xp = (tanks_A[ind].pos_x+change_x[0]) * 6, yp = (tanks_A[ind].pos_y+change_y[0]) * 6;
				if(ch == 'U') {
					if(grid[xp][yp] != 0) {
						wont_move_A[ind] = 1;
						continue;
					}
					change_X[0] = -1;
					change_Y[0] = -1;
					change_X[1] = 0;
					change_Y[1] = -1;
				}
				if (ch == 'D') {
					if(grid[xp + 5][yp] != 0) {
						wont_move_A[ind] = 1;
						continue;
					}
					change_X[0] = 1;
					change_Y[0] = -1;
					change_X[1] = 2;
					change_Y[1] = -1;
				}
				if (ch == 'L') { 
					if(grid[xp][yp] != 0) {
						wont_move_A[ind] = 1;
						continue;
					}
					change_X[0] = -1;
					change_Y[0] = -1;
					change_X[1] = -1;
					change_Y[1] = 0;
				}
				if (ch == 'R') {
					if(grid[xp][yp + 5] != 0) {
						wont_move_A[ind] = 1;
						continue;
					}
					change_X[0] = -1;
					change_Y[0] = 1;
					change_X[1] = -1;
					change_Y[1] = 2;
				}
				int tmp_x1 = tanks_A[ind].pos_x + change_X[0];
				int tmp_x2 = tanks_A[ind].pos_x + change_X[1];
				int tmp_y1 = tanks_A[ind].pos_y + change_Y[0];
				int tmp_y2 = tanks_A[ind].pos_y + change_Y[1];
				if(map[tmp_x1][tmp_y1] != map[tmp_x2][tmp_y2]) {
					wont_move_A[ind] = 1;
					continue;
				}
			}
			if(occ[tanks_A[ind].pos_x + change_x[1]][tanks_A[ind].pos_y + change_y[1]] == 2) {
				if(occ[tanks_A[ind].pos_x + Change_x[1]][tanks_A[ind].pos_y + Change_y[1]] != 2) {
					wont_move_A[ind] = 1;
					continue;
				}
				int xp = (tanks_A[ind].pos_x+change_x[1]) * 6, yp = (tanks_A[ind].pos_y+change_y[1]) * 6;
				if(ch == 'U') {
					if(grid[xp][yp + 5] != 0) {
						wont_move_A[ind] = 1;
						continue;
					}
					change_X[0] = -1;
					change_Y[0] = 2;
					change_X[1] = 0;
					change_Y[1] = 2;
				}
				if (ch == 'D') {
					if(grid[xp + 5][yp + 5] != 0) {
						wont_move_A[ind] = 1;
						continue;
					}
					change_X[0] = 1;
					change_Y[0] = 2;
					change_X[1] = 2;
					change_Y[1] = 2;
				}
				if (ch == 'L') {
					if(grid[xp + 5][yp] != 0) {
						wont_move_A[ind] = 1;
						continue;
					}
					change_X[0] = 2;
					change_Y[0] = -1;
					change_X[1] = 2;
					change_Y[1] = 0;
				}
				if (ch == 'R') {
					if(grid[xp + 5][yp + 5] != 0) {
						wont_move_A[ind] = 1;
						continue;
					}
					change_X[0] = 2;
					change_Y[0] = 1;
					change_X[1] = 2;
					change_Y[1] = 2;
				}
				int tmp_x1 = tanks_A[ind].pos_x + change_X[0];
				int tmp_x2 = tanks_A[ind].pos_x + change_X[1];
				int tmp_y1 = tanks_A[ind].pos_y + change_Y[0];
				int tmp_y2 = tanks_A[ind].pos_y + change_Y[1];
				if(map[tmp_x1][tmp_y1] != map[tmp_x2][tmp_y2]) {
					wont_move_A[ind] = 1;
					continue;
				}
			}
		}
		for(int ind = 0; ind < 4; ind++) {
			if(deny_B[ind]) {
				continue;
			}
			if(grid_pos_B[ind].first % 6 != 0 || grid_pos_B[ind].second % 6 != 0) {
				continue;
			}
			char ch = m2[ind].second[1];
			int change_x[2], change_y[2];
			int Change_x[2], Change_y[2];
			//finding the change of coordinate
			if(ch == 'U') {
				change_x[0] = -1;
				change_y[0] = 0;
				change_x[1] = -1;
				change_y[1] = 1;

				Change_x[0] = 0;
				Change_x[1] = 0;
				Change_y[0] = 0;
				Change_y[1] = 1;
			}
			if (ch == 'D') {
				change_x[0] = 2;
				change_y[0] = 0;
				change_x[1] = 2;
				change_y[1] = 1;

				Change_x[0] = 1;
				Change_x[1] = 1;
				Change_y[0] = 0;
				Change_y[1] = 1;
			}
			if (ch == 'L') {
				change_x[0] = 0;
				change_y[0] = -1;
				change_x[1] = 1;
				change_y[1] = -1;

				Change_x[0] = 0;
				Change_x[1] = 1;
				Change_y[0] = 0;
				Change_y[1] = 0;
			}
			if (ch == 'R') {
				change_x[0] = 0;
				change_y[0] = 2;
				change_x[1] = 1;
				change_y[1] = 2;
				
				Change_x[0] = 0;
				Change_x[1] = 1;
				Change_y[0] = 1;
				Change_y[1] = 1;
			}
			if(occ[tanks_B[ind].pos_x + change_x[0]][tanks_B[ind].pos_y + change_y[0]] >= 3) {
					wont_move_B[ind] = 1;
					continue;
			}
			if(occ[tanks_B[ind].pos_x + change_x[1]][tanks_B[ind].pos_y + change_y[1]] >= 3) {
					wont_move_B[ind] = 1;
					continue;
			}
			if(occ[tanks_B[ind].pos_x + change_x[0]][tanks_B[ind].pos_y + change_y[0]] == 1 && occ[tanks_B[ind].pos_x + change_x[1]][tanks_B[ind].pos_y + change_y[1]] == 1) {
					continue;
			}
			int change_X[2], change_Y[2];
			if(occ[tanks_B[ind].pos_x + change_x[0]][tanks_B[ind].pos_y + change_y[0]] == 2) {
				if(occ[tanks_B[ind].pos_x + Change_x[0]][tanks_B[ind].pos_y + Change_y[0]] != 2) {
					wont_move_B[ind] = 1;
					continue;
				}
				int xp = (tanks_B[ind].pos_x+change_x[0]) * 6, yp = (tanks_B[ind].pos_y+change_y[0]) * 6;
				if(ch == 'U') {
					if(grid[xp][yp] != 0) {
						wont_move_B[ind] = 1;
						continue;
					}
					change_X[0] = -1;
					change_Y[0] = -1;
					change_X[1] = 0;
					change_Y[1] = -1;
				}
				if (ch == 'D') {
					if(grid[xp + 5][yp] != 0) {
						wont_move_B[ind] = 1;
						continue;
					}
					change_X[0] = 1;
					change_Y[0] = -1;
					change_X[1] = 2;
					change_Y[1] = -1;
				}
				if (ch == 'L') { 
					if(grid[xp][yp] != 0) {
						wont_move_B[ind] = 1;
						continue;
					}
					change_X[0] = -1;
					change_Y[0] = -1;
					change_X[1] = -1;
					change_Y[1] = 0;
				}
				if (ch == 'R') {
					if(grid[xp][yp + 5] != 0) {
						wont_move_B[ind] = 1;
						continue;
					}
					change_X[0] = -1;
					change_Y[0] = 1;
					change_X[1] = -1;
					change_Y[1] = 2;
				}
				int tmp_x1 = tanks_B[ind].pos_x + change_X[0];
				int tmp_x2 = tanks_B[ind].pos_x + change_X[1];
				int tmp_y1 = tanks_B[ind].pos_y + change_Y[0];
				int tmp_y2 = tanks_B[ind].pos_y + change_Y[1];
				if(map[tmp_x1][tmp_y1] != map[tmp_x2][tmp_y2]) {
					wont_move_B[ind] = 1;
					continue;
				}
			}
			if(occ[tanks_B[ind].pos_x + change_x[1]][tanks_B[ind].pos_y + change_y[1]] == 2) {
				if(occ[tanks_B[ind].pos_x + Change_x[1]][tanks_B[ind].pos_y + Change_y[1]] != 2) {
					wont_move_B[ind] = 1;
					continue;
				}
				int xp = (tanks_B[ind].pos_x+change_x[1]) * 6, yp = (tanks_B[ind].pos_y+change_y[1]) * 6;
				if(ch == 'U') {
					if(grid[xp][yp + 5] != 0) {
						wont_move_B[ind] = 1;
						continue;
					}
					change_X[0] = -1;
					change_Y[0] = 2;
					change_X[1] = 0;
					change_Y[1] = 2;
				}
				if (ch == 'D') {
					if(grid[xp + 5][yp + 5] != 0) {
						wont_move_B[ind] = 1;
						continue;
					}
					change_X[0] = 1;
					change_Y[0] = 2;
					change_X[1] = 2;
					change_Y[1] = 2;
				}
				if (ch == 'L') {
					if(grid[xp + 5][yp] != 0) {
						wont_move_B[ind] = 1;
						continue;
					}
					change_X[0] = 2;
					change_Y[0] = -1;
					change_X[1] = 2;
					change_Y[1] = 0;
				}
				if (ch == 'R') {
					if(grid[xp + 5][yp + 5] != 0) {
						wont_move_B[ind] = 1;
						continue;
					}
					change_X[0] = 2;
					change_Y[0] = 1;
					change_X[1] = 2;
					change_Y[1] = 2;
				}
				int tmp_x1 = tanks_B[ind].pos_x + change_X[0];
				int tmp_x2 = tanks_B[ind].pos_x + change_X[1];
				int tmp_y1 = tanks_B[ind].pos_y + change_Y[0];
				int tmp_y2 = tanks_B[ind].pos_y + change_Y[1];
				if(map[tmp_x1][tmp_y1] != map[tmp_x2][tmp_y2]) {
					wont_move_B[ind] = 1;
					continue;
				}
			}
		}
		for(int ind = 0; ind < 4; ind++) {
			if(deny_A[ind]) {
				continue;
			}
			if(grid_pos_A[ind].first % 6 != 0 || grid_pos_A[ind].second % 6 != 0) {
				continue;
			}
			char ch = m1[ind].second[1];
			int change_x[2], change_y[2];
			//finding the change of coordinate
			if(ch == 'U') {
				change_x[0] = -1;
				change_y[0] = 0;
				change_x[1] = -1;
				change_y[1] = 1;
			}
			if (ch == 'D') {
				change_x[0] = 2;
				change_y[0] = 0;
				change_x[1] = 2;
				change_y[1] = 1;
			}
			if (ch == 'L') {
				change_x[0] = 0;
				change_y[0] = -1;
				change_x[1] = 1;
				change_y[1] = -1;
			}
			if (ch == 'R') {
				change_x[0] = 0;
				change_y[0] = 2;
				change_x[1] = 1;
				change_y[1] = 2;
			}
			occ[tanks_A[ind].pos_x+change_x[0]][tanks_A[ind].pos_y+change_y[0]]--;
			occ[tanks_A[ind].pos_x+change_x[1]][tanks_A[ind].pos_y+change_y[1]]--;
		}
		for(int ind = 0; ind < 4; ind++) {
			if(deny_B[ind]) {
				continue;
			}
			if(grid_pos_B[ind].first % 6 != 0 || grid_pos_B[ind].second % 6 != 0) {
				continue;
			}
			char ch = m2[ind].second[1];
			int change_x[2], change_y[2];
			//finding the change of coordinate
			if(ch == 'U') {
				change_x[0] = -1;
				change_y[0] = 0;
				change_x[1] = -1;
				change_y[1] = 1;
			}
			if (ch == 'D') {
				change_x[0] = 2;
				change_y[0] = 0;
				change_x[1] = 2;
				change_y[1] = 1;
			}
			if (ch == 'L') {
				change_x[0] = 0;
				change_y[0] = -1;
				change_x[1] = 1;
				change_y[1] = -1;
			}
			if (ch == 'R') {
				change_x[0] = 0;
				change_y[0] = 2;
				change_x[1] = 1;
				change_y[1] = 2;
			}
			occ[tanks_B[ind].pos_x+change_x[0]][tanks_B[ind].pos_y+change_y[0]]--;
			occ[tanks_B[ind].pos_x+change_x[1]][tanks_B[ind].pos_y+change_y[1]]--;
		}
		//moving the tanks
		move_tanks(tanks_A, wont_move_A, deny_A, m1, doil);
		move_tanks(tanks_B, wont_move_B, deny_B, m2, doil);
		hitting_bullets(does_hit);
		eliminating_bullets(does_hit, m1, m2);
	}
}

bool init() {
	cin >> ID >> H >> W;

	// resize
	map.resize(H, vector<int>(W));
	color.resize(H, vector<int>(W));
	grid.resize(6 * H, vector<int>(6 * W));
	occ.resize(H, vector<int>(W));

	// read map from file
	for(int i = 0; i < H; i++) {
		for(int j = 0; j < W; j++) {
			cin >> map[i][j];
			if(map[i][j]) {
				occ[i][j] = 1;
				fill(i * 6, j * 6, i * 6 + 5, j * 6 + 5, map[i][j]);
				if(map[i][j] >= 10 && map[i][j] < 100) {
						color[i][j] = 1;
				}
				if(map[i][j] <= -10 && map[i][j] > -100) {
						color[i][j] = -1;
				}
			}
		}
	}

	cin >> rounds;
	
	// read tank type information
	for(int i = 1; i <= 3; i++) {
		cin >> health[i] >> speed[i] >> damage[i];
	}

	// init tanks
	vector<int> type_A = init_A(ID);
	vector<int> type_B = init_B(ID);

	bool valid_A = ((int)type_A.size() == num_tanks);
	bool valid_B = ((int)type_B.size() == num_tanks);

	for(int i = 0; i < (int)type_A.size(); i++) {
		valid_A &= (type_A[i] <= 3 && type_A[i] >= 1);
        if (type_A[i] < 1) type_A[i] = 1;
        if (type_A[i] > 3) type_A[i] = 3;
	}

	for(int i = 0; i < (int)type_B.size(); i++) {
		valid_B &= (type_B[i] <= 3 && type_B[i] >= 1);
        if (type_B[i] < 1) type_B[i] = 1;
        if (type_B[i] > 3) type_B[i] = 3;
	}
    // cout << valid_A << ' ' << valid_B << endl;
    // cout << type_A.size() << ' ' << type_B.size() << endl;

	int xxx = is_valid(valid_A, valid_B);
	if (xxx != 100) {
		if (xxx  == 0) {
			score_A = 0;
			score_B = 0;
		}
		if (xxx == -1) {
			score_A = 0;
			score_B = max(score_B, 100);
		}
		if (xxx == 1) {
			score_B = 0;
			score_A = max(score_A, 100);
		}
        rounds = 0;
        initFailed = true;
	}

	vector<pair<int, int>> pos_A(num_tanks);
	vector<pair<int, int>> pos_B(num_tanks);
	for(int i = 0; i < H - 1; i++) {
		for(int j = 0; j < W - 1; ++j) {
			int val = map[i][j];
			if(val >= 10 && val <= 13 && map[i + 1][j + 1] == val) {
				pos_A[val - 10] = make_pair(i, j);
				grid_pos_A[val - 10] = make_pair(i * 6, j * 6);
			}
			if(val >= -13 && val <= -10 && map[i + 1][j + 1] == val) {
				pos_B[val * (-1) - 10] = make_pair(i, j);
				grid_pos_B[val * (-1) - 10] = make_pair(i * 6, j * 6);
			}
			if(val == 100 && map[i + 1][j + 1] == val) {
				flag_A.pos_x = i;
				flag_A.pos_y = j;
			}
			if(val == -100 && map[i + 1][j + 1] == val) {
				flag_B.pos_x = i;
				flag_B.pos_y = j;
			}
		}
	}

	for(int i = 0; i < num_tanks; i++) {
		int id_A = i + 10;
		int id_B = -i - 10;
		tanks_A.push_back({id_A,
						   type_A[i],
						   pos_A[i].first,
						   pos_A[i].second,
						   health[type_A[i]],
						   speed[type_A[i]],
						   damage[type_A[i]],
						   2,
						   true});

		tanks_B.push_back({id_B,
						   type_B[i],
						   pos_B[i].first,
						   pos_B[i].second,
						   health[type_B[i]],
						   speed[type_B[i]],
						   damage[type_B[i]],
						   2,
						   true});
	}
    return !initFailed;
}

void print_result() {
	if (score_A > score_B) cout << "You Winner!\n";
	else cout << "You Loser!\n";
	cout << "- Your Score: " << score_A << endl;
	cout << "- BOT Score:  " << score_B << endl;
}

void play() {
	int cnt = 0;
	for(int _ = 0; _ < rounds; _++) {
		diffA = 0;
		diffB = 0;
		cnt++;
		vector<pair<char, pair<int, int>>> as, bs;
		for (auto b : bullets) {
			if (b.id < 0) bs.push_back({b.direction, {b.pos_x, b.pos_y}});
			else as.push_back({rot_char(b.direction), {H - b.pos_x, W - b.pos_y}});
		}
		m_A = move_A(map, color, bs);
		auto color1 = color;
		for (auto &row : color) for (auto &item : row) item *= -1;
		m_B = move_B(rot_map(map), color1, as);
		for (auto &i : m_B) {
			i.first += 10;
		}
		for (auto &i : m_A) {
			i.first += 10;
		}
		
		vector<tank> tempA = tanks_A;
		vector<tank> tempB = tanks_B;

		bool valid_A = validate(m_A, 1);
		bool valid_B = validate(m_B, -1);

		int xxx = is_valid(valid_A, valid_B);
		if (xxx != 100) {
			if (xxx  == 0) {
				score_A = 0;
				score_B = 0;
			}
			if (xxx == -1) {
				score_A = 0;
				score_B = max(score_B, 100);
			}
			if (xxx == 1) {
				score_B = 0;
				score_A = max(score_A, 100);
			}
			return;
		}

		m_B = rot_move(m_B);
		// move
		move(m_A, m_B);
		for (int i = 0; i < num_tanks; i++) {
			if (m_A[i].second == "G" || m_A[i].second[0] == 'F') continue;
			char c = m_A[i].second[1];
			int dis = abs(tempA[i].pos_x - tanks_A[i].pos_x);
			dis += abs(tempA[i].pos_y - tanks_A[i].pos_y);
			m_A[i].second = "G";
			while (dis--) m_A[i].second.push_back(c);
		}

		for (int i = 0; i < num_tanks; i++) {
			if (m_B[i].second == "G" || m_B[i].second[0] == 'F') continue;
			char c = m_B[i].second[1];
			int dis = abs(tempB[i].pos_x - tanks_B[i].pos_x);
			dis += abs(tempB[i].pos_y - tanks_B[i].pos_y);
			m_B[i].second = "G";
			while (dis--) m_B[i].second.push_back(c);
		}

		if(!flag_A.alive || !flag_B.alive) {
			if (!flag_B.alive) {
				score_A = 9999;
			}
			if (!flag_A.alive) {
				score_B = 9999;
			}
			break;
		}
		int aa = 0, bb = 0;
		for (int i = 0; i < 4; i++) {
			if (tanks_A[i].alive) aa++;
			if (tanks_B[i].alive) bb++;
		}
		if (aa == 0 || bb == 0) {
			if (aa > 0) {
				score_A = 9999;
			}
			if (bb > 0) {
				score_B = 9999;
			}
			break;
		}
	}
	string message = "";
	int error = 0;
    if (initFailed) {
        error = 1;
        message = "Initialization failed!";
        cout << message << endl;
        exit(1);
    }
	int winner = 0;
	if (score_A > score_B) winner = 1;
	else if (score_B > score_A) winner = 2;
}

int main(int argc, char** argv) {
	srand(time(0));
	id1 = 1;
	id2 = 2;
	srand(time(0));
	bool res = init();
    play();
    print_result();
	return 0;
}
