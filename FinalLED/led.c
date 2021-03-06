#include "reg52.h"
#include <intrins.h>

#define led P0

typedef unsigned char u8;
typedef unsigned int u16;

sbit IRIN=P3^2; // 红外输出口

u8 IrValue[6];  // 数据数组
u8 Time;        // 计时暂存

u8 m;

// 延时函数，i=1时，大约延时10us
void delay(u16 i) {
	while(i--);	
}

// 初始化红外线接收
void IrInit() {
    
	IT0=1;//下降沿触发
	EX0=1;//打开中断0允许
	EA=1;	//打开总中断

	IRIN=1;//初始化端口
}


// 主函数
void main() {
    
    u8 data1;
    u8 data2;
    IrInit();
    
    
    led = 0x00;
    delay(50000); //大约延时450ms	
    
	while(1) {
		data1 = IrValue[2]/16;    // 数据码高半字节
		data2 = IrValue[2]%16;    // 数据码低半字节

        if (data1 == 0) {
            switch (data2) {
                case 1:
                    led = 0x01;
                    break;
                case 2:
                    led = 0x02;
                    break;
                case 3:
                    led = 0x04;
                    break;
                case 4:
                    led = 0x08;
                    break;
                case 5:
                    led = 0x10;
                    break;
                case 6:
                    led = 0x01;
                    delay(50000); //大约延时450ms
                    // 将led左移一位
                    for(m=0; m<7; m++) {
                        led = _crol_(led, 1);
                        delay(5000); //大约延时450ms	
                    }
                    // 将led右移一位
                    for(m=0; m<7; m++) {
                        led = _cror_(led, 1);
                        delay(5000); //大约延时450ms
                    }
                    break;
            }
        }
    }
}

// 读取红外数值的中断函数
void ReadIr() interrupt 0 {
    
	u8 j,k;
	u16 err;
	Time=0;					 
	delay(700);	//7ms
	if(IRIN==0)		//确认是否真的接收到正确的信号
	{
		
		err=1000;				//1000*10us=10ms,超过说明接收到错误的信号
		/*当两个条件都为真是循环，如果有一个条件为假的时候跳出循环，免得程序出错的时
		侯，程序死在这里*/	
		while((IRIN==0)&&(err>0))	//等待前面9ms的低电平过去  		
		{			
			delay(1);
			err--;
		} 
		if(IRIN==1)			//如果正确等到9ms低电平
		{
			err=500;
			while((IRIN==1)&&(err>0))		 //等待4.5ms的起始高电平过去
			{
				delay(1);
				err--;
			}
			for(k=0;k<4;k++)		//共有4组数据
			{				
				for(j=0;j<8;j++)	//接收一组数据
				{

					err=60;		
					while((IRIN==0)&&(err>0))//等待信号前面的560us低电平过去
					{
						delay(1);
						err--;
					}
					err=500;
					while((IRIN==1)&&(err>0))	 //计算高电平的时间长度。
					{
						delay(10);	 //0.1ms
						Time++;
						err--;
						if(Time>30)
						{
							return;
						}
					}
					IrValue[k]>>=1;	 //k表示第几组数据
					if(Time>=8)			//如果高电平出现大于565us，那么是1
					{
						IrValue[k]|=0x80;
					}
					Time=0;		//用完时间要重新赋值							
				}
			}
		}
		if(IrValue[2]!=~IrValue[3])
		{
			return;
		}
	}			
}

