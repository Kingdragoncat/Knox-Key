#include "ctap_crypto_atecc.h"
#include "compiler.h"

#include "aes.h"
#include <uECC.h>
#include <assert.h>
#include <cryptoauthlib.h>
#include <atca_cfgs.h>

// verify that TinyAES is compiled with AES-256-CBC support
static_assert(TINYAES_AES_BLOCKLEN == CTAP_CRYPTO_AES_BLOCK_SIZE, "TINYAES_AES_BLOCKLEN == CTAP_CRYPTO_AES_BLOCK_SIZE");
static_assert(TINYAES_ENABLE_AES256 == 1, "unexpected TINYAES_ENABLE_AES256 value for AES-256-CBC");
static_assert(TINYAES_ENABLE_CBC == 1, "unexpected TINYAES_ENABLE_CBC value for AES-256-CBC");
static_assert(TINYAES_AES_KEYLEN == 32, "unexpected TINYAES_AES_KEYLEN value for AES-256-CBC");
static_assert(TINYAES_AES_KEYLEN == CTAP_CRYPTO_AES_256_KEY_SIZE, "TINYAES_AES_KEYLEN == CTAP_CRYPTO_AES_256_KEY_SIZE");

static int micro_ecc_compatible_rng(void *ctx, uint8_t *dest, unsigned size) {
	const ctap_crypto_t *const crypto = ctx;
	ctap_crypto_status_t status = crypto->rng_generate_data(crypto, dest, size);
	// translate the status to the uECC-compatible return value
	return status == CTAP_CRYPTO_OK ? 1 : 0;
}

ctap_crypto_status_t ctap_atecc_crypto_init(
	const ctap_crypto_t *const crypto,
	uint32_t seed
) {
    ATCAIfaceCfg cfg = cfg_ateccx08a_i2c_default;
    cfg.atcai2c.bus = 1; // Used by our HAL
    cfg.atcai2c.baud = 100000;
    
    if (atcab_init(&cfg) != ATCA_SUCCESS) {
        return CTAP_CRYPTO_ERROR;
    }
	return crypto->rng_init(crypto, seed);
}

ctap_crypto_status_t ctap_atecc_crypto_rng_init(
	const ctap_crypto_t *const crypto,
	uint32_t seed
) {
    (void)crypto;
    (void)seed; // Seed is ignored since ATECC608B has a true TRNG
	return CTAP_CRYPTO_OK;
}

ctap_crypto_status_t ctap_atecc_crypto_rng_generate_data(
	const ctap_crypto_t *const crypto,
	uint8_t *const buffer,
	const size_t length
) {
    (void)crypto;
    size_t generated = 0;
    uint8_t rand_out[32];
    
    while (generated < length) {
        if (atcab_random(rand_out) != ATCA_SUCCESS) {
            return CTAP_CRYPTO_ERROR;
        }
        size_t chunk = length - generated;
        if (chunk > 32) chunk = 32;
        memcpy(buffer + generated, rand_out, chunk);
        generated += chunk;
    }

	return CTAP_CRYPTO_OK;
}

ctap_crypto_status_t ctap_atecc_crypto_ecc_secp256r1_compute_public_key(
	const ctap_crypto_t *const crypto,
	const uint8_t *const private_key,
	uint8_t *const public_key
) {
	if (uECC_compute_public_key(
		private_key,
		public_key,
		uECC_secp256r1(),
		micro_ecc_compatible_rng,
		(void *) crypto
	) != 1) {
		return CTAP_CRYPTO_ERROR;
	}
	return CTAP_CRYPTO_OK;
}

// uECC does not include this declaration in its header file
// because this function is not part of the public API (it is intended for testing only)
// (the functions is not declared as static, so it is possible to use with explicit declaration)
int uECC_sign_with_k(
	const uint8_t *private_key,
	const uint8_t *message_hash,
	unsigned hash_size,
	const uint8_t *k,
	uint8_t *signature,
	uECC_Curve curve,
	uECC_RNG_Function g_rng_function,
	void *g_rng_function_ctx
);

ctap_crypto_status_t ctap_atecc_crypto_ecc_secp256r1_sign(
	const ctap_crypto_t *const crypto,
	const uint8_t *const private_key,
	const uint8_t *const message_hash,
	const size_t message_hash_size,
	uint8_t *const signature,
	const uint8_t *const optional_fixed_k
) {
	if (optional_fixed_k != NULL) {
		if (uECC_sign_with_k(
			private_key,
			message_hash,
			message_hash_size,
			optional_fixed_k,
			signature,
			uECC_secp256r1(),
			micro_ecc_compatible_rng,
			(void *) crypto
		) != 1) {
			return CTAP_CRYPTO_ERROR;
		}
		return CTAP_CRYPTO_OK;
	}
	if (uECC_sign(
		private_key,
		message_hash,
		message_hash_size,
		signature,
		uECC_secp256r1(),
		micro_ecc_compatible_rng,
		(void *) crypto
	) != 1) {
		return CTAP_CRYPTO_ERROR;
	}
	return CTAP_CRYPTO_OK;
}

ctap_crypto_status_t ctap_atecc_crypto_ecc_secp256r1_shared_secret(
	const ctap_crypto_t *const crypto,
	const uint8_t *const public_key,
	const uint8_t *const private_key,
	uint8_t *const secret
) {
	if (uECC_shared_secret(
		public_key,
		private_key,
		secret,
		uECC_secp256r1(),
		micro_ecc_compatible_rng,
		(void *) crypto
	) != 1) {
		return CTAP_CRYPTO_ERROR;
	}
	return CTAP_CRYPTO_OK;
}

ctap_crypto_status_t ctap_atecc_crypto_aes_256_cbc_encrypt(
	const ctap_crypto_t *const crypto,
	const uint8_t *const iv,
	const uint8_t *const key,
	uint8_t *const data,
	const size_t data_length
) {
	(void) crypto;
	struct AES_ctx ctx;
	AES_init_ctx_iv(&ctx, key, iv);
	AES_CBC_encrypt_buffer(&ctx, data, data_length);
	return CTAP_CRYPTO_OK;
}

ctap_crypto_status_t ctap_atecc_crypto_aes_256_cbc_decrypt(
	const ctap_crypto_t *const crypto,
	const uint8_t *const iv,
	const uint8_t *const key,
	uint8_t *const data,
	const size_t data_length
) {
	(void) crypto;
	struct AES_ctx ctx;
	AES_init_ctx_iv(&ctx, key, iv);
	AES_CBC_decrypt_buffer(&ctx, data, data_length);
	return CTAP_CRYPTO_OK;
}

ctap_crypto_status_t ctap_atecc_crypto_sha256_bind_ctx(
	const struct ctap_crypto *const crypto,
	void *const ctx
) {
	(void) crypto;
	sha256_init(ctx);
	return CTAP_CRYPTO_OK;
}

ctap_crypto_status_t ctap_atecc_crypto_sha256_compute_digest(
	const struct ctap_crypto *const crypto,
	const uint8_t *const data,
	const size_t data_length,
	uint8_t *const hash
) {
	(void) crypto;
	sha256_ctx_t ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, data, data_length);
	sha256_final(&ctx, hash);
	return CTAP_CRYPTO_OK;
}
