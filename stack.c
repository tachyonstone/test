#include <stdio.h>

#define MAX 64


void push(int stack[], int *size, int n)
{

	if(*size < MAX)
		stack[(*size)++] = n; 

	return;

}

void pop(int *size)
{

	if(*size > 0)
		(*size)--;

	return;

}

void print_stack(int stack[], int size)
{
	int i;

	for(i = 0; i < size; i++){
		printf("%d ", stack[i]);
	}

	printf("\n");

	return;

}


int main(void)
{
	
	int n;

	int size = 0;
	int stack[MAX];

	n = 3;
	push(stack, &size, n);
	print_stack(stack, size);

	n = 1;
	push(stack, &size, n);
	print_stack(stack, size);

	n = 4;
	push(stack, &size, n);
	print_stack(stack, size);

	pop(&size);
	print_stack(stack, size);


	return 0;
}


