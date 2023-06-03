var int x = 10;                // integer variable
var int y = +3e10;             // integer variable in scientific notation

func int main()
{
    var int x = 10;                // integer variable
    var int y = +3e10;             // integer variable in scientific notation
    var int z = 0x3f;              // integer variable in hexadecimal notation
    var real o = 3.14;             // float variable
    var real p = -2.71828;         // double variable
    var real q = 1.0e-10;          // double variable in scientific notation
    var str s = "Hello World!"; // string variable

    var int result = x + 2 * (y - z);

    if (x > y)
    {
        1 + 1;
    }

    /*
    block comment 1
    */

    for (var int i = 0; i < x; i++)
    {
        print i;
    }

    // line comment

    return 0;
}