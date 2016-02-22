/*
  Copyright (c) 2012-2016, Matthias Schiffer <mschiffer@universe-factory.net>
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
   \file

   The XMM Salsa20/12 implementation for SSE2-capable x86 systems

   The assembly implementations were written by D. J. Bernstein and are
   Public Domain. For more information see http://cr.yp.to/snuffle.html
*/


#include "../../../../alloc.h"
#include "../../../../crypto.h"
#include "../../../../cpuid.h"


/** The length of the key used by Salsa20/12 */
#define KEYBYTES 32


/** The actual Salsa20/12 assembly implementation */
int fastd_salsa2012_xmm_xor(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *n, const unsigned char *k);


/** The cipher state */
struct fastd_cipher_state {
	uint8_t key[KEYBYTES];		/**< The encryption key */
};


/** Checks if the runtime platform supports SSE2 */
static bool salsa2012_available(void) {
	return fastd_cpuid() & CPUID_SSE2;
}

/** Initializes the cipher state */
static fastd_cipher_state_t * salsa2012_init(const uint8_t *key) {
	fastd_cipher_state_t *state = fastd_new(fastd_cipher_state_t);
	memcpy(state->key, key, KEYBYTES);

	return state;
}

/** XORs data with the Salsa20/12 cipher stream */
static bool salsa2012_crypt(const fastd_cipher_state_t *state, fastd_block128_t *out, const fastd_block128_t *in, size_t len, const uint8_t *iv) {
	fastd_salsa2012_xmm_xor(out->b, in->b, len, iv, state->key);
	return true;
}

/** Frees the cipher state */
static void salsa2012_free(fastd_cipher_state_t *state) {
	if (state) {
		secure_memzero(state, sizeof(*state));
		free(state);
	}
}


/** The xmm salsa2012 implementation */
const fastd_cipher_t fastd_cipher_salsa2012_xmm = {
	.available = salsa2012_available,

	.init = salsa2012_init,
	.crypt = salsa2012_crypt,
	.free = salsa2012_free,
};
