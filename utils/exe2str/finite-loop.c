int _start() 
{
	int a=0;
	int b = 3*2+1;
	int c;
	__asm__ __volatile__ (
	   "int $0x80"
	   :
	   :);
	
	for (a=0; a< 2000; ++a)
	{
		b=1;
		for (c=0; c< 16; ++c)
			b = b << 1;
	}
	return b;
}
