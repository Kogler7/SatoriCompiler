PATTERN ${
    BLANK       \s+
    IGNORE      //[^\r\n]*
    EPSILON     \\e
    NON_TERM    [\a_][\w]*
    TERMINAL    `[\w]`
    SEPARATOR   [=\(\){}\[\];\|]
$}

RESERVED ${
    =   (   )   {   }   [   ]   ;   |
$}