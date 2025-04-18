#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

int64_t minEditDistance(const std::string& A, const std::string& B, 
                       int64_t replCost, int64_t insCost, int64_t delCost) {
    const size_t m = A.length();
    const size_t n = B.length();
    
    // dp[i][j] - минимальная стоимость преобразования префикса A[0..i-1] в префикс B[0..j-1]
    std::vector<std::vector<int64_t>> dp(m + 1, std::vector<int64_t>(n + 1, 0));
    
    // Инициализация первой строки и столбца
    for (size_t i = 0; i <= m; i++) {
        dp[i][0] = i * delCost;
    }
    for (size_t j = 0; j <= n; j++) {
        dp[0][j] = j * insCost;
    }
    
    // Заполнение таблицы dp
    for (size_t i = 1; i <= m; i++) {
        for (size_t j = 1; j <= n; j++) {
            if (A[i-1] == B[j-1]) {
                dp[i][j] = dp[i-1][j-1];
            } else {
                // Минимум из трех операций:
                // 1. Замена (replace)
                // 2. Вставка (insert)
                // 3. Удаление (delete)
                dp[i][j] = std::min({
                    dp[i-1][j-1] + replCost,  // replace
                    dp[i][j-1] + insCost,     // insert
                    dp[i-1][j] + delCost      // delete
                });
            }
        }
    }
    
    return dp[m][n];
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    int64_t replCost, insCost, delCost;
    std::string A, B;
    
    // Ввод данных
    std::cin >> replCost >> insCost >> delCost;
    std::cin >> A >> B;
    
    // Вывод результата
    std::cout << minEditDistance(A, B, replCost, insCost, delCost) << std::endl;
    
    return 0;
} 