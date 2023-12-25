#include <iostream>
#include <bits/stl_pair.h>

using namespace std;

template <typename T1, typename T2>
class Demo {
   private:
    T1 first;
    T2 second;

   public:
    template <U1 = T1, U2 = T2>
    constexpr Demo(U1 x, U2 y) : first(x), second(y) {}
};

int main() {
    pair<int, int> p1('a', 'b');
    // Demo<int, int> demo1(1, 2);
    cout << p1.first << endl;
    return 0;
}