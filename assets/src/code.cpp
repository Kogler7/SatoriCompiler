#include <iostream> // header file
using namespace std;

// testing function
int test()
{

    int x = 10;                // integer variable
    int y = +3e10;             // integer variable in scientific notation
    int z = 0x3f;              // integer variable in hexadecimal notation
    float o = 3.14;            // float variable
    double p = -2.71828;       // double variable
    double q = 1.0e-10;        // double variable in scientific notation
    char c = 'a';              // character variable
    string s = "Hello World!"; // string variable

    cout << "x = " << x << endl;

    int result = x + 2 * (y - z);
    cout << "result = " << result << endl;

    if (x > y)
    {
        cout << "x is greater than y." << endl;
    }
    else
    {
        cout << "x is less than or equal to y." << endl;
    }

    /*
    block comment 1
    */

    for (int i = 0; i < x; i++)
    {
        cout << i << " "; /* block comment 2 */
    }
    cout << endl;

    // line comment

    return 0;
}