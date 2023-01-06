#include <iostream>
#include <math.h>

using namespace std;

int main() 
{

		/* output is like:
		
		1
		2 4
		3 6 9
		4 8 12 16
		5 10 15 20 25
		6 12 18 24 30 36
		7 14 21 28 35 42 49
		 */


		for (int i=1; i<=7; i++) 
		{
				for (int j=1; j<=i; j++) 
				{
						cout << i*j << " ";
				}
				cout << endl;
		}

		return 0;
}


