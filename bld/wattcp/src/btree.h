#ifndef __BTREE_H
#define __BTREE_H

typedef struct TreeNode {
        struct TreeNode *left;
        struct TreeNode *right;
        struct TreeNode *parent;
        void            *info;
        size_t           info_size;
      } TreeNode;

typedef enum TreeChild {
        NOT_CHILD  = 1,
        LEFT_CHILD,
        RIGHT_CHILD
      } TreeChild;

typedef int (*CmpFunc) (const void *a, const void *b);

int       tree_insert (TreeNode **root, void *info, size_t size, CmpFunc cmp);
TreeNode *tree_find   (TreeNode  *root, void *info, CmpFunc cmp);
TreeNode *tree_delete (TreeNode  *root, TreeNode *node);
void      tree_free   (TreeNode  *root);

#endif
