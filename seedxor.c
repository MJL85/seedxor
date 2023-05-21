#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <malloc.h>
#include <stdlib.h>

#define BUFSIZE		4096
//#define _XOR_DEBUG 1

int get_input(char* prompt, char* buf, int buf_size) {
	printf("%s: ", prompt);
	fflush(0);
	read(0, (void*)buf, buf_size-1);
	return 1;
}

/*
 * Convert a hex string into an array of shorts
 */
unsigned short* hexstr2int(char* hexstr, int* out_len) {
	int len = strlen(hexstr);
	unsigned short* r = (unsigned short*)malloc(sizeof(unsigned short*) * len + 1);
	memset(r, 0, sizeof(unsigned short*) * len + 1);
	int c = 0;

	for (int i = 0; i < len; i+=2, hexstr+=2) {
		if (*hexstr == '\r' || *hexstr == '\n') { break; }

		unsigned int h;
		sscanf(hexstr, "%02x", &h);
		r[c++] = h;
	}

	*out_len = c;
	return r;
}

/*
 * Convert a non-hex string into an array of shorts
 */
unsigned short* str2int(char* str) {
	int len = strlen(str);
	unsigned short* r = (unsigned short*)malloc(sizeof(unsigned short*) * len + 1);
	memset(r, 0, sizeof(unsigned short*) * len + 1);
	int c = 0;

	for (int i = 0; i < len; i++, str++) {
		if (*str == '\r' || *str == '\n') { break; }
		
		char h;
		sscanf(str, "%c", &h);
		r[c++] = h;
	}

	return r;
}

/*
 * Convert an array of shorts to a hex string.
 */
char* int2hexstr(unsigned short* s, int len) {
	char* r = (char*)malloc(sizeof(char) * len + 1);
	memset(r, 0, sizeof(char) * len + 1);

	for (int i = 0; i < len; i++) {
		sprintf(r+(i*2), "%02x", s[i]);
	}

	return r;
}

/*
 * xor the data and password.
 *
 * The data must be hexidecimal.
 * The password is treated as ascii.
 *
 * The password will be used repeatedly to cover the full length of the data.
 *
 * Example: data = 0xdeafbeef (length 4)
 *          pass = ab         (length 2)
 *           
 *                 de ad be ef
 *             xor  a  b  a  b
 *             ---------------
 *                 bf cf df 8d
 *
 * If the password is longer than the data, then the process repeats with the
 * remainder of the password that was not used on the first pass.
 *
 * Example: data = 0xdeadbeef (length 4)
 *          pass = abcdef     (length 6)
 *
 *                 de ad be ef
 *             xor  a  b  c  d
 *             xor  e  f  a  b
 *             ---------------
 *                 da a9 bc e9 
 */
unsigned short* xor(unsigned short* data, unsigned short* passwd, int data_len, int passwd_len) {
	unsigned short* r = (unsigned short*)malloc(sizeof(unsigned short*) * data_len);
	memcpy(r, data, sizeof(unsigned short*) * data_len);
	int pi = 0;

	#ifdef _XOR_DEBUG
	printf("\n");
	#endif

	while (pi < passwd_len) {
		for (int i = 0; i < data_len; i++) {
			#ifdef _XOR_DEBUG
			printf("%02x ^ %c = %02x\n", r[i], passwd[pi%passwd_len], (r[i]^passwd[pi%passwd_len]));
			#endif
			r[i] ^= passwd[pi % passwd_len];
			++pi;
		}
	}

	return r;
}

#ifdef _XOR_DEBUG
void test(char* data, char* pass) {
	/* convert hex data to ints */
	printf("Data:    ");
	int d_len;
	unsigned short* di = hexstr2int(data, &d_len);
	for (int i = 0; i < d_len; ++i) {
		printf("%02x ", di[i]);
	}
	printf("\n");
	
	/* convert hex data to ints */
	printf("Pass:    ");
	unsigned short* pi = str2int(pass);
	for (int i = 0; i < strlen(pass); ++i) {
		printf("%02x ", pi[i]);
	}
	printf("\n");

	/* xor to get the encoded data */
	printf("Encoded: ");
	unsigned short* enc = xor(di, pi, d_len, strlen(pass));
	for (int i = 0; i < d_len; ++i) {
		printf("%02x ", enc[i]);
	}
	printf("\n");
	char* encoded_str = int2hexstr(enc, d_len);
	printf("Encoded: %s\n", encoded_str);

	/* xor to get the decoded data */
	printf("Decoded: ");
	unsigned short* dec = xor(enc, pi, d_len, strlen(pass));
	for (int i = 0; i < d_len; ++i) {
		printf("%02x ", dec[i]);
	}
	printf("\n");

	char* decoded_str = int2hexstr(dec, d_len);
	printf("Decoded: %s\n", decoded_str);

	free(di);
	free(pi);
	free(enc);
	free(dec);
	free(encoded_str);
	free(decoded_str);
}
#endif

int main(int argc, char** argv) {
	#ifdef _XOR_DEBUG
	test("deadbeef", "ab");
	test("deadbeef", "abcdef");
	return 0;
	#endif

	char passwd[BUFSIZE] = {0};
	char data[BUFSIZE]   = {0};

	/* read in the passwords */
	if (!get_input("Enter password", passwd, sizeof(passwd))) {
		return 1;
	}
	if (!get_input("Enter password again", data, sizeof(data))) {
		return 1;
	}
	if (strcmp(passwd, data)) {
		printf("Passwords do not match.\n");
		return 1;
	}

	/* read in the data */
	memset(data, 0, BUFSIZE);
	get_input("Enter hexidecimal data", data, BUFSIZE);

	/* encode */
	int d_len;
	unsigned short* di = hexstr2int(data, &d_len);
	unsigned short* pi = str2int(passwd);
	unsigned short* ei = xor(di, pi, d_len, strlen(passwd));
	char* encoded_str  = int2hexstr(ei, d_len);
	printf("%s\n", encoded_str);

	free(di);
	free(pi);
	free(ei);
	free(encoded_str);

	return 0;
}

