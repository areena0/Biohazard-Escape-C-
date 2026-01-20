
#include <iostream> 
#include <vector> 
#include <cstdlib> 
#include <ctime> 

using namespace std;

const int MAP_SIZE = 10;
const char TILE_EMPTY = '.';
const char HERO = 'P';
const char BADGUY = 'V';
const char WALLBLOCK = '#';
const char GOAL = 'E';
const char BOOST = 'M';

void setupMap(char mapGrid[MAP_SIZE][MAP_SIZE]);
void drawMap(char mapGrid[MAP_SIZE][MAP_SIZE]);
void spawnBadguy(char mapGrid[MAP_SIZE][MAP_SIZE]);
void spawnWalls(char mapGrid[MAP_SIZE][MAP_SIZE]);
void spawnBoosts(char mapGrid[MAP_SIZE][MAP_SIZE]);
bool canStep(int r, int c, char mapGrid[MAP_SIZE][MAP_SIZE]);
void badguySpread(char mapGrid[MAP_SIZE][MAP_SIZE]);
void moveHero(char mapGrid[MAP_SIZE][MAP_SIZE], int& hr, int& hc);
void updateFogMask(char mapGrid[MAP_SIZE][MAP_SIZE], char

    fogMask[MAP_SIZE][MAP_SIZE], int hr, int hc);

int hp = 10;
bool isGameOver = false;
bool isWin = false;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int main() {

    srand(time(0));
    char mapGrid[MAP_SIZE][MAP_SIZE];
    char fogMask[MAP_SIZE][MAP_SIZE];
    int heroR, heroC;
    setupMap(mapGrid);
    for (int r = 0; r < MAP_SIZE; r++) {
        for (int c = 0; c < MAP_SIZE; c++) {
            if (mapGrid[r][c] == HERO) {
                heroR = r;
                heroC = c;
            }
        }
    }

    spawnWalls(mapGrid);
    spawnBoosts(mapGrid);
    for (int k = 0; k < 3; k++) {
        spawnBadguy(mapGrid);
    }

    cout << "--- BIOHAZARD ESCAPE ---" << endl;
    while (!isGameOver && !isWin) {
        clearScreen();
        updateFogMask(mapGrid, fogMask, heroR, heroC);
        drawMap(fogMask);
        cout << "Energy: " << hp << endl;
        cout << "Controls: 1=Up, 2=Down, 3=Left, 4=Right" << endl;
        if (hp <= 0) {
            cout << "Out of energy!" << endl;
            isGameOver = true;
            break;
        }

        moveHero(mapGrid, heroR, heroC);
        if (mapGrid[heroR][heroC] == GOAL) {
            isWin = true;
        }
        else if (mapGrid[heroR][heroC] == BADGUY) {
            isGameOver = true;
        }

        if (!isGameOver && !isWin) {
            badguySpread(mapGrid);
            if (mapGrid[heroR][heroC] == BADGUY) {
                isGameOver = true;
            }
        }
    }

    if (isWin) cout << "YOU WIN! You escaped the laboratory." << endl;
    else cout << "GAME OVER!" << endl;
    return 0;
}

void setupMap(char mapGrid[MAP_SIZE][MAP_SIZE]) {
    for (int r = 0; r < MAP_SIZE; r++) {
        for (int c = 0; c < MAP_SIZE; c++) {
            mapGrid[r][c] = TILE_EMPTY;
        }
    }

    mapGrid[MAP_SIZE - 1][0] = HERO;
    mapGrid[0][MAP_SIZE - 1] = GOAL;
}
void drawMap(char mapGrid[MAP_SIZE][MAP_SIZE]) {
    cout << "  ";
    for (int c = 0; c < MAP_SIZE; c++) cout << c << " ";
    cout << endl;
    for (int r = 0; r < MAP_SIZE; r++) {
        cout << r << " ";
        for (int c = 0; c < MAP_SIZE; c++) {
            cout << mapGrid[r][c] << " ";
        }
        cout << endl;
    }
}

void spawnBadguy(char mapGrid[MAP_SIZE][MAP_SIZE]) {
    while (true) {
        int r = rand() % MAP_SIZE;
        int c = rand() % MAP_SIZE;
        if (mapGrid[r][c] == TILE_EMPTY) {
            mapGrid[r][c] = BADGUY;
            break;
        }
    }
}

void spawnWalls(char mapGrid[MAP_SIZE][MAP_SIZE]) {
    for (int i = 0; i < 8; i++) {
        int r = rand() % MAP_SIZE;
        int c = rand() % MAP_SIZE;
        if (mapGrid[r][c] == TILE_EMPTY) mapGrid[r][c] = WALLBLOCK;
    }
}

void spawnBoosts(char mapGrid[MAP_SIZE][MAP_SIZE]) {
    for (int i = 0; i < 2; i++) {
        int r = rand() % MAP_SIZE;
        int c = rand() % MAP_SIZE;
        if (mapGrid[r][c] == TILE_EMPTY) mapGrid[r][c] = BOOST;
    }
}

bool canStep(int r, int c, char mapGrid[MAP_SIZE][MAP_SIZE]) {
    if (r < 0 || r >= MAP_SIZE || c < 0 || c >= MAP_SIZE) return false;
    if (mapGrid[r][c] == WALLBLOCK) return false;
    return true;
}



void moveHero(char mapGrid[MAP_SIZE][MAP_SIZE], int& hr, int& hc) {
    int cmd;
    cout << "Enter Move: ";
    cin >> cmd;
    int nr = hr, nc = hc;
    switch (cmd) {
    case 1: nr--; break;
    case 2: nr++; break;
    case 3: nc--; break;
    case 4: nc++; break;
    default: return;
    }

    if (canStep(nr, nc, mapGrid)) {
        if (mapGrid[nr][nc] == BOOST) {
            hp += 3;
            mapGrid[nr][nc] = TILE_EMPTY;
        }

        if (mapGrid[hr][hc] == HERO) mapGrid[hr][hc] = TILE_EMPTY;
        hp--;
        hr = nr;
        hc = nc;
        if (mapGrid[hr][hc] == TILE_EMPTY) mapGrid[hr][hc] = HERO;
    }
    else {
        cout << "Blocked!" << endl;
    }
}

void badguySpread(char mapGrid[MAP_SIZE][MAP_SIZE]) {
    vector<pair<int, int>> spreadOptions;
    for (int r = 0; r < MAP_SIZE; r++) {
        for (int c = 0; c < MAP_SIZE; c++) {
            if (mapGrid[r][c] == BADGUY) {
                int dr[] = { -1, 1, 0, 0 };
                int dc[] = { 0, 0, -1, 1 };
                for (int k = 0; k < 4; k++) {
                    int nr = r + dr[k];
                    int nc = c + dc[k];
                    if (canStep(nr, nc, mapGrid) && mapGrid[nr][nc] !=
                        BADGUY && mapGrid[nr][nc] != GOAL) {
                        spreadOptions.push_back({ nr, nc });
                    }
                }
            }
        }
    }

    if (!spreadOptions.empty()) {
        int pick = rand() % spreadOptions.size();
        mapGrid[spreadOptions[pick].first][spreadOptions[pick].second] =
            BADGUY;
    }
}

void updateFogMask(char mapGrid[MAP_SIZE][MAP_SIZE], char
    fogMask[MAP_SIZE][MAP_SIZE], int hr, int hc) {
    for (int r = 0; r < MAP_SIZE; r++) {
        for (int c = 0; c < MAP_SIZE; c++) {
            fogMask[r][c] = '?';
        }
    }

    for (int r = hr - 2; r <= hr + 2; r++) {
        for (int c = hc - 2; c <= hc + 2; c++) {
            if (r >= 0 && r < MAP_SIZE && c >= 0 && c < MAP_SIZE) {
                fogMask[r][c] = mapGrid[r][c];
            }
        }
    }
}
