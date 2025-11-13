#ifndef PSR_BITS_H_
#define PSR_BITS_H_

/* PSR Flag Bit Positions */
#define PSR_N_BIT 31	/* Negative flag */
#define PSR_Z_BIT 30	/* Zero flag */
#define PSR_C_BIT 29	/* Carry flag */
#define PSR_V_BIT 28	/* Overflow flag */
#define PSR_E_BIT 9	/* Endianness flag */
#define PSR_I_BIT 7	/* IRQ disable flag */
#define PSR_F_BIT 6	/* FIQ disable flag */
#define PSR_T_BIT 5	/* Thumb mode flag */

/* PSR Flag Bit Masks */
#define PSR_N (1U << PSR_N_BIT)
#define PSR_Z (1U << PSR_Z_BIT)
#define PSR_C (1U << PSR_C_BIT)
#define PSR_V (1U << PSR_V_BIT)
#define PSR_E (1U << PSR_E_BIT)
#define PSR_I (1U << PSR_I_BIT)
#define PSR_F (1U << PSR_F_BIT)
#define PSR_T (1U << PSR_T_BIT)

/* PSR Mode Bit Mask */
#define PSR_MODE_MASK 0x1F	/* Bits 0-4 contain the mode */

/* PSR Status Bit Mask */
#define PSR_STATUS_MASK 0xF0000000	/* Bits 28-31 (N, Z, C, V) */

#endif // PSR_BITS_H_

