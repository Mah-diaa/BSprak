#ifndef PSR_H_
#define PSR_H_

enum psr_mode {
	PSR_USR = 0x10,
	PSR_FIQ = 0x11,
	PSR_IRQ = 0x12,
	PSR_SVC = 0x13,
	PSR_ABT = 0x17,
	PSR_UND = 0x1b,
	PSR_SYS = 0x1f,
};

#define PSR_N_BIT 31
#define PSR_Z_BIT 30
#define PSR_C_BIT 29
#define PSR_V_BIT 28
#define PSR_E_BIT 9
#define PSR_I_BIT 7
#define PSR_F_BIT 6
#define PSR_T_BIT 5

#define PSR_N (1U << PSR_N_BIT)
#define PSR_Z (1U << PSR_Z_BIT)
#define PSR_C (1U << PSR_C_BIT)
#define PSR_V (1U << PSR_V_BIT)
#define PSR_E (1U << PSR_E_BIT)
#define PSR_I (1U << PSR_I_BIT)
#define PSR_F (1U << PSR_F_BIT)
#define PSR_T (1U << PSR_T_BIT)

#define PSR_MODE_MASK 0x1F
#define PSR_STATUS_MASK 0xF0000000

#endif // PSR_H_

