#ifndef CUSTOMBUILD_CERTIFICATE_CA_H
#define CUSTOMBUILD_CERTIFICATE_CA_H

#include "../../ESPEasy_common.h"

#if FEATURE_TLS

#include <t_bearssl_x509.h>

#if __has_include("../../Custom_CERTS.h")
#include "../../Custom_CERTS.h"
#else

// Root Certificates taken from:
// https://github.com/arendst/Tasmota/blob/b2b839fa83b622c3d28324ac4d6db7ab133741c6/tasmota/tasmota_support/tasmota_ca.ino#L177


#if ! defined(OMIT_LETS_ENCRYPT_CERT)

/*********************************************************************************************\
 * LetsEncrypt ISRG Root X1 certificate, RSA 4096 bits SHA 256, valid until 20350604
 *
 * https://letsencrypt.org/certificates/
 * Downloaded from https://letsencrypt.org/certs/isrgrootx1.pem
 *
 * to convert do: `bearssl ta isrgrootx1.pem`
 * then copy and paste below, chain the generic names to the same as below
 * remove "static" and add "PROGMEM"
\*********************************************************************************************/

static const unsigned char PROGMEM LetsEncrypt_ISRG_Root_X1_DN[] = {
	0x30, 0x4F, 0x31, 0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13,
	0x02, 0x55, 0x53, 0x31, 0x29, 0x30, 0x27, 0x06, 0x03, 0x55, 0x04, 0x0A,
	0x13, 0x20, 0x49, 0x6E, 0x74, 0x65, 0x72, 0x6E, 0x65, 0x74, 0x20, 0x53,
	0x65, 0x63, 0x75, 0x72, 0x69, 0x74, 0x79, 0x20, 0x52, 0x65, 0x73, 0x65,
	0x61, 0x72, 0x63, 0x68, 0x20, 0x47, 0x72, 0x6F, 0x75, 0x70, 0x31, 0x15,
	0x30, 0x13, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x0C, 0x49, 0x53, 0x52,
	0x47, 0x20, 0x52, 0x6F, 0x6F, 0x74, 0x20, 0x58, 0x31
};

static const unsigned char PROGMEM LetsEncrypt_ISRG_Root_X1_RSA_N[] = {
	0xAD, 0xE8, 0x24, 0x73, 0xF4, 0x14, 0x37, 0xF3, 0x9B, 0x9E, 0x2B, 0x57,
	0x28, 0x1C, 0x87, 0xBE, 0xDC, 0xB7, 0xDF, 0x38, 0x90, 0x8C, 0x6E, 0x3C,
	0xE6, 0x57, 0xA0, 0x78, 0xF7, 0x75, 0xC2, 0xA2, 0xFE, 0xF5, 0x6A, 0x6E,
	0xF6, 0x00, 0x4F, 0x28, 0xDB, 0xDE, 0x68, 0x86, 0x6C, 0x44, 0x93, 0xB6,
	0xB1, 0x63, 0xFD, 0x14, 0x12, 0x6B, 0xBF, 0x1F, 0xD2, 0xEA, 0x31, 0x9B,
	0x21, 0x7E, 0xD1, 0x33, 0x3C, 0xBA, 0x48, 0xF5, 0xDD, 0x79, 0xDF, 0xB3,
	0xB8, 0xFF, 0x12, 0xF1, 0x21, 0x9A, 0x4B, 0xC1, 0x8A, 0x86, 0x71, 0x69,
	0x4A, 0x66, 0x66, 0x6C, 0x8F, 0x7E, 0x3C, 0x70, 0xBF, 0xAD, 0x29, 0x22,
	0x06, 0xF3, 0xE4, 0xC0, 0xE6, 0x80, 0xAE, 0xE2, 0x4B, 0x8F, 0xB7, 0x99,
	0x7E, 0x94, 0x03, 0x9F, 0xD3, 0x47, 0x97, 0x7C, 0x99, 0x48, 0x23, 0x53,
	0xE8, 0x38, 0xAE, 0x4F, 0x0A, 0x6F, 0x83, 0x2E, 0xD1, 0x49, 0x57, 0x8C,
	0x80, 0x74, 0xB6, 0xDA, 0x2F, 0xD0, 0x38, 0x8D, 0x7B, 0x03, 0x70, 0x21,
	0x1B, 0x75, 0xF2, 0x30, 0x3C, 0xFA, 0x8F, 0xAE, 0xDD, 0xDA, 0x63, 0xAB,
	0xEB, 0x16, 0x4F, 0xC2, 0x8E, 0x11, 0x4B, 0x7E, 0xCF, 0x0B, 0xE8, 0xFF,
	0xB5, 0x77, 0x2E, 0xF4, 0xB2, 0x7B, 0x4A, 0xE0, 0x4C, 0x12, 0x25, 0x0C,
	0x70, 0x8D, 0x03, 0x29, 0xA0, 0xE1, 0x53, 0x24, 0xEC, 0x13, 0xD9, 0xEE,
	0x19, 0xBF, 0x10, 0xB3, 0x4A, 0x8C, 0x3F, 0x89, 0xA3, 0x61, 0x51, 0xDE,
	0xAC, 0x87, 0x07, 0x94, 0xF4, 0x63, 0x71, 0xEC, 0x2E, 0xE2, 0x6F, 0x5B,
	0x98, 0x81, 0xE1, 0x89, 0x5C, 0x34, 0x79, 0x6C, 0x76, 0xEF, 0x3B, 0x90,
	0x62, 0x79, 0xE6, 0xDB, 0xA4, 0x9A, 0x2F, 0x26, 0xC5, 0xD0, 0x10, 0xE1,
	0x0E, 0xDE, 0xD9, 0x10, 0x8E, 0x16, 0xFB, 0xB7, 0xF7, 0xA8, 0xF7, 0xC7,
	0xE5, 0x02, 0x07, 0x98, 0x8F, 0x36, 0x08, 0x95, 0xE7, 0xE2, 0x37, 0x96,
	0x0D, 0x36, 0x75, 0x9E, 0xFB, 0x0E, 0x72, 0xB1, 0x1D, 0x9B, 0xBC, 0x03,
	0xF9, 0x49, 0x05, 0xD8, 0x81, 0xDD, 0x05, 0xB4, 0x2A, 0xD6, 0x41, 0xE9,
	0xAC, 0x01, 0x76, 0x95, 0x0A, 0x0F, 0xD8, 0xDF, 0xD5, 0xBD, 0x12, 0x1F,
	0x35, 0x2F, 0x28, 0x17, 0x6C, 0xD2, 0x98, 0xC1, 0xA8, 0x09, 0x64, 0x77,
	0x6E, 0x47, 0x37, 0xBA, 0xCE, 0xAC, 0x59, 0x5E, 0x68, 0x9D, 0x7F, 0x72,
	0xD6, 0x89, 0xC5, 0x06, 0x41, 0x29, 0x3E, 0x59, 0x3E, 0xDD, 0x26, 0xF5,
	0x24, 0xC9, 0x11, 0xA7, 0x5A, 0xA3, 0x4C, 0x40, 0x1F, 0x46, 0xA1, 0x99,
	0xB5, 0xA7, 0x3A, 0x51, 0x6E, 0x86, 0x3B, 0x9E, 0x7D, 0x72, 0xA7, 0x12,
	0x05, 0x78, 0x59, 0xED, 0x3E, 0x51, 0x78, 0x15, 0x0B, 0x03, 0x8F, 0x8D,
	0xD0, 0x2F, 0x05, 0xB2, 0x3E, 0x7B, 0x4A, 0x1C, 0x4B, 0x73, 0x05, 0x12,
	0xFC, 0xC6, 0xEA, 0xE0, 0x50, 0x13, 0x7C, 0x43, 0x93, 0x74, 0xB3, 0xCA,
	0x74, 0xE7, 0x8E, 0x1F, 0x01, 0x08, 0xD0, 0x30, 0xD4, 0x5B, 0x71, 0x36,
	0xB4, 0x07, 0xBA, 0xC1, 0x30, 0x30, 0x5C, 0x48, 0xB7, 0x82, 0x3B, 0x98,
	0xA6, 0x7D, 0x60, 0x8A, 0xA2, 0xA3, 0x29, 0x82, 0xCC, 0xBA, 0xBD, 0x83,
	0x04, 0x1B, 0xA2, 0x83, 0x03, 0x41, 0xA1, 0xD6, 0x05, 0xF1, 0x1B, 0xC2,
	0xB6, 0xF0, 0xA8, 0x7C, 0x86, 0x3B, 0x46, 0xA8, 0x48, 0x2A, 0x88, 0xDC,
	0x76, 0x9A, 0x76, 0xBF, 0x1F, 0x6A, 0xA5, 0x3D, 0x19, 0x8F, 0xEB, 0x38,
	0xF3, 0x64, 0xDE, 0xC8, 0x2B, 0x0D, 0x0A, 0x28, 0xFF, 0xF7, 0xDB, 0xE2,
	0x15, 0x42, 0xD4, 0x22, 0xD0, 0x27, 0x5D, 0xE1, 0x79, 0xFE, 0x18, 0xE7,
	0x70, 0x88, 0xAD, 0x4E, 0xE6, 0xD9, 0x8B, 0x3A, 0xC6, 0xDD, 0x27, 0x51,
	0x6E, 0xFF, 0xBC, 0x64, 0xF5, 0x33, 0x43, 0x4F
};

static const unsigned char LetsEncrypt_ISRG_Root_X1_RSA_E[] = {
	0x01, 0x00, 0x01
};

#endif


#if ! defined(OMIT_AWS_CERT)
/*********************************************************************************************\
 * Amazon Root CA, RSA 2048 bits SHA 256, valid until 20380117
 *
 * https://www.amazontrust.com/repository/
 * Downloaded from https://www.amazontrust.com/repository/AmazonRootCA1.pem
 *
 * to convert do: "bearssl ta AmazonRootCA1.pem"
 * then copy and paste below, chain the generic names to the same as below
 * remove "static" and add "PROGMEM"
\*********************************************************************************************/


const unsigned char PROGMEM AmazonRootCA1_DN[] = {
	0x30, 0x39, 0x31, 0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13,
	0x02, 0x55, 0x53, 0x31, 0x0F, 0x30, 0x0D, 0x06, 0x03, 0x55, 0x04, 0x0A,
	0x13, 0x06, 0x41, 0x6D, 0x61, 0x7A, 0x6F, 0x6E, 0x31, 0x19, 0x30, 0x17,
	0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x10, 0x41, 0x6D, 0x61, 0x7A, 0x6F,
	0x6E, 0x20, 0x52, 0x6F, 0x6F, 0x74, 0x20, 0x43, 0x41, 0x20, 0x31
};

const unsigned char PROGMEM AmazonRootCA1_RSA_N[] = {
	0xB2, 0x78, 0x80, 0x71, 0xCA, 0x78, 0xD5, 0xE3, 0x71, 0xAF, 0x47, 0x80,
	0x50, 0x74, 0x7D, 0x6E, 0xD8, 0xD7, 0x88, 0x76, 0xF4, 0x99, 0x68, 0xF7,
	0x58, 0x21, 0x60, 0xF9, 0x74, 0x84, 0x01, 0x2F, 0xAC, 0x02, 0x2D, 0x86,
	0xD3, 0xA0, 0x43, 0x7A, 0x4E, 0xB2, 0xA4, 0xD0, 0x36, 0xBA, 0x01, 0xBE,
	0x8D, 0xDB, 0x48, 0xC8, 0x07, 0x17, 0x36, 0x4C, 0xF4, 0xEE, 0x88, 0x23,
	0xC7, 0x3E, 0xEB, 0x37, 0xF5, 0xB5, 0x19, 0xF8, 0x49, 0x68, 0xB0, 0xDE,
	0xD7, 0xB9, 0x76, 0x38, 0x1D, 0x61, 0x9E, 0xA4, 0xFE, 0x82, 0x36, 0xA5,
	0xE5, 0x4A, 0x56, 0xE4, 0x45, 0xE1, 0xF9, 0xFD, 0xB4, 0x16, 0xFA, 0x74,
	0xDA, 0x9C, 0x9B, 0x35, 0x39, 0x2F, 0xFA, 0xB0, 0x20, 0x50, 0x06, 0x6C,
	0x7A, 0xD0, 0x80, 0xB2, 0xA6, 0xF9, 0xAF, 0xEC, 0x47, 0x19, 0x8F, 0x50,
	0x38, 0x07, 0xDC, 0xA2, 0x87, 0x39, 0x58, 0xF8, 0xBA, 0xD5, 0xA9, 0xF9,
	0x48, 0x67, 0x30, 0x96, 0xEE, 0x94, 0x78, 0x5E, 0x6F, 0x89, 0xA3, 0x51,
	0xC0, 0x30, 0x86, 0x66, 0xA1, 0x45, 0x66, 0xBA, 0x54, 0xEB, 0xA3, 0xC3,
	0x91, 0xF9, 0x48, 0xDC, 0xFF, 0xD1, 0xE8, 0x30, 0x2D, 0x7D, 0x2D, 0x74,
	0x70, 0x35, 0xD7, 0x88, 0x24, 0xF7, 0x9E, 0xC4, 0x59, 0x6E, 0xBB, 0x73,
	0x87, 0x17, 0xF2, 0x32, 0x46, 0x28, 0xB8, 0x43, 0xFA, 0xB7, 0x1D, 0xAA,
	0xCA, 0xB4, 0xF2, 0x9F, 0x24, 0x0E, 0x2D, 0x4B, 0xF7, 0x71, 0x5C, 0x5E,
	0x69, 0xFF, 0xEA, 0x95, 0x02, 0xCB, 0x38, 0x8A, 0xAE, 0x50, 0x38, 0x6F,
	0xDB, 0xFB, 0x2D, 0x62, 0x1B, 0xC5, 0xC7, 0x1E, 0x54, 0xE1, 0x77, 0xE0,
	0x67, 0xC8, 0x0F, 0x9C, 0x87, 0x23, 0xD6, 0x3F, 0x40, 0x20, 0x7F, 0x20,
	0x80, 0xC4, 0x80, 0x4C, 0x3E, 0x3B, 0x24, 0x26, 0x8E, 0x04, 0xAE, 0x6C,
	0x9A, 0xC8, 0xAA, 0x0D
};

static const unsigned char PROGMEM AmazonRootCA1_RSA_E[] = {
	0x01, 0x00, 0x01
};

#endif


//
//  ========== cumulative CA =================
//
const br_x509_trust_anchor PROGMEM Tasmota_TA[] = {
#if ! defined(OMIT_LETS_ENCRYPT_CERT)
	{
		{ (unsigned char *)LetsEncrypt_ISRG_Root_X1_DN, sizeof LetsEncrypt_ISRG_Root_X1_DN },
		BR_X509_TA_CA,
		{
			BR_KEYTYPE_RSA,
			{ .rsa = {
				(unsigned char *)LetsEncrypt_ISRG_Root_X1_RSA_N, sizeof LetsEncrypt_ISRG_Root_X1_RSA_N,
				(unsigned char *)LetsEncrypt_ISRG_Root_X1_RSA_E, sizeof LetsEncrypt_ISRG_Root_X1_RSA_E,
			} }
		}
	}

#if ! defined(OMIT_AWS_CERT) || defined(INCLUDE_LOCAL_CERT)
    ,
#endif
#endif

#if ! defined(OMIT_AWS_CERT)
	{
		{ (unsigned char *)AmazonRootCA1_DN, sizeof AmazonRootCA1_DN },
		BR_X509_TA_CA,
		{
			BR_KEYTYPE_RSA,
			{ .rsa = {
				(unsigned char *)AmazonRootCA1_RSA_N, sizeof AmazonRootCA1_RSA_N,
				(unsigned char *)AmazonRootCA1_RSA_E, sizeof AmazonRootCA1_RSA_E,
			} }
		}
	}

#if defined(INCLUDE_LOCAL_CERT)
    ,
#endif
#endif


#if defined(INCLUDE_LOCAL_CERT)
#include <include/local_ca_descriptor.h>
#endif

};

constexpr size_t Tasmota_TA_size = sizeof(Tasmota_TA) / sizeof(br_x509_trust_anchor);


#endif


#endif

#endif