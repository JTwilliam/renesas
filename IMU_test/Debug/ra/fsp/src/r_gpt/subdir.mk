################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra/fsp/src/r_gpt/r_gpt.c 

C_DEPS += \
./ra/fsp/src/r_gpt/r_gpt.d 

OBJS += \
./ra/fsp/src/r_gpt/r_gpt.o 

SREC += \
IIC_templates.srec 

MAP += \
IIC_templates.map 


# Each subdirectory must supply rules for building sources it contributes
ra/fsp/src/r_gpt/%.o: ../ra/fsp/src/r_gpt/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CM33 -D_RA_ORDINAL=1 -I"D:/F570_drone_transplant_demo/renesas/IMU_test/ra_gen" -I"." -I"D:/F570_drone_transplant_demo/renesas/IMU_test/ra_cfg/fsp_cfg/bsp" -I"D:/F570_drone_transplant_demo/renesas/IMU_test/ra_cfg/fsp_cfg" -I"D:/F570_drone_transplant_demo/renesas/IMU_test/src" -I"D:/F570_drone_transplant_demo/renesas/IMU_test/ra/fsp/inc" -I"D:/F570_drone_transplant_demo/renesas/IMU_test/ra/fsp/inc/api" -I"D:/F570_drone_transplant_demo/renesas/IMU_test/ra/fsp/inc/instances" -I"D:/F570_drone_transplant_demo/renesas/IMU_test/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c99 -Wno-stringop-overflow -Wno-format-truncation --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

