PATTERN ${
    BLANK       \s+
    START       \*
    COMMENT     //[^\r\n]*
    EPSILON     \\e
    CONSTANT    REAL|INTEGER|STRING
    IDENTIFIER  IDENTIFIER
    NON_TERM    [\a_][\w']*
    TERMINAL    `[^`]*`
    DEFINITION  ::=
    SEPARATOR   [\(\){}\[\]\|;]
$}

IGNORE ${
    BLANK
    COMMENT
$}