#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;

enum {
    N = 109
};

struct cell {
    public:
        string key;
        int value;
        struct cell *next;
};

struct table {
    cell *buckets[N]{};

    private:
        unsigned int hash (string s) {
            unsigned int n=0;
            unsigned int i=0;
            int c=s[i];
            while (c) {
            n = n*65599u+(unsigned)c;
            i++;
            c=s[i];
            }
            return n;
        }

        inline unsigned int bucketOf(string s) { return hash(s) % N; }

    public:
        int get(string k){
            for(cell *p = buckets[bucketOf(k)]; p; p = p->next){
                if(p->key.compare(k) == 0) { return p->value; }
            }
            return 0;
        }

        void incr(const std::string& k) {
            const std::size_t b = bucketOf(k);
            for (cell* p = buckets[b]; p; p = p->next) {
                if (p->key == k) { 
                    p->value++; 
                    return; 
                }
            }
            buckets[b] = new cell{ k, 1, buckets[b] };
        }

        void print() {
            for (size_t i = 0; i < N; i++)
                for (cell *p = buckets[i]; p; p = p->next)
                {
                    std::cout << p->key + " " + (std::to_string(p->value)) << endl;
                }
        }

        ~table() {
        for (std::size_t i = 0; i < N; ++i) {
            for (cell* p = buckets[i]; p; ) {
                cell* nxt = p->next;
                delete p;
                p = nxt;
            }
        }
    }
};

int main() {
    table myTable;
    myTable.incr("foo");
    myTable.incr("bar");
    myTable.incr("bar");
    
    myTable.print();
}