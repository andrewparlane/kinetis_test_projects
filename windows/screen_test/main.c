/*

Copyright (c) Future Technology Devices International 2014

THIS SOFTWARE IS PROVIDED BY FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

FTDI DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON FTDI PARTS.

FTDI DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED.

IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS THE
RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL
RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.

Author : FTDI

Revision History:
0.1 - date 2013.04.24 - Initial version
0.2 - date 2013.08.19 - few minor edits
0.3 - date 2015.04.29 - New version for multi-emulation mode

*/

#include "ft81x_config.h"
#include "ft81x.h"

#include "ft81x/platforms/platform.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>

// forward declaration of tests
void init_lib_and_run_tests();

// called by the emulator
// functions as our main loop
void setup()
{
    init_lib_and_run_tests();
}

// called by the emulator,
// not sure what this is for
void loop()
{
    // wait for a key press then end
    system("PAUSE");
    FT8XXEMU_stop();
}

FT8XXEMU_EmulatorMode Ft_GpuEmu_Mode(){
#if defined(FT81X_GPU_TYPE_FT810)
		return FT8XXEMU_EmulatorFT810;
#elif defined(FT81X_GPU_TYPE_FT811)
		return FT8XXEMU_EmulatorFT811;
#elif defined(FT81X_GPU_TYPE_FT812)
		return FT8XXEMU_EmulatorFT812;
#elif defined(FT81X_GPU_TYPE_FT813)
		return FT8XXEMU_EmulatorFT813;
#else
#error Unsupported chip
#endif
}

void main(void)
{
	FT8XXEMU_EmulatorParameters params;

	FT8XXEMU_defaults(FT8XXEMU_VERSION_API, &params, Ft_GpuEmu_Mode());

	params.Flags &= (~FT8XXEMU_EmulatorEnableDynamicDegrade & ~FT8XXEMU_EmulatorEnableRegPwmDutyEmulation);
	params.Setup = setup;
	params.Loop = loop;

    // start the emulator running
	FT8XXEMU_run(FT8XXEMU_VERSION_API, &params);
	return;
}
