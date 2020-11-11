#include <msp430.h>
#include "LCD_128.h"
#include "HT1621.h"
#include "TCA6416A.h"

long RES;//�������
long Vin;//�����ѹ����
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
      ADC10CTL0 |= ENC + ADC10SC; //����ѭ���п��� ADC10 ת��
      _bis_SR_register(CPUOFF + GIE); //���߿����жϣ��ȴ� ADC10 ת����ɺ󣬽����ж����л��� CPU ����
      //-----ADCת������жϻ���CPU���ִ�����´���-----
      ADC = ADC10MEM;		//��ADC��ȡAD����ֵ
      sum=sum+ADC;//sum���������ۼ�20�β�����ֵ
      count++;//count������¼�ۼӵĴ���
      if(count==20)//count�ۼ�һ���ﵽ20
      {
      Average=sum/20;//Average���������ۼ�20�β�����ƽ��ֵ
      Vin=(Average*2.5*1000)/1023;	//��ѹ���ʽ������1000������mVΪ��λ
      RES=(0.3997*Vin + 7.891)*100;	//ת��Ϊ����ֵ����100��������ȷ��С�������λ
      LCD_Display();    	  //����LCD��ʾ����
      count=0;//count���㣬���¼�¼�ۼӴ���
      sum=0;//sum���㣬���¼�¼�ۼ�ֵ
      }
    }
}



/******************************************************************************************************
 * ��       �ƣ�LCD_Init()
 * ��       �ܣ���ʼ��LCD��ʾ��ص�Ӳ������Ԥ��̶��������ʾ����
 * ��ڲ�������
 * ���ڲ�������
 * ˵       ������
 * ��       ������
 ******************************************************************************************************/
void LCD_Init()
{
    TCA6416A_Init();
    HT1621_init();
    //���Ӳ���ĳ�ʼ�������� I2C ģ��ĳ�ʼ���� TCA6416A ��ʼ���������ڲ�����ˣ� LCD_128 �⺯���� HT1621 ��ʼ���������ڲ�������
	//-----��ʾ�̶������LCD��-----
    LCD_DisplaySeg(_LCD_AUTO);
    LCD_DisplaySeg(_LCD_RUN);
   	LCD_DisplaySeg(_LCD_TI_logo);
   	LCD_DisplaySeg(_LCD_QDU_logo);
   	LCD_DisplaySeg(_LCD_OHOM);
  	LCD_DisplaySeg(_LCD_DOT3);
}




/******************************************************************************************************
 * ��       �ƣ�LCD_Displaly()
 * ��       �ܣ�������ֵ��ʾ����
 * ��ڲ�������
 * ���ڲ�������
 * ˵       ����������ֵȼ�����
 * ��       ������
 ******************************************************************************************************/
void LCD_Display()
{
	      //-----���6λ��ʾ����-----
		  LCD_DisplayDigit(LCD_DIGIT_CLEAR,1);
		  LCD_DisplayDigit(LCD_DIGIT_CLEAR,2);
		  LCD_DisplayDigit(LCD_DIGIT_CLEAR,3);
	      LCD_DisplayDigit(LCD_DIGIT_CLEAR,4);
	      LCD_DisplayDigit(LCD_DIGIT_CLEAR,5);
	      LCD_DisplayDigit(LCD_DIGIT_CLEAR,6);
	      //-----����Res��ֲ���ʾ����-----

	      if(RES>=100000)//100000��6λ��
	      {
			  LCD_DisplayDigit(RES/100000,1);
			  LCD_DisplayDigit((RES%100000)/10000,2);
			  LCD_DisplayDigit((RES%10000)/1000,3);
			  LCD_DisplayDigit((RES%1000)/100,4);
			  LCD_DisplayDigit((RES%100)/10,5);
			  LCD_DisplayDigit(RES%10,6);
	      }
	      else if(RES>9999)//10000~99999��5λ��
	      {
	    	  LCD_DisplayDigit(RES/10000,2);
			  LCD_DisplayDigit((RES%10000)/1000,3);
			  LCD_DisplayDigit((RES%1000)/100,4);
			  LCD_DisplayDigit((RES%100)/10,5);
			  LCD_DisplayDigit(RES%10,6);
	      }
	      else if(RES>999)//1000~9999��4λ��
	      {
	    	  LCD_DisplayDigit(RES/1000,3);
			  LCD_DisplayDigit((RES%1000)/100,4);
			  LCD_DisplayDigit((RES%100)/10,5);
			  LCD_DisplayDigit(RES%10,6);
	      }
	      else if(RES>99)//100~999��3λ��
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

	      if(Vin>=1000)//1000~9999��4λ��
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
	      HT1621_Reflash(LCD_Buffer);//-----���»��棬������ʾ-----
}





/******************************************************************************************************
 * ��       �ƣ�ADC10_init(void)
 * ��       �ܣ���ʼ��ADC10�������ֶ�����
 * ��ڲ�������
 * ���ڲ�������
 * ˵       ������
 * ��       ������
 ******************************************************************************************************/
void ADC10_init(void)
{
    ADC10CTL0 &= ~ENC;
    ADC10CTL0 = ADC10IE|ADC10ON | REFON| REF2_5V | ADC10SHT_3 | SREF_1;
    //�����ڲ�2.5V�ο���ѹ����������ʱ��Ϊ64 x ADC10CLKs
    ADC10CTL1 = CONSEQ_0 | ADC10SSEL_0 | ADC10DIV_7 | SHS_0 | INCH_0;
    //ADCʱ�ӷ�ƵΪ7��Ƶ
    ADC10AE0 = 0x1;//P1.0ΪADC�����
    __delay_cycles(30000);
    ADC10CTL0 |= ENC;
}




/******************************************************************************************************
 * ��       �ƣ�ADC10_ISR_HOOK
 * ��       �ܣ�ADCת����ɺ���CPU
 * ��ڲ�������
 * ���ڲ�������
 * ˵       ����ֱ�Ӵ�CCS��Example����ֲ����
 * ��       ������
 ******************************************************************************************************/
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
	_bic_SR_register_on_exit(LPM4_bits);
}
