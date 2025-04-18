#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

void printDP(const std::vector<std::vector<int64_t>>& dp, const std::string& A, const std::string& B) {
    std::cout << "\nТекущая матрица DP:\n";
    std::cout << std::setw(4) << " ";
    std::cout << std::setw(4) << "#";
    for (char c : B) {
        std::cout << std::setw(4) << c;
    }
    std::cout << "\n";
    
    for (size_t i = 0; i <= A.length(); i++) {
        std::cout << std::setw(4) << (i == 0 ? std::string("#") : std::string(1, A[i-1]));
        for (size_t j = 0; j <= B.length(); j++) {
            std::cout << std::setw(4) << dp[i][j];
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

int64_t minEditDistance(const std::string& A, const std::string& B,
                       int64_t replCost, int64_t insCost, int64_t delCost,
                       char specialRepl, int64_t specialReplCost,
                       char specialDel, int64_t specialDelCost) {
    const size_t m = A.length();
    const size_t n = B.length();
    
    std::vector<std::vector<int64_t>> dp(m + 1, std::vector<int64_t>(n + 1, 0));
    
    std::cout << "Инициализация первого столбца (удаления):\n";
    for (size_t i = 0; i <= m; i++) {
        dp[i][0] = 0;
        for (size_t k = 0; k < i; k++) {
            int64_t cost = (A[k] == specialDel) ? specialDelCost : delCost;
            dp[i][0] += cost;
            if (i > 0) {
                std::cout << "Удаление " << A[k] << " стоит " << cost << "\n";
            }
        }
    }
    
    std::cout << "\nИнициализация первой строки (вставки):\n";
    for (size_t j = 0; j <= n; j++) {
        dp[0][j] = j * insCost;
        if (j > 0) {
            std::cout << "Вставка " << B[j-1] << " стоит " << insCost << "\n";
        }
    }
    
    printDP(dp, A, B);
    
    for (size_t i = 1; i <= m; i++) {
        for (size_t j = 1; j <= n; j++) {
            std::cout << "\nРассматриваем A[" << i-1 << "]=" << A[i-1] 
                     << " и B[" << j-1 << "]=" << B[j-1] << ":\n";
            
            if (A[i-1] == B[j-1]) {
                dp[i][j] = dp[i-1][j-1];
                std::cout << "Символы совпадают, стоимость 0\n";
            } else {
                int64_t replaceCost = replCost;
                if (B[j-1] == specialRepl) {
                    replaceCost = specialReplCost;
                    std::cout << "Особая замена на " << specialRepl << " стоит " << specialReplCost << "\n";
                }
                
                int64_t deleteCost = delCost;
                if (A[i-1] == specialDel) {
                    deleteCost = specialDelCost;
                    std::cout << "Особое удаление " << specialDel << " стоит " << specialDelCost << "\n";
                }
                
                int64_t replOp = dp[i-1][j-1] + replaceCost;
                int64_t insOp = dp[i][j-1] + insCost;
                int64_t delOp = dp[i-1][j] + deleteCost;
                
                std::cout << "Варианты:\n";
                std::cout << "1. Замена: " << dp[i-1][j-1] << " + " << replaceCost << " = " << replOp << "\n";
                std::cout << "2. Вставка: " << dp[i][j-1] << " + " << insCost << " = " << insOp << "\n";
                std::cout << "3. Удаление: " << dp[i-1][j] << " + " << deleteCost << " = " << delOp << "\n";
                
                dp[i][j] = std::min({replOp, insOp, delOp});
                std::cout << "Выбрана минимальная стоимость: " << dp[i][j] << "\n";
            }
            
            printDP(dp, A, B);
        }
    }
    
    return dp[m][n];
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    int64_t replCost, insCost, delCost;
    std::string A, B;
    char specialRepl;
    int64_t specialReplCost;
    char specialDel;
    int64_t specialDelCost;
    
    std::cin >> replCost >> insCost >> delCost;
    std::cin >> A >> B;
    std::cin >> specialRepl >> specialReplCost;
    std::cin >> specialDel >> specialDelCost;
    
    std::cout << "Входные данные:\n";
    std::cout << "Строка A: " << A << "\n";
    std::cout << "Строка B: " << B << "\n";
    std::cout << "Стоимость замены: " << replCost << "\n";
    std::cout << "Стоимость вставки: " << insCost << "\n";
    std::cout << "Стоимость удаления: " << delCost << "\n";
    std::cout << "Специальный символ замены: " << specialRepl << " (стоимость: " << specialReplCost << ")\n";
    std::cout << "Специальный символ удаления: " << specialDel << " (стоимость: " << specialDelCost << ")\n\n";
    
    int64_t result = minEditDistance(A, B, replCost, insCost, delCost,
                                   specialRepl, specialReplCost,
                                   specialDel, specialDelCost);
    
    std::cout << "\nМинимальная стоимость: " << result << std::endl;
    
    return 0;
} 