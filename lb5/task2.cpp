#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <algorithm>
#include <set>

using namespace std;

class AhoCorasick {
private:
    struct Node {
        map<char, int> next;
        int suffLink = 0;
        int termLink = -1;
        vector<int> patternIds;
    };
    
    vector<Node> trie;
    vector<int> patternLengths;
    
public:
    AhoCorasick() {
        trie.emplace_back(); // Создаем корневой узел
    }
    
    void addPattern(const string& pattern, int patternId) {
        int node = 0;
        
        for (char c : pattern) {
            if (trie[node].next.find(c) == trie[node].next.end()) {
                trie[node].next[c] = trie.size();
                trie.emplace_back();
            }
            node = trie[node].next[c];
        }
        
        trie[node].patternIds.push_back(patternId);
        patternLengths.push_back(pattern.size());
    }
    
    void buildLinks() {
        queue<int> q;
        
        // Инициализация для корневого узла
        trie[0].suffLink = -1;
        
        // Инициализация для узлов на глубине 1
        for (auto& [c, next] : trie[0].next) {
            trie[next].suffLink = 0;
            q.push(next);
        }
        
        // BFS для построения ссылок
        while (!q.empty()) {
            int current = q.front();
            q.pop();
            
            // Обрабатываем все переходы из текущего узла
            for (auto& [c, next] : trie[current].next) {
                // Находим суффиксную ссылку для next
                int suffix = trie[current].suffLink;
                
                while (suffix != -1 && trie[suffix].next.find(c) == trie[suffix].next.end()) {
                    suffix = trie[suffix].suffLink;
                }
                
                if (suffix != -1) {
                    trie[next].suffLink = trie[suffix].next[c];
                } else {
                    trie[next].suffLink = 0;
                }
                
                // Находим терминальную ссылку
                if (!trie[trie[next].suffLink].patternIds.empty()) {
                    // Если суффиксная ссылка указывает на терминальный узел,
                    // то терминальная ссылка указывает туда же
                    trie[next].termLink = trie[next].suffLink;
                } else if (trie[next].suffLink > 0) {
                    // Иначе, берем терминальную ссылку от суффиксной ссылки
                    trie[next].termLink = trie[trie[next].suffLink].termLink;
                }
                
                q.push(next);
            }
        }
    }
    
    vector<pair<int, int>> search(const string& text) {
        vector<pair<int, int>> result;
        int current = 0;
        
        for (int i = 0; i < text.length(); ++i) {
            char c = text[i];
            
            // Ищем переход из текущего состояния
            while (current > 0 && trie[current].next.find(c) == trie[current].next.end()) {
                current = trie[current].suffLink;
            }
            
            // Если нашли переход - используем его
            if (trie[current].next.find(c) != trie[current].next.end()) {
                current = trie[current].next[c];
            }
            
            // Обрабатываем найденные совпадения
            // Сначала обрабатываем текущий узел
            for (int patternId : trie[current].patternIds) {
                int patternPos = i - patternLengths[patternId - 1] + 1;
                result.emplace_back(patternPos + 1, patternId); // +1 для 1-индексации
            }
            
            // Затем обрабатываем все терминальные ссылки
            int state = trie[current].termLink;
            while (state != -1) {
                for (int patternId : trie[state].patternIds) {
                    int patternPos = i - patternLengths[patternId - 1] + 1;
                    result.emplace_back(patternPos + 1, patternId); // +1 для 1-индексации
                }
                state = trie[state].termLink;
            }
        }
        
        return result;
    }
    
    int getVertexCount() {
        return trie.size();
    }
};

// Разбивает шаблон на подстроки, разделенные джокерами
vector<string> splitPattern(const string& pattern, char wildcard) {
    vector<string> result;
    string current;
    
    for (char c : pattern) {
        if (c == wildcard) {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        result.push_back(current);
    }
    
    return result;
}

// Находит позиции начала подстрок в исходном шаблоне
vector<int> getSubstringPositions(const string& pattern, char wildcard) {
    vector<int> positions;
    int pos = 0;
    string current;
    
    for (int i = 0; i < pattern.size(); ++i) {
        if (pattern[i] == wildcard) {
            if (!current.empty()) {
                positions.push_back(pos);
                current.clear();
            }
            pos = i + 1;
        } else {
            if (current.empty()) {
                pos = i;
            }
            current += pattern[i];
        }
    }
    
    if (!current.empty()) {
        positions.push_back(pos);
    }
    
    return positions;
}

vector<int> findPatternWithWildcard(const string& text, const string& pattern, char wildcard) {
    // Разбиваем шаблон на подстроки без джокеров
    vector<string> substrings = splitPattern(pattern, wildcard);
    
    // Если нет подстрок (только джокеры), возвращаем пустой результат
    if (substrings.empty()) {
        return {};
    }
    
    // Находим позиции начала подстрок в исходном шаблоне
    vector<int> substringPositions = getSubstringPositions(pattern, wildcard);
    
    // Создаем автомат Ахо-Корасик и добавляем все подстроки
    AhoCorasick ac;
    for (int i = 0; i < substrings.size(); ++i) {
        ac.addPattern(substrings[i], i + 1);
    }
    
    // Строим ссылки
    ac.buildLinks();
    
    // Ищем вхождения подстрок в тексте
    vector<pair<int, int>> matches = ac.search(text);
    
    // Ищем позиции, где могут быть полные совпадения шаблона
    vector<int> counter(text.size(), 0);
    
    for (auto& [pos, patternId] : matches) {
        int posInText = pos - 1; // Переводим в 0-индексацию
        int posInPattern = substringPositions[patternId - 1];
        int textStart = posInText - posInPattern;
        
        if (textStart >= 0 && textStart + pattern.size() <= text.size()) {
            counter[textStart]++;
        }
    }
    
    // Проверяем потенциальные совпадения
    vector<int> result;
    for (int i = 0; i < text.size(); ++i) {
        if (counter[i] == substrings.size() && i + pattern.size() <= text.size()) {
            bool valid = true;
            for (int j = 0; j < pattern.size(); ++j) {
                if (pattern[j] != wildcard && text[i + j] != pattern[j]) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                result.push_back(i + 1); // +1 для 1-индексации
            }
        }
    }
    
    return result;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    
    string text, pattern;
    char wildcard;
    
    cin >> text;
    cin >> pattern;
    cin >> wildcard;
    
    // Проверяем, что в шаблоне есть хотя бы один не-джокер
    bool hasNonWildcard = false;
    for (char c : pattern) {
        if (c != wildcard) {
            hasNonWildcard = true;
            break;
        }
    }
    
    if (!hasNonWildcard) {
        cerr << "Шаблон должен содержать хотя бы один не-джокер символ" << endl;
        return 1;
    }
    
    vector<int> positions = findPatternWithWildcard(text, pattern, wildcard);
    
    for (int pos : positions) {
        cout << pos << "\n";
    }
    
    return 0;
}
