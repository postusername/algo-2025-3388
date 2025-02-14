#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <cstdint>
#include <stack>


using namespace std;

struct Square {
    int x, y, size;
};

void** matrix2d_new( size_t esize, size_t idim, size_t jdim ) {
    size_t const ptrs_size = sizeof(void*) * idim;
    size_t const row_size = esize * jdim;
    void **const rows = (void **)malloc(ptrs_size);
    for ( size_t i = 0; i < idim; ++i )
        rows[i] = malloc( row_size );
    return rows;
}

#define createMask(b) 1 << (31 - b)

struct Grid {
    int N;
    int* occupied;  // Массив целых чисел для представления строк
    int minX, minY, maxX, maxY;
    uint32_t bit_masks[32]= {0};
    uint32_t bit_prefixes[33] = {0};

    Grid(int n) : N(n), minX(n), minY(n), maxX(-1), maxY(-1) {
        occupied = (int*)malloc(sizeof(int) * N);  // Массив для хранения строк
        memset(occupied, 0, sizeof(int) * N);     // Инициализация всех строк нулями
        
        for (int b = 0; b < 32; b++) {
            bit_masks[b] = createMask(b);
        }
        bit_prefixes[0] = 0;
        for (int p = 1; p < 33; p++)
            bit_prefixes[p] = bit_prefixes[p - 1] + bit_masks[p - 1];
    }

    // Конструктор копирования
    Grid(const Grid& other) : N(other.N), minX(other.minX), minY(other.minY), 
                             maxX(other.maxX), maxY(other.maxY) {
        // Выделяем новую память
        occupied = (int*)malloc(sizeof(int) * N);
        // Копируем содержимое
        memcpy(occupied, other.occupied, sizeof(int) * N);
        // Копируем массивы
        memcpy(bit_masks, other.bit_masks, sizeof(bit_masks));
        memcpy(bit_prefixes, other.bit_prefixes, sizeof(bit_prefixes));
    }

    // Деструктор
    ~Grid() {
        free(occupied);
    }

    // Вспомогательный метод для обращения к ячейке (x, y)
    bool isOccupiedCell(int x, int y) {
        return (occupied[x] & bit_masks[y]) != 0;  // Проверяем, занят ли бит
    }

    bool isOccupied(int x, int y_start, int y_end) const {
        int bit_mask = bit_prefixes[y_end] ^ bit_prefixes[y_start];
        return (occupied[x] & bit_mask) != 0;  // Проверяем, заняты ли биты
    }

    // Вспомогательный метод для установки ячейки (x, y)
    void setOccupied(int x, int y_start, int y_end) {
        int bit_mask = bit_prefixes[y_end] ^ bit_prefixes[y_start];
        occupied[x] |= bit_mask;  // Устанавливаем биты
    }

    // Вспомогательный метод для сброса ячейки (x, y)
    void clearOccupied(int x, int y_start, int y_end) {
        int bit_mask = bit_prefixes[y_end] - bit_prefixes[y_start];
        occupied[x] &= ~bit_mask;  // Сбрасываем биты
    }

    bool canPlace(int x, int y, int size) const {
        if (x + size > N || y + size > N) return false;
        for (int i = x; i < x + size; ++i) {
            if (isOccupied(i, y, y + size)) return false;
        }
        return true;
    }

    void placeSquare(int x, int y, int size) {
        for (int i = x; i < x + size; ++i) {
            setOccupied(i, y, y + size);
        }
    }

    void removeSquare(int x, int y, int size) {
        for (int i = x; i < x + size; ++i) {
            clearOccupied(i, y, y + size);
        }
    }

    bool isRemainingSquare() {
        minX = N + 1, minY = N + 1, maxX = -1, maxY = -1;
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (!isOccupiedCell(i, j)) {
                    minX = min(minX, i);
                    minY = min(minY, j);
                    maxX = max(maxX, i);
                    maxY = max(maxY, j);
                }
            }
        }

        int side = maxX - minX + 1;
        if (side != maxY - minY + 1) return false;

        return canPlace(minX, minY, side);
    }
};

int maxSquareSize(int N) {
    return (N == 1) ? 1 : min(N - 1, N / 2 + 1);
}

struct State {
    Grid grid;
    vector<Square> currentResult;
};

void find_solution(Grid initial_grid,
                   int& minSquares, 
                   int target_count, 
                   vector<Square> &bestResult) {
    stack<State> states;
    states.push({initial_grid, {}});
    int best_side = maxSquareSize(initial_grid.N);
    initial_grid.placeSquare(0, 0, best_side);
    initial_grid.placeSquare(0, best_side, initial_grid.N - best_side);
    initial_grid.placeSquare(best_side, 0, initial_grid.N - best_side);
    states.push({initial_grid, {{1, 1, best_side},
                                {1, best_side + 1, initial_grid.N - best_side},
                                {best_side + 1, 1, initial_grid.N - best_side}}});

    while (!states.empty()) {
        State state = states.top();
        states.pop();

        Grid grid = state.grid;
        vector<Square> currentResult = state.currentResult;
        int current_minSquares = (bestResult.empty()) ? minSquares : bestResult.size();

        // >= minSquares ??
        if (currentResult.size() > current_minSquares || currentResult.size() > target_count) continue;

        int firstEmptyX = -1, firstEmptyY = -1;
        for (int i = 0; i < grid.N; ++i) {
            for (int j = 0; j < grid.N; ++j) {
                if (!grid.isOccupiedCell(i, j)) {
                    firstEmptyX = i;
                    firstEmptyY = j;
                    break;
                }
            }
            if (firstEmptyX != -1) break;
        }

        if (currentResult.size() == target_count) {
            if (firstEmptyX == -1) {
                minSquares = currentResult.size();
                bestResult = currentResult;
                return; //return ??
            }
            continue;
        }
         

        if (firstEmptyX == grid.N - 1 || firstEmptyY == grid.N - 1) {
            grid.placeSquare(firstEmptyX, firstEmptyY, 1);
            currentResult.push_back({firstEmptyX + 1, firstEmptyY + 1, 1});
            //cout << currentResult.size() << " " << firstEmptyX << " " << firstEmptyY << " "  << "one" << endl;
            states.push({grid, currentResult});

            grid.removeSquare(firstEmptyX, firstEmptyY, 1);
            currentResult.pop_back();
        } else {
            if (grid.isRemainingSquare()) {
                int remainingSize = grid.maxX - grid.minX + 1;
                if (remainingSize < grid.N) {
                    currentResult.push_back({grid.minX + 1, grid.minY + 1, remainingSize});
                    minSquares = currentResult.size();
                    bestResult = currentResult;
                    return; //return ??
                }
            }

            for (int size = 1; size < maxSquareSize(grid.N) + 1; ++size) {
                if (grid.canPlace(firstEmptyX, firstEmptyY, size)) {
                    grid.placeSquare(firstEmptyX, firstEmptyY, size);
                    currentResult.push_back({firstEmptyX + 1, firstEmptyY + 1, size});
                    //cout << currentResult.size() << " " << firstEmptyX << " " << firstEmptyY << " " << size << endl;
                    states.push({grid, currentResult});

                    grid.removeSquare(firstEmptyX, firstEmptyY, size);
                    currentResult.pop_back();
                }
            }
        }
    }
}


int lowerBound(int N) {
    return round( 1.09130775e-05 * N * N * N * N * N + 
                 -8.94639101e-04 * N * N * N * N +
                  2.77548151e-02 * N * N * N +
                 -4.14022709e-01 * N * N +
                  3.28286267e+00 * N +
                 -1.00446771e+00);
}

void solve(int N, Grid &grid, vector<Square> &bestResult) {
    int minSquares = N * N + 1; // naive approx
    int poly_approx = lowerBound(N);
    for (int target_count = poly_approx; target_count <= minSquares; ++target_count) {
        find_solution(grid, minSquares, target_count, bestResult);
    }
}

int smallestDivisor(int N) {
    for (int d = 2; d * d <= N; ++d) {
        if (N % d == 0) return d;
    }
    return N;
}

int main() {
    int N;
    cin >> N;

    int d = smallestDivisor(N);
    int scale = N / d;

    Grid grid(d);
    vector<Square> smallResult;
    solve(d, grid, smallResult);

    vector<Square> finalResult;
    for (const auto& sq : smallResult) {
        finalResult.push_back({sq.x * scale - (scale - 1), sq.y * scale - (scale - 1), sq.size * scale});
    }

    cout << finalResult.size() << endl;
    for (const auto& sq : finalResult) {
        cout << sq.x << " " << sq.y << " " << sq.size << endl;
    }

    return 0;
}
