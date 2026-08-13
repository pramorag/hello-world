#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

void **my_2d_alloc(int r, int c, int elem_size)
{
	char **ptr = (char **)malloc(sizeof(char *)*r);
	for (int i = 0; i < r; i++)
		ptr[i] = (char *)malloc(c*elem_size);
	
	return (void **)ptr;
}

void my_2d_free(void **ptr, int r)
{
	for (int i = 0; i < r; i++)
		free((char *)ptr[i]);
}

int main()
{
	/* 3x2 int array */
	int r = 3, c = 2;
	int count = 0;
	int **i_ptr = (int **)my_2d_alloc(r, c, sizeof(int));
	for (int i = 0; i < r; i++) {
		for (int j = 0; j < c; j++) {
			i_ptr[i][j] = count++;
		}
	}
	for (int i = 0; i < r; i++) {
		for (int j = 0; j < c; j++) {
			printf("%d\t", i_ptr[i][j]);
		}
		printf("\n");
	}
	my_2d_free((void **)i_ptr, r);

	/* 3x4 char array */
	printf("\n");
	r = 3; c = 4; count = 0;
	char ch = 'a';
	char **c_ptr = (char **)my_2d_alloc(r, c, sizeof(char));
	for (int i = 0; i < r; i++) {
		for (int j = 0; j < c; j++) {
			c_ptr[i][j] = ch + count;
			count++;
		}
	}
	for (int i = 0; i < r; i++) {
		for (int j = 0; j < c; j++) {
			printf("%c\t", c_ptr[i][j]);
		}
		printf("\n");
	}

	my_2d_free((void **)c_ptr, r);

	return 0;
}
