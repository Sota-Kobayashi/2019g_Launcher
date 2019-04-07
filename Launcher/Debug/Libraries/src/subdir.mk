################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
D:/Git/Libraries/src/can.cpp \
D:/Git/Libraries/src/motor_driver.cpp 

OBJS += \
./Libraries/src/can.o \
./Libraries/src/motor_driver.o 

CPP_DEPS += \
./Libraries/src/can.d \
./Libraries/src/motor_driver.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/src/can.o: D:/Git/Libraries/src/can.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU G++ Compiler'
	@echo $(PWD)
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c++2a -DSTM32 -DSTM32F4 -DSTM32F405RGTx -DDEBUG -DSTM32F40XX -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -D__GXX_EXPERIMENTAL_CXX17__ -I"D:/Git/2019g/2019g_Launcher/Launcher/StdPeriph_Driver/inc" -I"D:/Git/2019g/2019g_Launcher/Launcher/inc" -I"D:/Git/2019g/2019g_Launcher/Launcher/CMSIS/device" -I"D:/Git/2019g/2019g_Launcher/Launcher/CMSIS/core" -I"D:/Git/2019g/2019g_Launcher/Launcher/inc/myHeaders" -I"D:/Git/Libraries/inc" -O0 -g3 -Wall -Wconversion -fmessage-length=0 -ffunction-sections -c -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Libraries/src/motor_driver.o: D:/Git/Libraries/src/motor_driver.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU G++ Compiler'
	@echo $(PWD)
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c++2a -DSTM32 -DSTM32F4 -DSTM32F405RGTx -DDEBUG -DSTM32F40XX -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -D__GXX_EXPERIMENTAL_CXX17__ -I"D:/Git/2019g/2019g_Launcher/Launcher/StdPeriph_Driver/inc" -I"D:/Git/2019g/2019g_Launcher/Launcher/inc" -I"D:/Git/2019g/2019g_Launcher/Launcher/CMSIS/device" -I"D:/Git/2019g/2019g_Launcher/Launcher/CMSIS/core" -I"D:/Git/2019g/2019g_Launcher/Launcher/inc/myHeaders" -I"D:/Git/Libraries/inc" -O0 -g3 -Wall -Wconversion -fmessage-length=0 -ffunction-sections -c -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


