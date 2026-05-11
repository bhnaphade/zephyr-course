#include <zephyr/init.h>
#include <zephyr/kernel.h>

static int board_my_board_init(void) {
    printk("Board Initialized\n");
    return 0;
}

SYS_INIT(board_my_board_init, APPLICATION , 0);