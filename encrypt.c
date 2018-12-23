
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define PRIKEY "keys/private_key.pem"
#define PUBKEY "keys/public_key.pem"




//return length
int cencrypt(int fsz, const unsigned char* from, int tsz, unsigned char* to, RSA* rsa)
{
	int rsa_len=0;
	rsa_len = RSA_size(rsa);
	int blocks = fsz / (rsa_len-11); //   |____|____|____|..|     ----->>>> from can be parted as 3 blocks and a rest
	int rest = fsz % (rsa_len-11);
	int encryptsize=0;
	int donelength=0;
	int loopsize=0;  //only for test, each time rsa_len should be generated by encrytion
	
	encryptsize=blocks*rsa_len;
	if(rest)
		encryptsize+=1*rsa_len;
		
		
	if(tsz<encryptsize)
		return -1;
	
	
	//encrypt full blocks
	for(int i=0; i<=blocks-1; i++)
	{
		if((loopsize = RSA_public_encrypt(rsa_len-11 ,(unsigned char*)(from+i*(rsa_len-11)), (unsigned char*)(to+i*rsa_len), rsa, RSA_PKCS1_PADDING)) < 0)
		{
			fprintf(stderr, "encryt error\n");
			return -1;
		}
		///////////////////////////////
		//test for loopsize value
		printf("block : %d", loopsize);
		///////////////////////////////
		donelength+=loopsize;
	}
	
	if(rest)
	{
		if((loopsize = RSA_public_encrypt(rest ,(unsigned char*)(from+blocks*(rsa_len-11)), (unsigned char*)(to+blocks*rsa_len), rsa, RSA_PKCS1_PADDING)) < 0)
		{
			fprintf(stderr, "encrypt error\n");
			return -1;
		}
		///////////////////////////////
		//test for loopsize value 2
		printf("rest : %d", loopsize);
		///////////////////////////////
		
		donelength+=loopsize;
	}
	
	return donelength;	
}


int cdecrypt(int fsz, const unsigned char* from, int tsz, unsigned char* to, RSA* rsa)
{
	int rsa_len=0;
	
	
	//rest bytes
	int rest = 0;
	
	//this blocks should contain the rest
	int blocks = 0;
	unsigned char* tmp;  //store the last few byte outside block
	
	//int loopsize = 0; //only for test
	int sumsize = 0;
	
	rsa_len = RSA_size(rsa);
	blocks = fsz/rsa_len;
	
	//////////////////////
	//only for test
	if(fsz%rsa_len != 0)
	{
		fprintf(stderr, "error encryption length, cnnt decryp\n");
		return -1;
	}
	//////////////////////////
	
	
	//decrypt last block see if fsz is enough
	tmp = malloc(rsa_len);
	if((rest = RSA_private_decrypt(rsa_len, (unsigned char*)(from+fsz-rsa_len), (unsigned char*)tmp, rsa, RSA_PKCS1_PADDING)) < 0)
	{
		fprintf(stderr, "decrypt last block error\n");
		return -1; 
	}
	sumsize = rest + (blocks-1)*(rsa_len-11);
	if(tsz < sumsize)
	{
		fprintf(stderr, "no enoungh space\n");
		return -1;
	}
	
	//decrypt the rest blocks 
	for(int i=0; i<=blocks-2; i++)
	{
		if(RSA_private_decrypt(rsa_len, (unsigned char*)(from+i*rsa_len), (unsigned char*)(to+i*(rsa_len-11)), rsa, RSA_PKCS1_PADDING) != rsa_len-11)
		{
			fprintf(stderr,"decryton error\n");
			return -1;
		}
	}
	
	//copy from tmp to the data(*to)
	int last_start = (blocks-1)*(rsa_len-11); 
	for(int i=0; i<=rest-1; i++)
		to[last_start+i]=tmp[i];
	
	return sumsize;
	
	
}


int encrypt_fpem(int fsz, const unsigned char* from, int tsz, unsigned char* to)
{
	FILE* fp=0;
	RSA* rsa=0;
	int encrypt_size = 0;
	
    if ((fp = fopen(PUBKEY, "r")) == 0)
    {
    	fprintf(stderr, "open public_key file error\n");
        return 0;
    }
 	//get key from pem file
    if ((rsa = PEM_read_RSAPublicKey(fp, 0, 0, 0)) == 0)
    {
    	fprintf(stderr, "get key error\n");
    	return 0;
	}
	
	if((encrypt_size = cencrypt(fsz, from, tsz, to, rsa)) < 0)
	{
		fprintf(stderr, "encrypt error\n");
		return -1;
	}
	
	fclose(fp);
	free(rsa);
	
	return encrypt_size;
}

int decrypt_fpem(int fsz, const unsigned char* from, int tsz, unsigned char* to)
{
	FILE* fp=0;
	RSA* rsa=0;
	int decrypt_size = 0;
	
    if ((fp = fopen(PRIKEY, "r")) == 0)
    {
    	fprintf(stderr, "open public_key file error\n");
        return 0;
    }
 	//get key from pem file
    if ((rsa = PEM_read_RSAPrivateKey(fp, 0, 0, 0)) == 0)
    {
    	fprintf(stderr, "get key error\n");
    	return 0;
	}
	
	if((decrypt_size = cdecrypt(fsz, from, tsz, to, rsa)) < 0)
	{
		fprintf(stderr, "encrypt error\n");
		return -1;
	}
	
	fclose(fp);
	free(rsa);
	
	return decrypt_size;
}



//test prog
int main()
{
	unsigned char* en=0;
	unsigned char* de=0;
	unsigned char* data = 0;
	
	data = malloc(3910); 
	en=malloc(4096);
	de=malloc(3910);
	
	for(int i=0; i<=3909; i++)
		data[i]=i%256;
		
	int size;
	size=encrypt_fpem(3910,data, 4096, en);
	printf("\n");
	for(int i=0;i<=255;i++)
	{
		printf("%02x", en[i]);
		if(i%15==14)
			printf("\n");
		else
			printf(":");
	}
	
	printf("\nDECRYPTION: \n");
	size=decrypt_fpem(4096,en, 3910, de);
	

	for(int i=0;i<=3909;i++)
	{
		printf("%02x", de[i]);
		if(i%25==24)
			printf("\n");
		else
			printf(":");
	}
	free(en); 
	free(de);
	free(data);
}

