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
  struct def_less
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
    data_already_exists(const std::string &msg) : _custom_exception("Data already exists: " + msg) {}
  };

  class bad_input : public _custom_exception
  {
  public:
    bad_input() : _custom_exception("Invalid input") {}
    bad_input(const std::string &msg) : _custom_exception("Invalid input: " + msg) {}
  };

  template <typename Data_t, typename less = def_less<Data_t>>
  class tree
  {
  private:
    class _Node;
    _Node *__root;
    _Node *__min_element;
    _Node *__max_element;
    size_t __size;

    inline void set_root(_Node *new_root) { __root = new_root; }
    inline void set_size(size_t new_size) { __size = new_size; }
    inline void set_max_element(_Node *new_max) { __max_element = new_max; }
    inline void set_min_element(_Node *new_min) { __min_element = new_min; }

  public:
    tree();                                   // c'tor
    tree(std::initializer_list<Data_t> list); // list c'tor
    tree(tree &other);                        // copy c'tor
    tree(tree &&other);                       // move c'tor
    tree &operator=(const tree &other);       // copy assignment operator
    tree &operator=(const tree &&other);      // move assignment operator
    ~tree();                                  // d'tor

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
    // returns a reference to the data, use with caution, only implemented for flexibility
    inline Data_t &search(const Data_t &data);

    // inserts the data, throws data_already_exists in case data is already in
    inline void insert(const Data_t &data);
    // removes the data, throws data_not_found in case data not found
    inline void remove(const Data_t &data);
    // unites 2 trees into 1 in linear time
    inline static tree unite(const tree &t1, const tree &t2);
    // unites 2 trees into 1 in linear time
    inline static tree unite(tree &&t1, tree &&t2);

    // iterator
    class iterator;
    iterator begin() { return iterator(__root); }
    iterator end() { return iterator(nullptr); }

    // const iterator
    class const_iterator;
    const_iterator begin() const { return const_iterator(__root); }
    const_iterator end() const { return const_iterator(nullptr); }

  private:
    // private iterator
    class _iterator;
    _iterator _begin() { return _iterator(__root); }
    _iterator _end() { return _iterator(nullptr); }
    // private const iterator
    class _const_iterator;
    _const_iterator _begin() const { return _const_iterator(__root); }
    _const_iterator _end() const { return _const_iterator(nullptr); }

    // * helper methods

    // -*- general tree helper methods -*- //

    bool _insert_aux(const Data_t &data);
    bool _remove_aux(const Data_t &data);
    void _clear_aux();

    std::pair<_Node *, _Node **> _search_place_aux(const Data_t &data);
    std::pair<const _Node *, const _Node *const *> _search_place_aux(const Data_t &data) const;
    const _Node *_search_aux(const Data_t &data) const;
    _Node *_search_aux(const Data_t &data);

    size_t _destroy_tree_iter(_Node **root);
    size_t _destroy_tree_rec(_Node **root);
    inline size_t _destroy_tree(_Node **root) { return _destroy_tree_rec(root); }

    _Node *_copy_tree_iter(_Node *root);
    _Node *_copy_tree_rec(_Node *root);
    inline _Node *_copy_tree(_Node *root) { return _copy_tree_rec(root); }

    ssize_t _get_tree_height_iter(_Node &node) const;
    ssize_t _get_tree_height_rec(_Node *node) const;
    ssize_t _get_tree_height(_Node &node) const { return _get_tree_height_rec(&node); }

    ssize_t _get_tree_size_iter(_Node *node) const;
    ssize_t _get_tree_size_rec(_Node *node) const;
    ssize_t _get_tree_size(_Node *node) const { return _get_tree_size_rec(node); }

    inline static _Node *_get_left_most_node(_Node *node);
    inline static _Node *_get_right_most_node(_Node *node);

    inline void _swap_nodes(_Node **a, _Node **b);

    inline _Node **_get_node_pptr(_Node *node_ptr);

    inline static _Node *_create_almost_full_tree(const Data_t **data_ptr, size_t size);
    inline static _Node *_create_almost_full_tree_in_place(_Node **data_ptr, size_t size);

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

  template <typename Data_t, typename less>
  class avl::tree<Data_t, less>::_Node
  {
  private:
    friend class tree;            // so avl::tree can access the private members of avl::tree::_Node
    friend class _iterator;       // so avl::tree::_iterator can access the private members of avl::tree::_Node
    friend class _const_iterator; // so avl::tree::_const_iterator can access the private members of avl::tree::_Node
    template <typename T, typename L>
    friend tree<T, L> unite(tree<T, L> &&t1, tree<T, L> &&t2);

    Data_t __data;
    int __height;
    _Node *__parent, *__left, *__right;

    _Node(const Data_t &data)
        : __data(data),
          __height(0),
          __parent(nullptr),
          __left(nullptr),
          __right(nullptr) {}

    Data_t &get_data() { return __data; }
    const Data_t &get_data() const { return __data; }
  };

  template <typename Data_t, typename less>
  class avl::tree<Data_t, less>::iterator
  {
  private:
    friend class tree; // so avl::tree can access the private members of avl::tree::iterator
    inline explicit iterator(_Node *root) : current(_get_left_most_node(root)) {}

  public:
    inline const Data_t &operator*() const { return current->__data; }

    inline const Data_t &operator->() const { return current->__data; }

    inline iterator &operator++()
    {
      ++current;
      return *this;
    }

    inline iterator operator++(int)
    {
      iterator it = *this;
      ++(*this);
      return it;
    }

    inline bool operator!=(const iterator &other) const { return current != other.current; }

  private:
    _iterator current;
  };

  template <typename Data_t, typename less>
  class avl::tree<Data_t, less>::const_iterator
  {
  private:
    friend class tree; // so avl::tree can access the private members of avl::tree::const_iterator
    inline explicit const_iterator(_Node *root) : current(_get_left_most_node(root)) {}

  public:
    inline const Data_t &operator*() const { return current->__data; }

    inline const Data_t &operator->() const { return current->__data; }

    inline const_iterator &operator++()
    {
      ++current;
      return *this;
    }

    inline const_iterator operator++(int)
    {
      iterator it = *this;
      ++(*this);
      return it;
    }

    inline bool operator!=(const const_iterator &other) const { return current != other.current; }

  private:
    _const_iterator current;
  };

  template <typename Data_t, typename less>
  class avl::tree<Data_t, less>::_iterator
  {
  private:
    friend class tree; // so avl::tree can access the private members of avl::tree::_iterator
    inline explicit _iterator(_Node *root) : current(_get_left_most_node(root)) {}

  public:
    inline _Node *operator*() const { return current; }

    inline _Node *operator->() const { return current; }

    inline _iterator &operator++()
    {
      if (current->__right)
      {
        current = _get_left_most_node(current->__right);
      }
      else
      {
        _Node *p = current->__parent;
        while (p && current == p->__right)
        {
          current = p;
          p = p->__parent;
        }
        current = p;
      }
      return *this;
    }

    inline _iterator operator++(int)
    {
      _iterator it = *this;
      ++(*this);
      return it;
    }

    inline bool operator!=(const _iterator &other) const { return current != other.current; }

  private:
    _Node *current;
  };

  template <typename Data_t, typename less>
  class avl::tree<Data_t, less>::_const_iterator
  {
  private:
    friend class tree; // so avl::tree can access the private members of avl::tree::_const_iterator
    inline explicit _const_iterator(_Node *root) : current(_get_left_most_node(root)) {}

  public:
    inline const _Node *operator*() const { return current; }

    inline const _Node *operator->() const { return current; }

    inline _const_iterator &operator++()
    {
      if (current->__right)
      {
        current = _get_left_most_node(current->__right);
      }
      else
      {
        _Node *p = current->__parent;
        while (p && current == p->__right)
        {
          current = p;
          p = p->__parent;
        }
        current = p;
      }
      return *this;
    }

    inline _const_iterator operator++(int)
    {
      _const_iterator it = *this;
      ++(*this);
      return it;
    }

    inline bool operator!=(const _const_iterator &other) const { return current != other.current; }

  private:
    _Node *current;
  };

  template <typename Data_t, typename less>
  tree<Data_t, less>::tree()
      : __root(nullptr),
        __min_element(nullptr),
        __max_element(nullptr),
        __size(0)
  {
  }

  template <typename Data_t, typename less>
  tree<Data_t, less>::tree(std::initializer_list<Data_t> list)
      : __root(nullptr),
        __min_element(nullptr),
        __max_element(nullptr),
        __size(0)
  {
    // basic implementation, could be better
    for (const Data_t &data : list)
    {
      if (_insert_aux(data) == false) // problem in inserting
      {
        this->_destroy_tree(&__root); // clear the tree
        throw bad_input("inserting failed.");
      }
    }
    __size = list.size();
    __min_element = _find_min();
    __max_element = _find_max();
  }

  template <typename Data_t, typename less>
  tree<Data_t, less>::tree(tree &other)
  {
    this->__root = _copy_tree(other->__root);

    this->__size = other->__size;
    this->__max_element = _find_max(this->__root);
    this->__min_element = _find_min(this->__root);
  }

  template <typename Data_t, typename less>
  tree<Data_t, less>::tree(tree &&other)
  {
    this->__root = other.__root;
    other.__root = nullptr;

    this->__size = other.__size;
    other.__size = 0;

    this->__max_element = other.__max_element;
    other.__max_element = nullptr;

    this->__min_element = other.__min_element;
    other.__min_element = nullptr;
  }

  template <typename Data_t, typename less>
  tree<Data_t, less> &tree<Data_t, less>::operator=(const tree<Data_t, less> &other)
  {
    if (this != &other)
    {
      // copy the tree first so in case the copying failed (due to insufficient memory) this object will still be valid
      _Node *new_root = _copy_tree(other.__root);
      
      this->clear();

      this->__root = new_root;
      this->__size = other.__size;
      this->__max_element = _find_max(new_root);
      this->__min_element = _find_min(new_root);
    }
    return *this;
  }

  template <typename Data_t, typename less>
  tree<Data_t, less> &tree<Data_t, less>::operator=(const tree<Data_t, less> &&other)
  {
    if (this != &other)
    {
      if (this->__root)
      {
        _destroy_tree(this->__root);
      }
      this->__root = other.__root;
      this->__size = other.__size;
      this->__max_element = other.__max_element;
      this->__min_element = other.__min_element;
      other.__root = nullptr;
      other.__size = 0;
      other.__max_element = nullptr;
      other.__min_element = nullptr;
    }
    return *this;
  }

  template <typename Data_t, typename less>
  tree<Data_t, less>::~tree()
  {
    _clear_aux();
  }

  template <typename Data_t, typename less>
  void tree<Data_t, less>::clear()
  {
    _clear_aux();
  }

  template <typename Data_t, typename less>
  size_t tree<Data_t, less>::size() const
  {
    return __size;
  }

  template <typename Data_t, typename less>
  bool tree<Data_t, less>::empty() const
  {
#ifdef AVL_TREE_TEST
    bool empty = __root == nullptr;
    if (empty)
    {
      assert(__size == 0);
    }
    else
    {
      assert(__size != 0);
    }
#endif
    return __root == nullptr;
  }

  template <typename Data_t, typename less>
  inline ssize_t tree<Data_t, less>::height() const
  {
    return __root ? _get_tree_height_from_children(*__root) : -1;
  }

  template <typename Data_t, typename less>
  const Data_t &tree<Data_t, less>::search(const Data_t &data) const
  {
    return _search_aux(data);
  }

  template <typename Data_t, typename less>
  Data_t &tree<Data_t, less>::search(const Data_t &data)
  {
    return _search_aux(data);
  }

  template <typename Data_t, typename less>
  void tree<Data_t, less>::insert(const Data_t &data)
  {
    if (_insert_aux(data)) // insert successful
    {
      __size++;
      __min_element = _find_min();
      __max_element = _find_max();
    }
    else
    {
      throw data_already_exists();
    }
  }

  template <typename Data_t, typename less>
  void tree<Data_t, less>::remove(const Data_t &data)
  {
    if (_remove_aux(data)) // deletion successful
    {
      __size--;
      if (this->empty())
      {
        __min_element = nullptr;
        __max_element = nullptr;
      }
      else
      {
        __min_element = _find_min();
        __max_element = _find_max();
      }
    }
    else
    {
      throw data_not_found();
    }
  }

  template <typename Data_t, typename less>
  tree<Data_t, less> tree<Data_t, less>::unite(const tree<Data_t, less> &t1, const tree<Data_t, less> &t2)
  {
    tree<Data_t, less> united_tree;

    size_t size_sum = t1.size() + t2.size();
    // avoid new[](0) which has undefined behaviour
    if (size_sum > 0)
    {
      // allocate space for pointers to ordered data (in-order)
      const Data_t **data_ptr = new const Data_t *[size_sum];

      auto it1 = t1.begin();
      auto it2 = t2.begin();
      auto end1 = t1.end();
      auto end2 = t2.end();

      size_t index = 0;

      while ((it1 != end1) && (it2 != end2))
      {
        if (less()(*it1, *it2)) // *it1 < *it2
        {
          data_ptr[index] = &(*it1); // *it1 returns Data_t
          ++it1;
        }
        else if (less()(*it2, *it1)) // *it1 > *it2
        {
          data_ptr[index] = &(*it2); // *it2 returns Data_t
          ++it2;
        }
        else // *t1 == *t2
        {
          data_ptr[index] = &(*it1); // *it1 returns Data_t
          ++it1;
          ++it2; // duplicates will be copied 1 time only
        }
        ++index;
      }

      while (it1 != end1)
      {
        data_ptr[index] = &(*it1); // *it1 returns Data_t
        ++it1;
        ++index;
      }

      while (it2 != end2)
      {
        data_ptr[index] = &(*it2); // *it1 returns Data_t
        ++it2;
        ++index;
      }

      // data_ptr now holds pointers to the ordered data in both trees
      // index is the amount of (non-duplicates) data in the *new* tree, index > 0 guaranteed

      // we now need to build an almost complete binary tree using the array method
      // an almost complete binary (search) tree is considered an AVL tree since it abides by it's rules
      united_tree.set_root(tree::_create_almost_full_tree(data_ptr, index));
      united_tree.set_size(index);
      united_tree.set_max_element(united_tree._find_max());
      united_tree.set_min_element(united_tree._find_min());

      delete[] data_ptr;
    }
    return united_tree;
  }

  template <typename Data_t, typename less>
  tree<Data_t, less> tree<Data_t, less>::unite(tree<Data_t, less> &&t1, tree<Data_t, less> &&t2)
  {
    tree<Data_t, less> united_tree;

    size_t size_sum = t1.size() + t2.size();
    // avoid new[](0) which has undefined behaviour
    if (size_sum > 0)
    {
      // allocate space for pointers to ordered data (in-order)
      typename tree<Data_t, less>::_Node **node_ptr = new typename tree<Data_t, less>::_Node *[size_sum];

      auto it1 = t1._begin();
      auto it2 = t2._begin();
      auto end1 = t1._end();
      auto end2 = t2._end();

      size_t index = 0;

      while ((it1 != end1) && (it2 != end2))
      {
        if (less()((*it1)->get_data(), (*it2)->get_data())) // *it1 < *it2
        {
          node_ptr[index] = *it1; // *it1 returns *Data_t
          ++it1;
        }
        else if (less()((*it2)->get_data(), (*it1)->get_data())) // *it1 > *it2
        {
          node_ptr[index] = *it2; // *it2 returns *Data_t
          ++it2;
        }
        else // *t1 == *t2
        {
          node_ptr[index] = *it1; // *it1 returns *Data_t
          ++it1;
          ++it2; // duplicates will be copied 1 time only
        }
        ++index;
      }

      while (it1 != end1)
      {
        node_ptr[index] = *it1; // *it1 returns *Data_t
        ++it1;
        ++index;
      }

      while (it2 != end2)
      {
        node_ptr[index] = *it2; // *it1 returns *Data_t
        ++it2;
        ++index;
      }

      // clean up the trees so no accidental deletion of memory takes place
      t1.set_root(nullptr);
      t1.set_size(0);
      t1.set_max_element(nullptr);
      t1.set_min_element(nullptr);
      t2.set_root(nullptr);
      t2.set_size(0);
      t2.set_max_element(nullptr);
      t2.set_min_element(nullptr);

      // node_ptr now holds pointers to the ordered data in both trees
      // index is the amount of (non-duplicates) data in the *new* tree, index > 0 guaranteed

      // we now need to build an almost complete binary tree using the array method
      // an almost complete binary (search) tree is considered an AVL tree since it abides by it's rules
      united_tree.set_root(tree::_create_almost_full_tree_in_place(node_ptr, index));
      united_tree.set_size(index);
      united_tree.set_max_element(united_tree._find_max());
      united_tree.set_min_element(united_tree._find_min());

      delete[] node_ptr;
    }
    return united_tree;
  }

  // * helper methods

  // -*- general tree helper methods -*- //

  template <typename Data_t, typename less>
  bool tree<Data_t, less>::_insert_aux(const Data_t &data)
  {
    std::pair<_Node *, _Node **> parent_data_pair = _search_place_aux(data);
    _Node *parent_ptr = parent_data_pair.first;
    _Node **data_pptr = parent_data_pair.second;

    assert(data_pptr != nullptr);
    if (*data_pptr)
    {
      return false;
    }

    /* AVL tree specific implementation */

    /* adding node logic */
    *data_pptr = new _Node(data);
    (*data_pptr)->__parent = parent_ptr;

    _balance_to_root(data_pptr);

    return true;
  }

  template <typename Data_t, typename less>
  bool tree<Data_t, less>::_remove_aux(const Data_t &data)
  {

    std::pair<_Node *, _Node **> parent_data_pair = _search_place_aux(data);
    _Node *parent_ptr = parent_data_pair.first;
    _Node **data_pptr = parent_data_pair.second;

    assert(data_pptr != nullptr);
    if (*data_pptr == nullptr)
    {
      /* nothing to remove */
      return false;
    }

    /* removing node logic */
    /* AVL tree specific implementation */

    _Node **node_pptr = data_pptr;
    bool left_exists = (*data_pptr)->__left != nullptr;
    bool right_exists = (*data_pptr)->__right != nullptr;

    if (left_exists && right_exists) /* both children */
    {
      parent_ptr = (*node_pptr);
      node_pptr = &((*data_pptr)->__right);

      while ((*node_pptr)->__left)
      {
        parent_ptr = (*node_pptr);
        node_pptr = &((*node_pptr)->__left);
      }

      _swap_nodes(node_pptr, data_pptr);

      _Node *to_delete = *node_pptr;

      *node_pptr = (*node_pptr)->__right;
      if (*node_pptr)
      {
        // update the parent of the child of the node to delete
        (*node_pptr)->__parent = (*node_pptr)->__parent->__parent;
      }

      node_pptr = _get_node_pptr(parent_ptr);

      delete to_delete;
    }
    else
    {
      _Node *to_delete = *node_pptr;

      if (left_exists) /* only the left child */
      {
        *node_pptr = (*node_pptr)->__left;
      }
      else /* no children - or - only the right child */
      {
        *node_pptr = (*node_pptr)->__right;
      }

      if (*node_pptr)
      {
        (*node_pptr)->__parent = parent_ptr;
      }

      node_pptr = _get_node_pptr(parent_ptr);

      delete to_delete;
    }

    _balance_to_root(node_pptr);

    return true;
  }

  template <typename Data_t, typename less>
  void tree<Data_t, less>::_clear_aux()
  {
    __size -= _destroy_tree(&__root);
#ifdef AVL_TREE_TEST
    assert(__size == 0);
#endif
  }

  template <typename Data_t, typename less>
  std::pair<const typename tree<Data_t, less>::_Node *, const typename tree<Data_t, less>::_Node *const *> tree<Data_t, less>::_search_place_aux(const Data_t &data) const
  {
    const _Node *parent_ptr = nullptr;
    const _Node *const *curr_pptr = &__root;
    while (*curr_pptr)
    {
      // update the node
      if (less()(data, (*curr_pptr)->__data))
      {
        // go left, data < current data
        parent_ptr = *curr_pptr;
        curr_pptr = &((*curr_pptr)->__left);
      }
      else if (less()((*curr_pptr)->__data, data))
      {
        // go right, current data < data
        parent_ptr = *curr_pptr;
        curr_pptr = &((*curr_pptr)->__right);
      }
      else
      {
        // data is in node, so the parent is the node and
        // curr is a pointer to that pointer
        break;
      }
    }
    return {parent_ptr, curr_pptr};
  }

  template <typename Data_t, typename less>
  std::pair<typename tree<Data_t, less>::_Node *, typename tree<Data_t, less>::_Node **> tree<Data_t, less>::_search_place_aux(const Data_t &data)
  {
    _Node *parent_ptr = nullptr;
    _Node **curr_pptr = &__root;
    while (*curr_pptr)
    {
      // update the node
      if (less()(data, (*curr_pptr)->__data))
      {
        // go left, data < current data
        parent_ptr = *curr_pptr;
        curr_pptr = &((*curr_pptr)->__left);
      }
      else if (less()((*curr_pptr)->__data, data))
      {
        // go right, current data < data
        parent_ptr = *curr_pptr;
        curr_pptr = &((*curr_pptr)->__right);
      }
      else
      {
        // data is in node, so the parent is the node and
        // curr is a pointer to that pointer
        break;
      }
    }
    return {parent_ptr, curr_pptr};
  }

  template <typename Data_t, typename less>
  const typename tree<Data_t, less>::_Node *tree<Data_t, less>::_search_aux(const Data_t &data) const
  {
    _Node *ptr = *(_search_place_aux(data).second);
    if (ptr == nullptr)
    {
      throw data_not_found();
    }
    return ptr->__data;
  }

  template <typename Data_t, typename less>
  typename tree<Data_t, less>::_Node *tree<Data_t, less>::_search_aux(const Data_t &data)
  {
    _Node *ptr = *(_search_place_aux(data).second);
    if (ptr == nullptr)
    {
      throw data_not_found();
    }
    return ptr->__data;
  }

  template <typename Data_t, typename less>
  size_t tree<Data_t, less>::_destroy_tree_iter(typename tree<Data_t, less>::_Node **root)
  {
    // TODO: implement this iteratively
    return 0;
  }

  template <typename Data_t, typename less>
  size_t tree<Data_t, less>::_destroy_tree_rec(typename tree<Data_t, less>::_Node **root)
  {
    if (*root == nullptr)
    {
      return 0;
    }
    size_t amount = 1 + _destroy_tree_rec(&((*root)->__left)) + _destroy_tree_rec(&((*root)->__right));
    delete *root;
    *root = nullptr;
    return amount;
  }

  template <typename Data_t, typename less>
  typename tree<Data_t, less>::_Node *tree<Data_t, less>::_copy_tree_iter(typename tree<Data_t, less>::_Node *root)
  {
    // TODO: implement this iteratively
    return nullptr;
  }

  template <typename Data_t, typename less>
  typename tree<Data_t, less>::_Node *tree<Data_t, less>::_copy_tree_rec(typename tree<Data_t, less>::_Node *root)
  {
    if (root == nullptr)
    {
      return nullptr;
    }

    _Node *node = new _Node(root->__data);

    node->__left = _copy_tree_rec(&((*root)->__left));
    if (node->__left)
    {
      node->__left->__parent = root;
    }

    node->__right = _copy_tree_rec(&((*root)->__right));
    if (node->__right)
    {
      node->__right->__parent = root;
    }

    node->__height = root->__height;

    return node;
  }

  template <typename Data_t, typename less>
  ssize_t tree<Data_t, less>::_get_tree_height_iter(typename tree<Data_t, less>::_Node &node) const
  {
    // TODO: implement this iteratively
    return 0;
  }

  template <typename Data_t, typename less>
  ssize_t tree<Data_t, less>::_get_tree_height_rec(typename tree<Data_t, less>::_Node *node) const
  {
    if (!node)
      return -1;
    return 1 + std::max(_get_tree_height_rec(node->__left), _get_tree_height_rec(node->__right));
  }

  template <typename Data_t, typename less>
  ssize_t tree<Data_t, less>::_get_tree_size_iter(_Node *node) const
  {
    // TODO: implement this iteratively
    return 0;
  }

  template <typename Data_t, typename less>
  ssize_t tree<Data_t, less>::_get_tree_size_rec(_Node *node) const
  {
    if (!node)
      return 0;
    return 1 + _get_tree_size_rec(node->__left) + _get_tree_size_rec(node->__right);
  }

  template <typename Data_t, typename less>
  typename tree<Data_t, less>::_Node *tree<Data_t, less>::_get_left_most_node(typename tree<Data_t, less>::_Node *node)
  {
    while (node && node->__left)
      node = node->__left;
    return node;
  }

  template <typename Data_t, typename less>
  typename tree<Data_t, less>::_Node *tree<Data_t, less>::_get_right_most_node(typename tree<Data_t, less>::_Node *node)
  {
    while (node && node->__right)
      node = node->__right;
    return node;
  }

  template <typename Data_t, typename less>
  void tree<Data_t, less>::_swap_nodes(typename tree<Data_t, less>::_Node **a, typename tree<Data_t, less>::_Node **b)
  {
    if (a && *a && b && *b)
    {
      swap((*a)->__data, (*b)->__data);
    }
  }

  template <typename Data_t, typename less>
  typename tree<Data_t, less>::_Node **tree<Data_t, less>::_get_node_pptr(typename tree<Data_t, less>::_Node *node_ptr)
  {
    _Node **pptr = nullptr;
    if (node_ptr)
    {
      if (node_ptr->__parent)
      {
        if (node_ptr->__parent->__left == node_ptr)
        {
          pptr = &(node_ptr->__parent->__left);
        }
        else // (node_ptr->__parent->__right == node_ptr)
        {
          pptr = &(node_ptr->__parent->__right);
        }
      }
      else // root
      {
        pptr = &__root;
      }
    }
    return pptr;
  }

  template <typename Data_t, typename less>
  typename tree<Data_t, less>::_Node *tree<Data_t, less>::_create_almost_full_tree(const Data_t **data_ptr, size_t size)
  {
    if (data_ptr == nullptr || size == 0)
    {
      return nullptr;
    }
    else if (size == 1)
    {
      return new typename tree<Data_t, less>::_Node(*(data_ptr[0]));
    }
    else // size >= 2
    {
      size_t tree_height = static_cast<size_t>(std::log2(size));                                               // since (size >= 2) then (tree_height >= 1)
      size_t tree_leaves = size - (2 << (tree_height - 1)) + 1;                                                // n - 2^(h-1) - 1
      size_t left_size = (1 << (tree_height - 1)) - 1 + std::min<size_t>(tree_leaves, 1 << (tree_height - 1)); // 2^(h-1) - 1 + min( tree leaves , 2^(h-1) )
      size_t right_size = size - left_size - 1;

      typename tree<Data_t, less>::_Node *node = new typename tree<Data_t, less>::_Node(*(data_ptr[left_size]));
      size_t node_height = 0;

      node->__left = _create_almost_full_tree(data_ptr, left_size);
      if (node->__left)
      {
        node->__left->__parent = node;
        node_height += node->__left->__height;
      }

      node->__right = _create_almost_full_tree(data_ptr + left_size + 1, right_size);
      if (node->__right)
      {
        node->__right->__parent = node;
        node_height += node->__right->__height;
      }

      node->__height = node_height;

      return node;
    }
  }

  template <typename Data_t, typename less>
  typename tree<Data_t, less>::_Node *tree<Data_t, less>::_create_almost_full_tree_in_place(typename tree<Data_t, less>::_Node **node_ptr, size_t size)
  {
    if (node_ptr == nullptr || size == 0)
    {
      return nullptr;
    }
    else if (size == 1)
    {
      return new typename tree<Data_t, less>::_Node(*(node_ptr[0]));
    }
    else // size >= 2
    {
      size_t tree_height = static_cast<size_t>(std::log2(size));                                               // since (size >= 2) then (tree_height >= 1)
      size_t tree_leaves = size - (2 << (tree_height - 1)) + 1;                                                // n - 2^(h-1) - 1
      size_t left_size = (1 << (tree_height - 1)) - 1 + std::min<size_t>(tree_leaves, 1 << (tree_height - 1)); // 2^(h-1) - 1 + min( tree leaves , 2^(h-1) )
      size_t right_size = size - left_size - 1;

      typename tree<Data_t, less>::_Node *node = node_ptr[left_size];
      size_t node_height = 0;

      node->__left = _create_almost_full_tree_in_place(node_ptr, left_size);
      if (node->__left)
      {
        node->__left->__parent = node;
        node_height += node->__left->__height;
      }

      node->__right = _create_almost_full_tree_in_place(node_ptr + left_size + 1, right_size);
      if (node->__right)
      {
        node->__right->__parent = node;
        node_height += node->__right->__height;
      }

      node->__height = node_height;

      return node;
    }
  }

#ifdef AVL_TREE_TEST

  template <typename Data_t, typename less>
  void tree<Data_t, less>::_print_tree() const
  {
    std::cout << "printing tree:" << std::endl;
    std::cout << "size = " << size() << std::endl;
    std::cout << "height = " << ((__root) ? (_get_tree_height(*__root)) : (-1)) << std::endl;
    _print_tree_aux(__root, 0);
  }

  template <typename Data_t, typename less>
  void tree<Data_t, less>::_print_tree_aux(typename tree<Data_t, less>::_Node *root, size_t indentation) const
  {
    if (root)
    {
      const std::string displace(7, ' ');
      _print_tree_aux(root->__right, indentation + 1);

      std::cout << std::endl;
      for (size_t i = 0; i < indentation; i++)
      {
        std::cout << displace;
      }
      std::cout << "*-" << root->__height << "-[ " << root->__data << " ]";
      std::cout << std::endl;

      _print_tree_aux(root->__left, indentation + 1);
    }
    return;
  }

#endif // AVL_TREE_TEST

  template <typename Data_t, typename less>
  size_t tree<Data_t, less>::_get_tree_height_from_children(_Node &node) const
  {
    return std::max(
        (node.__left) ? (1 + node.__left->__height) : (0),
        (node.__right) ? (1 + node.__right->__height) : (0));
  }

  template <typename Data_t, typename less>
  typename tree<Data_t, less>::_Node *tree<Data_t, less>::_find_min() const
  {
    _Node *temp = __root;
    if (temp == nullptr)
      throw data_not_found();
    while (temp->__left != nullptr)
    {
      temp = temp->__left;
    }
    return temp;
  }

  template <typename Data_t, typename less>
  typename tree<Data_t, less>::_Node *tree<Data_t, less>::_find_max() const
  {
    _Node *temp = __root;
    if (temp == nullptr)
      throw data_not_found();
    while (temp->__right != nullptr)
    {
      temp = temp->__right;
    }
    return temp;
  }

  template <typename Data_t, typename less>
  ssize_t tree<Data_t, less>::_get_balance_factor(typename tree<Data_t, less>::_Node *node) const
  {
    if (node == nullptr)
    {
      return -1;
    }
    return ((node->__left) ? (node->__left->__height) : (-1)) - ((node->__right) ? (node->__right->__height) : (-1));
  }

  template <typename Data_t, typename less>
  void tree<Data_t, less>::_balance_to_root(typename tree<Data_t, less>::_Node **node_pptr)
  {
    if (node_pptr == nullptr || *node_pptr == nullptr)
    {
      return;
    }

    _Node *temp = *node_pptr;
    while (temp)
    {
      // update the node values traversing up the tree
      temp->__height = _get_tree_height_from_children(*temp);

      // rotate if needed
      int curr_bf = _get_balance_factor(temp);
      int left_bf = _get_balance_factor(temp->__left);
      int right_bf = _get_balance_factor(temp->__right);
      if (curr_bf >= 2 && left_bf >= 0) // LL
      {
        _rotate_LL(node_pptr);
      }
      else if (curr_bf >= 2 && left_bf == -1) // LR
      {
        _rotate_LR(node_pptr);
      }
      else if (curr_bf <= -2 && right_bf == 1) // RL
      {
        _rotate_RL(node_pptr);
      }
      else if (curr_bf <= -2 && right_bf <= 0) // RR
      {
        _rotate_RR(node_pptr);
      }
      /**
       * these were problematic lines, we changed it with this
       * node_pptr = &(temp->__parent);
       * temp = temp->__parent;
       * so they had to be changed to this
       */
      temp = temp->__parent;
      if (temp)
      {
        if (temp->__parent == nullptr) // temp is root
        {
          node_pptr = &(__root);
        }
        else
        {
          if (temp == temp->__parent->__left)
          {
            node_pptr = &(temp->__parent->__left);
          }
          else // temp == temp->__parent->__right
          {
            node_pptr = &(temp->__parent->__right);
          }
        }
      }
    }
  }

  /*
   *
   *          A      |        B
   *        /   \    |      /   \
   *       B    Ar   |    Bl     A
   *     /   \       |         /   \
   *    Bl   Br      |        Br   Ar
   *
   */
  template <typename Data_t, typename less>
  void tree<Data_t, less>::_rotate_right(typename tree<Data_t, less>::_Node **node)
  {
    if (!node || !(*node) || !(*node)->__left)
    {
      // if node doesn't have a left child (nothing to right rotate)
      return;
    }

    // save some pointers
    _Node *A_ptr = (*node);
    _Node *B_ptr = A_ptr->__left;
    _Node *Br_ptr = B_ptr->__right;

    // change the children
    (*node) = B_ptr;        // we lost A, good thing we have A_ptr
    B_ptr->__right = A_ptr; // we lost Br, good thing we have Br_ptr
    A_ptr->__left = Br_ptr; // A's left pointed to B, we change that into Br

    // change the parents
    B_ptr->__parent = A_ptr->__parent; // the only way to retrieve original A's parent
    B_ptr->__right->__parent = B_ptr;
    if (Br_ptr)
      Br_ptr->__parent = A_ptr;

    // update the heights (of the sub tree of the node)
    // Bl Br Ar didn't have their heights change
    A_ptr->__height = _get_tree_height_from_children(*A_ptr);
    B_ptr->__height = _get_tree_height_from_children(*B_ptr);

    // if the rotating node is the root, change the tree root accordingly
    // B may be the root now (already switched parents with A)
    if (B_ptr->__parent == nullptr)
      this->__root = B_ptr;

    // if other data should be update, update them here

    return;
  }

  /*
   *
   *        A        |        B
   *      /   \      |      /   \
   *    Al     B     |     A     Br
   *         /   \   |   /   \
   *        Bl   Br  |  Al   Bl
   *
   */
  template <typename Data_t, typename less>
  void tree<Data_t, less>::_rotate_left(typename tree<Data_t, less>::_Node **node)
  {
    if (node == nullptr || *node == nullptr || (*node)->__right == nullptr)
    {
      // if node doesn't have a right child (nothing to right rotate)
      return;
    }

    // save some pointers
    _Node *A_ptr = (*node);
    _Node *B_ptr = A_ptr->__right;
    _Node *Bl_ptr = B_ptr->__left;

    // change the children
    (*node) = B_ptr;         // in this line we lost A, good thing we have A_ptr
    B_ptr->__left = A_ptr;   // in this line we lost Bl, good thing we have Bl_ptr
    A_ptr->__right = Bl_ptr; // A's right pointed to B, we change that into Bl

    // change the parents
    B_ptr->__parent = A_ptr->__parent; // the only way to retrieve original A's parent
    A_ptr->__parent = B_ptr;
    if (Bl_ptr)
      Bl_ptr->__parent = A_ptr;

    // update the heights (of the sub tree of the node)
    // Bl Br Al didn't have their heights change
    A_ptr->__height = _get_tree_height_from_children(*A_ptr);
    B_ptr->__height = _get_tree_height_from_children(*B_ptr);

    // if the rotating node is the root, change the tree root accordingly
    // B may be the root now (already switched parents with A)
    if (B_ptr->__parent == nullptr)
      this->__root = B_ptr;

    // if other data should be update, update them here

    return;
  }

  template <typename Data_t, typename less>
  void tree<Data_t, less>::_rotate_LL(typename tree<Data_t, less>::_Node **node)
  {
    if (node == nullptr || *node == nullptr)
    {
      return;
    }

    if ((*node)->__left == nullptr)
    {
      return;
    }

    // counter intuitive, but thats how it works
    _rotate_right(node);

    return;
  }

  template <typename Data_t, typename less>
  void tree<Data_t, less>::_rotate_LR(typename tree<Data_t, less>::_Node **node_pptr)
  {
    if (node_pptr == nullptr || *node_pptr == nullptr)
    {
      return;
    }

    if ((*node_pptr)->__left == nullptr)
    {
      return;
    }

    // the order of rotation in important
    _rotate_left(&((*node_pptr)->__left));
    _rotate_right(node_pptr);

    return;
  }

  template <typename Data_t, typename less>
  void tree<Data_t, less>::_rotate_RL(typename tree<Data_t, less>::_Node **node_pptr)
  {
    if (node_pptr == nullptr || *node_pptr == nullptr)
    {
      return;
    }

    if ((*node_pptr)->__right == nullptr)
    {
      return;
    }

    // the order of rotation in important
    _rotate_right(&((*node_pptr)->__right));
    _rotate_left(node_pptr);

    return;
  }

  template <typename Data_t, typename less>
  void tree<Data_t, less>::_rotate_RR(typename tree<Data_t, less>::_Node **node)
  {
    if (node == nullptr || *node == nullptr)
    {
      return;
    }

    if ((*node)->__right == nullptr)
    {
      return;
    }

    // counter intuitive, but thats how it works
    _rotate_left(node);

    return;
  }

#ifdef AVL_TREE_TEST

  template <typename Data_t, typename less>
  bool tree<Data_t, less>::_validate() const
  {
    assert(__size == _get_tree_size(__root));
    _validate_min_element();
    _validate_max_element();

    _validate_data_order();

    _validate_aux(__root);
    return true;
  }

  template <typename Data_t, typename less>
  void tree<Data_t, less>::_validate_aux(typename tree<Data_t, less>::_Node *root) const
  {
    if (root == nullptr)
      return;

    // assert that the balance factor is correct is correct
    ssize_t bf = _get_balance_factor(root);
    assert(bf >= -1);
    assert(bf <= 1);

    assert(root->__height == _get_tree_height(*root));

    // if it has a right child
    if (root->__right)
    {
      // assert the parent of the right child is correct
      assert(root == root->__right->__parent);
      assert(less()(root->__data, root->__right->__data));

      _validate_aux(root->__right);
    }

    // if it has a left child
    if (root->__left)
    {
      // assert the parent of the left child is correct
      assert(root == root->__left->__parent);
      assert(less()(root->__left->__data, root->__data));

      _validate_aux(root->__left);
    }
  }

  template <typename Data_t, typename less>
  void tree<Data_t, less>::_validate_min_element() const
  {
    _validate_min_element_aux(__root);
  }

  template <typename Data_t, typename less>
  typename tree<Data_t, less>::_Node *tree<Data_t, less>::_validate_min_element_aux(typename tree<Data_t, less>::_Node *node) const
  {
    const _Node *min = node;
    if (node)
    {
      // check the left sub tree
      const _Node *left = _validate_min_element_aux(node->__left);

      // check the right sub tree
      const _Node *right = _validate_min_element_aux(node->__right);

      // check the node
    }
    return nullptr;
  }

  template <typename Data_t, typename less>
  void tree<Data_t, less>::_validate_max_element() const
  {
    _validate_max_element_aux(__root);
  }

  template <typename Data_t, typename less>
  typename tree<Data_t, less>::_Node *tree<Data_t, less>::_validate_max_element_aux(typename tree<Data_t, less>::_Node *node) const
  {
    if (node)
    {
      // check the left sub tree
      _validate_max_element_aux(node->__left);

      // check the right sub tree
      _validate_max_element_aux(node->__right);

      // check the node
      if (node->__left)
      {
        assert(less()(node->__left->__data, node->__data));
      }
      if (node->__right)
      {
        assert(less()(node->__data, node->__right->__data));
      }
    }
    return nullptr;
  }

  template <typename Data_t, typename less>
  void tree<Data_t, less>::_validate_data_order() const
  {
    _validate_data_order_aux(__root);
  }

  template <typename Data_t, typename less>
  void tree<Data_t, less>::_validate_data_order_aux(typename tree<Data_t, less>::_Node *node) const
  {
    if (node)
    {
      // check the left sub tree
      _validate_data_order_aux(node->__left);

      // check the right sub tree
      _validate_data_order_aux(node->__right);

      // check the node
      if (node->__left)
      {
        assert(less()(node->__left->__data, node->__data));
      }
      if (node->__right)
      {
        assert(less()(node->__data, node->__right->__data));
      }
    }
  }

#endif // AVL_TREE_TEST

}; // namespace avl

#endif // __AVL_TREE_H__