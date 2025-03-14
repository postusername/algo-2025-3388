#include <iostream>
#include <string>
#include <vector>
#define DEBUG
using namespace std;


vector<size_t> prefix_function(const string& x) {
    size_t m = x.length();
    vector<size_t> lps(m, 0);
    size_t len = 0;
    size_t i = 1;

    while (i < m) {
        if (x[i] == x[len]) {        // если текущий символ совпадает с символом на позиции len
            len++;                   // увеличиваем длину префикса/суффикса
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {          // у нас есть предыдущий префикс/суффикс
                len = lps[len - 1];
            } else {                 // нет предыдущего префикса/суффикса
                lps[i] = 0;
                i++;                     
            }
        }
    }

#ifdef DEBUG
    for (size_t i = 0; i < m; i++)
        cout << "prefix[" << i << "] = " << lps[i] << endl;
#endif

    return lps;
}


vector<size_t> kmp_search(const string& needle, const string& haystack) {
    size_t n = haystack.length();
    size_t m = needle.length(); 
    vector<size_t> lps = prefix_function(needle);
    vector<size_t> occurrences;
    size_t i = 0;
    size_t j = 0;

    while (i < n) { // перебираем текст
        if (needle[j] == haystack[i]) {
            j++;
            i++;
        }

        if (j == m) {                     // если мы достигли конца шаблона, значит, нашли вхождение
#ifdef DEBUG
            cout << "Нашли совпадение по индексу: " << i - j << endl;
#endif
            occurrences.push_back(i - j);
            j = lps[j - 1];               // переходим к длине совпадающего префикса/суффикса, чтобы найти перекрывающиеся вхождения
        }
        // расхождение после j совпадений
        else if (i < n && needle[j] != haystack[i]) {
            if (j != 0) {       // у нас есть уже совпадающий префикс/суффикс, можно перепрыгнуть
                j = lps[j - 1];
            } else {            // нет предыдущего префикса/суффикса
                i++;  
            }
        }
    }
    return occurrences;
}


int main() {
    string needle, haystack;
    cin >> needle >> haystack;

    vector<size_t> occurrences = kmp_search(needle, haystack);

    if (occurrences.empty()) {
        cout << -1 << endl;
    } else {
        for (size_t i = 0; i < occurrences.size(); ++i)
            cout << occurrences[i] << ((i < occurrences.size() - 1) ? "," : "");
        cout << endl;
    }

    return 0;
}
