#include <stdio.h>
#include <string.h>

#define BUFSIZE    512     //my define
#define N_MAX       10     //max value of selected cards
#define NUM_TIME    16     //my define
#define PERM_MAX  5040     //10P4


int InputCardInfo(int n[NUM_TIME], int k[NUM_TIME], int a[NUM_TIME][N_MAX+1]){
  int i;
  int z=0;

  while(1){
	scanf("%d", &n[z]);
	scanf("%d", &k[z]);

	if(n[z]==0 && k[z]==0){
	  return z;
	}

	for(i=0; i<n[z]; i++){
	  scanf("%d", &a[z][i]);
	}

	z++;
  }

}


void RemoveOverlappingCards(char buffer[BUFSIZE], int *no){
  int i=0;
  int flag=0;
  int figure[PERM_MAX];

  figure[*no] = atoi(buffer);
  for(i=0; i<*no; i++){
	if((figure[*no]-figure[i]) == 0){
	  flag = 1;
	  break;
	}
  }
  if(flag>0){
	flag=0;
  }else{
	(*no)++;
  }

}


void LineUp(int n[NUM_TIME], int k[NUM_TIME], int a[NUM_TIME][N_MAX+1], int end){
  int b1, b2, b3, b4;
  int i,j,l,m;
  char buffer[BUFSIZE];
  int figure[PERM_MAX]={0};
  int no;
  int z;

  for(z=0; z<end; z++){ //Start
	no=0;
	/*
	  printf("ok\n");
	  printf("%d %d %d %d %d %d %d %d %d %d %d\n"
	  ,a[z][0],a[z][1],a[z][2],a[z][3],a[z][4]
	  ,a[z][5],a[z][6],a[z][7],a[z][8],a[z][9],a[z][10]);
	  printf("z:%d n:%d k:%d\n",z,n[z],k[z]);
	  printf("i:%d,j:%d,l:%d,m%d\n", i,j,l,m);
	*/

	for(i=0; i<n[z]; i++){  //lining up 2 cards
	  b1=a[z][i];
	  for(j=0; j<n[z]; j++){
		while(1){
		  if(j==i){
			j++;
		  }else{
			b2=a[z][j];
			break;
		  }
		}
		if(b1>0 && b2>0 && k[z]==2){
		  sprintf(buffer,"%d%d", b1,b2);
		  RemoveOverlappingCards(buffer, &no);
		}
		if(k[z]>=3){  //lining up 3 cards
		  for(l=0; l<n[z]; l++){
			while(1){
			  if(l==i || l==j){
				l++;
			  }else{
				b3=a[z][l];
				break;
			  }
			}
			if(b1>0 && b2>0 && b3>0 && k[z]==3){
			  sprintf(buffer,"%d%d%d",b1,b2,b3);
			  RemoveOverlappingCards(buffer, &no);
			}
			if(k[z]>=4){  //lining up 4 cards
			  for(m=0; m<n[z]; m++){
				while(1){
				  if(m==i || m==j || m==l){
					m++;
				  }else{
					b4=a[z][m];
					break;
				  }
				}
				if(b1>0 && b2>0 && b3>0 && b4>0){
				  sprintf(buffer,"%d%d%d%d",b1,b2,b3,b4);
				  RemoveOverlappingCards(buffer, &no);
				}
			  }
			}//End of lining up 4 cards
		  }
		}//End of lining up 3 cards
	  }
	}//End of lining up 2 cards

	printf("%d\n", no);  //output

  }

}


int main(void){
  int n[NUM_TIME];
  int k[NUM_TIME];
  int a[NUM_TIME][N_MAX+1];
  int i,j;
  int end;

  for(i=0; i<NUM_TIME; i++){
	for(j=0; j<N_MAX+1; j++){
	  a[i][j]=-1;
	}
  }

  end = InputCardInfo(n,k,a);
  LineUp(n, k, a, end);

  return 0;

}
