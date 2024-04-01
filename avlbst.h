#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node);
    void removeFix(AVLNode<Key,Value>* node, int8_t diff);
    void rotateLeft(AVLNode<Key,Value>* Node);
    void rotateRight(AVLNode<Key,Value>* Node);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    // if the key is already in the tree, overwrite the current value
    AVLNode<Key,Value>* searchNode = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::internalFind(new_item.first));
    if (searchNode != nullptr)
    {
        searchNode->setValue(new_item.second);
        return;
    }
    AVLNode<Key,Value>* newNode = new AVLNode<Key,Value>(new_item.first, new_item.second, nullptr);
    // If empty tree => set n as root, b(n) = 0, done!
    if (this->root_ == nullptr)
    {
        newNode->setBalance(0);
        this->root_ = newNode;
        return; 
    }
    // Else insert n (by walking the tree to a leaf, p, and inserting the 
    // new node as its child), set balance to 0, and look at its parent, p
    else
    {
        AVLNode<Key,Value>* currentNode = static_cast<AVLNode<Key,Value>*>(this->root_);
        AVLNode<Key,Value>* parentNode = nullptr;
        while (currentNode != nullptr)
        {
            parentNode = currentNode;
            if (new_item.first < currentNode->getKey())
            {
                currentNode = currentNode->getLeft();
            }
            // will never equal because it would have been returned already
            else
            {
                currentNode = currentNode->getRight();
            }
        }
        if (parentNode == nullptr)
        {
            this->root_ = newNode;
            newNode->setBalance(0);
            return;
        }
        else if (newNode->getKey() < parentNode->getKey())
        {
            parentNode->setLeft(newNode);
        }
        else if (newNode->getKey() > parentNode->getKey())
        {
            parentNode->setRight(newNode);
        }
        // also need to update the parent
        newNode->setParent(parentNode);
        newNode->setBalance(0);
        // – If b(p) was -1, then b(p) = 0. Done!
        if (parentNode->getBalance() == -1)
        {
            parentNode->setBalance(0);
            return;
        }
        // – If b(p) was +1, then b(p) = 0. Done!
        else if (parentNode->getBalance() == +1)
        {
            parentNode->setBalance(0);
            return;
        }
        // – If b(p) was 0, then update b(p) and call insert-fix(p, n)
        else if (parentNode->getBalance() == 0)
        {
            // if new node is at right of parent, update b(p) plus 1
            if (newNode == parentNode->getRight())
            {
                parentNode->setBalance(1);
            }
            else
            {
                parentNode->setBalance(-1);
            }
            insertFix(parentNode, newNode);
        }
    }
}


template<class Key, class Value>
void AVLTree<Key,Value>::insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node)
{
    //If parent or grandparent are NULL, return
    if (parent == nullptr || parent->getParent() == nullptr)
    {
        return;
    }
    // Update grandparent’s balance (depends on whether parent is left/right child)
    AVLNode<Key,Value>* grandparent = parent->getParent();
    int8_t currentBalance = grandparent->getBalance();
    // if parent is right child of grandparent, and parent added 1 more
    if (parent == grandparent->getRight())
    {
        grandparent->setBalance(currentBalance + 1);
    }
    // parent is left child of grandparent
    else
    {
        grandparent->setBalance(currentBalance - 1);
    }
    // new balance is 0 -> DONE
    if (grandparent->getBalance() == 0)
    {
        return;
    }
    // new balance is +/- 1 -> call insertFix(grandparent, parent)
    else if (grandparent->getBalance() == 1 ||grandparent->getBalance() == -1)
    {
        insertFix(grandparent, parent);
    }
    // new balance is +/- 2
    else
    {
        // Case 3: b(g) == -2
        // Check for zig-zig or zig-zag and perform corresponding rotations
        if (grandparent->getBalance() == -2)
        {
            //        g
            //       /
            //      p
            //     /
            //    n
            if (parent->getLeft() == node)
            {
                // If zig-zig then rotateRight(g); b(p) = b(g) = 0
                rotateRight(grandparent);
                parent->setBalance(0);
                grandparent->setBalance(0);
            }
            //        g
            //       /
            //      p
            //       \
            //        n
            else
            {   // If zig-zag then rotateLeft(p); rotateRight(g); 
                rotateLeft(parent);
                rotateRight(grandparent);
                // Case 3a: b(n) == -1 then b(p) = 0; b(g) = +1; b(n) = 0;
                if (node->getBalance() == -1)
                {
                    parent->setBalance(0);
                    grandparent->setBalance(1);
                    node->setBalance(0);
                }
                // Case 3b: b(n) == 0 then b(p) = 0; b(g) = 0; b(n) = 0;
                else if (node->getBalance() == 0)
                {
                    parent->setBalance(0);
                    grandparent->setBalance(0);
                    node->setBalance(0);
                }
                // Case 3c: b(n) == +1 then b(p)= -1; b(g) = 0; b(n) = 0;
                else if (node->getBalance() == 1)
                {
                    parent->setBalance(-1);
                    grandparent->setBalance(0);
                    node->setBalance(0);
                }
            }
        }
        else
        {
            //        g
            //         \
            //          p
            //           \
            //            n
            if (parent->getRight() == node)
            {
                rotateLeft(grandparent);
                parent->setBalance(0);
                grandparent->setBalance(0);
            }
            //        g
            //         \
            //          p
            //         /
            //        n
            else
            {
                rotateRight(parent);
                rotateLeft(grandparent);
                // Case 3a: b(n) == -1 then b(p) = 0; b(g) = +1; b(n) = 0;
                if (node->getBalance() == 1)
                {
                    parent->setBalance(0);
                    grandparent->setBalance(-1);
                    node->setBalance(0);
                }
                // Case 3b: b(n) == 0 then b(p) = 0; b(g) = 0; b(n) = 0;
                else if (node->getBalance() == 0)
                {
                    parent->setBalance(0);
                    grandparent->setBalance(0);
                    node->setBalance(0);
                }
                // Case 3c: b(n) == +1 then b(p)= -1; b(g) = 0; b(n) = 0;
                else if (node->getBalance() == -1)
                {
                    parent->setBalance(1);
                    grandparent->setBalance(0);
                    node->setBalance(0);
                }
            }
        }
        // // Update balance of node, parent, and grandparent depending on original balance
        // // **************************
        // grandparent->setBalance(0);
        // parent->setBalance(0);
        // node->setBalance(0);
    }
}


/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    // Find the node to remove by walking the tree
    AVLNode<Key,Value>* node = static_cast<AVLNode<Key,Value>*>(BinarySearchTree<Key,Value>::internalFind(key));
    
    if(node == nullptr)
    {
        return;
    }
    // if a node has 2 children, swap with the predecessor
    if (node -> getLeft() != nullptr && node -> getRight() != nullptr)
    {
        AVLNode<Key,Value>* predecessor = static_cast<AVLNode<Key,Value>*>(BinarySearchTree<Key,Value>::predecessor(node));
        nodeSwap(node,predecessor);
    }
    // If the parent of the removed node exists
    AVLNode<Key,Value>* parent = node->getParent();
    int8_t diff = 0;
    if (parent != nullptr)
    {
        // If removed node is left child of its parent, diff = +1
        if (parent->getLeft() == node)
        {
            diff = 1;
        }
        // If removed node is right child of its parent, diff = -1
        else
        {
            diff = -1;
        }
    }
    // Remove the node and update pointers (parents, left/right children)
    AVLNode<Key, Value>* newNode = nullptr;
    if (node->getLeft() != nullptr)
    {
        newNode = node->getLeft();
    }
    else if (node->getRight() != nullptr)
    {
        newNode = node->getRight();
    }
    // parent is not null, then connect node with parent and delete
    if (parent != nullptr)
    {
        if (parent->getLeft() == node)
        {
            parent->setLeft(newNode);
        }
        else
        {
            parent->setRight(newNode);
        }
        if (newNode != nullptr)
        {
            newNode->setParent(parent);
        }
        delete node;
        removeFix(parent, diff);
    }
    // parent is null
    else
    {
        // 1st case: only 1 node in the tree
        if (node->getLeft() == nullptr && node -> getRight() == nullptr)
        {
            this->root_ = nullptr;
            delete node;
        }
        // 2nd case: node has a left child
        else if(node->getLeft() != nullptr)
        {
            node->getLeft()->setParent(nullptr);
            this->root_ = node->getLeft();
            delete node;
        }
        // 3rd case: node has a right child
        else
        {
            node->getRight()->setParent(nullptr);
            this->root_ = node->getRight();
            delete node;
        }
        // since the tree will always be balanced in this case, just return
        return;
    }
}



template<class Key, class Value>
void AVLTree<Key,Value>::removeFix(AVLNode<Key,Value>* node, int8_t diff)
{
    // If node is null, return
    if (node == nullptr)
    {
        return;
    }
    // Compute parent(node) and ndiff (for next recursive call)
    AVLNode<Key, Value>* parent = node->getParent();
    int8_t ndiff = 0;
    // if p is not NULL let ndiff (nextdiff) = +1 if n is a left child and -1 otherwise
    if (parent != nullptr)
    {
        if (parent->getLeft() == node)
        {
            ndiff = 1;
        }
        else
        {
            ndiff = -1;
        }
    }
    // diff = -1: 
    if (diff == -1)
    {
        // Case 1: balance(node) + diff == -2
        if (node->getBalance() + diff == -2)
        {
            // Let c = left(n), the taller of the children
            AVLNode<Key, Value>* child = node->getLeft();
            //Case 1a: b(c) == -1 // zig-zig case
            if (child->getBalance() == -1)
            {
                //rotateRight(n), b(n) = b(c) = 0, removeFix(p, ndiff)
                rotateRight(node);
                node->setBalance(0);
                child->setBalance(0);
                removeFix(parent,ndiff);
            }
            // Case 1b: b(c) == 0 // zig-zig case
            else if (child->getBalance() == 0)
            {
                // rotateRight(n), b(n) = -1, b(c) = +1 // Done!
                rotateRight(node);
                node->setBalance(-1);
                child->setBalance(1);
            }
            // Case 1c: b(c) == +1 // zig-zag case
            else if (child->getBalance() == 1)
            {
                // Let g = right(c)
                AVLNode<Key, Value>* grandchild = child->getRight();
                // rotateLeft(c) then rotateRight(n)
                rotateLeft(child);
                rotateRight(node);
                // If b(g) was +1 then b(n) = 0, b(c) = -1, b(g) = 0
                if (grandchild->getBalance() == 1)
                {
                    node->setBalance(0);
                    child->setBalance(-1);
                    grandchild->setBalance(0);
                }
                // If b(g) was 0 then b(n) = 0, b(c) = 0, b(g) = 0
                else if (grandchild->getBalance() == 0)
                {
                    node->setBalance(0);
                    child->setBalance(0);
                    grandchild->setBalance(0);
                }
                // If b(g) was -1 then b(n) = +1, b(c) = 0, b(g) = 0
                else if (grandchild->getBalance() == -1)
                {
                    node->setBalance(1);
                    child->setBalance(0);
                    grandchild->setBalance(0);
                }
                // removeFix(p, ndiff);
                removeFix(parent, ndiff);
            }
        }
        // Case 2: b(n) + diff == -1: then b(n) = -1; // Done!
        else if (node->getBalance() + diff == -1)
        {
            node->setBalance(-1);
        }
        // Case 3: b(n) + diff == 0: then b(n) = 0, removeFix(p, ndiff)
        else if (node->getBalance() + diff == 0)
        {
            node->setBalance(0);
            removeFix(parent, ndiff);
        }
    }
    if (diff == 1)
    {
        // Case 2: balance(node) + diff == 2
        if (node->getBalance() + diff == 2)
        {
            AVLNode<Key, Value>* child = node->getRight();
            //Case 2a: b(c) == 1 // zig-zig case
            if (child->getBalance() == 1)
            {
                //rotateLeft(n), b(n) = b(c) = 0, removeFix(p, ndiff)
                rotateLeft(node);
                node->setBalance(0);
                child->setBalance(0);
                removeFix(parent,ndiff);
            }
            // Case 1b: b(c) == 0 // zig-zig case
            else if (child->getBalance() == 0)
            {
                // rotateLeft(n), b(n) = 1, b(c) = -1 // Done!
                rotateLeft(node);
                node->setBalance(1);
                child->setBalance(-1);
            }
            // Case 1c: b(c) == -1 // zig-zag case
            else if (child->getBalance() == -1)
            {
                // std::cout << "Now we enter the zig-zag case for right: " << std::endl;
                // Let g = left(c)
                AVLNode<Key, Value>* grandchild = child->getLeft();
                // rotateRight(c) then rotateLeft(n)
                rotateRight(child);
                rotateLeft(node);
                // If b(g) was -1 then b(n) = 0, b(c) = 1, b(g) = 0
                if (grandchild->getBalance() == -1)
                {
                    node->setBalance(0);
                    child->setBalance(1);
                    grandchild->setBalance(0);
                }
                // If b(g) was 0 then b(n) = 0, b(c) = 0, b(g) = 0
                else if (grandchild->getBalance() == 0)
                {
                    node->setBalance(0);
                    child->setBalance(0);
                    grandchild->setBalance(0);
                }
                // If b(g) was 1 then b(n) = -1, b(c) = 0, b(g) = 0
                else if (grandchild->getBalance() == 1)
                {
                    node->setBalance(-1);
                    child->setBalance(0);
                    grandchild->setBalance(0);
                }
                // removeFix(p, ndiff);
                removeFix(parent, ndiff);
            }
        }
        // Case 2: b(n) + diff == 1: then b(n) = 1; // Done!
        else if (node->getBalance() + diff == 1)
        {
            node->setBalance(1);
        }
        // Case 3: b(n) + diff == 0: then b(n) = 0, removeFix(p, ndiff)
        else if (node->getBalance() + diff == 0)
        {
            node->setBalance(0);
            removeFix(parent, ndiff);
        }
    }

    

}


template<class Key, class Value>
void AVLTree<Key,Value>::rotateLeft(AVLNode<Key,Value>* node)
{
    // the node will be a left child of its right child
    // if (node == nullptr)
    // {
    //     return;
    // }
    AVLNode<Key,Value>* parent = node->getParent();
    AVLNode<Key,Value>* rightChild = node->getRight();
    // left child of the right child
    AVLNode<Key,Value>* rightleftGrandchild = rightChild->getLeft();
    if (parent == nullptr)
    {
        rightChild->setParent(nullptr);
        // no parent means this is the root
        this->root_ = rightChild;
    }
    else
    {
        rightChild->setParent(parent);
        // update the children of the parent node to break from the current node
        if (parent->getRight() == node)
        {
            parent->setRight(rightChild);
        }
        else
        {
            parent->setLeft(rightChild);
        }
    }
    rightChild->setLeft(node);
    node->setParent(rightChild);
    node->setRight(rightleftGrandchild);
    if (rightleftGrandchild != nullptr)
    {
        rightleftGrandchild->setParent(node);
    }
    // update the balance
}


template<class Key, class Value>
void AVLTree<Key,Value>::rotateRight(AVLNode<Key,Value>* node)
{
    AVLNode<Key,Value>* parent = node->getParent();
    AVLNode<Key,Value>* leftChild = node->getLeft();
    // left child of the right child
    AVLNode<Key,Value>* leftrightGrandchild = leftChild->getRight();
    if (parent == nullptr)
    {
        leftChild->setParent(nullptr);
        // no parent means this is the root
        this->root_ = leftChild;
    }
    else
    {
        leftChild->setParent(parent);
        // update the children of the parent node to break from the current node
        if (parent->getRight() == node)
        {
            parent->setRight(leftChild);
        }
        else
        {
            parent->setLeft(leftChild);
        }
    }
    leftChild->setRight(node);
    node->setParent(leftChild);
    node->setLeft(leftrightGrandchild);
    if (leftrightGrandchild != nullptr)
    {
        leftrightGrandchild->setParent(node);
    }
}


template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
