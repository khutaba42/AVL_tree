#ifndef __AVL_TREE_H__
#define __AVL_TREE_H__

#undef AVL_TREE_TEST // to solve any MACRO conflicts
/**
 * a MACRO to quickly toggle testing on/off.
 * comment out when you don't want to test.
 *
 * for example:
 *      #ifdef AVL_TREE_TEST
 *          test code
 *      #else
 *          good code
 *      #endif
 * or:
 *      #ifdef AVL_TREE_TEST
 *          code
 *      #endif
 */
#define AVL_TREE_TEST

// Includes

#ifdef AVL_TREE_TEST
#include <iostream>
// Define some common ANSI color codes for printing colored text
#define RED "\033[31m"    // Set text to red
#define GREEN "\033[32m"  // Set text to green
#define YELLOW "\033[33m" // Set text to yellow
#define RESET "\033[0m"   // Reset text color
#endif                    // AVL_TREE_TEST

#include <stdlib.h> // for size_t
#include <utility>  // for std::forward, std::swap, std::move
#include <cassert>  // for assert()
#include <stdexcept>
#include <exception>
#include <string>
#include <initializer_list>

namespace avl
{
  /**
   * `less` struct which overloads operator()
   * this is the default way to figure out which element is
   * smaller than the other
   */
  template <typename T>
  struct less
  {
    inline bool operator()(const T &x, const T &y) const { return x < y; }
  }; // could use std::less defined in <functional> instead

  template <typename T>
  inline void swap(T &x, T &y) { std::swap<T>(x, y); }

  class _custom_exception : public std::exception
  {
  protected:
    std::string message;

  public:
    explicit _custom_exception(const std::string &msg) : message(msg) {}
    virtual const char *what() const noexcept override { return message.c_str(); }
  };

  class data_not_found : public _custom_exception
  {
  public:
    data_not_found() : _custom_exception("Data not found") {}
    data_not_found(const std::string &msg) : _custom_exception("Data not found: " + msg) {}
  };

  class data_already_exists : public _custom_exception
  {
  public:
    data_already_exists() : _custom_exception("Data already exists") {}
    data_already_exists(const std::string &msg) : _custom_exception("Data not found: " + msg) {}
  };

  template <typename Data_t, typename less = less<Data_t>>
  class tree
  {
  public:
    tree();
    tree(std::initializer_list<Data_t> list);
    ~tree();

    // empty out the tree
    void clear();
    // returns the size of the tree
    inline size_t size() const;
    // return true if the tree doesn't have any elements
    inline bool empty() const;
    // return the tree's height
    inline ssize_t height() const;

    // returns a const reference to the data, throws data_not_found in case data not found
    inline const Data_t &search(const Data_t &data) const;
    // returns a reference to the data, use with caution
    inline Data_t &search(const Data_t &data);

    // inserts the data, throws data_already_exists in case data is already in
    inline void insert(const Data_t &data);
    // removes the data, throws data_not_found in case data not found
    inline void remove(const Data_t &data);

    class _Node
    {
      Data_t __data;
      int __height;
      _Node *__parent, *__left, *__right;

      _Node(const Data_t &data)
          : __data(data),
            __height(0),
            __parent(nullptr),
            __left(nullptr),
            __right(nullptr) {}

      friend class tree; // so avl::tree can access the private members of avl::tree::_Node
    };

  private:
    _Node *__root;
    _Node *__min_element;
    _Node *__max_element;
    size_t __size;

    // * helper methods

    // -*- general tree helper methods -*- //

    bool _insert_aux(const Data_t &data);
    bool _remove_aux(const Data_t &data);
    void _clear_aux();

    size_t _destroy_tree_iter(_Node **root);
    size_t _destroy_tree_rec(_Node **root);
    inline size_t _destroy_tree(_Node **root) { return _destroy_tree_rec(root); }

    std::pair<_Node *, _Node **> _search_place_aux(const Data_t &data);
    const _Node *_search_aux(const Data_t &data) const;

    _Node **_get_node_pptr(_Node *node_ptr);

    ssize_t _get_tree_height_iter(_Node &node) const;
    ssize_t _get_tree_height_rec(_Node *node) const;
    ssize_t _get_tree_height(_Node &node) const { return _get_tree_height_rec(&node); }

    ssize_t _get_tree_size_iter(_Node *node) const;
    ssize_t _get_tree_size_rec(_Node *node) const;
    ssize_t _get_tree_size(_Node *node) const { return _get_tree_size_rec(node); }

    inline void _swap_nodes(_Node **a, _Node **b);

#ifdef AVL_TREE_TEST
  public:
    void _print_tree() const;
    void _print_tree_aux(_Node *root, size_t indentation) const;

  private:
#endif // AVL_TREE_TEST

    // -*- tree with specifications - helper methods -*- //

    // requires a height field
    inline size_t _get_tree_height_from_children(_Node &node) const;
    // requires a *min field
    _Node *_find_min() const;
    // requires a *max field
    _Node *_find_max() const;

    // -*- AVL specific - helper methods -*- //

    inline ssize_t _get_balance_factor(_Node *node) const;
    inline void _balance_to_root(_Node **node);
    inline void _rotate_right(_Node **node);
    inline void _rotate_left(_Node **node);
    inline void _rotate_LL(_Node **node);
    inline void _rotate_LR(_Node **node);
    inline void _rotate_RL(_Node **node);
    inline void _rotate_RR(_Node **node);

#ifdef AVL_TREE_TEST
  public:
    bool _validate() const;
    void _validate_aux(_Node *root) const;
    void _validate_min_element() const;
    _Node *_validate_min_element_aux(_Node *node) const;
    void _validate_max_element() const;
    _Node *_validate_max_element_aux(_Node *node) const;
    void _validate_data_order() const;
    void _validate_data_order_aux(_Node *node) const;

  private:
#endif // AVL_TREE_TEST
  };

}; // namespace avl

#endif // __AVL_TREE_H__