

#include "base/tree.h"
#include "base/treewalk.h"
#include "base/memory.h"

#include <stdio.h>
#include <iostream.h>


class NodePrinter: public CL_Object {

public:
    bool Print (CL_VoidPtrTreeNode& node, long depth);
    bool Start (CL_VoidPtrTreeNode& node, long depth);
    bool End   (CL_VoidPtrTreeNode& node, long depth);
};

bool NodePrinter::Print (CL_VoidPtrTreeNode& node, long depth)
{
    printf ("Depth %ld content '%s' label %ld\n", depth, (char*)
            node.Content(), node.Label());
    return TRUE;
}

bool NodePrinter::Start  (CL_VoidPtrTreeNode& node, long depth)
{
    for (long i = 0; i < 4*depth; i++) putchar (' ');
    printf ("Depth %ld content '%s' label %ld\n", depth, (char*)
            node.Content(), node.Label());
    if (node.ChildCount() > 0) {
        for (long i = 0; i < 4*depth; i++) putchar (' ');
        printf ("{\n");
    }
    return TRUE;
}


bool NodePrinter::End  (CL_VoidPtrTreeNode& node, long depth)
{
    if (node.ChildCount() > 0) {
        for (long i = 0; i < 4*depth; i++) putchar (' ');
        printf ("}\n");
    }
    return TRUE;
}



typedef CL_Binding<NodePrinter> NodeBind;

int main ()
{
    CL_MemoryLeakChecker check (cout);
    CL_VoidPtrTree aTree;
    NodePrinter prt1;

    aTree.NewRoot (1)->Content() = "Level 0 (root)";
    aTree.AddChild (2, 1)->Content() = "Level 1, #1";
    aTree.AddChild (3, 1)->Content() = "Level 1, #2";
    aTree.AddChild (4, 1)->Content() = "Level 1, #3";

    aTree.AddChild (5, 3)->Content() = "Level 2, #1";
    aTree.AddChild (6, 3)->Content() = "Level 2, #2";

    for (long i = 1; i <= 6; i++)
        printf ("Node %ld has %ld children\n", i,
                aTree.Node (i)->ChildCount());

    printf ("----------------------\nPost-order walk:\n\n");
    CL_VoidPtrTreePostWalker walker (aTree.Root());
    for (walker.Reset (); walker.More(); ) {
        CL_VoidPtrTreeNode* n = walker.Next();
        printf ("%ld: %s\n", n->Label(), n->Content());
    }

    printf ("----------------------\nPre-order walk:\n\n");
    CL_VoidPtrTreePreWalker pwalker (aTree.Root());
    for (pwalker.Reset (); pwalker.More(); ) {
        CL_VoidPtrTreeNode* n = pwalker.Next();
        printf ("%ld: %s\n", n->Label(), n->Content());
    }



    NodeBind print_bind (&prt1, (NodeBind::MethodPtr) &NodePrinter::Print);
    aTree.PostOrderWalk (1, print_bind);

    printf ("--------------\nBegin general traversal:\n");
    NodeBind start_bind (&prt1, (NodeBind::MethodPtr) &NodePrinter::Start);
    NodeBind end_bind   (&prt1, (NodeBind::MethodPtr) &NodePrinter::End);
    long cnt = aTree.Traverse (1, start_bind, end_bind);

    printf ("--------------\n"
            "End General traversal\nCount returned:%d\n", cnt);

    CL_VoidPtrTree* anotherTree = aTree.ExtractSubtree (3);
    char* p = (char*) aTree.Node(1)->Content();


    aTree.PostOrderWalk (1, print_bind);
    printf ("--------------\n");
    anotherTree->PostOrderWalk (3, print_bind);
    delete anotherTree;
return(0);
}


