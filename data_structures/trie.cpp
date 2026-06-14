#include <bits/stdc++.h>
using namespace std;

/*
    Trie (prefix tree), pointer based
        insert / erase / find / countWord / countPrefix / contains: O(L)
            where L is the length of the queried string
        Space: O(total characters inserted)

    Configurable via template params:
        ALPHABET - size of the character set (default 26)
        FIRST    - the smallest character mapped to index 0 (default 'a')
        e.g. Trie<26, 'a'> for lowercase, Trie<10, '0'> for digits.

    Notes:
        - wordCount   = number of words ending exactly at a node
                        (duplicates counted, so insert("a") twice -> 2).
        - prefixCount = number of inserted words passing through a node.
        - erase removes ONE occurrence and frees any branch whose prefixCount
          drops to 0.
        - size() == root prefixCount == total words currently stored.

    Usage:
        Trie<> t;                       // lowercase English a-z
        t.insert("apple");
        bool ok = t.contains("apple");      // exact word present?
        int  w  = t.countWord("apple");     // how many times "apple" was inserted
        int  p  = t.countPrefix("app");     // how many words start with "app"
        t.erase("apple");                   // returns false if word wasn't present
*/

template <int ALPHABET = 26, char FIRST = 'a'>
struct Trie {
    struct Node {
        Node* child[ALPHABET] = {};  // all nullptr by default
        int wordCount = 0;           // words ending exactly here
        int prefixCount = 0;         // words passing through here
    };

    Node* root = new Node();

    void insert(const string& s) {
        Node* cur = root;
        cur->prefixCount++;
        for (char ch : s) {
            int c = ch - FIRST;
            if (cur->child[c] == nullptr) {
                cur->child[c] = new Node();
            }
            cur = cur->child[c];
            cur->prefixCount++;
        }
        cur->wordCount++;
    }

    // Node for the string, or nullptr if the path falls off the trie.
    Node* find(const string& s) const {
        Node* cur = root;
        for (char ch : s) {
            cur = cur->child[ch - FIRST];
            if (cur == nullptr) {
                return nullptr;
            }
        }
        return cur;
    }

    // Times this exact word is currently stored.
    int countWord(const string& s) const {
        Node* node = find(s);
        return node ? node->wordCount : 0;
    }

    // Number of stored words that have s as a prefix.
    int countPrefix(const string& s) const {
        Node* node = find(s);
        return node ? node->prefixCount : 0;
    }

    bool contains(const string& s) const {
        return countWord(s) > 0;
    }

    // Remove one occurrence of s. Returns false if it wasn't present.
    bool erase(const string& s) {
        if (countWord(s) == 0) {
            return false;
        }
        root->prefixCount--;
        Node* cur = root;
        for (char ch : s) {
            int c = ch - FIRST;
            Node* next = cur->child[c];
            if (--next->prefixCount == 0) {  // branch now empty: free it and stop
                cur->child[c] = nullptr;
                freeSubtree(next);
                return true;
            }
            cur = next;
        }
        cur->wordCount--;
        return true;
    }

    // Total number of words currently stored (with multiplicity).
    int size() const {
        return root->prefixCount;
    }

private:
    void freeSubtree(Node* node) {
        for (Node* c : node->child) {
            if (c != nullptr) {
                freeSubtree(c);
            }
        }
        delete node;
    }
};
