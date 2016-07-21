
#include "chip.h"
#include "et6290.h"

//#define RGB_STB2_NEW    0

/* Chip A */
#define A_DATA_UP()		    gpio_set_bitval(LPC_GPIO1, 2, GPIO_H);
#define A_DATA_DOWN()		gpio_set_bitval(LPC_GPIO1, 2, GPIO_L);
#define A_CLOCK_UP()		gpio_set_bitval(LPC_GPIO1, 1, GPIO_H);
#define A_CLOCK_DOWN()		gpio_set_bitval(LPC_GPIO1, 1, GPIO_L);
#define A_STB_UP()		    gpio_set_bitval(LPC_GPIO1, 4, GPIO_H);
#define A_STB_DOWN()		gpio_set_bitval(LPC_GPIO1, 4, GPIO_L);
#define A_SHUTDOWN_UP()
#define A_SHUTDOWN_DOWN()
/* Chip B */
#define B_DATA_UP()		    gpio_set_bitval(LPC_GPIO2, 0, GPIO_H);
#define B_DATA_DOWN()		gpio_set_bitval(LPC_GPIO2, 0, GPIO_L);
#define B_CLOCK_UP()		gpio_set_bitval(LPC_GPIO2, 6, GPIO_H);
#define B_CLOCK_DOWN()		gpio_set_bitval(LPC_GPIO2, 6, GPIO_L);
#ifdef RGB_STB2_NEW
#define B_STB_UP()		    gpio_set_bitval(LPC_GPIO2, 3, GPIO_H);
#define B_STB_DOWN()		gpio_set_bitval(LPC_GPIO2, 3, GPIO_L);
#else
#define B_STB_UP()		    gpio_set_bitval(LPC_GPIO0, 1, GPIO_H);
#define B_STB_DOWN()		gpio_set_bitval(LPC_GPIO0, 1, GPIO_L);
#endif
#define B_SHUTDOWN_UP()
#define B_SHUTDOWN_DOWN()

void et6290_delay(uint16_t count)
{
	while(count) --count;
}
/*
* 名称：
* 功能：
* 输入：
* 输出：
*/
void et6290a_sendbyte(uint8_t byte)
{
	uint8_t i = 0;
	for(i = 0; i < 8; i++){
		if(((byte<<i) & 0x80) >> 7){
			A_DATA_UP();
		}else{
			A_DATA_DOWN();
		}
		A_CLOCK_UP();
		et6290_delay(20);
		A_CLOCK_DOWN();
	}
}
/*
* 名称：
* 功能：
* 输入：
* 输出：
*/
void et6290b_sendbyte(uint8_t byte)
{
	uint8_t i = 0;
	for(i = 0; i < 8; i++){
		if(((byte<<i) & 0x80) >> 7){
			B_DATA_UP();
		}else{
			B_DATA_DOWN();
		}
		B_CLOCK_UP();
		et6290_delay(20);
		B_CLOCK_DOWN();
	}
}

/*
* 名称：et6290a_send_cmd
* 功能：Chip A 控制
* 输入：conf:配置字节;rpwm:R通道值;gpwm:G通道值;bpwm:B通道值;
* 输出：NONE
*/
void et6290a_send_cmd(const uint8_t conf, const uint8_t rpwm,
	const uint8_t gpwm, const uint8_t bpwm)
{
	//cli();
	/* configuration */
	et6290a_sendbyte(conf);
	/* out0 PWM */
	et6290a_sendbyte(rpwm);
	/* out1 PWM */
	et6290a_sendbyte(gpwm);
	/* out2 PWM */
	et6290a_sendbyte(bpwm);
	A_STB_UP();
	et6290_delay(20);
	A_STB_DOWN();
	//sei();
}

/*
* 名称：et6290b_send_cmd
* 功能：Chip B 控制
* 输入：conf:配置字节;rpwm:R通道值;gpwm:G通道值;bpwm:B通道值;
* 输出：NONE
*/
void et6290b_send_cmd(const uint8_t conf, const uint8_t rpwm,
	const uint8_t gpwm, const uint8_t bpwm)
{
	//cli();
	/* configuration */
	et6290b_sendbyte(conf);
	/* out0 PWM */
	et6290b_sendbyte(rpwm);
	/* out1 PWM */
	et6290b_sendbyte(gpwm);
	/* out2 PWM */
	et6290b_sendbyte(bpwm);
	B_STB_UP();
	et6290_delay(20);
	B_STB_DOWN();
	//sei();
}

void et6290_send_cmd(uint8_t chip,
    const uint8_t conf, const uint8_t rpwm,
	const uint8_t gpwm, const uint8_t bpwm)
{
    switch(chip){
    case ET6290A:
        et6290a_send_cmd(conf,rpwm,gpwm,bpwm);
        break;
    case ET6290B:
        et6290b_send_cmd(conf,rpwm,gpwm,bpwm);
        break;
    default:
        break;
    }
}

void et6290_init(void)
{
    syscon_ahb_enable(SYSCON_CLK_IOCON);
    // 39 STB1
    LPC_IOCON->PIO1_4 = (IOCON_FUNC0|IOCON_MODE_PULLUP);
    // 35 SDA1
    LPC_IOCON->R_PIO1_2 = (IOCON_FUNC1|IOCON_MODE_PULLUP);
    // 32 SCK1
    LPC_IOCON->R_PIO1_1 = (IOCON_FUNC1|IOCON_MODE_PULLUP);
	// 4 STB2
#ifdef RGB_STB2_NEW
    LPC_IOCON->PIO2_3 = (IOCON_FUNC0|IOCON_MODE_PULLUP);
#else
    LPC_IOCON->PIO0_1 = (IOCON_FUNC0|IOCON_MODE_PULLUP);
#endif
	// 2 SDA2
    LPC_IOCON->PIO2_0 = (IOCON_FUNC0|IOCON_MODE_PULLUP);
    // 1 SCK2 
    LPC_IOCON->PIO2_6 = (IOCON_FUNC0|IOCON_MODE_PULLUP);

    syscon_ahb_enable(SYSCON_CLK_GPIO);
    gpio_set_dir(LPC_GPIO1, 4, GPIO_OUT);
    gpio_set_dir(LPC_GPIO1, 2, GPIO_OUT);
    gpio_set_dir(LPC_GPIO1, 1, GPIO_OUT);
#ifdef RGB_STB2_NEW
    gpio_set_dir(LPC_GPIO2, 3, GPIO_OUT);
#else
	gpio_set_dir(LPC_GPIO0, 1, GPIO_OUT);
#endif
    gpio_set_dir(LPC_GPIO2, 0, GPIO_OUT);    
    gpio_set_dir(LPC_GPIO2, 6, GPIO_OUT);
}

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


