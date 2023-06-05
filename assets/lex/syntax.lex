#meta EBNF      ${$}
#meta MAPPING   ${$}
#meta IGNORED   ${$}

EBNF ${
    BLANK       \s+
    EPSILON     \\e
    START_MRK   \*
    TERMINAL    `[^`]*`
    NON_TERM    [\a_][\w']*
    MUL_TERM    $[\a_][\w']*
    TOK_TYPE    @[\a_][\w']*
    DELIMITER   [\(\){}\[\]\|]
    SEPARATOR   ;
    GRAMMAR_DEF ::=
    COMMENT     //[^\r\n]*
    COMMENT     /\*([^\*]|\*[^/])*\*/
$}

MAPPING ${
    BLANK       \s+
    MUL_TERM    $[\a_][\w']*
    TOK_TYPE    @[\a_][\w']*
    DELIMITER   \|
    SEPARATOR   ;
    MAPPING_DEF -->
    COMMENT     //[^\r\n]*
    COMMENT     /\*([^\*]|\*[^/])*\*/
$}

IGNORED ${
    BLANK
    COMMENT
$}