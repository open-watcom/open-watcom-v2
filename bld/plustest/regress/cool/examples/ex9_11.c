//
// Copyright (C) 1991 Texas Instruments Incorporated.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated provides this software "as is" without
// express or implied warranty.
//
// Updated: JAM 10/05/92 -- modernized template syntax
// Updated: JAM 10/05/92 -- fixed bug where nodes dtors called multiple times
//                          by making auto Nodes pointers and letting the
//                          Tree dtor delete them recursively

#include <cool/D_Node.h>                        // Include node class
#include <cool/N_Tree.h>                        // Include n-ary tree class
#include <cool/String.h>                        // Include COOL String class

#include <cool/Vector.C>
#include <cool/D_Node.C>
#include <cool/N_Tree.C>

int main (void) {
    cout << __FILE__ << endl;
  CoolD_Node<CoolString,3>* president = new CoolD_Node<CoolString,3>(CoolString("President")); // Create president
  CoolN_Tree<CoolD_Node<CoolString,3> > org_chart (president);    // Setup top of tree
  CoolD_Node<CoolString,3>* sales = new CoolD_Node<CoolString,3>(CoolString("Sales"));          // Create sales
  CoolD_Node<CoolString,3>* service = new CoolD_Node<CoolString,3>(CoolString("Service"));      // Create service
  CoolD_Node<CoolString,3>* finance = new CoolD_Node<CoolString,3>(CoolString("Finance"));      // Create finance
  CoolD_Node<CoolString,3>* legal = new CoolD_Node<CoolString,3>(CoolString("Legal"));          // Create legal
  (*president)[0] = sales;                                      // Add sales to chart
  president->insert_after(*service, 0);                         // Add service to chart
  president->insert_after(*finance, 1);                         // Add finance to chart
  president->insert_after(*legal, 2);                           // Add legal to chart
  (*sales)[0] = new CoolD_Node<CoolString,3> (CoolString("Domestic")); // Domestic sales
  CoolD_Node<CoolString,3>* international = new CoolD_Node<CoolString,3>(CoolString("International")); // International
  sales->insert_after(*international, 0);
  (*international)[0] = new CoolD_Node<CoolString,3> (CoolString("Asia"));
  international->insert_after(*(new CoolD_Node<CoolString,3> (CoolString("Europe"))), 0);
  international->insert_after(*(new CoolD_Node<CoolString,3> (CoolString("Africa"))), 1);
  (*finance)[0] = new CoolD_Node<CoolString,3> (CoolString("Short Term"));
  finance->insert_after(*(new CoolD_Node<CoolString,3> (CoolString("Long Term"))), 0);
  finance->insert_after(*(new CoolD_Node<CoolString,3> (CoolString("Collections"))), 1);
  org_chart.traversal() = PREORDER;             // Set traversal mode
  for (org_chart.reset (); org_chart.next (); ) { // For each node in tree
    for (int i = 0; i < org_chart.current_depth (); i++) // Indent level
      cout << "   ";
    cout << org_chart.value () << "\n";
  }
  return (0);                                   // Return success
}
