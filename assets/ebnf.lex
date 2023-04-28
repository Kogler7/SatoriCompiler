PATTERN ${
    ^BLANK      \s+
    ^IGNORE     //[^\r\n]*
    EPSILON     \\e
    NON_TERM    [\a_][\w]*
    TERMINAL    `[^`]*`
    *SEPARATOR  [=\(\){}\[\];\|]
$}

RESERVED ${
    =   (   )   {   }   [   ]   ;   |
$}