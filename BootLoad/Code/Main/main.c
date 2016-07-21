#include "chip.h"
#include "UpdateInfo.h"
#include "iap.h"

int fputc(int ch, FILE *fp) {
    
    uart0_send_byte(ch);

    return ch;
}

static void LogInit(void) {
    
    syscon_ahb_enable(SYSCON_CLK_IOCON);
    LPC_IOCON->PIO1_6 |= (IOCON_FUNC1 | IOCON_MODE_PULLUP);
    LPC_IOCON->PIO1_7 |= (IOCON_FUNC1 | IOCON_MODE_PULLUP);
    
    LOGInit();
    LOG("\r\nthe bootload start up[%s %s]\r\n", __DATE__, __TIME__);
    
}

int main(void) {
    
    LogInit();
    UpdateInfoInit();
    UpdateInfoAppCodeUpdate();

}


