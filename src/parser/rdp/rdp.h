/**
 * @file rdp.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Recursive Descent Parser
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "../gram.h"
#include "../../common/token.h"
#include "../tok_view.h"

class RDPParser
{
    Grammar grammar;

public:
    RDPParser(Grammar &grammar);
    bool parseSymbol(TokenViewer &tokens, term t);
    bool parse(vector<token> &tokens, vector<TokenType> &tok_types);
};