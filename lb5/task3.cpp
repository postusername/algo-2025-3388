#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <algorithm>
#include <set>
#include <iomanip>

using namespace std;

bool DEBUG = true;

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
        trie.emplace_back();
        if (DEBUG) cerr << "Создан корневой узел (id: 0)" << endl;
    }
    
    void addPattern(const string& pattern, int patternId) {
        int node = 0;
        
        if (DEBUG) cerr << "Добавление шаблона: '" << pattern << "' с id=" << patternId << endl;
        
        for (char c : pattern) {
            if (trie[node].next.find(c) == trie[node].next.end()) {
                int newNodeId = trie.size();
                trie[node].next[c] = newNodeId;
                trie.emplace_back();
                if (DEBUG) cerr << "  Создан новый узел: " << newNodeId << " по переходу '" 
                         << c << "' из узла " << node << endl;
            }
            node = trie[node].next[c];
        }
        
        trie[node].patternIds.push_back(patternId);
        patternLengths.push_back(pattern.size());
        if (DEBUG) cerr << "  Узел " << node << " помечен как терминальный для шаблона " 
                 << patternId << " (длина=" << pattern.size() << ")" << endl;
    }
    
    void buildLinks() {
        queue<int> q;
        
        if (DEBUG) cerr << "\nПостроение суффиксных и терминальных ссылок:" << endl;
        
        trie[0].suffLink = -1;
        if (DEBUG) cerr << "  Узел 0 (корень): suffLink = -1" << endl;
        
        for (auto& [c, next] : trie[0].next) {
            trie[next].suffLink = 0;
            if (DEBUG) cerr << "  Узел " << next << " (первый уровень, символ '" << c 
                     << "'): suffLink = 0" << endl;
            q.push(next);
        }
        
        // BFS для построения ссылок
        while (!q.empty()) {
            int current = q.front();
            q.pop();
            
            if (DEBUG) cerr << "  Обработка узла " << current << ":" << endl;
            
            // Обрабатываем все переходы из текущего узла
            for (auto& [c, next] : trie[current].next) {
                if (DEBUG) cerr << "    Переход по '" << c << "' в узел " << next << ":" << endl;
                
                // Находим суффиксную ссылку для next
                int suffix = trie[current].suffLink;
                
                if (DEBUG) cerr << "      Ищем суффиксную ссылку, начиная с " << suffix << endl;
                
                while (suffix != -1 && trie[suffix].next.find(c) == trie[suffix].next.end()) {
                    if (DEBUG) cerr << "      В узле " << suffix << " нет перехода по '" 
                             << c << "', переходим к его suffLink" << endl;
                    suffix = trie[suffix].suffLink;
                }
                
                if (suffix != -1) {
                    trie[next].suffLink = trie[suffix].next[c];
                    if (DEBUG) cerr << "      Найден переход в узле " << suffix << " -> узел " 
                             << trie[suffix].next[c] << endl;
                } else {
                    trie[next].suffLink = 0;
                    if (DEBUG) cerr << "      Не найдено переходов, устанавливаем suffLink = 0" << endl;
                }
                
                if (DEBUG) cerr << "      Установлена суффиксная ссылка: узел " << next 
                         << " -> узел " << trie[next].suffLink << endl;
                
                // Находим терминальную ссылку
                if (!trie[trie[next].suffLink].patternIds.empty()) {
                    // Если суффиксная ссылка указывает на терминальный узел,
                    // то терминальная ссылка указывает туда же
                    trie[next].termLink = trie[next].suffLink;
                    if (DEBUG) cerr << "      Узел " << trie[next].suffLink 
                             << " терминальный, устанавливаем termLink = " << trie[next].termLink << endl;
                } else if (trie[next].suffLink > 0) {
                    // Иначе, берем терминальную ссылку от суффиксной ссылки
                    trie[next].termLink = trie[trie[next].suffLink].termLink;
                    if (DEBUG) cerr << "      Узел " << trie[next].suffLink 
                             << " не терминальный, берем его termLink: " << trie[next].termLink << endl;
                }
                
                q.push(next);
            }
        }
    }
    
    void printTrie() {
        if (!DEBUG) return;
        
        cerr << "\nСтруктура автомата Ахо-Корасик:" << endl;
        cerr << "-------------------------------------------------------------------------" << endl;
        cerr << "| Вершина | Переходы                | SuffLink | TermLink | Шаблоны     |" << endl;
        cerr << "-------------------------------------------------------------------------" << endl;
        
        for (int i = 0; i < trie.size(); ++i) {
            cerr << "| " << setw(7) << i << " | ";
            
            // Переходы
            string transitions;
            for (auto& [c, next] : trie[i].next) {
                if (!transitions.empty()) transitions += ", ";
                transitions += "'" + string(1, c) + "'→" + to_string(next);
            }
            cerr << setw(24) << left << transitions << " | "; 
            cerr << setw(8) << trie[i].suffLink << " | ";
            cerr << setw(8) << trie[i].termLink << " | ";
            
            // Шаблоны
            string patterns;
            for (int id : trie[i].patternIds) {
                if (!patterns.empty()) patterns += ", ";
                patterns += to_string(id);
            }
            cerr << setw(12) << patterns << " |" << endl;
        }
        cerr << "-------------------------------------------------------------------------" << endl;
    }
    
    vector<pair<int, int>> search(const string& text) {
        vector<pair<int, int>> result;
        int current = 0;
        
        if (DEBUG) cerr << "\nПроцесс поиска в тексте: '" << text << "'" << endl;
        
        for (int i = 0; i < text.length(); ++i) {
            char c = text[i];
            
            if (DEBUG) cerr << "  Позиция " << i << ", символ '" << c << "', текущее состояние: " << current << endl;
            
            // Ищем переход из текущего состояния
            while (current > 0 && trie[current].next.find(c) == trie[current].next.end()) {
                if (DEBUG) cerr << "    Нет перехода по '" << c << "', переход по суффиксной ссылке: " 
                         << current << " -> " << trie[current].suffLink << endl;
                current = trie[current].suffLink;
            }
            
            // Если нашли переход - используем его
            if (trie[current].next.find(c) != trie[current].next.end()) {
                current = trie[current].next[c];
                if (DEBUG) cerr << "    Найден переход по '" << c << "', новое состояние: " << current << endl;
            }
            
            // Обрабатываем найденные совпадения
            // Сначала обрабатываем текущий узел
            for (int patternId : trie[current].patternIds) {
                int patternPos = i - patternLengths[patternId - 1] + 1;
                if (DEBUG) cerr << "    Найден шаблон " << patternId << " в позиции " << (patternPos + 1) << endl;
                result.emplace_back(patternPos + 1, patternId); // +1 для 1-индексации
            }
            
            // Затем обрабатываем все терминальные ссылки
            int state = trie[current].termLink;
            while (state != -1) {
                for (int patternId : trie[state].patternIds) {
                    int patternPos = i - patternLengths[patternId - 1] + 1;
                    if (DEBUG) cerr << "    Найден шаблон " << patternId << " по termLink в позиции " 
                             << (patternPos + 1) << endl;
                    result.emplace_back(patternPos + 1, patternId); // +1 для 1-индексации
                }
                state = trie[state].termLink;
                if (DEBUG && state != -1) cerr << "    Переход по следующей termLink: " << state << endl;
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

vector<int> findPatternWithWildcard(const string& text, const string& pattern, char wildcard, char excludedChar) {
    if (DEBUG) {
        cerr << "\n=== Поиск шаблона с джокером ===" << endl;
        cerr << "Текст: '" << text << "'" << endl;
        cerr << "Шаблон: '" << pattern << "'" << endl;
        cerr << "Джокер: '" << wildcard << "'" << endl;
        cerr << "Исключаемый символ: '" << excludedChar << "'" << endl;
    }
    
    vector<string> substrings = splitPattern(pattern, wildcard);
    
    if (DEBUG) {
        cerr << "\nПодстроки после разбиения шаблона:" << endl;
        for (size_t i = 0; i < substrings.size(); ++i) {
            cerr << "  " << i + 1 << ": '" << substrings[i] << "'" << endl;
        }
    }

    if (substrings.empty()) {
        if (DEBUG) cerr << "Шаблон содержит только джокеры, возвращаем пустой результат" << endl;
        return {};
    }
    
    // Находим позиции начала подстрок в исходном шаблоне
    vector<int> substringPositions = getSubstringPositions(pattern, wildcard);
    
    if (DEBUG) {
        cerr << "\nПозиции подстрок в шаблоне:" << endl;
        for (size_t i = 0; i < substringPositions.size(); ++i) {
            cerr << "  Подстрока " << i + 1 << " начинается в позиции " << substringPositions[i] << endl;
        }
    }
    
    AhoCorasick ac;
    for (int i = 0; i < substrings.size(); ++i) {
        ac.addPattern(substrings[i], i + 1);
    }
    ac.buildLinks();
    ac.printTrie();
    
    // Ищем вхождения подстрок в тексте
    vector<pair<int, int>> matches = ac.search(text);
    
    if (DEBUG) {
        cerr << "\nНайденные совпадения подстрок:" << endl;
        for (auto& [pos, patternId] : matches) {
            cerr << "  Подстрока " << patternId << " в позиции " << pos << endl;
        }
    }
    
    // Ищем позиции, где могут быть полные совпадения шаблона
    vector<int> counter(text.size(), 0);
    
    for (auto& [pos, patternId] : matches) {
        int posInText = pos - 1; // Переводим в 0-индексацию
        int posInPattern = substringPositions[patternId - 1];
        int textStart = posInText - posInPattern;
        
        if (textStart >= 0 && textStart + pattern.size() <= text.size()) {
            counter[textStart]++;
            if (DEBUG) cerr << "  Потенциальное совпадение шаблона в позиции " << (textStart + 1) 
                     << " для подстроки " << patternId << endl;
        }
    }
    
    // Проверяем потенциальные совпадения
    vector<int> result;
    for (int i = 0; i < text.size(); ++i) {
        if (counter[i] == substrings.size() && i + pattern.size() <= text.size()) {
            if (DEBUG) cerr << "\nПроверка полного совпадения в позиции " << (i + 1) << ":" << endl;
            
            bool valid = true;
            for (int j = 0; j < pattern.size(); ++j) {
                if (pattern[j] != wildcard && text[i + j] != pattern[j]) {
                    if (DEBUG) cerr << "  Позиция " << (i + j + 1) << ": ожидался символ '" 
                             << pattern[j] << "', получен '" << text[i + j] << "' - несовпадение" << endl;
                    valid = false;
                    break;
                }
                // Проверяем, что джокер не соответствует исключаемому символу
                if (pattern[j] == wildcard && text[i + j] == excludedChar) {
                    if (DEBUG) cerr << "  Позиция " << (i + j + 1) << ": найден исключаемый символ '" 
                             << excludedChar << "' на месте джокера - несовпадение" << endl;
                    valid = false;
                    break;
                }
                
                if (DEBUG) {
                    if (pattern[j] == wildcard) {
                        cerr << "  Позиция " << (i + j + 1) << ": джокер совпадает с символом '" 
                             << text[i + j] << "'" << endl;
                    } else {
                        cerr << "  Позиция " << (i + j + 1) << ": символы совпадают ('" 
                             << pattern[j] << "')" << endl;
                    }
                }
            }
            
            if (valid) {
                if (DEBUG) cerr << "  Найдено полное совпадение в позиции " << (i + 1) << endl;
                result.push_back(i + 1); // +1 для 1-индексации
            }
        }
    }
    
    if (DEBUG) {
        cerr << "\nИтоговые позиции найденных совпадений:" << endl;
        for (int pos : result) {
            cerr << "  " << pos << endl;
        }
    }
    
    return result;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    
    string text, pattern;
    char wildcard, excludedChar;

    cin >> text;
    cin >> pattern;
    cin >> wildcard;
    cin >> excludedChar;
    
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
    
    vector<int> positions = findPatternWithWildcard(text, pattern, wildcard, excludedChar);
    
    for (int pos : positions) {
        cout << pos << "\n";
    }
    
    return 0;
}
