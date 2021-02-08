//	tklog.c
//	2021-02-02	Markku-Juhani O. Saarinen <mjos@mjos.fi>

//	Decompositions of Streebog/Kuznyechik S-Box (research code snippets)

#include <stdio.h>
#include <stdint.h>
#include <string.h>

//	8-bit GOST "Nonlinear bijection" S-Box, as it appears in
//	Sect 6.2. of RFC 6986 (GOST R 34.11-2012: Hash Function "Streebog")
//	Sect 4.1. of RFC 7801 (GOST R 34.12-2015: Block Cipher "Kuznyechik")

const uint8_t gost_pi[256] = {
	252, 238, 221,	17, 207, 110,  49,	22, 251, 196, 250,
	218,  35, 197,	 4,	 77, 233, 119, 240, 219, 147,  46,
	153, 186,  23,	54, 241, 187,  20, 205,	 95, 193, 249,
	 24, 101,  90, 226,	 92, 239,  33, 129,	 28,  60,  66,
	139,   1, 142,	79,	  5, 132,	2, 174, 227, 106, 143,
	160,   6,  11, 237, 152, 127, 212, 211,	 31, 235,  52,
	 44,  81, 234, 200,	 72, 171, 242,	42, 104, 162, 253,
	 58, 206, 204, 181, 112,  14,  86,	 8,	 12, 118,  18,
	191, 114,  19,	71, 156, 183,  93, 135,	 21, 161, 150,
	 41,  16, 123, 154, 199, 243, 145, 120, 111, 157, 158,
	178, 177,  50, 117,	 25,  61, 255,	53, 138, 126, 109,
	 84, 198, 128, 195, 189,  13,  87, 223, 245,  36, 169,
	 62, 168,  67, 201, 215, 121, 214, 246, 124,  34, 185,
	  3, 224,  15, 236, 222, 122, 148, 176, 188, 220, 232,
	 40,  80,  78,	51,	 10,  74, 167, 151,	 96, 115,  30,
	  0,  98,  68,	26, 184,  56, 130, 100, 159,  38,  65,
	173,  69,  70, 146,	 39,  94,  85,	47, 140, 163, 165,
	125, 105, 213, 149,	 59,   7,  88, 179,	 64, 134, 172,
	 29, 247,  48,	55, 107, 228, 136, 217, 231, 137, 225,
	 27, 131,  73,	76,	 63, 248, 254, 141,	 83, 170, 144,
	202, 216, 133,	97,	 32, 113, 103, 164,	 45,  43,	9,
	 91, 203, 155,	37, 208, 190, 229, 108,	 82,  89, 166,
	116, 210, 230, 244, 180, 192, 209, 102, 175, 194,  57,
	 75,  99, 182
};

//	=== Print the rotational-xor differential

int rol_diff(const uint8_t s[256])
{
	int x, y, d;

	for (x = 0; x < 256; x++) {

		y = ((x << 1) | (x >> 7)) & 0xFF;	//	rotated left by 1 bit
		d = s[y] ^ s[x];					//	xor-difference f(x) xor f(y)

		printf(" %02X", d);

		if ((x & 0xF) == 0xF)				//	newline
			printf("\n");
	}

	return 0;
}

//	=== check for equivalence with the pi permutation. 0 == match

int check_p(const uint8_t p[256], const char *name)
{
	int i, d;

	d = 0;
	for (i = 0; i < 0x100; i++) {
		if (p[i] != gost_pi[i]) {
			printf("%s[0x%02X] = 0x%02X	 Pi[0x%02X] = 0x%02X\n",
				name, i, p[i], i, gost_pi[i]);
			d++;
		}
	}

	printf("%s distance = %d\n", name, d);

	return d;
}

//	=== Compute the s-box in a similar way as gostbox.asm (self-contained)

int gostbox()
{
	const uint8_t tab_s[15]	 =	{
		0x01, 0xDD, 0x92, 0x4F, 0x93,
		0x99, 0x0B, 0x44, 0xD6, 0xD7,
		0x4E, 0xDC, 0x98, 0x0A, 0x45
	};
	const uint8_t tab_k[17] = {
		0xDC, 0xCE, 0xFA, 0xE8, 0xF8, 0xEA, 0xDE, 0xCC,
		0xEC, 0xFE, 0xCA, 0xD8, 0xC8, 0xDA, 0xEE, 0xFC,
		0	//	can be anything
	};

	uint8_t p[256] = { 0 };		//	permutation

	int i, j;
	uint8_t x, z;

	p[0] = 0xFC;				//	first entry
	x = 0x01;					//	exp(0)
	z = 0xEE;

	for (i = 0; i < 15; i++) {
		for (j = 0; j < 17; j++) {
			p[x] = z;					//	store at exp position
			x = (x << 1) ^ ((-(x >> 7)) & 0x1D);	//	lfsr step
			z = tab_k[j] ^ tab_s[i];	//	(last iteration will be ignored)
		}
		z = tab_k[i];
	}

	//	test it
	return check_p(p, "gostbox");
}

//	=== compute the s-box using TKLog decomposition

//	kappa is a linear function

int kappa(int x)
{
	return	(x & 1 ? 0x12 : 0) ^
			(x & 2 ? 0x26 : 0) ^
			(x & 4 ? 0x24 : 0) ^
			(x & 8 ? 0x30 : 0) ^ 0xDC;
}

//	sigma (or s) is a non-linear table; a permutation of 0..14

const int sigma[15] =
	{ 0, 12, 9, 8, 7, 4, 14, 6, 5, 10, 2, 11, 1, 3, 13 };

int tklog()
{
	int i, j, k;
	uint8_t x;
	uint8_t l[256] = { 0 }, e[256] = { 0 }, p[256] = { 0 };

	//	log and exp tables
	x = 0x01;
	for (i = 0; i < 256; i++) {
		l[x] = i;
		e[i] = x;
		x = (x << 1) ^ ((-(x >> 7)) & 0x1D);
	}

	//	generate values 1..255
	for (k = 1; k < 256; k++) {

		j = l[k] - 1;
		i = j / 17;						//	"row"
		j = j % 17;						//	"column"

		if (j < 16) {
			x = kappa(j) ^ e[ 17 * sigma[i] ];
		} else {
			x = kappa(i);
		}
		p[k] = x;
	}
	p[0] = 0xFC;

	return check_p(p, "tklog");
}

//	=== stub main() for basic tests

int main()
{
	gostbox();
	tklog();

	printf("\nRotational differentials   S(x) ^ S(x <<< 1):\n");
	rol_diff(gost_pi);

	return 0;
}

