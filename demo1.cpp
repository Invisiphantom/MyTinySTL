#include <vector>
#include <iostream>

using namespace std;

int main()
{
    vector<int> demo = { 1, 2, 3 };
    *demo.rbegin() = 4;
    demo.push_back(4);
    for (auto i : demo)
        cout << i << endl;
}