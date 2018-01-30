

#include<wiringPiI2C.h>
#include<wiringPi.h>
#include<stdio.h>

#define soc_adress 0x62

int  get_Vcell(const int fd);                 		 //get Vcell date
int  get_Soc(const int fd);                  		 //get Soc   date
int  get_remrun_time(const int fd);                  //get remaining run time
int  set_ALRT(int ALRT_date,const int fd);           //set date to ALRT

 
int main(void)
{
    int fd;
    int num=0;    
	int flag=0;
	int soc_alert=0;
    
       wiringPiSetup();               //configure pi
      
       fd=wiringPiI2CSetup(soc_adress);
       if(fd<0)                //configure I2C
       {
          printf("GG\n");
       }
	   
       wiringPiI2CWriteReg8(fd,0x0a,0x00);//strating cw2015
       num=wiringPiI2CReadReg8(fd,0x0a);  
    
           if(num==0x00)//success start cw2015
	   {
			printf("start cw2015!\n");
	   }
   	   else        //error start cw2015  exit
	   {
			printf("error start!\n");
			return 0;
	   }
	 
    printf("press 1 \n");	 
    while(1)
    {
	 scanf("%d",&flag);
	 if(flag==1)
	  {
		get_Vcell(fd);
		get_Soc(fd);
		if(get_remrun_time(fd)>0)printf("Lack of electricity!!!!\n");
		flag=0;
	  }
	 if(flag==2) //set ALRT Soc 
	 {
	    printf("Please enter the value you want to set ");
		scanf("%d",&soc_alert);
		set_ALRT(soc_alert,fd);
		
	 }
	  else
	  {
		flag=0;
	  }
    }

}

//  get Vcell date
//  reg:0x02 0x03
//  14bit use date

int get_Vcell(const int fd)
{
      long int date=0;         
      float prt_date=0.0;    

     date=wiringPiI2CReadReg8(fd,0x02);
     date <<=8;
     date +=wiringPiI2CReadReg8(fd,0x03);
	
     prt_date=date*0.305/1000;	

     printf("Vcell is %.2f V\n ",prt_date);

}	
// get Soc date
// reg:0x04 0x05
// 16 bit
	
int get_Soc(const int fd)
{
    int zhengshu=0;   // Soc  ge wei
	int xiaoshu=0;    // Soc  xiaoshu 
	int ptr_num=0;
	
	zhengshu=wiringPiI2CReadReg8(fd,0x04);
	ptr_num=wiringPiI2CReadReg8(fd,0x05);
        	 
	if(ptr_num&0x80) xiaoshu +=500;
    if(ptr_num&0x40) xiaoshu +=250;
	if(ptr_num&0x20) xiaoshu +=125;
	if(ptr_num&0x10) xiaoshu +=62;
	if(ptr_num&0x08) xiaoshu +=31;
	if(ptr_num&0x04) xiaoshu +=15;
    if(ptr_num&0x02) xiaoshu +=7;
	if(ptr_num&0x01) xiaoshu +=3;        
	
	xiaoshu /=10;

	printf("Current electricity is %d.%d%%\n",zhengshu,xiaoshu);	
}
//  get remaining time
//  reg :0x06 0x07
//	0x06 2^12~2^8
//  0x07 2^7~2^0
 
int get_remrun_time(const int fd)
{
	int minute=0;
	int hour=0;
	
	minute=wiringPiI2CReadReg8(fd,0x06);
	minute <<=8;
	minute +=wiringPiI2CReadReg8(fd,0x07);
	
	hour=minute/60;
	minute%=60;
	
	printf("remaining time");
	if(hour>0)
	{
		printf("  %dhr",hour);
	}
	printf("  %dmin\n",minute);
	
	
	//minute 16 bit is ALRT flag
	if(minute & 0x8000)return 1;
	
	
}

//set ALRT_date to Notice

int  set_ALRT(int ALRT_date,const int fd)
{
    //set ALRT_date
	ALRT_date <<=3;
	wiringPiI2CWriteReg8(fd,0x08,ALRT_date);
	if(ALRT_date==((wiringPiI2CReadReg8(fd,0x08))&0xf8))printf("Set up success!\n");
	
	//UFG   flag
	if(((wiringPiI2CReadReg8(fd,0x08))&0x02)==1)return 1;
	

}




