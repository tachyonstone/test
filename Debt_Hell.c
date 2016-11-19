#include <stdio.h>


int main(void)
{
  int week;
  int i;
  int debt = 100000;
  int interest;

  scanf("%d",&week);

  for(i=0; i<week; i++){
	interest = debt*0.05;

	if(interest%1000 != 0){
	  interest += 1000;
	}

	debt = debt + interest;
	debt = debt/1000;
	debt = debt*1000;

  }

  printf("%d\n",debt);

  return 0;

}
