#include "../../hal/imu_hal.h"

// Core Basic has no IMU; the Gray's MPU6886 is left uninitialized (fixed
// desk orientation, no rotation).

void    imu_hal_init(void) {}
void    imu_hal_tick(void) {}
uint8_t imu_hal_rotation_quadrant(void) { return 0; }
