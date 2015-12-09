#include <stdio.h>
char* full_string;
int counter = 0;

void iter_swap(char *ptr1, char *ptr2) {
char tmp = *ptr1; *ptr1 = *ptr2;
*ptr2 = tmp;
}

char * min_element(char *start, char *end) { // end is 1 beyond the last valid element
if(start == end) return end; // empty range
char *min_pos = start;
for(char *iter = start+1; iter != end; ++iter) {
if(*iter < *min_pos) min_pos = iter;
}
return min_pos;
}

void permute(char *str, int length) {
    if(length == 0) {
        printf("%s\n", full_string);
        ++counter;
        return;
    } else {
        // Find the smallest char in the string set it to be the first character. Solve the subproblem for the smaller string.
        char *smallest = min_element(str, str + length);
        iter_swap(str, smallest);
        permute(str+1, length-1);
        
        // Look for the smallest element strictly greater than the first element of the current string
        char *smallest_greater = str + length;
        for(char *i = str+1; i != str+length; ++i)
            if(*i > *str && (smallest_greater == str + length || *i < *smallest_greater))
                smallest_greater = i;
        while(smallest_greater != str+length) {
            // If such an element is found, swap it into the first slot and recurse
            iter_swap(str, smallest_greater);
            permute(str+1, length-1);

            // Repeat the loop if possible for the next greater character
            smallest_greater = str + length;
            for(char *i = str+1; i != str+length; ++i)
                if(*i > *str && (smallest_greater == str + length || *i < *smallest_greater))
                    smallest_greater = i;

        }
    }
}

int main()
{
    char x[7]="shashan";
    char *s=x;
    permute(s,7);
    return 0;
}