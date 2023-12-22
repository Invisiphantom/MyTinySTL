#include <iostream>

#include "MyTinySTL/vector.h"

int main() {
    mystl::vector<int> demo = {1, 2, 3};
    demo.push_back(4);
    for (auto i : demo)
        std::cout << i << std::endl;
}