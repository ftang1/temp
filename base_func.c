
qemu-system-i386 -m 512M -kernel ../desktop/arch/x86/boot/bzImage -append "ip=10.239.14.43 nfsroot=10.239.97.14:/nfsroot/wfg,v3 root=/dev/nfs rw

qemu -m 512M -kernel arch/x86/boot/bzImage -append "root=/dev/sda" -hda /home/feng/linux-0.2.img

qemu -m 512M -kernel arch/x86/boot/bzImage -append "root=/dev/sda boot_delay=50" -hda  /home/feng/linux-0.2.img

#define mp(f, x...) \
        printf("[%s()]: " f "\n", __func__,##x)
#define mpi() \
        printf("[%s()]: enter\n", __func__)
#define mpo() \
        printf("[%s()]: exit\n", __func__)

#define thang() \
	while (1)
		sleep(5);

#define mp(f, x...) \
        printk(KERN_ERR "[%s()]: " f "\n", __func__,##x)
#define mpi() \
        printk(KERN_ERR "[%s()]: enter\n", __func__)
#define mpo() \
        printk(KERN_ERR "[%s()]: exit\n", __func__)

#define mcp(f, x...) \
        printk("[%s()] C[%d]: " f "\n", __func__, smp_processor_id(),##x)
#define mcpi() \
        printk("[%s()] C[%d]: enter\n", __func__, smp_processor_id())
#define mcpo() \
        printk("[%s()] C[%d]: exit\n", __func__, smp_processor_id())

#define mcpp(f, x...) \
        printk("[%s()] C[%d] P[%s]: " f "\n", __func__, smp_processor_id(), current->comm,##x)
#define mcppi() \
        printk("[%s()] C[%d] P[%s]: enter\n", __func__, smp_processor_id(), current->comm)
#define mcppo() \
        printk("[%s()] C[%d] P[%s]: exit\n", __func__, smp_processor_id(), current->comm)

#define mtp(f, x...) \
        trace_printk(KERN_ERR "" f "\n",##x)
#define mtpi() \
        trace_printk(KERN_ERR "enter\n")
#define mtpo() \
        trace_printk(KERN_ERR "exit\n")

#define mtcp(f, x...) \
        trace_printk("C[%d]: " f "\n", smp_processor_id(),##x)
#define mtcpi() \
        trace_printk("C[%d]: enter\n", smp_processor_id())
#define mtcpo() \
        trace_printk("C[%d]: exit\n", smp_processor_id())

#define mtcpp(f, x...) \
        trace_printk("C[%d] P[%s]: " f "\n", smp_processor_id(), current->comm,##x)
#define mtcppi() \
        trace_printk("C[%d] P[%s]: enter\n", smp_processor_id(), current->comm)
#define mtcppo() \
        trace_printk("C[%d] P[%s]: exit\n", smp_processor_id(), current->comm)

#define m2p(f, x...) \
	do { \
		printk(KERN_ERR "[%s()]: " f "\n", __func__,##x); \
		trace_printk(KERN_ERR "" f "\n",##x); \
	} while (0)
#define m2pi() \
	do { \
		printk(KERN_ERR "[%s()]: enter\n", __func__); \
		trace_printk("C[%d]: enter\n", smp_processor_id()); \
	} while (0)
#define m2po() \
	do { \
		printk(KERN_ERR "[%s()]: exit\n", __func__); \
		trace_printk("C[%d]: exit\n", smp_processor_id()); \
	} while (0)

#define first_dump() \
	do { \
		static int first = 1; \
		\
		if (first++ == 1) \
			dump_stack(); \
	} while (0)


static inline void mpn(int num)
{
	int i;
	char name[64];

	extern int lookup_symbol_name(unsigned long addr, char *symname);
	lookup_symbol_name((unsigned long)__builtin_return_address(0), name);

	printk("%s(): ", name);
	for (i = 0; i < 5; i ++)
		printk("%d", num);
	printk("\n");
}

/* it will surely cause memory leak, so be careful to use */
static inline char *addr_hex2str(void *addr)
{
	char *name;
	extern void *__kmalloc(size_t size, gfp_t flags);
	extern int lookup_symbol_name(unsigned long addr, char *symname);

	if (!addr)
		return NULL;
	name = __kmalloc(64, 0x70);
	lookup_symbol_name((unsigned long)addr, name);

	return name;
}

static void print_ts(struct timespec *ts, char *name)
{
	m2p("%s:  %d:%d",
		name,
		ts->tv_sec,
		ts->tv_nsec);
}

#define IOAPIC_ADDR_BASE	0xFEC00000
#define ioapic_read()	(*(volatile unsigned int *)(0xFEC00000 + 0x10))
#define ioapic_write(val) \
	do  { \
		*(volatile unsigned int *)0xFEC00000 = val; \
	} while (0) 

static inline void ioapic_dump(void)
{
	int i;

	ioapic_write(0x0);
	printk("IOAPIC_ID: 0x%x  ", ioapic_read());
	ioapic_write(0x1);
	printk("VER: 0x%x    RTE Dump: \n", ioapic_read());

	for (i = 0; i < 24; i++) {
		if (i % 4 == 0)
			printk("[%2d]: ", i);
		ioapic_write(i*2+0x10);
		printk(" 0x%08x ", ioapic_read()); 
		if (i % 4 == 3)
			printk("\n");
	}
}


#define SFI_DBG 1
#if SFI_DBG
#define sfi_dbg(f, x...) \
        printk(KERN_ERR "[%s()]: " f "\n", __func__,##x)
#define sfi_enter() \
        printk(KERN_ERR "[%s()]: enter\n", __func__)
#define sfi_exit() \
        printk(KERN_ERR "[%s()]: exit\n", __func__)
#else
#define sfi_dbg(f, x...)  do{} while(0)
#define sfi_enter()  do{} while(0)
#define sfi_exit()  do{} while(0)
#endif /* end of SFI_DBG */

void hexdump(void *buf, int bytelen, const char *str)
{
	unsigned int *p32, i;
	
	if (str)
		printk("Dump %s:\n", str);
		
	p32 = (unsigned int *)buf;
	for (i = 0; i < (bytelen / 4)+1; i++){
		printk(" 0x%08x ", p32[i]);
	}
	printk("\n\n");		
}

#define mdp(num) \
	{ \
		char i; \
		printf("%s(): ", __func__); \
		for (i = 0; i < 10; i++) \
			printf("%d", num); \
		printf("\n"); \
	}


static inline void myn(int num)
{
	uint32_t i;

	for (i = 0; i < 5; i ++)
		printf("%d", num);
	printf("\n");
}

/* inode dumping */
static inline void page_inode_dump(struct page *page)
{
	struct address_space *as = page->mapping;
	struct inode *inode = as->host;
	struct buffer_head *bh, *head;

	inode = (unsigned long)as ? as->host : NULL;
	if (!inode) {
		mp("a page without a inode");
		return;
	}

	mp("inode ino = %d, sb = %s, size = %d\n",
		inode->i_ino,
		inode->i_sb->s_id,
		i_size_read(inode);
		);

	mp("inode->end_index = %d, page->index = %d\n", 
		i_size_read(inode) > PAGE_CACHE_SHIFT, page->index);

	if (!page_has_buffers(page))
		mp("we have buffer caches");
}

void addr_resolv()
{
	char *tmp = NULL, name[64];

	if (lookup_symbol_name(__builtin_return_address(0), name) >= 0)
		tmp = name;

	if (tmp)
		printk("%s(), this is called from %s()\n", __func__, tmp);
}

#define addr_resolv() \
{ \
	char *tmp = NULL, name[64]; \
	if (lookup_symbol_name(__builtin_return_address(0), name) >= 0)\
		tmp = name; \
	if (tmp) \
		printk("%s(): called by %s()\n", __func__, tmp); \
}

/* it will surely cause memory leak, so be careful to use */
static inline char *addr_hex2str(void *addr)
{
	char *name;
	
	extern void *__kmalloc(size_t size, gfp_t flags);
	extern int lookup_symbol_name(unsigned long addr, char *symname);

	name = __kmalloc(64, 0x70);
	/* 0x70 == GFP_KERNEL */
	lookup_symbol_name((unsigned long)addr, name);

	return name;
}



#define mpp(f, x...) \
        printk(KERN_ERR "P[%s][%s()]: " f "\n", current->comm,  __func__,##x)
#define mppi() \
        printk(KERN_ERR "P[%s][%s()]: enter\n", current->comm, __func__)
#define mppo() \
        printk(KERN_ERR "P[%s][%s()]: exit\n", current->comm, __func__)








#define IOAPIC_ADDR_BASE	0xFEC00000
#define ioapic_read(addr)	*(volatile unsigned int *)(0xFEC00000 + addr)
#define ioapic_write(addr, val)  \
	do  { \
	*(volatile unsigned int *)(0xFEC00000 + addr) = val; \
	} while (0) 

void ioapic_dump()
{
	ioapic_write(0, 0);
	printk("IOAPIC_ID: 0x%x\n", ioapic_read(0x10));

	ioapic_write(0, 1);
	printk("IOAPIC_VER: 0x%x\n", ioapic_read(0x10));

	for (i = 0; i < 24; i++) {
		ioapic_write(i*2+0x10);
		printk("[%d]: 0x%08x\n", i, ioapic_read(0x10)); 
	}
}








===========================================
	Shell:
===========================================

1. sample for the basic setup

USERS=` cat /etc/passwd | awk -F: '{print $1}'`
MACHINES=`cat ${PORTCONFIG} | awk '{print $1}'`

tmpn()
{
	echo "$0(): $1$1$1$1$1$1$1"
}

tmpn 1

// read data from a config file to get tty and baud rate
TTY=$(cat ${PORTCONFIG} | grep $MACHINE | awk '{ print $2 }')
BAUDRATE=$(cat ${PORTCONFIG} | grep $MACHINE | awk '{ print $3 }')


