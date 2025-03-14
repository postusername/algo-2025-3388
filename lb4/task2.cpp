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


size_t kmp_first(const string& needle, const string& haystack) {
    size_t n = haystack.length();
    size_t m = needle.length(); 
    vector<size_t> lps = prefix_function(needle);
    size_t i = 0;
    size_t j = 0;

    while (i < n) { // перебираем текст
        if (needle[j] == haystack[i]) {
            j++;
            i++;
        }

        if (j == m) {                     // если мы достигли конца шаблона, значит, нашли вхождение
            return i - j;                 // в этом задании нам надо только первое вхождение
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

    return -1;
}


int main() {
    string a, b;
    cin >> a >> b;

    // при разной длине точно нельзя сдвигом получить другую строку
    if (a.length() != b.length()) {
        cout << -1 << endl;
        return 0;
    }

    size_t index = kmp_first(b, a + a);
    if (index == -1)
        cout << -1 << endl;
    else
        cout << index << endl;

    return 0;
}
