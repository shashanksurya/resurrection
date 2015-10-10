// C++ program to find 3 elements such that max(abs(A[i]-B[j]), abs(B[j]-
// C[k]), abs(C[k]-A[i])) is minimized.
#include<bits/stdc++.h>
using namespace std;
 
void findClosest(int A[], int B[], int C[], int p, int q, int r)
{
 
    int diff = INT_MAX;  // Initialize min diff
 
    // Initialize result
    int res_i =0, res_j = 0, res_k = 0;
 
    // Traverse arrays
    int i=0,j=0,k=0;
    while (i < p && j < q && k < r)
    {
        // Find minimum and maximum of current three elements
        int minimum = min(A[i], min(B[j], C[k]));
        int maximum = max(A[i], max(B[j], C[k]));
 
        // Update result if current diff is less than the min
        // diff so far
        if (maximum-minimum < diff)
        {
             res_i = i, res_j = j, res_k = k;
             diff = maximum - minimum;
        }
 
        // We can't get less than 0 as values are absolute
        if (diff == 0) break;
 
        // Increment index of array with smallest value
        if (A[i] == minimum) i++;
        else if (B[j] == minimum) j++;
        else k++;
    }
 
    // Print result
    cout << A[res_i] << " " << B[res_j] << " " << C[res_k];
}
 
// Driver program
int main()
{
    int A[] = {1, 4, 10};
    int B[] = {2, 15, 20};
    int C[] = {10, 12};
 
    int p = sizeof A / sizeof A[0];
    int q = sizeof B / sizeof B[0];
    int r = sizeof C / sizeof C[0];
 
    findClosest(A, B, C, p, q, r);
    return 0;
}