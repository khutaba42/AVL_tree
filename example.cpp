#include "avl_tree.h"

/* compilation line:
g++ -std=c++11 -Wall -Wextra -pedantic -o example.out example.cpp
*/

int main()
{
    avl::tree<int> t1;
    for (int i = 0; i < 10; i++)
    {
        t1.insert(i);
    }

    avl::tree<int> t2({101010, 137, 42});

    return 0;
}