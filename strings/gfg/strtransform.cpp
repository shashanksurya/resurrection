// C++ program to find minimum number of
// operations required to transform one string to other
#include <bits/stdc++.h>
using namespace std;
 
// Function to find minimum number of operations required to transform
// A to B.
int minOps(string& A, string& B)
{
    int m = A.length(), n = B.length();
 
     // This parts checks whether conversion is
     // possible or not
    if (n != m)
       return -1;
    int count[256];
    memset(count, 0, sizeof(count));
    for (int i=0; i<n; i++)   // count characters in A
       count[B[i]]++;
    for (int i=0; i<n; i++)   // subtract count for
       count[A[i]]--;         // every character in B
    for (int i=0; i<256; i++)   // Check if all counts become 0
      if (count[i])
         return -1;
 
    // This part calculates the number of operations required
    cout << "length of string is " << n;
    int res = 0;
    for (int i=n-1, j=n-1; i>=0; )
    {
        // If there is a mismatch, then keep incrementing
        // result 'res' until B[j] is not found in A[0..i]
        while (i>=0 && A[i] != B[j])
        {
            cout << "\n" <<A[i] << " " << B[j] << i<< " " <<j;
            i--;
            res++;
        }
 
        // If A[i] and B[j] match
        if (i >= 0)
        {
          cout << "\n" <<A[i] << " " << B[i] << "[match]";
            i--;
            j--;
        }
    }
    return res;
}

// Driver program
int main()
{
    string A = "DCEA";
    string B = "EDAC";
    cout << "Minimum number of operations "
            "required is " << minOps(A, B);
    cout << "\n";
    return 0;
}