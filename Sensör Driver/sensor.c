#include <linux/module.h> //Tüm linux modullerinin barındırıldığı header
#include <linux/kernel.h> //Dmesg de görünen INFO mesajları için kullanılan header
#include <linux/fs.h>	  //filp fonksiyonu için kullanılan header	
#include <linux/uaccess.h>//
#include <asm/uaccess.h>  //
#include <linux/slab.h>   //kmalloc kullanımı için kullanılan header
#include <linux/device.h> //Device Driver için kullanılan header
#include <linux/cdev.h>   //Character Device için kullanılan header




static char msg[2];
static char bosluk[2] = {'\0','\0'};

struct tty_driver *tty;      //tty_driver ımızın referans aldığı değişken

static dev_t cdev_node;     //Character device için ayırıp bağlayacağımız ilk node 
static struct class *cl;    //Class değişkenimiz
static struct cdev c_dev;	//Character devicenin tutulacağı değişken


char buf[256];				//ttyACM den okunan verilerin tutulduğu buffer görevi gören string değişkeni

//-----------------------------MAİN FUNKs
//ttyACM yi okuyan fonk;
/*
	-> bilgi_oku usb aygıtından çıkan verileri yakalayan fonksiyon
	-> hedef okunan datanın geri besleneceği string

*/

void bilgi_oku(char*  hedef){
	struct file *f;
	mm_segment_t fs;
	ssize_t result;
	//char *usbyol;

	//usbyol = (char*)kmalloc(sizeof(char)*20,GFP_KERNEL); //usb yolu için bellekte alan ayırdık

	//sprintf(usbyol,"/dev/ttyACM%c",msg[0]);

	f= filp_open("/dev/ttyACM2", O_RDONLY, 0);           //Usb den veri okunması için bağlantı olusturan fonksiyon
	
	if(f== NULL)              
		printk(KERN_ALERT "filp_open error!!\n");        //Okuma başarılı değil ise hata ver   
	else{            
		fs = get_fs();                                   //İşlem başarılı olduğu durumda hedef değişkenini veri gönderiyoruz
		set_fs(get_ds());
	//	f->f_op->read(f, buf, 256 ,&f->f_pos);		
	result =vfs_read(f, (void __user *)buf, 256, &f->f_pos);
		//read fonksiyonunun kullanımı kernel versiyonuna göre değişiklik gösterir.
		//ret = vfs_read(file, data, size, &offset);
		
		set_fs(fs);
		sprintf(hedef,buf);
	}

	filp_close(f,NULL); //dosyayı kapat

	//kfree(usbyol);
}

/*-------------------Device Functions --------------------------*/

static int c_open(struct inode *i,struct file *f){     //Device açılınca tetiklenen foksiyon
	return 0;
}
static int c_release(struct inode *i,struct file *f){  //Device kapanınca tetiklenen fonksiyon
	return 0;
}

static ssize_t c_read(struct file *f,char __user *buf,size_t len, loff_t *off){  //Device dan veri okuma yapılacağı zaman tetiklenen fonksiyon
	ssize_t ret;
	size_t str_len = 10;
	char *temp;

	temp = (char *)kmalloc(sizeof(char)*20,GFP_KERNEL);   //temp e yer ayırıyoruz
 
	bilgi_oku(buf);                                        //Bilgi_oku fonksiyonu çağır ve datayı buf a sakla
	sprintf(temp,"Nem : %s \n",buf);					   //Gelen datayı anlamlı hale getir		
	printk(KERN_INFO "Nem : %s", buf);					   //Anlamlı hale gelen datayı loga yaz 	

	ret =  simple_read_from_buffer(buf, len, off, temp, str_len); //Anlamlı hale gelen datayı user moda gönder
	kfree(temp); 							//temp e ayrılan yeri sil
	return ret;
}


/*
	echo ile ~~~



*/
//Device e parametre gönderileceği zaman tetiklenen fonksiyon
static ssize_t c_write(struct file *f, const char __user *buf,size_t len, loff_t *off){ 
	ssize_t ret;

	if(len > 3)
		len=3;

	strcpy(msg,bosluk);
	memcpy(msg,buf,len);		//Aldiğımız değişkeni msg degişkenine koy
	ret = strlen(msg);
	return len;
}

static struct file_operations fops ={
	.owner=THIS_MODULE,
	.open=c_open,
	.read=c_read,
	.release=c_release,
	.write=c_write
};

static int __init first_init(void){

int i;

	if(alloc_chrdev_region(&cdev_node,0,1,"DevelOp")<0)
	return -1;

	if ((cl = class_create(THIS_MODULE,"dvlOp"))==NULL){
		unregister_chrdev_region(cdev_node,1);
		return -1;
	}
	if (device_create(cl,NULL,cdev_node,NULL,"DevelOpNull")== NULL)
	{
		class_destroy(cl);
		unregister_chrdev_region(cdev_node,1);
		return -1;
	}

	for (i = 0; i < 128; i++)
								 buf[i] = 0;
		cdev_init(&c_dev,&fops);
		
		if (cdev_add(&c_dev,cdev_node,1) == -1){
	device_destroy(cl,cdev_node);
	class_destroy(cl);
	unregister_chrdev_region(cdev_node,1);
	return -1;
	}						

	printk(KERN_INFO "\n DEvelop registered\n");
	return 0;

	}




static void __exit first_exit(void) {
 
 cdev_del(&c_dev);
 device_destroy(cl, cdev_node);
 class_destroy(cl);
 unregister_chrdev_region(cdev_node, 1);
 
 printk(KERN_INFO "\n unregistered");


}

MODULE_LICENSE("GPL");
module_init(first_init);
module_exit(first_exit);
