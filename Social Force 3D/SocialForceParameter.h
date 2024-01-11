#ifndef __SOCIAL_FORCE_PARAMETER_H__
#define __SOCIAL_FORCE_PARAMETER_H__

//#define CA_A 2000.0f / 80.0f
//#define CA_B 0.08f
#define CA_K 1.2 * 100000.0f / 80.0f		// 80 is the mass value from Helbing, D., Farkas, I., & Vicsek, T. (2000)
#define CA_KAPPA 2.4 * 100000.0f / 80.0f

#define CS_A 10.0f
#define CS_B 0.34f
#define CS_LAMBDA 1.0f

#define ES1_TAU 0.53f
#define ES1_A 9.2f
//#define ES1_B 0.44f
#define ES1_B 1.64f
#define ES1_LAMBDA 0.58f

#define ES2_TAU 1.74f
#define ES2_A 0.8f
#define ES2_B 0.62f
#define ES2_LAMBDA 0.19f

#define NES_TAU 1.78f
#define NES_A 1.33f
#define NES_B 0.34f
#define NES_LAMBDA 0.08f

// Repulsive Force on Border
#define BR_A 2000.0f
#define BR_B 0.05f

class SFMGlobal {
public:
	static float ca_k;
	static float ca_kappa;

	static float cs_a;
	static float cs_b;
	static float cs_lambda;

	static float es1_tau;
	static float es1_a;
	static float es1_b;
	static float es1_lambda;

	static float es2_tau;
	static float es2_a;
	static float es2_b;
	static float es2_lambda;

	static float nes_tau;
	static float nes_a;
	static float nes_b;
	static float nes_lambda;

	static float br_a;
	static float br_b;
};

#endif // !__SOCIAL_FORCE_PARAMETER_H__