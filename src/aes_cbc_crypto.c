#include <string.h>
#include <openssl/aes.h>
#include <stdio.h>

#ifndef AES_KEY_LENGTH
#	define AES_KEY_LENGTH 32	//256bit key
#endif




#ifndef AES_BLOCK_SIZE
#	define AES_BLOCK_SIZE 16
#endif



//return the encryted code size
//if fsz == 0
//will encrypt 0x10101010101010101010101010101010

//bug will apear if fsz < 0
int aes_cbc_pkcs7_encrypt(int fsz, unsigned char* from, int tsz, unsigned char* to, AES_KEY* key, unsigned char* iv)
{
	int blocks;		// Number of 128bit(16 Byte) blocks in *from
	int rest;		//rest Byte beside full blocks
	unsigned char padding_data;
	int padding_size;
	int sumsize;
	unsigned char last_block[AES_BLOCK_SIZE];
	
	
	blocks = fsz / AES_BLOCK_SIZE;
	rest = fsz % AES_BLOCK_SIZE;
	padding_size = AES_BLOCK_SIZE - rest;
	sumsize = fsz + padding_size;
	padding_data = padding_size;
	
	//check if tsz is large enough
	if(tsz<sumsize)
	{
		fprintf(stderr, "no enough space!!\n");
		return -1;
	}
	
	//copy the last block to last_block
	for(int i=0; i<=rest-1; i++)
		last_block[i] = from[fsz-rest+i];
	
	//padding the last block
	for(int i=rest; i <= AES_BLOCK_SIZE-1; i++)
		last_block[i] = padding_data;
	
	//encrypt the data with full blocks
	AES_cbc_encrypt(from, to, fsz-rest, key, iv, AES_ENCRYPT);
	
	//encrypt the last block with padding
	AES_cbc_encrypt(last_block, to + blocks*AES_BLOCK_SIZE, AES_BLOCK_SIZE, key, iv, AES_ENCRYPT);
	
	return sumsize;
}


//return true data size
int aes_cbc_pkcs7_decrypt(int fsz, unsigned char* from, int tsz, unsigned char* to, AES_KEY* key, unsigned char* iv)
{
	//full block number
	int blocks;
	unsigned char last_block[AES_BLOCK_SIZE];
	unsigned char padding_data;
	int padding_size;
	int truesize;
	
	
	if(fsz%AES_BLOCK_SIZE != 0)
	{
		fprintf(stderr, "encrytion code should aways be a multiply of AES_BLOCK_SIZE\n");
		return -1;
	}
	
	if (fsz == 0)
		return 0;
		
	//decrypt all but the last_block
	AES_cbc_encrypt(from, to, fsz-AES_BLOCK_SIZE, key, iv, AES_DECRYPT);
	
	//decrypt the last block
	AES_cbc_encrypt(from+fsz-AES_BLOCK_SIZE, last_block, AES_BLOCK_SIZE, key, iv, AES_DECRYPT);
	
	padding_data = last_block[AES_BLOCK_SIZE - 1];
	padding_size = padding_data;
	
	truesize = fsz - padding_size;
	
	//combine last_block to the target buf;
	for(int i=0; i <= AES_BLOCK_SIZE-padding_size-1; i++)
		to[fsz-AES_BLOCK_SIZE+i] = last_block[i];
	
	return truesize;
}



//test prog
int main()
{
	unsigned char user_key[32]="0123456789abcdef0123456789abcde"; //append 'f'
	unsigned char iv[16]="9876543210fedcb";	//append 'a'
	unsigned char data[]="01234567890123456789012345678901234567890123456789";  //74 including 0
	unsigned char de[300];

	unsigned char en[300];
	for(int i=0; i<= 299; i++)
		en[i] = de[i] = 1;
		
		
	int size;
	user_key[31]='f';
	iv[15] = 'a';
	
	AES_KEY key;
	AES_set_encrypt_key(user_key, 256, &key);
	size = aes_cbc_pkcs7_encrypt(51, data, 300, en, &key, iv);
	
	printf("ENCRYPTING: \nencrypt size : %d\ncontent:\n", size);
	for(int i=0; i <= size-1; i++)
	{
		printf("%02x", en[i]);
		if(i%25==24)
			printf("\n");
		else
			printf(":");
	}
	
	
	
	
	AES_set_decrypt_key(user_key, 256, &key);
	strcpy(iv,"9876543210fedcb");
	iv[15]='a';
	
	size = aes_cbc_pkcs7_decrypt(size, en, 300, de, &key, iv);
	
	printf("\n\nDECRYPTING: \ndecrypt size : %d\ncontent:\n", size);
	printf("%s", de);
	
	
	
	return 0;
	
}

