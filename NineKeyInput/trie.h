#ifndef TRIE_H
#define TRIE_H

#endif // TRIE_H

#pragma once
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>

struct TrieNode {
    std::unordered_map<char, TrieNode*> child;
    bool isWord = false;
    int wordCount = 0;     // 本词出现次数
    int prefixCount = 0;   // 以此前缀的总次数
};

class Trie {
public:
    Trie();
    ~Trie();

    // 基本操作
    void insert(const std::string& s, int cnt);
    bool remove(const std::string& s);
    bool update(const std::string& s, int newCnt);

    // 附加功能
    std::vector<std::string> regexSearch(const std::string& pattern);
    std::vector<std::pair<std::string,int>> prefixSearch(const std::string& prefix);
    std::vector<std::pair<std::string,int>> fuzzySearch(const std::string& digits);

    // 数字串查询：返回最佳匹配，并把每步中间结果塞到 steps
    std::string digitQuery(const std::string& digits, std::vector<std::string>& steps);

    // TXT 导入（dict.txt）
    bool importFromTxt(const std::string& resourcePath);

    int getCount(const std::string &s) const;


private:
    TrieNode* root;
    void clearNode(TrieNode* node);

};
