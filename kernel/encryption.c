// CaneOS Encryption
#include <stdint.h>


#include <stdint.h>
#include <string.h>

typedef enum { ENC_XOR, ENC_AES, ENC_SHA256 } EncAlgo;
struct Key {
	uint8_t data[32];
	int size;
};
struct Key key_store[8];
int key_count = 0;

void add_key(const uint8_t* data, int size) {
	if (key_count < 8) {
		memcpy(key_store[key_count].data, data, size);
		key_store[key_count].size = size;
		key_count++;
	}
}

void encrypt_data(const void* in, void* out, int size, EncAlgo algo, int key_id) {
	if (algo == ENC_XOR) {
		uint8_t k = key_id < key_count ? key_store[key_id].data[0] : 0xAA;
		for (int i = 0; i < size; i++) {
			((uint8_t*)out)[i] = ((const uint8_t*)in)[i] ^ k;
		}
	} else if (algo == ENC_AES) {
		// Demo: pretend to use AES
		memcpy(out, in, size);
	} else if (algo == ENC_SHA256) {
		// Demo: pretend to use SHA256
		memset(out, 0x5A, size);
	}
}

void decrypt_data(const void* in, void* out, int size, EncAlgo algo, int key_id) {
	if (algo == ENC_XOR) {
		uint8_t k = key_id < key_count ? key_store[key_id].data[0] : 0xAA;
		for (int i = 0; i < size; i++) {
			((uint8_t*)out)[i] = ((const uint8_t*)in)[i] ^ k;
		}
	} else if (algo == ENC_AES) {
		// Demo: pretend to use AES
		memcpy(out, in, size);
	} else if (algo == ENC_SHA256) {
		// Demo: SHA256 is one-way, cannot decrypt
		memset(out, 0, size);
	}
}

void encrypt_hardware(const void* in, void* out, int size) {
	// Demo: pretend to use hardware acceleration
	memcpy(out, in, size);
}
