#include <iostream>
#include <math.h>

using namespace std;

// 4
// E		1+2+3+4     n*(n+1)/2
// k=1







// sum of k=1 to 4 and i=0 to 3 of (3k+2i+7)

int question() {
		int k, i, sum=0;
		for (k=1; k<=4; k++) {
				for (i=0; i<=3; i++) 
				{
						sum += 3*k + 2*i + 7;
				}
		}
		cout << "\nsum = " << sum << endl;
		return 0;
}


int main() {

		// input from user and set it to j
		int j;
		cout << "Enter a number: ";
		cin >> j;

		// for loop example
		int i, sum=0;

			   // i = 1
				 // i < 10
				 // i = i + 1
		for (i=1; i<=j; i++) // loop 
		{
				sum += i;
				// print sum
				cout << "sum = " << sum << endl;

		}


		cout << "sum = " << sum << endl;


	
		return 0;
}