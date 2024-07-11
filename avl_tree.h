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
    data_already_exists(const std::string &msg) : _custom_exception("Data already exists: " + msg) {}
  };

  class bad_input : public _custom_exception
  {
  public:
    bad_input() : _custom_exception("Invalid input") {}
    bad_input(const std::string &msg) : _custom_exception("Invalid input: " + msg) {}
  };

  template <typename Data_t, typename less = less<Data_t>>
  class tree
  {
  public:
    tree();
    tree(std::initializer_list<Data_t> list);
    tree(tree &other);
    tree(tree &&other);
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
    // returns a reference to the data, use with caution, only implemented for flexibility
    inline Data_t &search(const Data_t &data);

    // inserts the data, throws data_already_exists in case data is already in
    inline void insert(const Data_t &data);
    // removes the data, throws data_not_found in case data not found
    inline void remove(const Data_t &data);
    // unites 2 trees into 1 in linear time
    inline tree unite(tree &t1, tree &t2);

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

    class iterator
    {
    public:
      explicit iterator(_Node *root) : current(_left_most(root)) {}

      const Data_t &operator*() const { return current->__data; }

      iterator &operator++()
      {
        if (current->__right)
        {
          current = _left_most(current->__right);
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

      iterator operator++(int)
      {
        iterator it = *this;
        ++(*this);
        return it;
      }

      bool operator!=(const iterator &other) const { return current != other.current; }

    private:
      _Node *current;

      _Node *_left_most(_Node *node) const
      {
        while (node && node->__left)
          node = node->__left;
        return node;
      }
    };

    iterator begin() { return iterator(__root); }
    iterator end() { return iterator(nullptr); }

    class const_iterator
    {
    public:
      explicit const_iterator(_Node *root) : current(_left_most(root)) {}

      const Data_t &operator*() const { return current->__data; }

      iterator &operator++()
      {
        if (current->__right)
        {
          current = _left_most(current->__right);
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

      iterator operator++(int)
      {
        iterator it = *this;
        ++(*this);
        return it;
      }

      bool operator!=(const const_iterator &other) const { return current != other.current; }

    private:
      _Node *current;

      _Node *_left_most(_Node *node) const
      {
        while (node && node->__left)
          node = node->__left;
        return node;
      }
    };

    const_iterator begin() const { return const_iterator(__root); }
    const_iterator end() const { return const_iterator(nullptr); }

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

    inline void _swap_nodes(_Node **a, _Node **b);

    inline _Node **_get_node_pptr(_Node *node_ptr);

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
    this->__root = other->__root;
    other->__root = nullptr;

    this->__size = other->__size;
    other->__size = 0;

    this->__max_element = other->__max_element;
    other->__max_element = nullptr;
    
    this->__min_element = other->__min_element;  
    other->__min_element = nullptr;
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
  tree<Data_t, less> tree<Data_t, less>::unite(tree<Data_t, less> &t1, tree<Data_t, less> &t2)
  {
    tree avl;
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