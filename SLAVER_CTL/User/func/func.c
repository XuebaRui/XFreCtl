#include "func.h"
#include "nfre.h"
#include "hmc832.h"
/***** N5 N4 N3 N2 N1 N0 S1 S0**/
/* ’Ê÷µ±Ì
**4.7  0  0  1  0  1  0  1  1          
**4.8  0  0  1  0  1  1  0  0 
**4.9	 0  0  1  0  1  1  0  1				
**5.0  0  0  1  0  1  1  1  0
**5.1  0  0  1  0  1  1  1  1
**5.2  0  0  1  1  0  0  0  0
**5.3  0  0  1  1  0  0  0  1
*/
void Fre_Ctl(u32 fre)
{
	if(fre <= 8000000)
	{
		N3(1);N2(0);N1(1);N0(0);S1(1);S0(0);
		HMC832_FreSet(8000000 - fre);
	}
	else if (fre <= 8200000)
	{
		N3(1);N2(0);N1(1);N0(0);S1(1);S0(0);
		HMC832_FreSet(8200000 - fre);
	}
	else if (fre <= 8400000)
	{
		N3(1);N2(0);N1(1);N0(0);S1(1);S0(0);
		HMC832_FreSet(8400000 - fre);
	}
	else if (fre <= 8600000)
	{
		N3(1);N2(0);N1(1);N0(0);S1(1);S0(0);
		HMC832_FreSet(8600000 - fre);
	}
	else if (fre <= 8800000)
	{
		N3(1);N2(0);N1(1);N0(0);S1(1);S0(0);
		HMC832_FreSet(8800000 - fre);
	}
	else if (fre <= 9000000)
	{
		N3(1);N2(0);N1(1);N0(0);S1(1);S0(0);
		HMC832_FreSet(9000000 - fre);
	}

}