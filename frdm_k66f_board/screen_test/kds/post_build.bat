REM Script to be called after a build

@echo off

echo "post build script: %1\%2"

del %1\..\screen_test.bin
arm-none-eabi-objcopy.exe -O binary %1\%2.elf %1\..\screen_test.bin

echo "post build script complete"
