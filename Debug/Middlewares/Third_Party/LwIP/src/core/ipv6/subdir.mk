################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/LwIP/src/core/ipv6/dhcp6.c \
../Middlewares/Third_Party/LwIP/src/core/ipv6/ethip6.c \
../Middlewares/Third_Party/LwIP/src/core/ipv6/icmp6.c \
../Middlewares/Third_Party/LwIP/src/core/ipv6/inet6.c \
../Middlewares/Third_Party/LwIP/src/core/ipv6/ip6.c \
../Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_addr.c \
../Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_frag.c \
../Middlewares/Third_Party/LwIP/src/core/ipv6/mld6.c \
../Middlewares/Third_Party/LwIP/src/core/ipv6/nd6.c 

OBJS += \
./Middlewares/Third_Party/LwIP/src/core/ipv6/dhcp6.o \
./Middlewares/Third_Party/LwIP/src/core/ipv6/ethip6.o \
./Middlewares/Third_Party/LwIP/src/core/ipv6/icmp6.o \
./Middlewares/Third_Party/LwIP/src/core/ipv6/inet6.o \
./Middlewares/Third_Party/LwIP/src/core/ipv6/ip6.o \
./Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_addr.o \
./Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_frag.o \
./Middlewares/Third_Party/LwIP/src/core/ipv6/mld6.o \
./Middlewares/Third_Party/LwIP/src/core/ipv6/nd6.o 

C_DEPS += \
./Middlewares/Third_Party/LwIP/src/core/ipv6/dhcp6.d \
./Middlewares/Third_Party/LwIP/src/core/ipv6/ethip6.d \
./Middlewares/Third_Party/LwIP/src/core/ipv6/icmp6.d \
./Middlewares/Third_Party/LwIP/src/core/ipv6/inet6.d \
./Middlewares/Third_Party/LwIP/src/core/ipv6/ip6.d \
./Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_addr.d \
./Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_frag.d \
./Middlewares/Third_Party/LwIP/src/core/ipv6/mld6.d \
./Middlewares/Third_Party/LwIP/src/core/ipv6/nd6.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/LwIP/src/core/ipv6/%.o: ../Middlewares/Third_Party/LwIP/src/core/ipv6/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -D__weak=__attribute__((weak)) -D__packed=__attribute__((__packed__)) -DUSE_HAL_DRIVER -DSTM32F207xx -I"E:/Projects/StepperTest2/StepperTest2/Inc" -I"E:/Projects/StepperTest2/StepperTest2/Middlewares/Third_Party/LwIP/src/include" -I"E:/Projects/StepperTest2/StepperTest2/Middlewares/Third_Party/LwIP/system" -I"E:/Projects/StepperTest2/StepperTest2/Drivers/STM32F2xx_HAL_Driver/Inc" -I"E:/Projects/StepperTest2/StepperTest2/Drivers/STM32F2xx_HAL_Driver/Inc/Legacy" -I"E:/Projects/StepperTest2/StepperTest2/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3" -I"E:/Projects/StepperTest2/StepperTest2/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"E:/Projects/StepperTest2/StepperTest2/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc" -I"E:/Projects/StepperTest2/StepperTest2/Drivers/CMSIS/Device/ST/STM32F2xx/Include" -I"E:/Projects/StepperTest2/StepperTest2/Middlewares/Third_Party/FreeRTOS/Source/include" -I"E:/Projects/StepperTest2/StepperTest2/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"E:/Projects/StepperTest2/StepperTest2/Middlewares/Third_Party/LwIP/src/include/lwip" -I"E:/Projects/StepperTest2/StepperTest2/Middlewares/Third_Party/LwIP/src/include/lwip/apps" -I"E:/Projects/StepperTest2/StepperTest2/Middlewares/Third_Party/LwIP/src/include/lwip/priv" -I"E:/Projects/StepperTest2/StepperTest2/Middlewares/Third_Party/LwIP/src/include/lwip/prot" -I"E:/Projects/StepperTest2/StepperTest2/Middlewares/Third_Party/LwIP/src/include/netif" -I"E:/Projects/StepperTest2/StepperTest2/Middlewares/Third_Party/LwIP/src/include/netif/ppp" -I"E:/Projects/StepperTest2/StepperTest2/Middlewares/Third_Party/LwIP/src/include/netif/ppp/polarssl" -I"E:/Projects/StepperTest2/StepperTest2/Middlewares/Third_Party/LwIP/src/include/posix" -I"E:/Projects/StepperTest2/StepperTest2/Middlewares/Third_Party/LwIP/src/include/posix/sys" -I"E:/Projects/StepperTest2/StepperTest2/Middlewares/Third_Party/LwIP/system/arch" -I"E:/Projects/StepperTest2/StepperTest2/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


