/**************************************************************************//**
 * @file     main.c
 * @brief    This sample program demonstrates how to use the TSI commands to
 *           perform ECC public key generation.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "tsi_cmd.h"
#include "crypto.h"

#define MAX_KEY_LEN		168

char kbuff_pool[4096] __attribute__((aligned(32)));

typedef struct kp_tv_t
{
	char   curve_name[16];
	E_ECC_CURVE   curve;
	int    keylen;
	char   d[MAX_KEY_LEN];
	char   Qx[MAX_KEY_LEN];
	char   Qy[MAX_KEY_LEN];
}  KP_TV_T;

KP_TV_T _kp_pool[] __attribute__((aligned(32))) =
{
	{
		"K-233", CURVE_K_233, 233,
		"1da7422b50e3ff051f2aaaed10acea6cbf6110c517da2f4eaca8b5b87",
		"1c7475da9a161e4b3f7d6b086494063543a979e34b8d7ac44204d47bf9f",
		"131cbd433f112871cc175943991b6a1350bf0cdd57ed8c831a2a7710c92"
	},
	{
		"K-163", CURVE_K_163, 163,
		"28a7447f95b43c072722ee52f2a68897518830272",
		"72dadf24b00f9a2a0ad6fbfb9d86181e939900174",
		"4bc1d4987dde0d2f633df16d686e2a78d6d3f49f3"
	},
	{
		"P-521", CURVE_P_521, 521,
		"184258ea667ab99d09d4363b3f51384fc0acd2f3b66258ef31203ed30363fcda7661b6a817daaf831415a1f21cb1cda3a74cc1865f2ef40f683c14174ea72803cff",
		"19ee818048f86ada6db866b7e49a9b535750c3673cb61bbfe5585c2df263860fe4d8aa8f7486aed5ea2a4d733e346eaefa87ac515c78b9a986ee861584926ce4860",
		"1b6809c89c0aa7fb057a32acbb9ab4d7b06ba39dba8833b9b54424add2956e95fe48b7fbf60c3df5172bf386f2505f1e1bb2893da3b96d4f5ae78f2544881a238f7"
	},
	{
		"SM2", CURVE_SM2_256, 256,
		"bdca6455a55b9c2722d0f580f7f3c5633cbfcee85517aaa57f119b4b25569b43",
		"8a689f2ea87a601cdba2cd46e0862d66deb48ff1c636d068ed1ddbe47201bbdd",
		"02be58c5acc94fa3fb82e1cbd220172f1f304bdd89ab7e294a4f672c04eb3de4"
	},
	{
		"P-192", CURVE_P_192, 192,
		"e5ce89a34adddf25ff3bf1ffe6803f57d0220de3118798ea",
		"8abf7b3ceb2b02438af19543d3e5b1d573fa9ac60085840f",
		"a87f80182dcd56a6a061f81f7da393e7cffd5e0738c6b245"
	},
	{
		"P-224", CURVE_P_224, 224,
		"e7c92383846a4e6887a10498d8eaca2bd0487d985bd7d3f92ce3ab30",
		"0a3682d2aaa4dd931bee042d32e95755507ab164b12f84843f4b7b96",
		"a6313a938eff7a293222e0e3c7b4c6132489b33255a61c3fc1ce2256"
	},
	{
		"P-256", CURVE_P_256, 256,
		"c9806898a0334916c860748880a541f093b579a9b1f32934d86c363c39800357",
		"d0720dc691aa80096ba32fed1cb97c2b620690d06de0317b8618d5ce65eb728f",
		"9681b517b1cda17d0d83d335d9c4a8a9a9b0b1b3c7106d8f3c72bc5093dc275f"
	},
	{
		"P-384", CURVE_P_384, 384,
		"5394f7973ea868c52bf3ff8d8ceeb4db90a683653b12485d5f627c3ce5abd8978fc9673d14a71d925747931662493c37",
		"fd3c84e5689bed270e601b3d80f90d67a9ae451cce890f53e583229ad0e2ee645611fa9936dfa45306ec18066774aa24",
		"b83ca4126cfc4c4d1d18a4b6c21c7f699d5123dd9c24f66f833846eeb58296196b42ec06425db5b70a4b81b7fcf705a0"
	},
	{
		"P-521", CURVE_P_521, 521,
		"184258ea667ab99d09d4363b3f51384fc0acd2f3b66258ef31203ed30363fcda7661b6a817daaf831415a1f21cb1cda3a74cc1865f2ef40f683c14174ea72803cff",
		"19ee818048f86ada6db866b7e49a9b535750c3673cb61bbfe5585c2df263860fe4d8aa8f7486aed5ea2a4d733e346eaefa87ac515c78b9a986ee861584926ce4860",
		"1b6809c89c0aa7fb057a32acbb9ab4d7b06ba39dba8833b9b54424add2956e95fe48b7fbf60c3df5172bf386f2505f1e1bb2893da3b96d4f5ae78f2544881a238f7"
	},
	{
		"SM2", CURVE_SM2_256, 256,
		"bdca6455a55b9c2722d0f580f7f3c5633cbfcee85517aaa57f119b4b25569b43",
		"8a689f2ea87a601cdba2cd46e0862d66deb48ff1c636d068ed1ddbe47201bbdd",
		"02be58c5acc94fa3fb82e1cbd220172f1f304bdd89ab7e294a4f672c04eb3de4"
	},
	{
		"K-163", CURVE_K_163, 163,
		"28a7447f95b43c072722ee52f2a68897518830272",
		"72dadf24b00f9a2a0ad6fbfb9d86181e939900174",
		"4bc1d4987dde0d2f633df16d686e2a78d6d3f49f3"
	},
	{
		"K-233", CURVE_K_233, 233,
		"1da7422b50e3ff051f2aaaed10acea6cbf6110c517da2f4eaca8b5b87",
		"1c7475da9a161e4b3f7d6b086494063543a979e34b8d7ac44204d47bf9f",
		"131cbd433f112871cc175943991b6a1350bf0cdd57ed8c831a2a7710c92"
	},
	{
		"K-283", CURVE_K_283, 283,
		"1de6fc561ce8c3ec9a7c03a51e0c61204991f8caca8c7b073cd07945ffb22c48c30e5d4",
		"21e41033585949f5bf30a73d935c580946c3f15b942b42b54e3397fc4115ee96bbbcff0",
		"50789e0c1dacaebb72d7fe27081b2048a8fac3a58693e52807b8c346930b5c4deb549cb"
	},
	{
		"K-409", CURVE_K_409, 409,
		"190c5a00374cc3254fdd421c8e52b0cb0f00317bbfb4153195eb6195557989b8e78b27df35c8f47bb4b4ee4608ea04f2adb72",
		"0415d296d3d421801dd4ef870cdd234220af52c896f2d8e70c368622167655d45ab7db524552f7aeb9c1159bcac10f24b9b1864",
		"00f824d69ec629e2dabd323cfc93992f253c901ada1427967e591ca0e0970ae7ed35e252159255a3bdbf21d09b0c7bfeb72626a"
	},
	{
		"K-571", CURVE_K_571, 571,
		"4b7223994f77708dbefe1e76fedb6279710b8769933f87d12d4304bac646fc453055632beb70f87c6bcf6f28fcccba25088789d1f15013f25320ff09321e921eb3e66b0829e87c",
		"23691a3028fc2ea92f707f13c61953ebf411a247739f225f21878fa786e416c5aac32a5d73368bf3ca350f1e05022d17093dc318b42e5fa7234e32f959f20146da2165db36230c0",
		"0fd2635485e32d637bfd8f53ff600b9b2bcc6d79884be54dc50103e25c460d41c8d502d7927bb19adfb2cd59a83ec92f4186ac5c75014d3946f4a2a725d3324f6dc206197d19d79"
	},
	{
		"B-163", CURVE_B_163, 163,
		"25d594310681b01fd63333cdd4315e54e18fe2623",
		"07e7162c48dcab690aa9ef76d2ed066cedae33364",
		"08cc32f4b5a88985c6e0c418e4abe988d5375371d"
	},
	{
		"B-233", CURVE_B_233, 233,
		"1e0da3dca621aab89a54e9528937ca7567464e6e783357878c1ecef15c",
		"0bf1e4d6ad911b7d4cfdfc990132b1e23bd279f4692bbac82e9e8b80dd4",
		"06c2a7599c395b8cc01b29b33ad6808361a7417d0dd7bd478a4a4783446"
	},
	{
		"B-283", CURVE_B_283, 283,
		"10d57c6f40baac97852771cee44a04137fb0ae504df7d6bb4153e5f13678f511520d47",
		"5c555fecdea33c76bbc3498a2cf3f64eda57f3bedc9579439162a736953d25d16ffb6a3",
		"08808d8babe945f2f0040f70c9f10714b8852179314d17f8f1cef8164fe5d1705e33eff"
	},
	{
		"B-409", CURVE_B_409, 409,
		"ebd71c6f6a42bb485480526d916977665df53c198dbd027e2a36ddd4e1178bed069ca6758d0069098301e9ef89dc545ce9c691",
		"1fc79d655eb2f07e8127fb0857de31fadb25afc04ea340fa448d669439e7519a3487c7601875d1f3431d3707a5a36de3532408d",
		"19a4dae9a205ead5fc6dac8b84f7b8846667b1853d02bfd696115f266b380b5be63eb684a46fb3536f9c44ac33cb5aa32000246"
	},
	{
		"B-571", CURVE_B_571, 571,
		"1443e93c7ef6802655f641ecbe95e75f1f15b02d2e172f49a32e22047d5c00ebe1b3ff0456374461360667dbf07bc67f7d6135ee0d1d46a226a530fefe8ebf3b926e9fbad8d57a6",
		"53e3710d8e7d4138db0a369c97e5332c1be38a20a4a84c36f5e55ea9fd6f34545b864ea64f319e74b5ee9e4e1fa1b7c5b2db0e52467518f8c45b658824871d5d4025a6320ca06f8",
		"3a22cfd370c4a449b936ae97ab97aab11c57686cca99d14ef184f9417fad8bedae4df8357e3710bcda1833b30e297d4bf637938b995d231e557d13f062e81e830af5ab052208ead"
	},
	{
		"SM2", CURVE_SM2_256, 256,
		"bdca6455a55b9c2722d0f580f7f3c5633cbfcee85517aaa57f119b4b25569b43",
		"8a689f2ea87a601cdba2cd46e0862d66deb48ff1c636d068ed1ddbe47201bbdd",
		"02be58c5acc94fa3fb82e1cbd220172f1f304bdd89ab7e294a4f672c04eb3de4"
	},
	{
		"Curve 25519", CURVE_25519, 256,
		"5AC99F33632E5A768DE7E81BF854C27C46E3FBF2ABBACD29EC4AFF517369C660",
		"057E23EA9F1CBE8A27168F6E696A791DE61DD3AF7ACD4EEACC6E7BA514FDA863",
		"01517b1765bcd87ab4a844dc83ecac033f172cf952440148363cbfc59dd4234a"
	},
};

int ecc_strcmp(char *s1, char *s2)
{
	char  c1, c2;

	while (*s1 == '0') s1++;
	while (*s2 == '0') s2++;

	for ( ; *s1 || *s2; s1++, s2++)
	{
		if ((*s1 >= 'A') && (*s1 <= 'Z'))
			c1 = *s1 + 32;
		else
			c1 = *s1;

		if ((*s2 >= 'A') && (*s2 <= 'Z'))
			c2 = *s2 + 32;
		else
			c2 = *s2;

		if (c1 != c2)
			return 1;
	}
	return 0;
}

void SYS_Init(void)
{
	/* Enable UART module clock */
	CLK_EnableModuleClock(UART0_MODULE);

	/* Select UART module clock source as SYSCLK1 and UART module clock divider as 15 */
	CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL2_UART0SEL_SYSCLK1_DIV2, CLK_CLKDIV1_UART0(15));

	/* enable Wormhole 1 clock */
	CLK_EnableModuleClock(WH1_MODULE);

	/* Set GPE multi-function pins for UART0 RXD and TXD */
	SYS->GPE_MFPH &= ~(SYS_GPE_MFPH_PE14MFP_Msk | SYS_GPE_MFPH_PE15MFP_Msk);
	SYS->GPE_MFPH |= (SYS_GPE_MFPH_PE14MFP_UART0_TXD | SYS_GPE_MFPH_PE15MFP_UART0_RXD);
}

void UART0_Init()
{
	/* Configure UART0 and set UART0 baud rate */
	UART_Open(UART0, 115200);
}

void delay_us(int usec)
{
	uint64_t   t0;
	t0  = EL0_GetCurrentPhysicalValue();
	while ((EL0_GetCurrentPhysicalValue() - t0) < (usec * 12));
}

int32_t main(void)
{
	int         i, ret;
	char        name[16];
	char        *pub_x, *pub_y;       /* temporary buffer used to keep output public keys */
	KP_TV_T     *tv;

	/* Unlock protected registers */
	SYS_UnlockReg();

	/* Init System, IP clock and multi-function I/O */
	SYS_Init();

	/* Init UART0 for sysprintf */
	UART0_Init();

	if (TSI_Init() != 0)
	{
		sysprintf("TSI Init failed!\n");
		while (1);
	}

	pub_x = nc_ptr(kbuff_pool);
	pub_y = nc_ptr(ptr_to_u32(kbuff_pool) + 576);
	tv = nc_ptr(_kp_pool);

	sysprintf("+---------------------------------------------+\n");
	sysprintf("|   Crypto ECC Public Key Generation Demo     |\n");
	sysprintf("+---------------------------------------------+\n");

	for (i = 0; i < sizeof(_kp_pool) / sizeof(KP_TV_T); i++) {
		memset(name, 0, sizeof(name));
		strncpy(name, tv[i].curve_name, 5);
		sysprintf("Run curve %s test.......", name);

		ret = TSI_ECC_GenPublicKey(tv[i].curve,           /* curve_id  */
					   0,                     /* is_ecdh   */
					   ECC_KEY_SEL_USER,      /* psel      */
					   0,                     /* d_knum    */
					   ptr_to_u32(tv[i].d),   /* priv_key  */
					   ptr_to_u32(pub_x)      /* pub_key   */
					   );
		if (ret != 0) {
			sysprintf("ECC key generation failed!!\n");
			TSI_Print_Error(ret);
			while (1);
		}
		sysprintf("ECC done, compare...\n");
		if (ecc_strcmp(tv[i].Qx, pub_x) != 0) {
			sysprintf("Public key X [%s] is not matched with expected [%s]!\n", pub_x, tv[i].Qx);
			while (1);
		}
		if (ecc_strcmp(tv[i].Qy, pub_y) != 0) {
			sysprintf("Public key Y [%s] is not matched with expected [%s]!\n", pub_y, tv[i].Qy);
			while (1);
		}
		sysprintf("[PASS]\n");
	}
	sysprintf("ECC public key generation test vector compared passed.\n");
	while (1);
}
