#include "trie.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <algorithm>
#include <stack>
#include <string>
#include <vector>
#include <functional>


Trie::Trie() { root = new TrieNode(); }
void Trie::clearNode(TrieNode* node) {
    for (auto &p : node->child) clearNode(p.second);
    delete node;
}
Trie::~Trie() { clearNode(root); }

//添加
void Trie::insert(const std::string& s, int cnt) {
    TrieNode* cur = root;
    cur->prefixCount += cnt;
    for (char c : s) {
        if (!cur->child.count(c))
            cur->child[c] = new TrieNode();
        cur = cur->child[c];
        cur->prefixCount += cnt;
    }
    cur->isWord = true;
    cur->wordCount = cnt;
}

//删除
bool Trie::remove(const std::string& s) {
    TrieNode* cur = root;
    std::vector<TrieNode*> path = {root};
    for (char c : s) {
        if (!cur->child.count(c)) return false;
        cur = cur->child[c];
        path.push_back(cur);
    }
    if (!cur->isWord) return false;
    int oldCnt = cur->wordCount;
    for (auto node : path)
        node->prefixCount -= oldCnt;
    cur->isWord = false;
    cur->wordCount = 0;
    return true;
}

//修改
bool Trie::update(const std::string& s, int newCnt) {
    TrieNode* cur = root;
    for (char c : s) {
        if (!cur->child.count(c)) return false;
        cur = cur->child[c];
    }
    if (!cur->isWord) return false;
    remove(s);
    insert(s, newCnt);
    return true;
}

//导入txt
bool Trie::importFromTxt(const std::string& resourcePath) {
    QFile f(QString::fromStdString(resourcePath));
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        auto parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() != 2) continue;
        std::string word = parts[0].toStdString();
        bool ok; int cnt = parts[1].toInt(&ok);
        if (!ok) continue;
        insert(word, cnt);
    }
    f.close();
    return true;
}

//正则查询
static bool wildcardMatch(const std::string &p, size_t i,
                          const std::string &s, size_t j)
{
    if (i == p.size())
        return j == s.size();
    if (p[i] == '*') {
        // '*' 匹配 0..(剩余长度) 个字符
        for (size_t k = j; k <= s.size(); ++k) {
            if (wildcardMatch(p, i+1, s, k))
                return true;
        }
        return false;
    }
    // 普通字符或 '?'
    if (j < s.size() && (p[i] == '?' || p[i] == s[j]))
        return wildcardMatch(p, i+1, s, j+1);
    return false;
}


static void collectRegex(TrieNode *node,
                         std::string &path,
                         const std::string &pattern,
                         std::vector<std::string> &out)
{
    if (node->isWord) {
        if (wildcardMatch(pattern, 0, path, 0))
            out.push_back(path);
    }
    for (auto &kv : node->child) {
        path.push_back(kv.first);
        collectRegex(kv.second, path, pattern, out);
        path.pop_back();
    }
}

std::vector<std::string> Trie::regexSearch(const std::string &pattern)
{
    std::vector<std::string> results;
    std::string path;
    collectRegex(root, path, pattern, results);
    std::sort(results.begin(), results.end());
    return results;
}

// —— 前缀查询 ——
std::vector<std::pair<std::string,int>> Trie::prefixSearch(const std::string &prefix)
{
    std::vector<std::pair<std::string,int>> results;

    TrieNode* node = root;
    for (char c : prefix) {
        auto it = node->child.find(c);
        if (it == node->child.end())
            return results;
        node = it->second;
    }

    //DFS
    std::string path = prefix;
    std::function<void(TrieNode*)> dfs = [&](TrieNode* cur) {
        if (cur->isWord) {
            results.emplace_back(path, cur->wordCount);
        }
        for (auto &kv : cur->child) {
            path.push_back(kv.first);
            dfs(kv.second);
            path.pop_back();
        }
    };
    dfs(node);

    std::sort(results.begin(), results.end(),
              [](auto &a, auto &b) {
                  if (a.second != b.second)
                      return a.second < b.second;
                  return a.first < b.first;
              }
              );

    return results;
}

// —— 计算两串的 Levenshtein 距离 ——
static int levenshtein(const std::string &s, const std::string &t) {
    size_t n = s.size(), m = t.size();
    std::vector<std::vector<int>> dp(n+1, std::vector<int>(m+1));
    for (size_t i = 0; i <= n; ++i) dp[i][0] = int(i);
    for (size_t j = 0; j <= m; ++j) dp[0][j] = int(j);
    for (size_t i = 1; i <= n; ++i) {
        for (size_t j = 1; j <= m; ++j) {
            if (s[i-1] == t[j-1]) {
                dp[i][j] = dp[i-1][j-1];
            } else {
                dp[i][j] = 1 + std::min({
                               dp[i-1][j],    // 删除
                               dp[i][j-1],    // 插入
                               dp[i-1][j-1]   // 替换
                           });
            }
        }
    }
    return dp[n][m];
}
static char letter2digit(char c) {
    static const char map[26] = {
        '2','2','2',  // a b c
        '3','3','3',  // d e f
        '4','4','4',  // g h i
        '5','5','5',  // j k l
        '6','6','6',  // m n o
        '7','7','7','7', // p q r s
        '8','8','8',  // t u v
        '9','9','9','9' // w x y z
    };
    if (c >= 'a' && c <= 'z')   return map[c - 'a'];
    if (c >= 'A' && c <= 'Z')   return map[c - 'A'];
    return '0';
}

//自动校正
std::vector<std::pair<std::string,int>> Trie::fuzzySearch(const std::string &digits) {
    std::vector<std::pair<std::string,int>> candidates;
    std::vector<std::pair<std::string,int>> allWords;
    std::string path;

    std::function<void(TrieNode*)> dfs = [&](TrieNode *node) {
        if (node->isWord) {
            allWords.emplace_back(path, node->wordCount);
        }
        for (auto &kv : node->child) {
            path.push_back(kv.first);
            dfs(kv.second);
            path.pop_back();
        }
    };
    dfs(root);

    for (auto &p : allWords) {
        const std::string &word = p.first;
        int cnt = p.second;

        std::string code;
        code.reserve(word.size());
        for (char c : word) {
            code.push_back(letter2digit(c));
        }


        if (levenshtein(code, digits) <= 1) {
            candidates.emplace_back(word, cnt);
        }
    }

    std::sort(candidates.begin(), candidates.end(),
              [](auto &a, auto &b) {
                  if (a.second != b.second)
                      return a.second > b.second;
                  return a.first < b.first;
              }
              );

    return candidates;
}

int Trie::getCount(const std::string &s) const {
    TrieNode* cur = root;
    for (char c : s) {
        auto it = cur->child.find(c);
        if (it == cur->child.end()) return 0;
        cur = it->second;
    }
    return cur->isWord ? cur->wordCount : 0;
}





std::string Trie::digitQuery(const std::string &digits,
                             std::vector<std::string> &steps) {
    steps.clear();

    static const char* mp[10] = {
        "", "", "abc","def","ghi","jkl","mno","pqrs","tuv","wxyz"
    };

    for (size_t i = 1; i <= digits.size(); ++i) {
        const std::string pref = digits.substr(0, i);
        int bestCnt = -1;
        std::string bestLetters(i, ' ');

        struct Frame { TrieNode* node; size_t depth; std::string path; };

        std::stack<Frame> st;
        st.push({root, 0, std::string()});

        while (!st.empty()) {
            auto fr = st.top(); st.pop();
            TrieNode* node = fr.node;
            size_t    d    = fr.depth;
            std::string path = fr.path;

            if (d == i) {
                int cnt = node->prefixCount;
                if (cnt > bestCnt
                    || (cnt == bestCnt && path < bestLetters))
                {
                    bestCnt     = cnt;
                    bestLetters = path;
                }

                continue;
            }
            int digit = digits[d] - '0';
            const char* letters = mp[digit];

            for (int k = 0; letters[k] != '\0'; ++k) {
                char c = letters[k];
                auto it = node->child.find(c);
                if (it != node->child.end()) {

                    st.push({it->second, d+1, path + c});
                }
            }
        }

        if (bestCnt < 0) {

            break;
        }

        steps.push_back(pref + " " + bestLetters);
    }

    return steps.empty() ? std::string() : steps.back();
}
