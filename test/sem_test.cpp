#include "test.h"
#include "irgen/sem.h"
#include "lexer/lexer.h"
#include "parser/syntax.h"
#include "parser/slr1/parser.h"
#include "utils/log.h"

using namespace std;

#include "test.sem"

void semTest()
{
    reg_nodes();
    auto node = MAKE_NODE("ExprNode");
    node->act();

    SyntaxParser syntax("./assets/lex/syntax.lex");
    Grammar g = syntax.parse("./assets/stx/rsc-1.estx");
    g.printRules();
    // g.extractLeftCommonFactor();
    g.printRules();
    g.printTerminals();
    g.printNonTerms();
    SLR1Grammar G = SLR1Grammar(g);
    G.printFirst();
    G.printFollow();
    G.printFirstS();
    // G.printItems();
    G.printClusters();
    // G.printGoTrans();
    // G.printSLR1Table();
    assert(G.checkSLR1(), "Not SLR(1) grammar!");
    SLR1Parser slr1(G);
    Lexer lexer("./assets/lex/rsc.lex");
    Viewer code = Viewer::fromFile("./assets/src/test.rsc");
    auto tokens = lexer.tokenize(code);
    Lexer::printTokens(tokens);
    tokens = G.transferTokens(tokens);
    Lexer::printTokens(tokens);
    info << "result: \n"
         << slr1.parse(tokens, code) << endl;
    slr1.getTree()->print();
}