#include <iostream>

#include "MyTinySTL/vector.h"

using namespace std;

int main() {
    mystl::vector<int> v = {1, 2, 3};
    v.push_back(4);
    for (const auto& i : v) cout << i << " ";
    cout << endl;
}