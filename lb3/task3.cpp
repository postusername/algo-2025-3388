#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

int64_t levenshteinDistance(const std::string& source, const std::string& target) {
    const size_t m = source.length();
    const size_t n = target.length();
    
    // Создаем матрицу dp[i][j], где i и j - длины префиксов строк
    std::vector<std::vector<int64_t>> dp(m + 1, std::vector<int64_t>(n + 1, 0));
    
    // Инициализация первой строки и столбца
    for (size_t i = 0; i <= m; i++) {
        dp[i][0] = i; // Стоимость удаления i символов
    }
    for (size_t j = 0; j <= n; j++) {
        dp[0][j] = j; // Стоимость вставки j символов
    }
    
    // Заполняем матрицу
    for (size_t i = 1; i <= m; i++) {
        for (size_t j = 1; j <= n; j++) {
            if (source[i-1] == target[j-1]) {
                dp[i][j] = dp[i-1][j-1];
            } else {
                // Выбираем минимальную операцию:
                // 1. Замена символа (dp[i-1][j-1] + 1)
                // 2. Удаление символа из source (dp[i-1][j] + 1)
                // 3. Вставка символа из target (dp[i][j-1] + 1)
                dp[i][j] = std::min({
                    dp[i-1][j-1] + 1, // замена
                    dp[i-1][j] + 1,   // удаление
                    dp[i][j-1] + 1    // вставка
                });
            }
        }
    }
    
    return dp[m][n];
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    std::string s, t;
    std::getline(std::cin, s);
    std::getline(std::cin, t);
    
    std::cout << levenshteinDistance(s, t) << std::endl;
    
    return 0;
}
