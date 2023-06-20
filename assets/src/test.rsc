/**
 * block comment 1
*/

decl func add(x : int, y : int) : int;
decl func add2(x : int, y : int, z : real, s : str) : int;
func sub() { return; }

var x : int = 10;                // integer variable
var y : bool = true;             // integer variable in scientific notation


func main() : int
{
    var a : int = 10, b : int = 1, c : bool = false;
    var d : int = +3;                    // integer variable in scientific notation
    var e : int = -3;                     // integer variable in hexadecimal notation
    var o : real = 3.14;                    // float variable
    var p : real = -2.71828;                // double variable
    var q : real = 1.0;                 // double variable in scientific notation
    var s : str = "Hello World!";           // string variable

    var result : int = a + 2 * (d - e);

    if (d > e && a < b || -b == add(a, d) || 1)
    {
        print "Hello World!";
    }

    /*
    block comment 2
    */

    for (var i : int = 0; i < 10; i = i + 1)
    {
        var flag : bool = true;
        while (flag)
        {
            if (b[i] == 0)
            {
                flag = false;
            }
            if (b[i] == 1)
            {
                continue;
            }
            else
            {
                b[i] = b[i] - 1;
            }
        }
    }

    // line comment

    return 0;
}

func add(x : int, y : int) : int
{
    return x + y;
}