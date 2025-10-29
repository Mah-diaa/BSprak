void start_kernel(){
    ...
	kprintf("=== Betriebssystem gestartet ===\n");
	test_kernel();
		
	while(true){
	    char c = uart_getc();
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
		kprintf("Es wurde folgendes Zeichen eingegeben: %c, In Hexadezimal: %x, "
		    "In Dezimal: %08i, Als Ptr: %p\n",
			c, (unsigned int)c, (int)c, (void*)c);
		#pragma GCC diagnostic pop
	}
}
