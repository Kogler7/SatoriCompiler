PATTERN ${
    BLANK       \s+
    GRAMMAR     GRAMMAR
    MAPPING     MAPPING
    EPSILON     \\e
    START_MRK   \*
    BLOCK_SRT   $\{
    BLOCK_END   $\}
    TERMINAL    `[^`]*`
    NON_TERM    [\a_][\w']*
    MUL_TERM    $[\a_][\w']*
    TOK_TYPE    @[\a_][\w']*
    DELIMITER   [\(\){}\[\]\|]
    SEPARATOR   ;
    GRAMMAR_DEF ::=
    MAPPING_DEF -->
    COMMENT     //[^\r\n]*
$}

IGNORE ${
    BLANK
    COMMENT
$}