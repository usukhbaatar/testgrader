# Test Grader
Hackateen Tank test grader.

## Compile (Win | Linux | Mac)
```sh
g++ --std=c++11 grader.cpp team1.cpp bot.cpp
```

## Run (Linux | Mac)
```sh
./a.out < map3.txt
```

## Run (Win)
```sh
.\a.exe < map3.txt
```

## Development
`team1.cpp` дотор байрлах `init_A`, `move_A` функцуудыг хэрэгжүүлнэ.

```cpp
vector<int> init_A():
```
Вектор буцаана. Урт нь яг 4 байна. Элементийн утга нь бүр танкын төрлүүдийг заах `1, 2, 3` утгуудыг аль нэг байна.
 