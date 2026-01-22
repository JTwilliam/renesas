################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/hal_entry.c \
../src/hal_warmstart.c 

C_DEPS += \
./src/hal_entry.d \
./src/hal_warmstart.d 

OBJS += \
./src/hal_entry.o \
./src/hal_warmstart.o 

SREC += \
IIC_templates.srec 

MAP += \
IIC_templates.map 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CM33 -D_RA_ORDINAL=1 -I"C:/Users/22456/Desktop/F570_drone_tansplant_demo1/IMU_test/ra_gen" -I"." -I"C:/Users/22456/Desktop/F570_drone_tansplant_demo1/IMU_test/ra_cfg/fsp_cfg/bsp" -I"C:/Users/22456/Desktop/F570_drone_tansplant_demo1/IMU_test/ra_cfg/fsp_cfg" -I"C:/Users/22456/Desktop/F570_drone_tansplant_demo1/IMU_test/src" -I"C:/Users/22456/Desktop/F570_drone_tansplant_demo1/IMU_test/ra/fsp/inc" -I"C:/Users/22456/Desktop/F570_drone_tansplant_demo1/IMU_test/ra/fsp/inc/api" -I"C:/Users/22456/Desktop/F570_drone_tansplant_demo1/IMU_test/ra/fsp/inc/instances" -I"C:/Users/22456/Desktop/F570_drone_tansplant_demo1/IMU_test/ra/arm/CMSIS_6/CMSIS/Core/Include" -std=c99 -Wno-stringop-overflow -Wno-format-truncation --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

