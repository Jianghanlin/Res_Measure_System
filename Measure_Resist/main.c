#include <msp430.h>
#include "LCD_128.h"
#include "HT1621.h"
#include "TCA6416A.h"

long RES;//电阻变量
long Vin;//输入电压变量
void ADC10_init(void);
void LCD_Init();
void LCD_Display();

void main()
{
    long sum=0,Average,ADC;
    int count=0;
	WDTCTL=WDTPW+WDTHOLD;
	ADC10_init();
	LCD_Init();
    while(1)
    {
      ADC10CTL0 |= ENC + ADC10SC; //在主循环中开启 ADC10 转换
      _bis_SR_register(CPUOFF + GIE); //休眠开总中断，等待 ADC10 转换完成后，进入中断运行唤醒 CPU 代码
      //-----ADC转换完成中断唤醒CPU后才执行以下代码-----
      ADC = ADC10MEM;		//用ADC读取AD采样值
      sum=sum+ADC;//sum变量保存累加20次采样的值
      count++;//count用来记录累加的次数
      if(count==20)//count累加一旦达到20
      {
      Average=sum/20;//Average变量保存累加20次采样的平均值
      Vin=(Average*2.5*1000)/1023;	//电压表达式，扩大1000倍，以mV为单位
      RES=(0.3997*Vin + 7.891)*100;	//转换为电阻值，并100倍处理，精确到小数点后两位
      LCD_Display();    	  //调用LCD显示函数
      count=0;//count清零，重新记录累加次数
      sum=0;//sum清零，重新记录累加值
      }
    }
}



/******************************************************************************************************
 * 名       称：LCD_Init()
 * 功       能：初始化LCD显示相关的硬件，并预设固定不变的显示内容
 * 入口参数：无
 * 出口参数：无
 * 说       明：无
 * 范       例：无
 ******************************************************************************************************/
void LCD_Init()
{
    TCA6416A_Init();
    HT1621_init();
    //相关硬件的初始化，其中 I2C 模块的初始化由 TCA6416A 初始化函数在内部完成了， LCD_128 库函数由 HT1621 初始化函数在内部引用了
	//-----显示固定不变的LCD段-----
    LCD_DisplaySeg(_LCD_AUTO);
    LCD_DisplaySeg(_LCD_RUN);
   	LCD_DisplaySeg(_LCD_TI_logo);
   	LCD_DisplaySeg(_LCD_QDU_logo);
   	LCD_DisplaySeg(_LCD_OHOM);
  	LCD_DisplaySeg(_LCD_DOT3);
}




/******************************************************************************************************
 * 名       称：LCD_Displaly()
 * 功       能：将电阻值显示出来
 * 入口参数：无
 * 出口参数：无
 * 说       明：拆分数字等几部分
 * 范       例：无
 ******************************************************************************************************/
void LCD_Display()
{
	      //-----清除6位显示数字-----
		  LCD_DisplayDigit(LCD_DIGIT_CLEAR,1);
		  LCD_DisplayDigit(LCD_DIGIT_CLEAR,2);
		  LCD_DisplayDigit(LCD_DIGIT_CLEAR,3);
	      LCD_DisplayDigit(LCD_DIGIT_CLEAR,4);
	      LCD_DisplayDigit(LCD_DIGIT_CLEAR,5);
	      LCD_DisplayDigit(LCD_DIGIT_CLEAR,6);
	      //-----根据Res拆分并显示数字-----

	      if(RES>=100000)//100000（6位）
	      {
			  LCD_DisplayDigit(RES/100000,1);
			  LCD_DisplayDigit((RES%100000)/10000,2);
			  LCD_DisplayDigit((RES%10000)/1000,3);
			  LCD_DisplayDigit((RES%1000)/100,4);
			  LCD_DisplayDigit((RES%100)/10,5);
			  LCD_DisplayDigit(RES%10,6);
	      }
	      else if(RES>9999)//10000~99999（5位）
	      {
	    	  LCD_DisplayDigit(RES/10000,2);
			  LCD_DisplayDigit((RES%10000)/1000,3);
			  LCD_DisplayDigit((RES%1000)/100,4);
			  LCD_DisplayDigit((RES%100)/10,5);
			  LCD_DisplayDigit(RES%10,6);
	      }
	      else if(RES>999)//1000~9999（4位）
	      {
	    	  LCD_DisplayDigit(RES/1000,3);
			  LCD_DisplayDigit((RES%1000)/100,4);
			  LCD_DisplayDigit((RES%100)/10,5);
			  LCD_DisplayDigit(RES%10,6);
	      }
	      else if(RES>99)//100~999（3位）
	      {
	    	  LCD_DisplayDigit(RES/100,4);
			  LCD_DisplayDigit((RES%100)/10,5);
			  LCD_DisplayDigit(RES%10,6);
	      }
	      else
	      {
	    	  LCD_DisplayDigit(0,4);
	    	  LCD_DisplayDigit(RES/10,5);
			  LCD_DisplayDigit(RES%10,6);
	      }
/*

	      if(Vin>=1000)//1000~9999（4位）
	      {
	    	  LCD_DisplayDigit(Vin/1000,3);
			  LCD_DisplayDigit((Vin%1000)/100,4);
			  LCD_DisplayDigit((Vin%100)/10,5);
			  LCD_DisplayDigit(Vin%10,6);
	      }
	      else if(Vin>=100)//100~999
	      {
	    	  LCD_DisplayDigit(Vin/100,4);
			  LCD_DisplayDigit((Vin%100)/10,5);
			  LCD_DisplayDigit(Vin%10,6);
	      }
	      else
	      {
	    	  LCD_DisplayDigit(Vin/10,5);
			  LCD_DisplayDigit(Vin%10,6);
	      }
*/
	      HT1621_Reflash(LCD_Buffer);//-----更新缓存，真正显示-----
}





/******************************************************************************************************
 * 名       称：ADC10_init(void)
 * 功       能：初始化ADC10，单次手动采样
 * 入口参数：无
 * 出口参数：无
 * 说       明：无
 * 范       例：无
 ******************************************************************************************************/
void ADC10_init(void)
{
    ADC10CTL0 &= ~ENC;
    ADC10CTL0 = ADC10IE|ADC10ON | REFON| REF2_5V | ADC10SHT_3 | SREF_1;
    //启用内部2.5V参考电压，采样保持时间为64 x ADC10CLKs
    ADC10CTL1 = CONSEQ_0 | ADC10SSEL_0 | ADC10DIV_7 | SHS_0 | INCH_0;
    //ADC时钟分频为7分频
    ADC10AE0 = 0x1;//P1.0为ADC输入端
    __delay_cycles(30000);
    ADC10CTL0 |= ENC;
}




/******************************************************************************************************
 * 名       称：ADC10_ISR_HOOK
 * 功       能：ADC转换完成后唤醒CPU
 * 入口参数：无
 * 出口参数：无
 * 说       明：直接从CCS的Example中移植过来
 * 范       例：无
 ******************************************************************************************************/
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
	_bic_SR_register_on_exit(LPM4_bits);
}
