#include <linux/module.h>
#include <linux/proc_fs.h>		//proc_create() , remove_proc_entry() , simple_read_from_buffer() ve proc struct için gerekli kütüphane
#include <linux/uaccess.h>		//copy_from_user() için uaccess.h gerekli
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/slab.h>			//kmalloc için gerekli kütüphane
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sefa");

static char *msg;
static char deger1[20];
static char deger2[20];
static int temp;
static long m;
static long n;
static int matris[m][n];

static void matrisBoyut(void);
static void matrisElemanlar(void);
static void matrisArryaz(void);


static void matrisBoyut(){	
	static char str1[3];
	static char str2[3];
	
	int sayac,i=0;
	printk(KERN_ALERT "M-Boyut çalıştı\n");
	//"2x3 4 5 8 1"
	
	if(msg[1]=='x'){
		str1[0]=msg[0];
		str2[0]=msg[2];
	}
	else{
		printk(KERN_ALERT "geçersiz dizi boyutu!\n");
		return;
	}	
	

	if (kstrtol(str1,10,&m) != 0){
  		printk(KERN_ALERT "1.Sayı : Geçersiz Karakter\n");
  		return;
 	}
 	if (kstrtol(str2,10,&n) != 0){
  		printk(KERN_ALERT "2.Sayı : Geçersiz Karakter\n");
  		return;
 	}

 	printk("m=%ld   n=%ld\n",m,n);
 	
}

static void matrisElemanlar(){
	static char i,j=0,str[5];
	static int sayac1,sayac2=0;
	char dizi[temp-4];
	// 0123456789
	//"2x3 9 6 2 4 7 5"
	printk("\n\n");
	for(sayac1=0; sayac1<temp; sayac1++){
		if(msg[sayac1]==' '){
			i=sayac1+1;
			while(msg[i]!=' '){
				str[j]=msg[i];
				printk("str[%d]= %c  \n",j,str[j]);
				j++;
				i++;
			}
			
			dizi[sayac2]=str[sayac2];
 			sayac2++;


			/*
			if (kstrtol(str,10,&x) != 0){
  				printk(KERN_ALERT "dizi(%d) Geçersiz Karakter\n",sayac2);
  				printk("x= %ld\n\n\n",x);
  				return;
 			}
 			else {
 				dizi[sayac2]=x;
 				sayac2++;
 			}*/
		} 		
	}
	printk("dizi: \n\n");
	int s;
	for(s=0;s<(temp-4);s++){
		printk("%s  ",dizi[s]);
	}
	
}

static void matrisArryaz(){
	static int sayac1=0;
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			matris[i][j]=msg[sayac1];
			printk(matris[i][j]);
        sayac1++;
		}
		
		printk("marisegdenarr sayac :%d",sayac1);

	}
}



static ssize_t read_proc(struct file *filp,char *buf,size_t count,loff_t *offp ){		
	
	if(count>temp){
		count=temp;
	}
	temp=temp-count;
	copy_to_user(buf,msg, count);
	return count;
}

static ssize_t write_proc(struct file *filp,const char *buf,size_t count,loff_t *offp){		
	/*
	örnek:
	9	6	2

	4	7	5
	*/
	//"2x3 9 6 2 4 7 5"
	msg=kmalloc(20*sizeof(char),GFP_KERNEL);			
	copy_from_user(msg,buf,count);	
	temp=count;

	printk("temp= %u \n",temp);
	
	matrisBoyut();
	printk(KERN_ALERT "m= %ld   n= %ld",m,n);
	//matrisElemanlar();
	return count;		
} 


static const struct file_operations proc_fops = {				
	.owner=THIS_MODULE,
	.read=read_proc,	
	.write=write_proc,	
};

static int  __init proc_init(void){								
	proc_create("basitProc",0777,NULL,&proc_fops);		
 	return 0;
}

static void __exit proc_exit(void){							
 	remove_proc_entry("basitProc",NULL);					
 	kfree(msg);									
}

module_init(proc_init);
module_exit(proc_exit);




