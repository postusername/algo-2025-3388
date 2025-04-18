#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

std::pair<int64_t, std::string> getEditSequence(const std::string& A, const std::string& B, 
                                               int64_t replCost, int64_t insCost, int64_t delCost) {
    const size_t m = A.length();
    const size_t n = B.length();
    
    // dp[i][j] - минимальная стоимость преобразования префикса A[0..i-1] в префикс B[0..j-1]
    std::vector<std::vector<int64_t>> dp(m + 1, std::vector<int64_t>(n + 1, 0));
    // operations[i][j] хранит операцию, которая привела к оптимальному результату
    std::vector<std::vector<char>> operations(m + 1, std::vector<char>(n + 1, ' '));
    
    // Инициализация первой строки и столбца
    for (size_t i = 0; i <= m; i++) {
        dp[i][0] = i * delCost;
        if (i > 0) operations[i][0] = 'D';
    }
    for (size_t j = 0; j <= n; j++) {
        dp[0][j] = j * insCost;
        if (j > 0) operations[0][j] = 'I';
    }
    
    // Заполнение таблицы dp и operations
    for (size_t i = 1; i <= m; i++) {
        for (size_t j = 1; j <= n; j++) {
            if (A[i-1] == B[j-1]) {
                dp[i][j] = dp[i-1][j-1];
                operations[i][j] = 'M';
            } else {
                int64_t replace_cost = dp[i-1][j-1] + replCost;
                int64_t insert_cost = dp[i][j-1] + insCost;
                int64_t delete_cost = dp[i-1][j] + delCost;
                
                dp[i][j] = replace_cost;
                operations[i][j] = 'R';
                
                if (insert_cost < dp[i][j]) {
                    dp[i][j] = insert_cost;
                    operations[i][j] = 'I';
                }
                if (delete_cost < dp[i][j]) {
                    dp[i][j] = delete_cost;
                    operations[i][j] = 'D';
                }
            }
        }
    }
    
    // Восстановление последовательности операций
    std::string sequence;
    size_t i = m, j = n;
    
    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && (operations[i][j] == 'M' || operations[i][j] == 'R')) {
            sequence = operations[i][j] + sequence;
            i--; j--;
        }
        else if (i > 0 && operations[i][j] == 'D') {
            sequence = 'D' + sequence;
            i--;
        }
        else if (j > 0) {
            sequence = 'I' + sequence;
            j--;
        }
    }
    
    return {dp[m][n], sequence};
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    int64_t replCost, insCost, delCost;
    std::string A, B;
    
    // Ввод данных
    std::cin >> replCost >> insCost >> delCost;
    std::cin >> A >> B;
    
    // Получение результата
    auto [cost, sequence] = getEditSequence(A, B, replCost, insCost, delCost);
    
    // Вывод результата
    std::cout << sequence << std::endl;
    std::cout << A << std::endl;
    std::cout << B << std::endl;
    
    return 0;
} 