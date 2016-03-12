#include <stdio.h>

int main(){
	for (int i = 0; i < 7; ++i)
        printf("%d\tsecond_str%d\n",(int)(1*(i*i*1.0)/4), i);
	return 0;
}