#include <stdio.h>

#define BUFSIZE 512 //my define
#define N_MAX 10 //max value of selected cards
#define PERM_MAX 5040 //10P4


void RmOverlappingCards(char buffer[BUFSIZE], int *no){
  int i;
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


int LineUp(int n, int k, int a[N_MAX+1]){
  int b1, b2, b3, b4;
  int i,j,l,m;
  char buffer[BUFSIZE];
  int figure[PERM_MAX]={0};
  int no=0;

  for(i=0; i<n; i++){ //lining up 2 cards
	b1=a[i];
	for(j=0; j<n; j++){
	  while(1){
		if(j==i){
		  j++;
		}else{
		  b2=a[j];
		  break;
		}
	  }
	  if(b1>0 && b2>0 && k==2){
		sprintf(buffer,"%d%d", b1,b2);
		RmOverlappingCards(buffer, &no);
	  }
	  if(k>=3){ //lining up 3 cards
		for(l=0; l<n; l++){
		  while(1){
			if(l==i || l==j){
			  l++;
			}else{
			  b3=a[l];
			  break;
			}
		  }
		  if(b1>0 && b2>0 && b3>0 && k==3){
			sprintf(buffer,"%d%d%d",b1,b2,b3);
			RmOverlappingCards(buffer, &no);
		  }
		  if(k>=4){ //lining up 4 cards
			for(m=0; m<n; m++){
			  while(1){
				if(m==i || m==j || m==l){
				  m++;
				}else{
				  b4=a[m];
				  break;
				}
			  }
			  if(b1>0 && b2>0 && b3>0 && b4>0){
				sprintf(buffer,"%d%d%d%d",b1,b2,b3,b4);
				RmOverlappingCards(buffer, &no);
			  }
			}
		  }//End of lining up 4 cards
		}
	  }//End of lining up 3 cards
	}
  }//End of lining up 2 cards

  return no;

}


int main(void){
  int n, k;
  int a[N_MAX+1];
  int i;

  while(1){

	for(i=0; i<N_MAX+1; i++){
	  a[i]=-1;
	}

	scanf("%d", &n);
	scanf("%d", &k);

	if(n==0 && k==0){
	  return 0;
	}
	for(i=0; i<n; i++){
	  scanf("%d", &a[i]);
	}

	printf("%d\n", LineUp(n, k, a));
  }

  return 0;

}
