#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <omp.h>

using namespace std;

void drawHorizontalLine(int width) {
    std::cout << "+";
    for (int i = 0; i < width; ++i) {
        std::cout << "-";
    }
    std::cout << "+" << std::endl;
}

void drawVerticalLine(int width, vector<vector<int>>& grid, int row) {
    std::cout << "|";
    for (int j = 0; j < width; ++j) {
        if (grid[row][j] == 1) {
            std::cout << "*";
        }
        else {
            std::cout << " ";
        }
    }
    std::cout << "|" << std::endl;
}

void frame(int width, int height, vector<vector<int>>& grid) {
    drawHorizontalLine(width);
    for (int i = 0; i < height; ++i) {
        drawVerticalLine(width, grid, i);
    }
    drawHorizontalLine(width);
}

void print(vector<vector<int>> grid, int iteration) {
    cout << "\x1B[2J\x1B[H";
    cout << "Iteration" << iteration << endl;
    frame(grid[0].size(), grid.size(), grid);
}

int main()
{
    int rows = 50, cols = 100;
    vector<vector<int>> grid;
    for (int i = 0; i < rows; i++) {
        grid.push_back({});
        for (int j = 0; j < cols; j++) {
            grid[i].push_back(rand() % 2);
        }
    }

    int iteration = 1;
    print(grid, iteration);
    this_thread::sleep_for(chrono::seconds(3));

    while (true) {
        auto begin_time = chrono::steady_clock::now();
        vector<vector<int>> new_grid = grid;
#pragma omp parallel for
        for (int i = 0; i < rows; i++) {
#pragma omp parallel for
            for (int j = 0; j < cols; j++) {
                int row_above = (i + 1) % rows;
                int row_below = (i - 1 + rows) % rows;
                int col_left = (j - 1 + cols) % cols;
                int col_right = (j + 1) % cols;
                int sum = grid[row_above][col_left] + grid[row_above][j] + grid[row_above][col_right] +
                    grid[i][col_left] + grid[i][col_right] +
                    grid[row_below][col_left] + grid[row_below][j] + grid[row_below][col_right];
                if (grid[i][j] == 1) {
                    if (sum < 2 || sum > 3) {
                        new_grid[i][j] = 0;
                    }
                    else {
                        new_grid[i][j] = 1;
                    }
                }
                else {
                    if (sum == 3) {
                        new_grid[i][j] = 1;
                    }
                    else {
                        new_grid[i][j] = 0;
                    }
                }
            }
        }
        grid = new_grid;

        auto end_time = chrono::steady_clock::now();
        auto elapsed_ns = chrono::duration_cast<chrono::nanoseconds>(end_time - begin_time);
        print(grid, ++iteration);
        cout << elapsed_ns.count() << "ns" << endl;
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    return 0;
}

