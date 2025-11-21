#include <odrive_main.h>

void MechanicalBrake::engage() {
    /*通过硬件 IO 将电路切换到动能回收充电电路*/
	if (odrv.config_.gpio_modes[config_.gpio_num] == ODriveIntf::GPIO_MODE_MECH_BRAKE){
		get_gpio(config_.gpio_num).write(config_.is_active_low ? 0 : 1);
	}
}

void MechanicalBrake::release() {
    /*通过硬件 IO 将电路切出动能回收充电电路*/
	if (odrv.config_.gpio_modes[config_.gpio_num] == ODriveIntf::GPIO_MODE_MECH_BRAKE){
		get_gpio(config_.gpio_num).write(config_.is_active_low ? 1 : 0);
	}
}
