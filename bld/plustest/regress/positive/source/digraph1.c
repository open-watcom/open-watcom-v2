%:include "fail.h"

struct A <%
    A() <%
    %>
%>;

// :> should be ], but it currently isn't for backwards compatibility
// reasons
int main(int argc, char *argv<: ]) <%
    A a;

    _PASS;
%>
