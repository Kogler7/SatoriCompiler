PATTERN ${
    BLANK       \s+
    LIN_CMT     //[^\r\n]*
    IDENTIFIER  [\a_][\w]*
    SEPARATOR   [\+\-\*/=\(\)]
$}

IGNORE ${
    BLANK
    LIN_CMT
$}