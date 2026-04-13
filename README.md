# AVL Tree — C++ Header-Only Library

A self-balancing binary search tree implementation in C++. Header-only, no dependencies beyond the standard library. Works with C++11 and above.

## Getting Started

Drop `avl_tree.h` into your project and include it:

```cpp
#include "avl_tree.h"
```

The tree lives in the `avl` namespace. It's templated on the data type (`int` in the example below), and optionally on a comparator (defaults to `<`, we will cover that later):

```cpp
avl::tree<int> numbers;
avl::tree<std::string> words;
```

## Complexity

| Operation       | Time         | Space        |
|-----------------|--------------|--------------|
| `insert`        | O(log n)     | O(1)         |
| `remove`        | O(log n)     | O(1)         |
| `search`        | O(log n)     | O(1)         |
| `unite`         | O(n + m)     | O(n + m)     |
| `size`          | O(1)         | O(1)         |
| `empty`         | O(1)         | O(1)         |
| `height`        | O(1)         | O(1)         |
| `clear`         | O(n)         | O(log n)*    |
| full iteration  | O(n)         | O(1)         |

\* `clear` uses O(log n) stack space due to recursive destruction.

## Basic Usage

### Creating a tree

You can construct an empty tree, or initialize it directly from a list:

```cpp
avl::tree<int> t1;
avl::tree<int> t2({10, 20, 30, 40, 50});
```

### Inserting and removing elements

Both operations throw if something goes wrong — `insert` throws `avl::data_already_exists` if the value is already in the tree, and `remove` throws `avl::data_not_found` if it isn't. Both run in **O(log n)** time.

```cpp
t1.insert(42);
t1.insert(17);
t1.remove(42);
```

### Searching

Returns a reference to the stored data. Throws `avl::data_not_found` if the element doesn't exist. Runs in **O(log n)** time.

```cpp
try {
    const int &val = t1.search(17);
} catch (const avl::data_not_found &e) {
    // not in the tree
}
```

### Iteration

Supports range-based for loops. Elements are visited in sorted (in-order) order. A full traversal runs in **O(n)** time, and each individual `++` is **O(1)** amortized.

```cpp
avl::tree<int> t({5, 3, 8, 1});

for (const auto &elem : t) {
    std::cout << elem << " ";
}
// Output: 1 3 5 8
```

### Merging two trees

`unite` merges two trees into a new one. Duplicates are kept once. Works with both lvalue and rvalue (move) references. Runs in **O(n + m)** time, where n and m are the sizes of the two trees.

```cpp
avl::tree<int> a({1, 2, 3});
avl::tree<int> b({3, 4, 5});

// copies both trees
avl::tree<int> merged = avl::tree<int>::unite(a, b);
// merged contains: 1 2 3 4 5

// moves both trees (a and b are empty afterward)
avl::tree<int> moved = avl::tree<int>::unite(std::move(a), std::move(b));
```

### Other operations

```cpp
t.size();    // number of elements  — O(1)
t.empty();   // true if no elements — O(1)
t.height();  // height of the tree  — O(1)
t.clear();   // remove all elements — O(n)
```

## Custom comparator

If your type doesn't support `<`, or you want a different ordering, pass a comparator as the second template argument:

```cpp
struct reverse_cmp {
    bool operator()(const int &a, const int &b) const {
        return a > b;
    }
};

avl::tree<int, reverse_cmp> descending;
```

## Compilation

```bash
g++ -std=c++11 -o main main.cpp
```

No special flags or linked libraries needed — everything is in the header.

## License

Feel free to use, modify, and share.
