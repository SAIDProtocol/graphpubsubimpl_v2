#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

using namespace std;

inline char getRandomChar(default_random_engine &rand) {
    char c = static_cast<char>(rand() % 26 + 'a');
//    cout << c << endl;
    return c;
}

template<typename T>
inline T randBetween(T min, T max, default_random_engine &rand) {
    if (min == max) return min;
    else if (min > max) return rand() % (min - max) + max;
    else return rand() % (max - min) + min;
}

string getRandomString(size_t minLen, size_t maxLen, default_random_engine &rand) {
    ostringstream oss(ostringstream::ate);
    size_t len = randBetween(minLen, maxLen, rand);
    for (decltype(len) i = 0; i < len; i++) oss << randBetween('a', 'z', rand);
    return oss.str();
}

void generateNamespace(
        int currLevel, int maxLevel, size_t minChildCount, size_t maxChildCount, size_t minLen, size_t maxLen,
        const string &prefix, vector<string> &res, default_random_engine &rand) {
    res.push_back(prefix);
    if (currLevel < maxLevel) {
        size_t childrenSize = randBetween(minChildCount, maxChildCount, rand);
        for (int i = 0; i < childrenSize; i++) {
            string childName = prefix + getRandomString(minLen, maxLen, rand) + '/';
            generateNamespace(currLevel + 1, maxLevel, minChildCount, maxChildCount, minLen, maxLen,
                              childName, res, rand);
        }
    }
}

void outputFib(const vector<string> &names, const string &fileName, int nextHopMin, int nextHopMax,
               default_random_engine &rand) {
    ofstream os(fileName);
    for (auto &name : names)
        os << name << "\t" << randBetween(nextHopMin, nextHopMax, rand) << endl;
    os.flush();
}

void outputGNRS(const vector<string> &names, const string &fileName) {
    ofstream os(fileName);
    int i = 0;
    for (auto &name : names)
        os << ++i << "\t" << name << endl;
    os.flush();

}

int main() {
    default_random_engine rand(0);
    vector<string> names;
//    generateNamespace(0, 8, 2, 2, 8, 4, "/", names, rand);
//    outputFib(names, "ben-client-1-B512.fib", 0x10, 0x10, rand);
//    outputFib(names, "ben-router-B512.fib", 0x11, 0x16, rand);
//    outputGNRS(names, "ben-client-1-B512.gnrs");
    generateNamespace(0, 13, 2, 2, 8, 4, "/", names, rand);
    outputFib(names, "ben-client-1-B16383.fib", 0x10, 0x10, rand);
    outputFib(names, "ben-router-B16383.fib", 0x11, 0x16, rand);
    outputGNRS(names, "ben-client-1-B16383.gnrs");
    return 0;
}