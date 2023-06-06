#meta PATTERN ${ $}
#meta IGNORED ${ $}

PATTERN ${
    BLANK       \s+
    LIN_CMT     //[^\r\n]*
    IDENTIFIER  [\a_][\w]*
    SEPARATOR   [\+\-\*/=\(\)]
$}

IGNORED ${
    BLANK
    LIN_CMT
$}