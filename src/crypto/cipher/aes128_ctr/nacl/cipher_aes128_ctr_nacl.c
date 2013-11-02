/*
  Copyright (c) 2012-2013, Matthias Schiffer <mschiffer@universe-factory.net>
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


#include "../../../../fastd.h"
#include <crypto_stream_aes128ctr.h>


struct fastd_cipher_state {
	fastd_buffer_t d;
};


static fastd_cipher_context_t* aes128_ctr_initialize(fastd_context_t *ctx UNUSED) {
	return NULL;
}

static fastd_cipher_state_t* aes128_ctr_init_state(fastd_context_t *ctx, const fastd_cipher_context_t *cctx UNUSED, const uint8_t *key) {
	fastd_block128_t k;
	memcpy(k.b, key, sizeof(fastd_block128_t));

	fastd_cipher_state_t *state = malloc(sizeof(fastd_cipher_state_t));

	state->d = fastd_buffer_alloc(ctx, crypto_stream_aes128ctr_BEFORENMBYTES, 0, 0);
	crypto_stream_aes128ctr_beforenm(state->d.data, k.b);

	return state;
}

static bool aes128_ctr_crypt(fastd_context_t *ctx UNUSED, const fastd_cipher_state_t *state, fastd_block128_t *out, const fastd_block128_t *in, size_t len, const fastd_block128_t *iv) {
	crypto_stream_aes128ctr_xor_afternm(out->b, in->b, len, iv->b, state->d.data);
	return true;
}

static void aes128_ctr_free_state(fastd_context_t *ctx UNUSED, fastd_cipher_state_t *state) {
	if (state) {
		fastd_buffer_free(state->d);
		free(state);
	}
}

static void aes128_ctr_free(fastd_context_t *ctx UNUSED, fastd_cipher_context_t *cctx UNUSED) {
}

const fastd_cipher_t fastd_cipher_aes128_ctr_nacl = {
	.name = "nacl",

	.initialize = aes128_ctr_initialize,
	.init_state = aes128_ctr_init_state,
	.crypt = aes128_ctr_crypt,

	.free_state = aes128_ctr_free_state,
	.free = aes128_ctr_free,
};