################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/tire/tire_parse.cpp 

OBJS += \
./src/tire/tire_parse.o 

CPP_DEPS += \
./src/tire/tire_parse.d 


# Each subdirectory must supply rules for building sources it contributes
src/tire/%.o: ../src/tire/%.cpp
	@echo.exe 'Compile $(subst ../,,$<)'
	@riscv64-unknown-linux-musl-gcc -std=c++0x -I"C:\flythings\0916_ad003\F133CarSystem_hengcheng\f133_0911\F133CarSystem_hengcheng\src" -O3 -Wall -c -fmessage-length=0 -pipe -Wformat -Werror=format-security -fstack-protector -fno-caller-saves -fexceptions -fPIC -I"C:\flythings\FlyThingsIDE\bin\.dep\public\f133\easyui\2.3.0\include" -I"C:\flythings\FlyThingsIDE\bin\.dep\public\f133\log\1.0.0\include" -I"C:\flythings\FlyThingsIDE\bin\.dep\public\f133\zkhardware\1.1.0\include" -I"C:\flythings\FlyThingsIDE\bin\.dep\public\f133\zknet\1.0.0\include" -I"C:\flythings\FlyThingsIDE\bin\.dep\public\f133\ntp\1.0.1\include" -I"C:\flythings\FlyThingsIDE\bin\.dep\public\f133\base-utility\10.3.0\include" -I"..\src" -I"..\src\dependencies\include" -D__PLATFORM_F133__=1 -DUSE_ANDROID_LOG=1 -D_FORTIFY_SOURCE=1 -D_RELEASE=1 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"

