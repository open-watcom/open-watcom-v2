

#include "base/gbtree.h"
#include <iostream.h>
#include <iomanip.h>

void TreePrint (CL_GenericBTreeNode* root, short level)
{
    if (!root)
        return;
    CL_BTreeNodeSpace* space = root->NodeSpace();
    CL_GenericBTreeNode* child;
    child = space->BorrowNode (root->Subtree(0));
    TreePrint (child, level+1);
    space->ReturnNode (child);
    register short n = root->Size();
    for (short i = 0; i < n; i++) {
        for (short j = 0; j < 4*level; j++) cout << ' '; // Indentation
        cout << (i == n/2 ? "|---> |" : "|     |");
        cout << (long) root->Item (i) << endl;
        child = space->BorrowNode (root->Subtree(i+1));
        TreePrint (child, level+1);
        space->ReturnNode (child);
    }
}

void PrettyPrint (const CL_GenericBTree& tree)
{
    CL_BTreeNodeSpace* space = tree.NodeSpace();
    CL_GenericBTreeNode* root = space->BorrowRoot ();
    TreePrint (root, 0);
    space->ReturnNode (root);
}




main ()
{
    CL_AbstractComparator cmp;
    CL_GenericBTree tree (cmp);
    long num[] = { 42, 49, 63, 75, 15, 23, 37, 41, 81, 99};
    for (short i = 0; i < sizeof num/sizeof (long); i++)
        tree.Add ((void*) num[i]);
    PrettyPrint (tree);
    return(0);
}

