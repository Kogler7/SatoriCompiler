#meta PATTERN ${ $}
#meta IGNORED ${ $}

PATTERN ${
    LINE_MID    \\\r?\n
    LINE_END    (\r?\n)+
    BLANK       \s+
    LIN_CMT     //[^\r\n]*
    LIN_CMT     #\s[^\r\n]*
    BLK_CMT     /\*([^\*]|\*[^/])*\*/
    MULTI_STR   '''[^']*'''
    MULTI_STR   """[^"]*"""
    IDENTIFIER  [\a_][\w]*
    UNIT        `[^`]*`
    STRING      "[^"]*" | '[^']*'
    REAL        (\-|\+|\e)[\d]+\.[\d]+(\e|e(\-|\+|\e)[\d]+)
    REAL        (\-|\+|\e)0[bB][01]+\.[01]+(e(\-|\+|\e)[\d]+)?
    REAL        (\-|\+|\e)0[oO][0-7]+\.[0-7]+(e(\-|\+|\e)[\d]+)?
    REAL        (\-|\+|\e)0[xX]([\da-fA-F]+|[\d]+)\.([\da-fA-F]+|[\d]+)(e(\-|\+|\e)[\d]+)?
    INTEGER     (\-|\+|\e)[\d]+(e(\-|\+|\e)[\d]+)?
    INTEGER     (\-|\+|\e)0[bB][01]+(e(\-|\+|\e)[\d]+)?
    INTEGER     (\-|\+|\e)0[oO][0-7]+(e(\-|\+|\e)[\d]+)?
    INTEGER     (\-|\+|\e)0[xX]([\da-fA-F]+|[\d]+)(e(\-|\+|\e)[\d]+)?
    SEPARATOR   [\+\-\*\\\(\){}\[\]<>;,.\|&!=:\?@]
    SEPARATOR   >=|<=|!=|==|\|\||&&|->|:=
$}

IGNORED ${
    BLANK
    LIN_CMT
    BLK_CMT
    LINE_MID
$}