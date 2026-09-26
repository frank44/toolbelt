#include <bits/stdc++.h>
using namespace std;

/*
    Trie (prefix tree), pointer based. Every Trie object is a node; the one you
    declare is the root and owns the whole tree (children are freed on destruction).
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
        - insert / find / count* / contains work relative to whichever node you
          call them on. erase and size are ROOT-ONLY.
        - erase removes ONE occurrence and frees any branch whose prefixCount
          drops to 0.
        - size() == root prefixCount == total words currently stored.
        - Non-copyable (it owns raw pointers). Pass by reference.

    Usage:
        Trie<> t;                       // lowercase English a-z
        t.insert("apple");
        bool ok = t.contains("apple");      // exact word present?
        int  w  = t.countWord("apple");     // how many times "apple" was inserted
        int  p  = t.countPrefix("app");     // how many words start with "app"
        t.erase("apple");                   // returns false if word wasn't present
        Trie<>* node = t.find("app");       // node for "app", or nullptr
        debug(t);                           // indented dump via toString()

    toString() output for insert("app"), insert("apple") x2:
        Trie size=3
          a prefix=3
            p prefix=3
              p prefix=3 word=1
                l prefix=2
                  e prefix=2 word=2
*/

template <int ALPHABET = 26, char FIRST = 'a'>
struct Trie {
    array<Trie*, ALPHABET> child{};  // all nullptr by default
    int wordCount = 0;               // words ending exactly here
    int prefixCount = 0;             // words passing through here

    Trie() = default;
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;
    ~Trie() {
        for (Trie* c : child) {
            delete c;  // recursively frees the subtree; delete nullptr is a no-op
        }
    }

    void insert(const string& s) {
        Trie* iter = this;
        iter->prefixCount++;
        for (char ch : s) {
            int c = ch - FIRST;
            if (iter->child[c] == nullptr) {
                iter->child[c] = new Trie();
            }
            iter = iter->child[c];
            iter->prefixCount++;
        }
        iter->wordCount++;
    }

    // Node for the string, or nullptr if the path falls off the trie.
    Trie* find(const string& s) {
        Trie* iter = this;
        for (char ch : s) {
            iter = iter->child[ch - FIRST];
            if (iter == nullptr) {
                return nullptr;
            }
        }
        return iter;
    }

    // Times this exact word is currently stored.
    int countWord(const string& s) {
        Trie* node = find(s);
        return node ? node->wordCount : 0;
    }

    // Number of stored words that have s as a prefix.
    int countPrefix(const string& s) {
        Trie* node = find(s);
        return node ? node->prefixCount : 0;
    }

    bool contains(const string& s) {
        return countWord(s) > 0;
    }

    // ROOT-ONLY. Remove one occurrence of s. Returns false if it wasn't present.
    bool erase(const string& s) {
        if (countWord(s) == 0) {
            return false;
        }
        prefixCount--;
        Trie* iter = this;
        for (char ch : s) {
            int c = ch - FIRST;
            Trie* next = iter->child[c];
            if (--next->prefixCount == 0) {  // branch now empty: free it and stop
                iter->child[c] = nullptr;
                delete next;
                return true;
            }
            iter = next;
        }
        iter->wordCount--;
        return true;
    }

    // ROOT-ONLY. Total number of words currently stored (with multiplicity).
    int size() const {
        return prefixCount;
    }

    // Indented dump of the subtree rooted here, one node per line. No trailing newline.
    string toString() const {
        string out = "Trie size=" + to_string(prefixCount);
        if (wordCount > 0) {
            out += " word=" + to_string(wordCount);  // only if "" was inserted
        }
        dump(out, 1);
        return out;
    }

    friend ostream& operator<<(ostream& os, const Trie& t) {
        return os << t.toString();
    }

private:
    void dump(string& out, int depth) const {
        for (int c = 0; c < ALPHABET; c++) {
            const Trie* node = child[c];
            if (node == nullptr) {
                continue;
            }
            out += "\n" + string(2 * depth, ' ') + char(FIRST + c) + " prefix=" + to_string(node->prefixCount);
            if (node->wordCount > 0) {
                out += " word=" + to_string(node->wordCount);
            }
            node->dump(out, depth + 1);
        }
    }
};
