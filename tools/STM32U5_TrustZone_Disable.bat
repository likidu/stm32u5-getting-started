::******************************************************************************
::* @file    STM32U5_TrustZone_Disable.bat
::* @author  MCD Application Team
::* @brief   Disables TrustZone on STM32U5
::******************************************************************************
:: * Copyright (c) 2021 STMicroelectronics.
::
:: * All rights reserved.
::
:: * This software is licensed under terms that can be found in the LICENSE file
::
:: * in the root directory of this software component.
::
:: * If no LICENSE file comes with this software, it is provided AS-IS.
::******************************************************************************

@echo off
setlocal
:PROMPT
SET /P AREYOUSURE=Disabling trustzone requires that a trustzone enabled project is running on the board. Proceed (Y/[N])?
IF /I "%AREYOUSURE%" NEQ "Y" GOTO END

::Find Cube Programmer CLI Path
if exist "C:\Program Files (x86)\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe" (
    set ST_PROGRAMMER_PATH="C:\Program Files (x86)\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe" 
) else (
    set ST_PROGRAMMER_PATH="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
)

%ST_PROGRAMMER_PATH% -c port=SWD mode=HotPlug -ob nSWBOOT0=0 nBOOT0=0
%ST_PROGRAMMER_PATH% -c port=SWD mode=HotPlug -ob RDP=0xDC
%ST_PROGRAMMER_PATH% -c port=SWD mode=HotPlug -ob RDP=0xAA TZEN=0
%ST_PROGRAMMER_PATH% -c port=SWD mode=HotPlug -ob nSWBOOT0=1 nBOOT0=1

:END
endlocal
