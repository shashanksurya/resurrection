#include <stdio.h>
#include <stdlib.h>
 #define true 0
#define false 0
 int INT_MAX=32255;

int balancePartition(int set[], int n,int count)
{
 count++;
 /*The value of subset[i][j] will be true if there is a subset 
     of set[0..j-1] with sum equal to i */
        int i,j;
        int sum =0;
 
        for(i =0; i<=n; i++){
                sum += set[i];
        }
 
        int subset[sum+1][n+1];
        // If sum is 0, then answer is true
        for (i = 0; i <= n; i++)
                subset[0][i] = true;
 
        // If sum is not 0 and set is empty, then answer is false
        for (i = 1; i <= sum; i++)
                subset[i][0] = false;
 
 
        // Fill the subset table in botton up manner
        for (i = 1; i <= sum; i++)
        {
         for ( j = 1; j <= n ; j++)
         {
           subset[i][j] = subset[i][j-1];
           if (i >= set[j-1]){
              subset[i][j] = subset[i][j] ||subset[i-set[j-1]][j-1];
           }
         }
        }
 
        int min =INT_MAX;
    
        for(i=1; i<=sum; i++){
           for(j=1; j<=n; j++){
           /* If there is s subset with sum i, then check if the 
              difference between overall sum and twice this sum is least or not.
              If yes update the min */
              
              if(subset[i][j] == true){
                   if(abs(sum - 2*i) < min){
                       min  = abs(sum - 2 *i);
                   }
              }
           }
        }
 
        printf("\n Differece between two sub sets will be : %d\n", min);
        return count;
}
int main(){
        int a[] = {4,5,8,9};
        int count=0;
        //{11,8,5,6,12,4,7,9,3,10};
        int n = sizeof(a)/sizeof(a[0]);
        printf("%d",balancePartition(a,n-1,count));
        
        return 0;
}