#meta PATTERN ${ $}
#meta IGNORED ${ $}

PATTERN ${
    BLANK       \s+
    LIN_CMT     //[^\r\n]*
    IDENTIFIER  [a-z]
    SEPARATOR   [\+\-\*/\[\]\(\),=]
$}

IGNORED ${
    BLANK
    LIN_CMT
$}