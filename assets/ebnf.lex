PATTERN ${
    BLANK      \s+
    COMMENT    //[^\r\n]*
    EPSILON    \\e
    CONSTANT   REAL|INTEGER|STRING
    IDENTIFIER IDENTIFIER
    NON_TERM    [\a_][\w]*
    TERMINAL    `[^`]*`
    SEPARATOR  [=\(\){}\[\];\|]
$}

IGNORE ${
    BLANK
    COMMENT
$}