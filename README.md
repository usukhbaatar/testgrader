# Test Grader

Hackateen Tank test grader.

## Compile (Win | Linux | Mac)

```sh
g++ --std=c++11 grader.cpp team1.cpp bot.cpp
```

## Run (Linux | Mac)

```sh
./a.out < map1.txt
```

`map1.txt` -н оронд `map2.txt`, `map3.txt`, `map4.txt` байж болно.

## Run (Win)

```sh
.\a.exe < map1.txt
```

## Development

`team1.cpp` дотор байрлах `init_A`, `move_A` функцуудыг хэрэгжүүлнэ.

```cpp
vector<int> init_A(int map_id):
```

- map_id: Map - ийн дугаар, утга нь 1, 2 эсвэл 3 байна.
  > Return: Вектор буцаана. Урт нь яг 4 байна. Элементийн утга нь бүр танкын төрлүүдийг заах `1, 2, 3` утгуудыг аль нэг байна.

```cpp
vector<pair<int, string>> move_A(
	vector<vector<int>> M,
	vector<vector<int>> C,
	vector<pair<char, pair<int, int>>> B):
```

M: `HxW` хэмжээтэй 2D вектор. Энэ нь яг одоогийн идэвхитэй газрын зураг.
C: `HxW` хэмжээтэй 2D вектор. Энэ нь тухайн нүдний өнгийг илэрхийлнэ. 1 Team1 эзэлсэн, -1: Team2 эдэлсэн, 0: одоогоор хэн ч эзлээгүй байгаа. Та үргэлж Team1 байна.
B: Team2-ийн талбай дээр байгаа сумнууд, чиглэлтэйгээ. B[i].first: Чиглэл ('LUDR'), B[i].second.first - мөрнийн дугаар, B[i].second.second - баганын дугаар. Сумыг тунайх сумны байрлаж буй байрлалын зүүн дээд булангийн нүдний координатаар дүрсэлнэ.
Return: Яг 4 урттай `vector<pair<int, string>> v` байна. V[i].first: Танкын дугаар [0, 1, 2, 3], v[i].second: Тухайн танкны гүйцэтгэх коммнад.
