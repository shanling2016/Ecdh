#include <iostream>
#include <string.h>
#include <stdio.h>
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/ecdsa.h>
#include <openssl/objects.h>
#include <openssl/err.h>

#define MALLOC_SIZE 0x400u
#define PUB_KEY "04af8efd25576ed8ab550b5d65e0ad36916566708cbe5a3ad54e127ae42951d09212fc04a1ac463aff66d3dbf5ac6f46a1"

using namespace std;

/*hex转bin*/
int String2Buffer(unsigned char* src, int srclen, unsigned char* dest) {
	int i = 0;
	if (srclen % 2 != 0) return 0;
	for (i = 0; i < srclen / 2; i++)
	{
		char tmp[3];
		tmp[0] = *(src + 2 * i);
		tmp[1] = *(src + 2 * i + 1);
		tmp[2] = 0;
		int out = 0;
		sscanf(tmp, "%x", &out);
		unsigned char ch = (unsigned char)out;
		*(dest + i) = ch;
	}
	return i;
}

/*bin转hex*/
int Buffer2String(unsigned char* src, int srclen, unsigned char* dest) {
	int i;
	for (i = 0; i < srclen; i++)
	{
		char tmp[3] = { 0 };
		sprintf(tmp, "%x", *(src + i));
		if (strlen(tmp) == 1) {
			strcat((char*)dest, "0");
			strncat((char*)dest, tmp, 1);
		}
		else if (strlen(tmp) == 2) {
			strncat((char*)dest, tmp, 2);
		}
		else {
			strcat((char*)dest, "00");
		}
	}
	return i * 2;
}

/*16进制展示数据*/
static void display(const char *tripstr, const char *src, const int src_size)
{
	int i = 0;
	if (tripstr != NULL) {
		printf("%s", tripstr);
	}
	unsigned char*tmp = (unsigned char*)malloc(MALLOC_SIZE);
	memset(tmp, 0, MALLOC_SIZE);
	Buffer2String((unsigned char*)src, src_size, tmp);
	cout << tmp << endl;
	free(tmp);
}

int main()
{
	EC_KEY * ec_key;
	const EC_GROUP * ec_group;
	const EC_POINT * ec_point;

	unsigned char * c_Sha_Key = (unsigned char*)malloc(MALLOC_SIZE); memset(c_Sha_Key, 0, MALLOC_SIZE);
	unsigned char * s_Sha_Key = (unsigned char*)malloc(MALLOC_SIZE); memset(s_Sha_Key, 0, MALLOC_SIZE);
	unsigned char * c_Pub_Key = (unsigned char*)malloc(MALLOC_SIZE); memset(c_Pub_Key, 0, MALLOC_SIZE);
	unsigned char * s_Pub_Key = (unsigned char*)malloc(MALLOC_SIZE); memset(s_Pub_Key, 0, MALLOC_SIZE);

	if ((ec_key = EC_KEY_new()) == NULL) {
		printf("Error：EC_KEY_new()\n");
		return 0;
	}

	ec_key = EC_KEY_new_by_curve_name(NID_secp192k1);
	ec_group = EC_KEY_get0_group(ec_key);
	ec_point = EC_POINT_new(ec_group);

	if (EC_KEY_generate_key(ec_key) == 1)
	{

		EC_POINT_point2oct(ec_group, EC_KEY_get0_public_key(ec_key), POINT_CONVERSION_COMPRESSED, c_Pub_Key, 25, 0);

		cout << "/* 一次协商" << endl;
		display("c_Pub_Key : ", (const char*)c_Pub_Key, 25);
		String2Buffer((unsigned char*)PUB_KEY, 98, s_Pub_Key);
		
		if (EC_POINT_oct2point(ec_group, (EC_POINT*)ec_point, s_Pub_Key, 49, 0) == 1)
		{
			ECDH_compute_key(c_Sha_Key, 16, ec_point, ec_key, 0);
			display("c_Sha_Key : ", (const char*)c_Sha_Key, 16);
		}

		cout << "\n/* 二次协商" << endl;
		char* ch1 = (char*)malloc(MALLOC_SIZE); memset(ch1, 0, MALLOC_SIZE);
		cout << "s_Pub_Key : ";cin >> ch1;
		String2Buffer((unsigned char*)ch1, 98, s_Pub_Key);

		if (EC_POINT_oct2point(ec_group, (EC_POINT*)ec_point, s_Pub_Key, 49, 0) == 1)
		{
			ECDH_compute_key(s_Sha_Key, 16, ec_point, ec_key, 0);
			display("s_Sha_Key : ", (const char*)s_Sha_Key, 16);
		}
	}

	EC_KEY_free(ec_key);
	free(s_Sha_Key);
	free(c_Sha_Key);
	free(s_Pub_Key);
	free(c_Pub_Key);

	getchar();

	return 0;
}