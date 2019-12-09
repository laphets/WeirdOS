/* kernel.c - the C part of the kernel
 * vim:ts=4 noexpandtab
 */

#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "task.h"
#include "i8259.h"
#include "idt.h"
#include "rtc.h"
#include "keyboard.h"
#include "paging.h"
#include "fs.h"
#include "syscall.h"
#include "debug.h"
#include "tests.h"
#include "pit.h"
#include "pci.h"
#include "rtl8139.h"
#include "ethernet.h"
#include "arp.h"
#include "ip.h"
#include "udp.h"
#include "tcp.h"
#include "dns.h"
#include "socket.h"
#include "http.h"
#include "vga.h"
#include "mouse.h"

#define RUN_TESTS 0
#define ENABLE_NETWORK 0
#define ENABLE_GUI 0

/* Macros. */
/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags, bit)   ((flags) & (1 << (bit)))

/* Check if MAGIC is valid and print the Multiboot information structure
   pointed by ADDR. */
void entry(unsigned long magic, unsigned long addr) {
    init_ed = 0;
    gui_enabled = 0;
    printf("Hello world");
    multiboot_info_t *mbi;
    uint32_t fs_start_addr, fs_end_addr;

    /* Clear the screen. */
    clear();

    /* Am I booted by a Multiboot-compliant boot loader? */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        printf("Invalid magic number: 0x%#x\n", (unsigned)magic);
        return;
    }

    /* Set MBI to the address of the Multiboot information structure. */
    mbi = (multiboot_info_t *) addr;

    /* Print out the flags. */
    printf("flags = 0x%#x\n", (unsigned)mbi->flags);

    /* Are mem_* valid? */
    if (CHECK_FLAG(mbi->flags, 0))
        printf("mem_lower = %uKB, mem_upper = %uKB\n", (unsigned)mbi->mem_lower, (unsigned)mbi->mem_upper);

    /* Is boot_device valid? */
    if (CHECK_FLAG(mbi->flags, 1))
        printf("boot_device = 0x%#x\n", (unsigned)mbi->boot_device);

    /* Is the command line passed? */
    if (CHECK_FLAG(mbi->flags, 2))
        printf("cmdline = %s\n", (char *)mbi->cmdline);

    if (CHECK_FLAG(mbi->flags, 3)) {
        int mod_count = 0;
        int i;
        module_t* mod = (module_t*)mbi->mods_addr;
        while (mod_count < mbi->mods_count) {
            /**
             * Init file system addr
             */
            if(mod_count == 0) {
                fs_start_addr = mod->mod_start;
                fs_end_addr = mod->mod_end;
            }
            printf("Module %d loaded at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_start);
            printf("Module %d ends at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_end);
            printf("First few bytes of module:\n");
            for (i = 0; i < 16; i++) {
                printf("0x%x ", *((char*)(mod->mod_start+i)));
            }
            printf("\n");
            mod_count++;
            mod++;
        }
    }
    /* Bits 4 and 5 are mutually exclusive! */
    if (CHECK_FLAG(mbi->flags, 4) && CHECK_FLAG(mbi->flags, 5)) {
        printf("Both bits 4 and 5 are set.\n");
        return;
    }

    /* Is the section header table of ELF valid? */
    if (CHECK_FLAG(mbi->flags, 5)) {
        elf_section_header_table_t *elf_sec = &(mbi->elf_sec);
        printf("elf_sec: num = %u, size = 0x%#x, addr = 0x%#x, shndx = 0x%#x\n",
                (unsigned)elf_sec->num, (unsigned)elf_sec->size,
                (unsigned)elf_sec->addr, (unsigned)elf_sec->shndx);
    }

    /* Are mmap_* valid? */
    if (CHECK_FLAG(mbi->flags, 6)) {
//        memory_map_t *mmap;
//        printf("mmap_addr = 0x%#x, mmap_length = 0x%x\n",
//                (unsigned)mbi->mmap_addr, (unsigned)mbi->mmap_length);
//        for (mmap = (memory_map_t *)mbi->mmap_addr;
//                (unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length;
//                mmap = (memory_map_t *)((unsigned long)mmap + mmap->size + sizeof (mmap->size)))
//            printf("    size = 0x%x, base_addr = 0x%#x%#x\n    type = 0x%x,  length    = 0x%#x%#x\n",
//                    (unsigned)mmap->size,
//                    (unsigned)mmap->base_addr_high,
//                    (unsigned)mmap->base_addr_low,
//                    (unsigned)mmap->type,
//                    (unsigned)mmap->length_high,
//                    (unsigned)mmap->length_low);
    }

    /* Construct an LDT entry in the GDT */
    {
        seg_desc_t the_ldt_desc;
        the_ldt_desc.granularity = 0x0;
        the_ldt_desc.opsize      = 0x1;
        the_ldt_desc.reserved    = 0x0;
        the_ldt_desc.avail       = 0x0;
        the_ldt_desc.present     = 0x1;
        the_ldt_desc.dpl         = 0x0;
        the_ldt_desc.sys         = 0x0;
        the_ldt_desc.type        = 0x2;

        SET_LDT_PARAMS(the_ldt_desc, &ldt, ldt_size);
        ldt_desc_ptr = the_ldt_desc;
        lldt(KERNEL_LDT);
    }

    /* Construct a TSS entry in the GDT */
    {
        seg_desc_t the_tss_desc;
        the_tss_desc.granularity   = 0x0;
        the_tss_desc.opsize        = 0x0;
        the_tss_desc.reserved      = 0x0;
        the_tss_desc.avail         = 0x0;
        the_tss_desc.seg_lim_19_16 = TSS_SIZE & 0x000F0000;
        the_tss_desc.present       = 0x1;
        the_tss_desc.dpl           = 0x0;
        the_tss_desc.sys           = 0x0;
        the_tss_desc.type          = 0x9;
        the_tss_desc.seg_lim_15_00 = TSS_SIZE & 0x0000FFFF;

        SET_TSS_PARAMS(the_tss_desc, &tss, tss_size);

        tss_desc_ptr = the_tss_desc;

        tss.ldt_segment_selector = KERNEL_LDT;
        tss.ss0 = KERNEL_DS;
        tss.esp0 = 0x800000;
        ltr(KERNEL_TSS);
    }

    /* Init IDT */
    init_idt();

    /* Init the PIC */
    i8259_init();
    

    /* Initialize devices, memory, filesystem, enable device interrupts on the
     * PIC, any other initialization stuff... */

    /* Init paging */
    init_paging(((unsigned)mbi->mem_upper) << 10);

#if (ENABLE_GUI == 1)
    init_vga();
#endif

    /* Init filesystem */
    init_fs(fs_start_addr, fs_end_addr);

    /* Set up device interrupt */
    init_rtc();
    init_keyboard();
    init_pit();
    init_mouse();

    /* init vfs operators */
    init_file_operator();
    init_directory_operator();
    init_rtc_operator();
    init_terminal_operator();
    init_syscall_list();

    /* Init task */
    init_tasks();
    init_scheduler();

    /* Init terminals */
    init_terminal();

    /* Init pci */
    init_pci();

#if (ENABLE_NETWORK == 1)
    /* Init rtl8139 */
    init_rtl8139();
    init_arp();
    init_ip();  /* Init ip layer */
    init_socket();
    init_dns();
#endif

    /* Enable interrupts */
    /* Do not enable the following until after you have set up your
     * IDT correctly otherwise QEMU will triple fault and simple close
     * without showing you any output */
    printf("Enabling Interrupts\n");
    init_ed = 1;
    sti();

//    uint8_t* packet = kmalloc(200);
//    int d = 0;
//    for(d = 0; d < 200; d++) {
//        packet[d] = d;
//    }
//    rtl8139_send(packet, 200);
//    rtl8139_send(packet, 200);
//    http_res_t response = http_request((uint8_t*)"lumetta.web.engr.illinois.edu");
////    http_res_t response = http_request("mdbailey.ece.illinois.edu");
//    kprintf("%s\n, length: %d\n", response.data, response.length);

//    ip_wrapper_t res = dns_query("blog.laphets.com");
//    print_ip(res.ip_addr);
//
//    res = dns_query("lumetta.web.engr.illinois.edu");
//    print_ip(res.ip_addr);

//    dns_send("lumetta.web.engr.illinois.edu");
//    dns_send("google.com");
//    dns_send("blog.laphets.com");
//    dns_send("www.baidu.com");

//    int32_t socket = socket_open();
//    uint8_t target_ip[4] = {18, 220, 149, 166};
//    char http_request[] = "GET / HTTP/1.1\r\nHost: lumetta.web.engr.illinois.edu\r\n\r\n";
//    uint32_t res_length = 0;
//    uint8_t* res_data = socket_http_send(socket, target_ip, 80, (uint8_t*)http_request, strlen(http_request), &res_length);
//    kprintf("RES DATA, size: %d!!!\n", res_length);
//    kprintf("%s\n", res_data);
//    kprintf("%s\n", res_data + res_length - 50);
//    socket_close(socket);

//    uint8_t dhcp_ip[4] = {10, 0, 2, 2};
//    ip_send(dhcp_ip, IP_PROTOCOL_UDP, packet, 200);

//    uint8_t dns_ip[4] = {10, 0, 2, 3};
//    uint8_t dns_body[12] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
//    udp_send(50032, 53, dns_ip, dns_body, 12);
//    arp_request(dhcp_ip);

//    uint8_t host_ip[4] = {10, 0, 2, 2};
//    tcp_send(50032, 22, host_ip, 23333, 0, TCP_FLAG_SYN, NULL, 0);

//    int i = 0;
//    for(i = 0 ; i < 10; i++) {
//        kprintf("rand: %d\n", rand_port());
//    }
//

//    tcp_test();

//    print_arp_table();
//    uint32_t big_block = kmalloc(0x800000);
//    memset(big_block, 0, 0x800000);
//    uint32_t big_block1 = kmalloc(0x800000);
//    memset(big_block1, 0, 0x800000);
//    kprintf("malloc done, big_block: 0x%x, big_block1: 0x%x\n", big_block, big_block1);
//    uint32_t b1 = kmalloc_a(PAGE_TABLE_SIZE * sizeof(page_table_entry_t), 1);
//    uint32_t b2 = kmalloc_a(PAGE_TABLE_SIZE * sizeof(page_table_entry_t), 1);

//    kprintf("b1: 0x%x  b2: 0x%x\n", b1, b2);

    /* *(uint32_t*)(0xDEADBEEF) = 33; */


#if (RUN_TESTS == 1)
    /* Run tests */
    launch_tests();
#endif
    /* Execute the first program ("shell") ... */
    /* int32_t ret = execute((const uint8_t*)"shell"); */
    /* printf("execute_ret: %d\n", ret); */
    /* Launch the terminal */
    launch_terminal();

    /* Spin (nicely, so we don't chew up cycles) */
    asm volatile (".1: hlt; jmp .1;");
}
