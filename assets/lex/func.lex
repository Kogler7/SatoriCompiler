#meta PATTERN ${ $}
#meta IGNORED ${ $}

PATTERN ${
    BLANK       \s+
    LIN_CMT     //[^\r\n]*
    BLK_CMT     /\*([^\*]|\*[^/])*\*/
    IDENTIFIER  [\a_][\w]*
    SEPARATOR   [\+\-\*\\\(\)\[\]/,;=]
$}

IGNORED ${
    BLANK
    LIN_CMT
    BLK_CMT
$}