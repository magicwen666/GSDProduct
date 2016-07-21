#include "sys.h"
#include "board_light.h"

// OPEN:¶Ï¿ª, CLOSE:±ÕºÏ
#define RL1_OPEN()     gpio_set_bitval(LPC_GPIO0, 3, GPIO_L)
#define RL1_CLOSE()    gpio_set_bitval(LPC_GPIO0, 3, GPIO_H)
#define RL2_OPEN()     gpio_set_bitval(LPC_GPIO1, 8, GPIO_L)
#define RL2_CLOSE()    gpio_set_bitval(LPC_GPIO1, 8, GPIO_H)
#define RL3_OPEN()     gpio_set_bitval(LPC_GPIO2, 9, GPIO_L)
#define RL3_CLOSE()    gpio_set_bitval(LPC_GPIO2, 9, GPIO_H)
#define RL4_OPEN()     gpio_set_bitval(LPC_GPIO2, 2, GPIO_L)
#define RL4_CLOSE()    gpio_set_bitval(LPC_GPIO2, 2, GPIO_H)

light_t _light_array[4];

void board_light_devinit(light_t *light_p, uint8_t light_name)
{
    light_p->light_name = light_name;
    light_p->light_state = LIGHT_STATE_CLOSE;
}

// ºÁÃë¶¨Ê±Æ÷×¢²á
void handler_light(void)
{

}

void board_light_init(void)
{
    syscon_ahb_enable(SYSCON_CLK_IOCON);
    // 14 RL1
    LPC_IOCON->PIO0_3 = (IOCON_FUNC0|IOCON_MODE_PULLUP);
    // 9  RL2
    LPC_IOCON->PIO1_8 = (IOCON_FUNC0|IOCON_MODE_PULLUP);
    // 24 RL3
    LPC_IOCON->PIO2_9 = (IOCON_FUNC0|IOCON_MODE_PULLUP);
    // 25 RL4
    LPC_IOCON->PIO2_2 = (IOCON_FUNC0|IOCON_MODE_PULLUP);
    
    syscon_ahb_enable(SYSCON_CLK_GPIO);    
    gpio_set_dir(LPC_GPIO0, 3, GPIO_OUT);
    gpio_set_dir(LPC_GPIO1, 8, GPIO_OUT);
    gpio_set_dir(LPC_GPIO2, 9, GPIO_OUT);
    gpio_set_dir(LPC_GPIO2, 2, GPIO_OUT);

    RL1_OPEN();
    RL2_OPEN();
    RL3_OPEN();
    RL4_OPEN();

    board_light_devinit(&_light_array[0], LIGHT1);
    board_light_devinit(&_light_array[1], LIGHT2);
    board_light_devinit(&_light_array[2], LIGHT3);
    board_light_devinit(&_light_array[3], LIGHT4);

}

light_t * board_light_register(uint8_t light_name)
{
    switch(light_name){
    case LIGHT1:
        return &_light_array[0];
    case LIGHT2:
        return &_light_array[1];
    case LIGHT3:
        return &_light_array[2];
    case LIGHT4:
        return &_light_array[3];
    default:
        return NULL;
    }
}

void board_light_open(light_t *light_p)
{
    if(light_p == NULL){
        return;
    }
    
    light_p->light_state = LIGHT_STATE_OPEN;
    switch(light_p->light_name){
    case LIGHT1:
        RL1_CLOSE();
        break;
    case LIGHT2:
        RL2_CLOSE();
        break;
    case LIGHT3:
        RL3_CLOSE();
        break;
    case LIGHT4:
        RL4_CLOSE();
        break;
    default:
        break;
    }
}

void board_light_close(light_t *light_p)
{
    if(light_p == NULL){
        return;
    }
    
    light_p->light_state = LIGHT_STATE_CLOSE;

    switch(light_p->light_name){
    case LIGHT1:
        RL1_OPEN();
        break;
    case LIGHT2:
        RL2_OPEN();
        break;
    case LIGHT3:
        RL3_OPEN();
        break;
    case LIGHT4:
        RL4_OPEN();
        break;
    default:
        break;
    }
}

void board_rl_delay_open(light_t *light_p, uint32_t delay_us)
{

}

void board_rl_delay_close(light_t *light_p, uint32_t delay_us)
{

}

/******************************* (C) COPYRIGHT 2015 GAKATO ******************************/
/****************************************END OF FILE*************************************/


