#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <algorithm>
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
                    trie[next].termLink = trie[next].suffLink;
                    if (DEBUG) cerr << "      Узел " << trie[next].suffLink 
                             << " терминальный, устанавливаем termLink = " << trie[next].termLink << endl;
                } else if (trie[next].suffLink > 0) {
                    trie[next].termLink = trie[trie[next].suffLink].termLink;
                    if (DEBUG) cerr << "      Узел " << trie[next].suffLink 
                             << " не терминальный, берем его termLink: " << trie[next].termLink << endl;
                }
                
                q.push(next);
            }
        }
        
        if (DEBUG) printTrie();
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
    
    vector<pair<int, int>> findPatterns(const string& text) {
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
            for (int patternId : trie[current].patternIds) {
                int patternPos = i - patternLengths[patternId - 1] + 1;
                if (DEBUG) cerr << "    Найден шаблон " << patternId << " в позиции " << (patternPos + 1) << endl;
                result.emplace_back(patternPos + 1, patternId);
            }
            
            // Обрабатываем терминальные ссылки
            int state = trie[current].termLink;
            while (state != -1) {
                for (int patternId : trie[state].patternIds) {
                    int patternPos = i - patternLengths[patternId - 1] + 1;
                    if (DEBUG) cerr << "    Найден шаблон " << patternId << " по termLink в позиции " 
                             << (patternPos + 1) << endl;
                    result.emplace_back(patternPos + 1, patternId);
                }
                state = trie[state].termLink;
                if (DEBUG && state != -1) cerr << "    Переход по следующей termLink: " << state << endl;
            }
        }
        
        if (DEBUG) {
            cerr << "\nНайденные совпадения до сортировки:" << endl;
            for (const auto& [pos, patternId] : result) {
                cerr << "  Позиция " << pos << ", шаблон " << patternId << endl;
            }
        }
        
        sort(result.begin(), result.end());
        
        if (DEBUG) {
            cerr << "\nОтсортированные совпадения:" << endl;
            for (const auto& [pos, patternId] : result) {
                cerr << "  Позиция " << pos << ", шаблон " << patternId << endl;
            }
        }
        
        return result;
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    
    string text;
    cin >> text;
    
    int n;
    cin >> n;
    
    AhoCorasick ac;
    
    for (int i = 1; i <= n; ++i) {
        string pattern;
        cin >> pattern;
        if (DEBUG) cerr << "Шаблон " << i << ": " << pattern << endl;
        ac.addPattern(pattern, i);
    }
    
    ac.buildLinks();
    
    vector<pair<int, int>> results = ac.findPatterns(text);
    
    if (DEBUG) cerr << "\nВывод результатов:" << endl;
    
    for (const auto& [pos, patternId] : results) {
        cout << pos << " " << patternId << "\n";
        if (DEBUG) cerr << "  " << pos << " " << patternId << endl;
    }
    
    return 0;
} 