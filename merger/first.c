#include <stdio.h>

int main(){
	for (int i = 0; i < 13; ++i)
		printf("%d\tfirst_str%d\n",(int)(2*(i*1.0)/3)-1, i);
	return 0;
}