/**************************************************************************//**
 * @file     main.c
 * @brief    This sample program demonstrates how to use the TSI commands to
 *           perform ECC ECDSA signature generation and verification.
 *
 * @copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "NuMicro.h"
#include "tsi_cmd.h"
#include "crypto.h"

#define MAX_KEY_LEN     512

typedef struct sg_tv_t
{
	char   curve_name[16];
	E_ECC_CURVE   curve;
	int    keylen;
	int    dlen;              // digest length
	char   Msg[512];          // digest
	char   d[MAX_KEY_LEN];    // private key
	char   Qx[MAX_KEY_LEN];
	char   Qy[MAX_KEY_LEN];
	char   k[MAX_KEY_LEN];
	char   R[MAX_KEY_LEN];    // (r,s) pair digital signature
	char   S[MAX_KEY_LEN];
}  SG_TV_T;

char  sig_R[MAX_KEY_LEN] __attribute__((aligned(32)));
char  sig_S[MAX_KEY_LEN] __attribute__((aligned(32)));

char param_mem[8192] __attribute__((aligned(32)));
char sbuff_mem[4096] __attribute__((aligned(32)));

static SG_TV_T _sg_pool[] __attribute__((aligned(32))) =
{
	{
		"P-224", CURVE_P_224, 224,
		224,  /* SHA-224 */
		"07eb2a50bf70eee87467600614a490e7600437d077ec651a27e65e67",
		"16797b5c0c7ed5461e2ff1b88e6eafa03c0f46bf072000dfc830d615",
		"605495756e6e88f1d07ae5f98787af9b4da8a641d1a9492a12174eab",
		"f5cc733b17decc806ef1df861a42505d0af9ef7c3df3959b8dfc6669",
		"d9a5a7328117f48b4b8dd8c17dae722e756b3ff64bd29a527137eec0",
		"2fc2cff8cdd4866b1d74e45b07d333af46b7af0888049d0fdbc7b0d6",
		"8d9cc4c8ea93e0fd9d6431b9a1fd99b88f281793396321b11dac41eb"
	},
	{
		"P-256", CURVE_P_256, 256,
		160,  /* SHA-1 */
		"a3f91ae21ba6b3039864472f184144c6af62cd0e",
		"be34baa8d040a3b991f9075b56ba292f755b90e4b6dc10dad36715c33cfdac25",
		"fa2737fb93488d19caef11ae7faf6b7f4bcd67b286e3fc54e8a65c2b74aeccb0",
		"d4ccd6dae698208aa8c3a6f39e45510d03be09b2f124bfc067856c324f9b4d09",
		"18731ef637fe84872cf89a879567946a50f327f3af3aaeb6074a86f117e332b0",
		"2b826f5d44e2d0b6de531ad96b51e8f0c56fdfead3c236892e4d84eacfc3b75c",
		"a2248b62c03db35a7cd63e8a120a3521a89d3d2f61ff99035a2148ae32e3a248"
	},
	{
		"P-521", CURVE_P_521, 521,
		160,  /* SHA-1 */
		"1bf7039cca2394273f11a1d48dccb4466f3161df",
		"1bd56bd106118eda246155bd43b42b8e13f0a6e25dd3bb376026fab4dc92b6157bc6dfec2d15dd3d0cf2a39aa68494042af48ba9601118da82c6f2108a3a203ad74",
		"12fbcaeffa6a51f3ee4d3d2b51c5dec6d7c726ca353fc014ea2bf7cfbb9b910d32cbfa6a00fe39b6cdb8946f22775398b2e233c0cf144d78c8a7742b5c7a3bb5d23",
		"09cdef823dd7bf9a79e8cceacd2e4527c231d0ae5967af0958e931d7ddccf2805a3e618dc3039fec9febbd33052fe4c0fee98f033106064982d88f4e03549d4a64d",
		"06fdfbd1af79c6c91da3eb00e494e18107d8431d5bf90f4b3db33114207a5add73114b50f97bee07443b9f14ff43155899476be2e91f27aef31a781aacb5e6225bf",
		"0bd117b4807710898f9dd7778056485777668f0e78e6ddf5b000356121eb7a220e9493c7f9a57c077947f89ac45d5acb6661bbcd17abb3faea149ba0aa3bb1521be",
		"019cd2c5c3f9870ecdeb9b323abdf3a98cd5e231d85c6ddc5b71ab190739f7f226e6b134ba1d5889ddeb2751dabd97911dff90c34684cdbe7bb669b6c3d22f2480c"
	},
	{
		"P-192", CURVE_P_192, 192,
		224,  /* SHA-224 */
		"4faffccea0c4d580f9b3dccfd4639a9364f14536d02cfc50",
		"694b9faf76b8c6be78bb33ccf1cd6f34809ab57d049f8eeb",
		"db4388f00504a57b8ce69bc48953d251575becf9ef0a7430",
		"25f0d877af0b276dcdcba7807199f4259da914dbaecdc1a7",
		"41e8c2b1bc11fa47f5a23753d016fe2e366009083400d58e",
		"0361ca48731dc017023c59beff86096b56242708e4bc9e41",
		"f765431cdc56a73ef3ae3f1b588e723801b6592508d452bf"
	},
	{
		"P-192", CURVE_P_192, 192,
		160,  /* SHA-1 */
		"7f276bf574a4c80a68edd070385d8bbcbee084a5",
		"f310bd616126225ddf94fb886b62f894585619365aba1317",
		"fc044c1dc333da8ec98e6598ffaef60e5b230d098102791a",
		"8e76c0c9a17d292e1aee839766ebcc6ed7b83330d29b8858",
		"017448988e6169673975943a6d2de804741d4e8ac16f1545",
		"08975a982c05da87ba46ae400ac47b017aa7a82d9a6a8069",
		"4ec2b5db62bd287b2ca1ead38e52c36ae03762196848a36a"
	},
	{
		"P-192", CURVE_P_192, 192,
		256,  /* SHA-256 */
		"5ae8317d34d1e595e3fa7247db80c0af4320cce1116de187",
		"24edd22f7ddd6fa5bc61fc0653479aa40809ef865cf27a47",
		"9bf12d7174b7708a076a38bc80aa28662f251e2ed8d414dc",
		"4854c8d07dfc08824e9e471ca2fedcfcff3ddcb011573498",
		"a5c817a236a5f7faa329b8ecc3c596687c71aaaf86c7703e",
		"354abaecf4361fea90c29b9199882edf8573e686a813eff8",
		"80f50000ac86111c9b3047385a15d78e632c58b7949e82c1"
	},
	{
		"P-224", CURVE_P_224, 224,
		160,  /* SHA-1 */
		"6cae5f54a0c89f810b57cfed811b44a58327ce35",
		"e6f0a6ab81b7bd50dc5581b03d43308fb28356408392e29a5321ca7f",
		"c87c46fb7a07789aa352d0055b3a4ad588851a80f6f0915ab5547dda",
		"b7741b39048588bae158f2568a4c1dcfbc0843210f2be6ce3e2ffbe9",
		"6b8c3a59ff56793cf3fc434abd01c7eb91cebbb01dd32f484b7deaf1",
		"8c0e764cffd1845b37ad0eb3337e06bee7265c298056d9009845a301",
		"84fa1e38c2911b575f33916bcbac7f63dd219ce59a31674369316fbe"
	},
	{
		"P-224", CURVE_P_224, 224,
		224,  /* SHA-224 */
		"07eb2a50bf70eee87467600614a490e7600437d077ec651a27e65e67",
		"16797b5c0c7ed5461e2ff1b88e6eafa03c0f46bf072000dfc830d615",
		"605495756e6e88f1d07ae5f98787af9b4da8a641d1a9492a12174eab",
		"f5cc733b17decc806ef1df861a42505d0af9ef7c3df3959b8dfc6669",
		"d9a5a7328117f48b4b8dd8c17dae722e756b3ff64bd29a527137eec0",
		"2fc2cff8cdd4866b1d74e45b07d333af46b7af0888049d0fdbc7b0d6",
		"8d9cc4c8ea93e0fd9d6431b9a1fd99b88f281793396321b11dac41eb"
	},
	{
		"P-256", CURVE_P_256, 256,
		160,  /* SHA-1 */
		"a3f91ae21ba6b3039864472f184144c6af62cd0e",
		"be34baa8d040a3b991f9075b56ba292f755b90e4b6dc10dad36715c33cfdac25",
		"fa2737fb93488d19caef11ae7faf6b7f4bcd67b286e3fc54e8a65c2b74aeccb0",
		"d4ccd6dae698208aa8c3a6f39e45510d03be09b2f124bfc067856c324f9b4d09",
		"18731ef637fe84872cf89a879567946a50f327f3af3aaeb6074a86f117e332b0",
		"2b826f5d44e2d0b6de531ad96b51e8f0c56fdfead3c236892e4d84eacfc3b75c",
		"a2248b62c03db35a7cd63e8a120a3521a89d3d2f61ff99035a2148ae32e3a248"
	},
	{
		"P-384", CURVE_P_384, 384,
		160,  /* SHA-1 */
		"9b9f8c9535a5ca26605db7f2fa573bdfc32eab8b",
		"a492ce8fa90084c227e1a32f7974d39e9ff67a7e8705ec3419b35fb607582bebd461e0b1520ac76ec2dd4e9b63ebae71",
		"e55fee6c49d8d523f5ce7bf9c0425ce4ff650708b7de5cfb095901523979a7f042602db30854735369813b5c3f5ef868",
		"28f59cc5dc509892a988d38a8e2519de3d0c4fd0fbdb0993e38f18506c17606c5e24249246f1ce94983a5361c5be983e",
		"ba25756f1a4a9853bbf60cb2c789569fd551abf3a9cceb889ff71ab5ef7758a3e302166bf2a01a29af18ffbd329cd781",
		"6820b8585204648aed63bdff47f6d9acebdea62944774a7d14f0e14aa0b9a5b99545b2daee6b3c74ebf606667a3f39b7",
		"491af1d0cccd56ddd520b233775d0bc6b40a6255cc55207d8e9356741f23c96c14714221078dbd5c17f4fdd89b32a907"
	},
	{
		"P-521", CURVE_P_521, 521,
		160,  /* SHA-1 */
		"1bf7039cca2394273f11a1d48dccb4466f3161df",
		"1bd56bd106118eda246155bd43b42b8e13f0a6e25dd3bb376026fab4dc92b6157bc6dfec2d15dd3d0cf2a39aa68494042af48ba9601118da82c6f2108a3a203ad74",
		"12fbcaeffa6a51f3ee4d3d2b51c5dec6d7c726ca353fc014ea2bf7cfbb9b910d32cbfa6a00fe39b6cdb8946f22775398b2e233c0cf144d78c8a7742b5c7a3bb5d23",
		"09cdef823dd7bf9a79e8cceacd2e4527c231d0ae5967af0958e931d7ddccf2805a3e618dc3039fec9febbd33052fe4c0fee98f033106064982d88f4e03549d4a64d",
		"06fdfbd1af79c6c91da3eb00e494e18107d8431d5bf90f4b3db33114207a5add73114b50f97bee07443b9f14ff43155899476be2e91f27aef31a781aacb5e6225bf",
		"0bd117b4807710898f9dd7778056485777668f0e78e6ddf5b000356121eb7a220e9493c7f9a57c077947f89ac45d5acb6661bbcd17abb3faea149ba0aa3bb1521be",
		"019cd2c5c3f9870ecdeb9b323abdf3a98cd5e231d85c6ddc5b71ab190739f7f226e6b134ba1d5889ddeb2751dabd97911dff90c34684cdbe7bb669b6c3d22f2480c"
	},
	{
		"K-163", CURVE_K_163, 163,
		160,  /* SHA-1 */
		"48011d541ec726c67ddb4a48b84d459f3baf5fd1",
		"093c46340022f6f6e668693ef277a41ec25fbb46c",
		"5ef1a3a8748356ada9dcaa2d2a30ba3a56c51bc8a",
		"6c0b5f0d40199d759583aa594be3787b760020ee9",
		"33ba961eb737ffdff6e3a61eec8b32b16d8adafc0",
		"3cb596165dd34f99f73036f69fc8c72bbc588cd3b",
		"191a23c0186f25d0837d77313dc7a9333883eac9e"
	},
	{
		"K-233", CURVE_K_233, 233,
		160,  /* SHA-1 */
		"9b237d72c5a4370532f5d21a1d2de316535469dd",
		"07527560e0ff2873d0376c61a574bb26ef6ea7e24cb4cd981e811207b93",
		"116bff61d54ef7453b53c6c7e512bc5ff5c3d21384ae084dbc4e787a3a4",
		"0caab368fff9762f44c5accd8d36b3492c5c0224149013ac1c9af8c7644",
		"0716097dbfe89e9957d5912734afa0430896ddc1eca54b28b5d23ef39c9",
		"056d66fea6a74da9bc9210d517d25c60e123453bcb2aa7e7ecc4f31b738",
		"02da451255adc17c6b972e0846c1b5557dc7d2e27897bdd385d66e067da"
	},
	{
		"K-283", CURVE_K_283, 283,
		160,  /* SHA-1 */
		"76083ee70e726311c3e114f7f9a566bfba2f3514",
		"16a372a6678f48cf7f08ec30e24983f1eaa305afe99fac77b2b3b63669479995839bbd0",
		"55c98a8556ef969538be24bf52fb70fd285a62987ebd726c7a14ba14f2cb129f3e9e84b",
		"661f5af5b799d9b93591e847c2aca99dde1e9a6897de960ab27be5479d66e6cbcb64212",
		"787b7b6a7b80e421326a90d88bc608e2839a35b16bece3ab77a8573c384e810e3dc035",
		"1e42534e7ca330504bc151c3dd469f4c2f49e3164aa907aa98b63f5a8711894efb1e85e",
		"035c270ccae9cbe34ac5236a6d94c69a3b37f1b2a43e444a6b0de48eb26d679932d4721"
	},
	{
		"K-409", CURVE_K_409, 409,
		160,  /* SHA-1 */
		"8c5d3d80e6d03556cd458e6d67b8e5b4b15d3948",
		"020949c88cd44684f6d19c0d7b385796760f1c31f34beb3b8a26ca421eb62fccc51bcd5d1029a648ea1ca16c8f032dc3ebe2472",
		"077ae5742fffe220aef07e5cdc001e17f223c22eff6eb5bcfee4d9c9677377f82fcbcd04a6f2a88586802f9dbfabcbe88e1d9c2",
		"13957372470c01196758f5e836491649e04c302b5ea11f48e73b8516d2d1c632cdfe7545c6b9b9a7638ded507905659b03dbe7c",
		"03b19d4a3b2b7851cc316f323b5d303f2da929392a8ac6fadad82d3117945e9a8c47097a7e21c0dea193d593058022596413dc1",
		"036b1b97d3a2308f11792f04580bc2d194f0f48bd27c71ff98e60ddc6fa73ea8314a8337e4890bc857a9ea84e57f15767e95673",
		"0249ba42cfb7a426a2b4ccac06c6b8d01d390f236a11c0dce7466b7637d4dd0de0522f6553cb8634d3ce71844c88127b15489fe"
	},
	{
		"K-571", CURVE_K_571, 571,
		160,  /* SHA-1 */
		"37c99c912a0ba82801261e22bfcb75e4827f8c89",
		"0a21e0bbe904738a47ae93d3798b8423ae87351d4705b461634a544bccea500c21d832e7ce62f024af2d9618b12abe323ab3bb8d966add118fb00df5fa80a992585a003e9b8526b",
		"2bf77bc3d81c9e3aa0657c5051a2fe5091ff88186de4dc000ba4686317601971cdec69b2f336e9662ef73d94a618226a33cdd3154f361b40855d394b4fc3d77758b35e0efa221fe",
		"170d498127641719f14e5197b17c84d929ab60aa8008889570ee42614d73c2ace4baed0e9c9a12145ded66a951eac1e50f690c563b5dccdc9dd8e89af76a979c126667ee085427c",
		"047e74e6132b789f0c7429ce6b9d192308ed26b72fbf825f0052c3ca6afc7df4d7998b1293b0586238480e6cacf704ef4c7bdc38f9c44dff184202d51f9733d4d9d8a8f3a87ec1c",
		"0276c3f6fe7f983d6b9a7ea8f598f1655b634dc1eadd7daf39ed2a13689c0b322068c7aeca96f6cff1149581b86cc7a272aece7fc297fca553209126f3c0c137af26d7e1d02ff1b",
		"193b53275ffbc5f9ba3e9977c2f63598b3de7946b8e494e23f01067c9aa71899a1769df25b17aa8e1495d9c32221e5aea188272effcfac4bcd547354a4288a78a4c66623a6eac85"
	},
	{
		"B-163", CURVE_B_163, 163,
		160,  /* SHA-1 */
		"86752230200fc292fcb89597605c9ce117397779",
		"13486dc5ca0ba84956d2f6dc43df0415656f0eac5",
		"71765ccb031969d7332cc53890ee209520fb8ceab",
		"2e99b4c30d3de389735cbeebb6e73ce9f67dc5412",
		"17cdf80f62e42b21349a55a62591436363ec43c59",
		"2ddace85a086746d8a4691ca61765719fbb69d928",
		"17a9d0c14ff04cb6ae6d72d26701e5f69c5320e6b"
	},
	{
		"B-233", CURVE_B_233, 233,
		160,  /* SHA-1 */
		"5b68920962ef6bcb6ed40ef143f9eb8038eb7c78",
		"0e06e069767ebe602c3a7c62bfc17f1234fc7f4715b056c8196e985a04b",
		"100f28fee82546b6b823e539617f35ff548247b0b9d2cc85cdb501628af",
		"04bbbc8981840eb2bad640252b65c6025d2310780467933559833003bd9",
		"04922f77acd4767af7d56374bc0a66ecdd91ed6d90c873955900b0a7420",
		"09dd7fc9d73e533ec8aff3efed7e286fdc1cfd71435482ae0c83e159768",
		"0883162f274273da5e4375009a824ad35e6c15615bf5ad555d904b1d8c7"
	},
	{
		"B-283", CURVE_B_283, 283,
		160,  /* SHA-1 */
		"351d43907b0f62eb5950d6045108027b4456fb44",
		"385f5bbd23b5028a66168359927a850fe1b0e9e6c8ed351a63bc2430ffc7816e0f24ed2",
		"4ea11a96ed2d2067e639fffab0c9e1b3e54288012b831c12e1a8bc5d4081e56efd9c023",
		"3fae371b9688ae1551f564dfa853862f9509c0dd24a0ace85398ab80d7b460a4f51fdd8",
		"1c659c6fbeded27fbf19495f760e47f1e956781b5d28f2b9a5de6a86cd72b11ce46d87e",
		"3a8f6548ed8a0a14b6567b6ebac8545cef67bc2209d12419584f3a6d7272149c41a0227",
		"188af56f110ef8135195eea4ddfc93886c5562af5e600426ab2a9223849e1e49f50bb54"
	},
	{
		"B-571", CURVE_B_571, 571,
		160,  /* SHA-1 */
		"88a0d0b3c8d0002ce947cb9ac5f89c7f6adae6d3",

		"0cb1580ec606dc554dbf10ec12972016add2434ef6a1e6babfed818321b64ffbcc71fdb661ce26cef3f97084ab666da43d8e6504718c3d2ee91899b2735e0f8d1aa660efbdeed95",
		"23850e6cc3fd25b09ef1d5e2a3a7bfd23082bd05059df67e26a46021d48f3698d9c03b0ee384caedf10c1d673bd30057f0c13bc490bc28e1ca7435272a7742c1a70a8f8657756e0",
		"346437ff275506260d72bb5e44e29abd5a06d8da6acdbcc8c089328be023e1316226f4ce0146eacfbf00c837cda311abf640f4d6c1aeb6da949d8789e464038a89bb9c7563b2e43",
		"3674564fc87a51db16152d0e3964185fba28748e95e5565e24bfb570382db06eadfb5500e9c2bb1fe2676ebcd1fdbe4dbf9408e402ace0c9a3626600c06344c756c0cf7c361caec",
		"36bdc93dff47d4886bdb6941cc4135f42795e4cb5b02e35554013675f24d73782d082e59273e7b32c2210d15d7f2eb7e2498c3c28f2c9d4684aa594b3d32dece81dc5a2c9d3b620",
		"1d5dc1e051ab0807a50829045be38be3be581b6609f63331852923b1fa4e7baae45825a88fe52afdb14ace9f8c8a269160a8de55312ed09476da71bba3a98775b96a4424a7ffe7d"
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
	int      i, ret;
	char     name[16];
	SG_TV_T  *sg_tv;
	char     *sig_r, *sig_s;
	char     *param_block;   // param_block[576*6 + 4];
	char     *sbuff;         // sbuff[576*2 + 4];

	param_block = nc_ptr(param_mem);
	sbuff = nc_ptr(sbuff_mem);


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

	sig_r = &sbuff[0];
	sig_s = &sbuff[576];
	sg_tv = nc_ptr(_sg_pool);

	sysprintf("+---------------------------------------------+\n");
	sysprintf("|   MA35D1 Crypto ECDSA sign and verify       |\n");
	sysprintf("+---------------------------------------------+\n");

	for (i = 0; i < sizeof(_sg_pool)/sizeof(SG_TV_T); i++) {
		memset(name, 0, sizeof(name));
		strncpy(name, sg_tv[i].curve_name, 6);

		sysprintf("SigGen [%d] Run curve %s test.......", i, name);
		sysprintf("  %s\n", sg_tv[i].Msg);

		memcpy(param_block, sg_tv[i].Msg, MAX_KEY_LEN);
		memcpy(&param_block[576], sg_tv[i].d, MAX_KEY_LEN);
		memcpy(&param_block[1152], sg_tv[i].k, MAX_KEY_LEN);

		ret = TSI_ECC_GenSignature(sg_tv[i].curve,            /* curve_id   */
					   0,                       /* rsel       */
					   ECC_KEY_SEL_USER,        /* psel       */
					   0,                       /* d_knum     */
					   ptr_to_u32(param_block), /* param_addr */
					   ptr_to_u32(sbuff)        /* sig_addr   */
					   );
		if (ret != 0) {
			sysprintf("ECC signature generation failed!!\n");
			TSI_Print_Error(ret);
			while (1);
		}
		if (ecc_strcmp(sig_r, sg_tv[i].R) != 0) {
			sysprintf("Signature R [%s] is not matched with expected [%s]!\n", sig_r, sg_tv[i].R);
			while (1);
		}
		if (ecc_strcmp(sig_s, sg_tv[i].S) != 0) {
			sysprintf("Signature S [%s] is not matched with expected [%s]!\n", sig_s, sg_tv[i].S);
			while (1);
		}
		sysprintf("[PASS]\n");
	}
	sysprintf("ECC signature generation demo done.\n\n\n");

	sysprintf("+---------------------------------------------+\n");
	sysprintf("|   Crypto ECC Signature Verification Test    |\n");
	sysprintf("+---------------------------------------------+\n");

	for (i = 0; i < sizeof(_sg_pool)/sizeof(SG_TV_T); i++) {
		memset(name, 0, sizeof(name));
		strncpy(name, sg_tv[i].curve_name, 6);

		sysprintf("SigVerify [%d] Run curve %s test.......", i, name);
		sysprintf("  %s\n", sg_tv[i].Msg);

		memcpy(param_block, sg_tv[i].Msg, MAX_KEY_LEN);
		memcpy(&param_block[576], sg_tv[i].Qx, MAX_KEY_LEN);
		memcpy(&param_block[1152], sg_tv[i].Qy, MAX_KEY_LEN);
		memcpy(&param_block[1728], sg_tv[i].R, MAX_KEY_LEN);
		memcpy(&param_block[2304], sg_tv[i].S, MAX_KEY_LEN);

		ret = TSI_ECC_VerifySignature(sg_tv[i].curve,            /* curve_id   */
					  ECC_KEY_SEL_USER,        /* psel       */
					  0,                       /* x_knum     */
					  0,                       /* y_knum     */
					  ptr_to_u32(param_block)  /* param_addr */
					  );
		if (ret != 0) {
			sysprintf("ECC signature verification failed!!\n");
			TSI_Print_Error(ret);
			while (1);
		}
		sysprintf("[PASS]\n");
	}
	sysprintf("ECC signature verification demo done.\n");
	while (1);
}
