#include "button.h" // "" 현재 나의 directory에서 include

#define FND_DATA_PORT GPIOB // data port
#define FND_COM_PORT  GPIOC   // data port

#define FND_a  GPIO_PIN_1
#define FND_b  GPIO_PIN_2
#define FND_c  GPIO_PIN_3
#define FND_d  GPIO_PIN_4
#define FND_e  GPIO_PIN_5
#define FND_f  GPIO_PIN_6
#define FND_g  GPIO_PIN_10
#define FND_p  GPIO_PIN_11

#define FND_d1000 GPIO_PIN_8
#define FND_d100  GPIO_PIN_9
#define FND_d10   GPIO_PIN_10
#define FND_d1    GPIO_PIN_11

void fnd4digit_main(void);
void FND4digit_off(void);
void FND4digit_on(void);







