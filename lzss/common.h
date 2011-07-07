/* LZSS encoder-decoder  (c) Haruhiko Okumura */

#define EI 11  /* typically 10..13 */
#define EJ  4  /* typically 4..5 */
#define P   1  /* If match length <= P then output one character */
#define N (1 << EI)  /* buffer size */
#define F ((1 << EJ) + P)  /* lookahead buffer size */

//unsigned short bit_buffer, bit_mask;
unsigned char buffer[N * 2];
