/*
 * led.h
 *
 *  Created on: 2019/03/12
 *      Author: User
 */

#ifndef MYHEADERS_LED_HPP_
#define MYHEADERS_LED_HPP_

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

enum class ledColor : uint16_t { Yellow = 0x8000, Orange = 0x0004, Red = 0x0010 };
enum class state : bool { ON = true, OFF = false };

template<ledColor useColor>
class LED
{
public:
	LED()
	{
		/*Enable to supply clock for GPIOs.*/
		if constexpr(useColor == ledColor::Yellow)
		{
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		}
		else
		{
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		}

		/*Configuration GPIO pins.*/
		constexpr GPIO_InitTypeDef GPIO_InitStruct
		{
			(uint32_t)useColor,
			GPIO_Mode_OUT,
			GPIO_Speed_100MHz,
			GPIO_OType_PP,
			GPIO_PuPd_DOWN
		};

		GPIO_Init_reference(use_gpioPort_(), GPIO_InitStruct);
	}



	void setNewState(state newState)
	{
		if(newState == state::ON)GPIO_SetBits(use_gpioPort_(), (uint16_t)useColor);
		else GPIO_ResetBits(use_gpioPort_(), (uint16_t)useColor);
	}


	inline void toggle(){ GPIO_ToggleBits(use_gpioPort_(), (uint16_t)useColor); }

protected:
	constexpr GPIO_TypeDef* use_gpioPort_(){ return useColor == ledColor::Yellow ? GPIOA : GPIOB; }

	void GPIO_Init_reference(GPIO_TypeDef* const GPIOx, const GPIO_InitTypeDef& GPIO_InitStruct)
	{
	  uint32_t pinpos = 0x00, pos = 0x00 , currentpin = 0x00;

	  /* Check the parameters */
	  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
	  assert_param(IS_GPIO_PIN(GPIO_InitStruct.GPIO_Pin));
	  assert_param(IS_GPIO_MODE(GPIO_InitStruct.GPIO_Mode));
	  assert_param(IS_GPIO_PUPD(GPIO_InitStruct.GPIO_PuPd));

	  /* ------------------------- Configure the port pins ---------------- */
	  /*-- GPIO Mode Configuration --*/
	  for (pinpos = 0x00; pinpos < 0x10; pinpos++)
	  {
	    pos = ((uint32_t)0x01) << pinpos;
	    /* Get the port pins position */
	    currentpin = (GPIO_InitStruct.GPIO_Pin) & pos;

	    if (currentpin == pos)
	    {
	      GPIOx->MODER  &= ~(GPIO_MODER_MODER0 << (pinpos * 2));
	      GPIOx->MODER |= (((uint32_t)GPIO_InitStruct.GPIO_Mode) << (pinpos * 2));

	      if ((GPIO_InitStruct.GPIO_Mode == GPIO_Mode_OUT) || (GPIO_InitStruct.GPIO_Mode == GPIO_Mode_AF))
	      {
	        /* Check Speed mode parameters */
	        assert_param(IS_GPIO_SPEED(GPIO_InitStruct.GPIO_Speed));

	        /* Speed mode configuration */
	        GPIOx->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (pinpos * 2));
	        GPIOx->OSPEEDR |= ((uint32_t)(GPIO_InitStruct.GPIO_Speed) << (pinpos * 2));

	        /* Check Output mode parameters */
	        assert_param(IS_GPIO_OTYPE(GPIO_InitStruct.GPIO_OType));

	        /* Output mode configuration*/
	        GPIOx->OTYPER  &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)pinpos)) ;
	        GPIOx->OTYPER |= (uint16_t)(((uint16_t)GPIO_InitStruct.GPIO_OType) << ((uint16_t)pinpos));
	      }

	      /* Pull-up Pull down resistor configuration*/
	      GPIOx->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << ((uint16_t)pinpos * 2));
	      GPIOx->PUPDR |= (((uint32_t)GPIO_InitStruct.GPIO_PuPd) << (pinpos * 2));
	    }
	  }
	}
};



#endif /* MYHEADERS_LED_HPP_ */
