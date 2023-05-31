#include "test.h"
#include "irgen/sem.h"

using namespace std;

#include "test.sem"

void semTest()
{
    reg_nodes();
    auto node = MAKE_NODE("ExprNode");
    node->act();
}