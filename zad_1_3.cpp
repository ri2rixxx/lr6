#include <iostream>
#include <vector>
#include <unistd.h>
using namespace std;

const int HEIGHT = 25;
const int WIDTH = 25;
const int STEPS = 100;
const int DELAY = 2000; // 100ms

using Grid = vector<vector<int>>;

// Replicator pattern (примерная форма, 9x9)
vector<pair<int,int>> replicator = {
    {1,3}, {1,4}, {2,2}, {2,4}, {3,1}, {3,2}, {3,5}, {4,1}, {4,3}, {4,5}, {5,2}, {5,4},
    {6,3}
};

// Glider (паразит)
vector<pair<int,int>> parasite = {
    {10,10}, {11,11}, {12,9}, {12,10}, {12,11}
};

void initializePatterns(Grid& grid) {
    for (auto& row : grid) fill(row.begin(), row.end(), 0);
    for (auto [r, c] : replicator)
        if (r < HEIGHT && c < WIDTH)
            grid[r][c] = 1;
    for (auto [r, c] : parasite)
        if (r < HEIGHT && c < WIDTH)
            grid[r][c] = 1;
}

void printGrid(const Grid& g) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    for (const auto& row : g) {
        for (int cell : row)
            cout << (cell ? 'O' : ' ');
        cout << '\n';
    }
}

int countNeighbors(const Grid& g, int x, int y) {
    int cnt = 0;
    for (int dx = -1; dx <= 1; ++dx)
        for (int dy = -1; dy <= 1; ++dy)
            if (dx != 0 || dy != 0)
                cnt += g[(x + dx + HEIGHT) % HEIGHT][(y + dy + WIDTH) % WIDTH];
    return cnt;
}

void updateGrid(Grid& g) {
    Grid newGrid = g;
    for (int i = 0; i < HEIGHT; ++i)
        for (int j = 0; j < WIDTH; ++j) {
            int n = countNeighbors(g, i, j);
            if (g[i][j])
                newGrid[i][j] = (n == 2 || n == 3);
            else
                newGrid[i][j] = (n == 3);
        }
    g = newGrid;
}

int main() {
    cout << "Game of Life: Replicator + Parasite\nНажмите Enter...";
    cin.get();

    Grid grid(HEIGHT, vector<int>(WIDTH));
    initializePatterns(grid);

    for (int step = 0; step < STEPS; ++step) {
        printGrid(grid);
        updateGrid(grid);
        usleep(DELAY);
    }

    cout << "Симуляция завершена.\n";
    return 0;
}

