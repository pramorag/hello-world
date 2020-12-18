#include <stdio.h>
#include <rpc/rpc.h>

int main()
{
	XDR xdre;
	char enc_buf[20];

	XDR xdrd;
	char dec_buf[20];

	printf("Starting \n");

	xdrmem_create(&xdre, enc_buf, 20, XDR_ENCODE);

	printf("Encode mem create done \n");

	int i = 35, j = 45;

	xdr_int(&xdre, &i);
	xdr_int(&xdre, &j);

	printf("Encode done \n");

	xdrmem_create(&xdrd, enc_buf, 20, XDR_DECODE);

	printf("Decode mem create done \n");

	int ii = 0, jj = 0;
	xdr_int(&xdrd, &ii);
	xdr_int(&xdrd, &jj);

	printf("Decode done \n");

	printf("ii = %d, jj = %d \n", ii, jj);

	return 0;
}
