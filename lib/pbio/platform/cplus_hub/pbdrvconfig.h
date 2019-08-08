// SPDX-License-Identifier: MIT
// Copyright (c) 2019 David Lechner

#ifndef _PBDRVCONFIG_H_
#define _PBDRVCONFIG_H_

#define PBDRV_CONFIG_ADC                            (1)
#define PBDRV_CONFIG_ADC_STM32_HAL                  (1)
#define PBDRV_CONFIG_ADC_STM32_HAL_ADC_INSTANCE     ADC1
#define PBDRV_CONFIG_ADC_STM32_HAL_ADC_NUM_CHANNELS 3
#define PBDRV_CONFIG_ADC_STM32_HAL_DMA_INSTANCE     DMA1_Channel1
#define PBDRV_CONFIG_ADC_STM32_HAL_DMA_REQUEST      DMA_REQUEST_0
#define PBDRV_CONFIG_ADC_STM32_HAL_DMA_IRQ          DMA1_Channel1_IRQn
#define PBDRV_CONFIG_ADC_STM32_HAL_TIMER_INSTANCE   TIM2
#define PBDRV_CONFIG_ADC_STM32_HAL_TIMER_TRIGGER    ADC_EXTERNALTRIG_T2_TRGO

#define PBDRV_CONFIG_BATTERY                        (1)
#define PBDRV_CONFIG_BATTERY_ADC                    (1)
#define PBDRV_CONFIG_BATTERY_ADC_VOLTAGE_CH         0
#define PBDRV_CONFIG_BATTERY_ADC_VOLTAGE_RAW_MAX    4095
#define PBDRV_CONFIG_BATTERY_ADC_VOLTAGE_SCALED_MAX 9615
#define PBDRV_CONFIG_BATTERY_ADC_CURRENT_CH         1
#define PBDRV_CONFIG_BATTERY_ADC_CURRENT_RAW_MAX    4095
#define PBDRV_CONFIG_BATTERY_ADC_CURRENT_SCALED_MAX 4175

#define PBDRV_CONFIG_BUTTON                         (1)
#define PBDRV_CONFIG_BUTTON_GPIO                    (1)
#define PBDRV_CONFIG_BUTTON_GPIO_NUM_BUTTON         (1)

#define PBDRV_CONFIG_GPIO                           (1)
#define PBDRV_CONFIG_GPIO_STM32L4                   (1)

#define PBDRV_CONFIG_IOPORT                         (0)
#define PBDRV_CONFIG_IOPORT_LPF2                    (1)
#define PBDRV_CONFIG_IOPORT_LPF2_NUM_PORTS          (3)
// #define PBDRV_CONFIG_IOPORT_LPF2_NUM_PORTS          (4)
#define PBDRV_CONFIG_IOPORT_LPF2_FIRST_PORT         PBIO_PORT_A
#define PBDRV_CONFIG_IOPORT_LPF2_LAST_PORT          PBIO_PORT_C
// #define PBDRV_CONFIG_IOPORT_LPF2_LAST_PORT          PBIO_PORT_D

#define PBDRV_CONFIG_UART                           (0)
#define PBDRV_CONFIG_UART_STM32_HAL                 (0)
#define PBDRV_CONFIG_UART_STM32_HAL_NUM_UART        (3)
// #define PBDRV_CONFIG_UART_STM32_HAL_NUM_UART        (4)

#define PBDRV_CONFIG_BLUETOOTH      (0)
#define PBDRV_CONFIG_LIGHT          (1)
#define PBDRV_CONFIG_MOTOR          (0)

#define PBDRV_CONFIG_HAS_PORT_A (1)
#define PBDRV_CONFIG_HAS_PORT_B (1)
#define PBDRV_CONFIG_HAS_PORT_C (1)
// #define PBDRV_CONFIG_HAS_PORT_D (1)

#define PBDRV_CONFIG_FIRST_IO_PORT          PBIO_PORT_A
#define PBDRV_CONFIG_LAST_IO_PORT           PBIO_PORT_C
// #define PBDRV_CONFIG_LAST_IO_PORT           PBIO_PORT_D
#define PBDRV_CONFIG_NUM_IO_PORT            (3)
// #define PBDRV_CONFIG_NUM_IO_PORT            (4)

#define PBDRV_CONFIG_FIRST_MOTOR_PORT       PBIO_PORT_A
#define PBDRV_CONFIG_LAST_MOTOR_PORT        PBIO_PORT_C
// #define PBDRV_CONFIG_LAST_MOTOR_PORT        PBIO_PORT_D
#define PBDRV_CONFIG_NUM_MOTOR_CONTROLLER   (3)
// #define PBDRV_CONFIG_NUM_MOTOR_CONTROLLER   (4)

#define PBDRV_CONFIG_SYS_CLOCK_RATE 80000000

#endif // _PBDRVCONFIG_H_
