#include <stdarg.h>
#include <arch/bsp/uart.h>
#include <stdbool.h>
#include <limits.h>

char *digit_lookup = "0123456789ABCDEF";

//function to handle signed values
void int_to_str(int num, bool padding, int field_width)
{
	char buffer[12]; //why 12--> enough for int of any size
	int  index = 10; //start from 10 because last index is null terminated
	bool neg   = false;
	buffer[11] = '\0';

	if (num < 0) { //handle signed
		neg = true;
		field_width--;
		if (num ==
		    INT_MIN) { //workaround for the min int edge case, TODO: find a way to make this less explicit
			const char *s = "-2147483648";
			for (int i = 0; s[i]; i++)
				uart_putc(s[i]);
			return;
		}
		num = -num; //this causes problems if its the smallest negative int -.- => -INT_MIN > INT_MAX  (because why not)
	}
	if (num == 0) {
		buffer[index] = '0';
		index--;
		field_width--;
	}
	while (num > 0) {
		int digit     = num % 10;
		buffer[index] = digit + '0';
		num /= 10;
		index--;
		field_width--;
	}

	while (field_width > 0) {
		if (padding) {
			buffer[index] = '0';
			index--;
		} else {
			if (neg) {
				buffer[index] = '-';
				index--;
				neg = false;
			}
			buffer[index] = ' ';
			index--;
		}
		field_width--;
	}
	if (neg) {
		buffer[index] = '-';
		index--;
		field_width--;
		neg = false;
	}

	for (index++; buffer[index];
	     index++) { //we could reuse the same index, just increment once.
		uart_putc(buffer[index]);
	}
}

//
void unsigned_to_str(unsigned int num, int base, bool padding, int field_width)
{
	char buffer[12];
	int  index = 10;
	buffer[11] = '\0';

	if (num == 0) {
		uart_putc('0');
		return;
	}
	while (num > 0) {
		int digit     = num % base;
		buffer[index] = digit_lookup[digit];
		num /= base;
		index--;
		field_width--;
	}
	while (field_width > 0) {
		if (padding) {
			buffer[index] = '0';
			index--;
			field_width--;
		} else {
			buffer[index] = ' ';
			index--;
			field_width--;
		}
	}

	for (index++; buffer[index]; index++) {
		uart_putc(buffer[index]);
	}
}

void kprintf(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	for (int i = 0; format[i]; i++) {
		bool zero_pad	 = false;
		int  field_width = 0;

		switch (format[i]) {
		case '%':
			if (format[i + 1] == '0') {
				zero_pad = true;
				i++;
			}
			if (format[i + 1] == '8') {
				field_width = 8;
				i++;
			}
			switch (format[i + 1]) {
			case 'i':
				int_to_str(va_arg(args, int), zero_pad, field_width);
				i++;
				continue;
				break;
			case 'u':
				unsigned_to_str(va_arg(args, unsigned int), 10, zero_pad,
						field_width);
				i++;
				continue;
				break;

			case 's':
				char *str = va_arg(args, char *);
				for (int i = 0; str[i]; i++) {
					uart_putc(str[i]);
				}
				i++;
				break;
			case 'c':
				uart_putc((unsigned char)va_arg(args, int));
				i++;
				break;

			case 'x':
				unsigned_to_str(va_arg(args, unsigned int), 16, zero_pad,
						field_width);
				i++;
				break;
			case 'p': 
				void *ptr = va_arg(args, void *);
				uart_putc('0');
				uart_putc('x');
				unsigned_to_str((unsigned int)(unsigned long)ptr, 16, true, 8);
				i++;
				break;
			case '%':
				uart_putc('%');
				i++;
				break;
			default:
				char *unknown = "Unknown conversion specifier";
				int   j	      = 0;
				while (unknown[j]) {
					uart_putc(unknown[j]);
					j++;
				}
				break;
			}
			break;
		default:
			uart_putc(format[i]);
		}
	}
	va_end(args);
}
