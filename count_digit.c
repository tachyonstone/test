#include <stdio.h>
#include <string.h>

#define BUFSIZE 20

int main(void)
{

  int i=0;
  int a,b,c;
  char pita[3];
  char d[BUFSIZE];
  char data[BUFSIZE];
  char num_ch[BUFSIZE];
  int num;
  int hante[BUFSIZE];

  //scanf("%d", &num);

  fgets(num_ch, BUFSIZE, stdin);
  num = atoi(num_ch);

  for(i=0; i<num; i++){
	fgets(data, BUFSIZE,stdin);
	//printf("%s",data);
	sscanf(data ,"%d %d %d\n",&a, &b, &c);


	if((a*a + b*b == c*c || b*b + c*c == a*a || c*c + a*a == b*b) && a>0 && b > 0 && c > 0){
	  hante[i] = 1;

	}else{
	  hante[i] = 0;
	}

  }


  for(i = 0; i < num; i++){
	if(hante[i] == 1){
	  printf("yes\n");
	}else{
	  printf("no\n");
	}



  }

  return 0;

}
