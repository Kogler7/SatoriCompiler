PATTERN ${
    BLANK       \s+
    IDENTIFIER [\a_][\w]*
    SEPARATOR  [\+\-\*/=\(\)]
$}

IGNORE ${
    BLANK
$}