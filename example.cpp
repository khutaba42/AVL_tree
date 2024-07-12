#include "avl_tree.h"

/* compilation line:
g++ -std=c++11 -g -Wall -Wextra -pedantic -o example.out example.cpp
*/

int main()
{
    avl::tree<int> t1;
    for (int i = 300; i < 310; i++)
    {
        t1.insert(i);
    }

    for (auto &e : t1)
    {
        std::cout << e << " ,";
    }
    std::cout << std::endl;

    avl::tree<int> t2({101010, 137, 42});

    for (auto &e : t2)
    {
        std::cout << e << " ,";
    }
    std::cout << std::endl;

    avl::tree<int> t3 = avl::tree<int>::unite(t1, t2);

    for (auto &e : t3)
    {
        std::cout << e << " ,";
    }
    std::cout << std::endl;

    t3._print_tree();

    avl::tree<int> t4 = avl::tree<int>::unite(t1, t1);

    for (auto &e : t4)
    {
        std::cout << e << " ,";
    }
    std::cout << std::endl;

    t4._print_tree();

    avl::tree<int> t5 = avl::tree<int>::unite({2}, {1});

    for (auto &e : t5)
    {
        std::cout << e << " ,";
    }
    std::cout << std::endl;

    t5._print_tree();

    avl::tree<int> t6 = avl::tree<int>::unite({2}, {2});

    for (auto &e : t6)
    {
        std::cout << e << " ,";
    }
    std::cout << std::endl;

    t6._print_tree();

    return 0;
}
