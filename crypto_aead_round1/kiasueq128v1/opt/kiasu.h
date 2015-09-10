#ifndef KIASU_H
#define KIASU_H

void kiasu_aead_encrypt(const unsigned char *ass_data, unsigned long long int ass_data_len, 
			const unsigned char *message, unsigned long long int m_len, 
			const unsigned char *key, const unsigned char *nonce, 
			unsigned char *ciphertext, unsigned long long int *c_len);



int kiasu_aead_decrypt(const unsigned char *ass_data, unsigned long long ass_data_len, 
		       unsigned char *message, unsigned long long int *m_len, 
		       const unsigned char *key, const unsigned char *nonce, 
		       const unsigned char *c, unsigned long long c_len);


#endif