#include "test.h"
#include "def.h"

using namespace std;

std::map<std::string, std::function<std::shared_ptr<ASTNode>()>> astNodeFactories;

#include "test.sem"

void defTest()
{
    reg_nodes();
    auto node = MAKE_NODE("ExprNode");
    node->act();
}