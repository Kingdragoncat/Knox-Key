#ifndef knoxkey_CTAP_CRYPTO_ATECC_H
#define knoxkey_CTAP_CRYPTO_ATECC_H

#include "ctap_crypto.h"
#include <sha256.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ctap_atecc_crypto_context {
	uint32_t placeholder;
} ctap_atecc_crypto_context_t;

ctap_crypto_status_t ctap_atecc_crypto_init(
	const ctap_crypto_t *crypto,
	uint32_t seed
);

ctap_crypto_status_t ctap_atecc_crypto_rng_init(
	const ctap_crypto_t *crypto,
	uint32_t seed
);

ctap_crypto_status_t ctap_atecc_crypto_rng_generate_data(
	const ctap_crypto_t *crypto,
	uint8_t *buffer,
	size_t length
);

ctap_crypto_status_t ctap_atecc_crypto_ecc_secp256r1_compute_public_key(
	const ctap_crypto_t *crypto,
	const uint8_t *private_key,
	uint8_t *public_key
);

ctap_crypto_status_t ctap_atecc_crypto_ecc_secp256r1_sign(
	const ctap_crypto_t *crypto,
	const uint8_t *private_key,
	const uint8_t *message_hash,
	size_t message_hash_size,
	uint8_t *signature,
	const uint8_t *optional_fixed_k
);

ctap_crypto_status_t ctap_atecc_crypto_ecc_secp256r1_shared_secret(
	const ctap_crypto_t *crypto,
	const uint8_t *public_key,
	const uint8_t *private_key,
	uint8_t *secret
);

ctap_crypto_status_t ctap_atecc_crypto_aes_256_cbc_encrypt(
	const ctap_crypto_t *crypto,
	const uint8_t *iv,
	const uint8_t *key,
	uint8_t *data,
	size_t data_length
);

ctap_crypto_status_t ctap_atecc_crypto_aes_256_cbc_decrypt(
	const ctap_crypto_t *crypto,
	const uint8_t *iv,
	const uint8_t *key,
	uint8_t *data,
	size_t data_length
);

ctap_crypto_status_t ctap_atecc_crypto_sha256_bind_ctx(
	const struct ctap_crypto *crypto,
	void *sha256_ctx
);

ctap_crypto_status_t ctap_atecc_crypto_sha256_compute_digest(
	const struct ctap_crypto *crypto,
	const uint8_t *data, size_t data_length,
	uint8_t *hash
);

#define CTAP_CRYPTO_ATECC_CONST_INIT(_context)                 \
	{                                                          \
		.context = _context,                                   \
		.init = ctap_atecc_crypto_init,                        \
		.rng_init = ctap_atecc_crypto_rng_init,                \
		.rng_generate_data = ctap_atecc_crypto_rng_generate_data,\
		.ecc_secp256r1_compute_public_key = ctap_atecc_crypto_ecc_secp256r1_compute_public_key,\
		.ecc_secp256r1_sign = ctap_atecc_crypto_ecc_secp256r1_sign,\
		.ecc_secp256r1_shared_secret = ctap_atecc_crypto_ecc_secp256r1_shared_secret,\
		.aes_256_cbc_encrypt = ctap_atecc_crypto_aes_256_cbc_encrypt,\
		.aes_256_cbc_decrypt = ctap_atecc_crypto_aes_256_cbc_decrypt,\
		.sha256_bind_ctx = ctap_atecc_crypto_sha256_bind_ctx,  \
		.sha256_compute_digest = ctap_atecc_crypto_sha256_compute_digest,\
		.sha256 = &hash_alg_sha256,                            \
	}

#ifdef __cplusplus
}
#endif

#endif // knoxkey_CTAP_CRYPTO_ATECC_H
