#include <iostream> // from cpp
#include <math.h> // h means header file from C lang

// f(x) = 1 , -5 <= x < -1
// f(x) = x.x - 1 , -1 <= x < 2
// f(x) = 1 - x , x >= 2

// for a x value entered from the keyboard, find the value of f(x)

using namespace std;

main()
{
		float x, y;
		cout << "Enter x: ";
		cin >> x;
		if (x >= -5 && x < -1)
		{
				y = 1;
		}
		else if (x >= -1 && x < 2)
		{
				y = x * x - 1;
		}
		else if (x >= 2)
		{
				y = 1 - x;
		}
		else
		{
				cout << "x is not in the domain of f(x)";
		}
		cout << "f(x) = " << y;
}


