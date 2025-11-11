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

#endif // PSR_H_

