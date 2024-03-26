#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>
#include<algorithm>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here


bool equalPaths(Node * root)
{
    // Add your code below
  if(root == nullptr)
  {
    return true;
  }

  // both children null, return true
  if (root->left == nullptr && root->right == nullptr)
  {
    return true;
  }
  
  // one of the children null, the other not, reuturn false
  if (root->left == nullptr && root->right != nullptr)
  {
    return equalPaths(root->right);
  }

  if(root->right == nullptr && root->left != nullptr)
  {
    return equalPaths(root->left);
  }\

  // first check the whole tree, if the children of the 
  // root node does not have same length, return false
  if(rootDepth(root->left) != rootDepth(root->right))
  {
    return false;
  }
  // recursively check if the left and rightchildren of 
  // the current node has same length 
  return equalPaths(root->left) && equalPaths(root->right);

}
int rootDepth(Node* root)
{
  if(root == nullptr)
  {
    return 0;
  }

  int leftDepth = rootDepth(root->left);
  int rightDepth = rootDepth(root->right);

  return max(leftDepth, rightDepth) + 1;
}

