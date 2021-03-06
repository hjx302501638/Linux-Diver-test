#include "nand_flash.h"
#include "s3c2440_soc.h"

void sdram_init(void)
{
	BWSCON = 0x22000000;

	BANKCON6 = 0x18001;
	BANKCON7 = 0x18001;

	REFRESH  = 0x8404f5;

	BANKSIZE = 0xb1;

	MRSRB6   = 0x20;
	MRSRB7   = 0x20;
}

int sdram_test(void)
{
	volatile unsigned char *p = (volatile unsigned char *)0x30000000;
	int i;

	// write sdram
	for (i = 0; i < 1000; i++)
		p[i] = 0x55;

	// read sdram
	for (i = 0; i < 1000; i++)
		if (p[i] != 0x55)
			return -1;

	return 0;
}

unsigned char  isbootfromnorflash(void)
{
	volatile unsigned int *p=(volatile unsigned int *)0;
	unsigned int val = *p;
	*p = 0x12345678;
		if(*p == 0x12345678)
			{
				*p = val;
				return 0;
			}else return 1;
}
void copy2asdram(void)
{

	/*要从lds文件中获得 __code_start ,__bss_start
	*然后从0地址将数据复制到__code_start
	*/
	//首先声明这两个外部变量
	extern int __code_start,__bss_start;
	volatile unsigned int *dest = (volatile unsigned int *)&__code_start;
	volatile unsigned int *end = (volatile unsigned int *)&__bss_start;
	volatile unsigned int *src = 0;
	unsigned int i;
	int len = ((int)&__bss_start)	-((int) &__code_start);	
	if(isbootfromnorflash())
		{
				while(dest<end)
			{
				*dest++ = *src++;			
			}
		}
	else 
		{	
			nand_init();
			nand_read(src, dest, len);
		}
}

void clenbss(void)
{
	/*
	*需要从链接脚本中得到__bss_start                _end
	*/
	extern int __bss_start,_end;
	volatile unsigned int *start = (volatile unsigned int *)&__bss_start;	
	volatile unsigned int *end = (volatile unsigned int *)&_end;
	while(start <= end)
		{
			*start++ =0;			
		}
}

