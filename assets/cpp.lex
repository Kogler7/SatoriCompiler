PATTERN ${
    ^BLANK      \s+
    ^IGNORE     //[^\r\n]*
    ^IGNORE     /\*([^\*]|\*[^/])*\*/
    MACRO       #[\w]+
    INCLUDE     "[\w.]+"|<[\w.]+>
    *IDENTIFIER [\a_][\w]*
    STRING      "[^"]*"
    CHARACTER   '.'
    REAL        (\-|\+|\e)[\d]+\.[\d]+(\e|e(\-|\+|\e)[\d]+)
    REAL        (\-|\+|\e)0[bB][01]+\.[01]+(e(\-|\+|\e)[\d]+)?
    REAL        (\-|\+|\e)0[oO][0-7]+\.[0-7]+(e(\-|\+|\e)[\d]+)?
    REAL        (\-|\+|\e)0[xX]([\da-fA-F]+|[\d]+)\.([\da-fA-F]+|[\d]+)(e(\-|\+|\e)[\d]+)?
    INTEGER     (\-|\+|\e)[\d]+(e(\-|\+|\e)[\d]+)?
    INTEGER     (\-|\+|\e)0[bB][01]+(e(\-|\+|\e)[\d]+)?
    INTEGER     (\-|\+|\e)0[oO][0-7]+(e(\-|\+|\e)[\d]+)?
    INTEGER     (\-|\+|\e)0[xX]([\da-fA-F]+|[\d]+)(e(\-|\+|\e)[\d]+)?
    *SEPARATOR  [\+\-\*\\%=\(\){}\[\]<>;,.\|&^!:~\?]
    *SEPARATOR  >=|<=|!=|==|\+\+|\-\-|\|\||&&|\*=|/=|\+=|\-=|%=|<<|>>|::|->
$}

RESERVED ${
    auto	break	case	char	const	continue	default
	do	double	else	enum	extern	float	for	goto
	if	int	long	register	return	short	signed
	sizeof	static	struct	switch	typedef	union
	unsigned	void	volatile	while   return
    bool    true    false   nullptr
    +   -   *   /   %   =   (   )   {   }   [   ]
    <   >   ;   ,   .   |   &   ^   !   :   ~   ?
    >=  <=  !=  ==  ++  --  ||  &&  *=  /=  +=  -=  %=  <<  >>  ::  ->
$}