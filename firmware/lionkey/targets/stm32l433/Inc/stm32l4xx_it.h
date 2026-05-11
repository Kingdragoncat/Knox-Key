#ifndef KNOXKEY_stm32l4xx_IT_H
#define KNOXKEY_stm32l4xx_IT_H

void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void EXTI0_1_IRQHandler(void);
void USB_IRQHandler(void);

#endif /* KNOXKEY_stm32l4xx_IT_H */
