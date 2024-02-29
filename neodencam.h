#pragma once
#include <stdbool.h>
#include <stdint.h>

// This is the same API as [jrobgh/NewNDCamera](github.com/jrobgh/NewNDCamera)
// exposes. It should work with OpenPNP's Neoden4Camera class, which expects
// a `NeodenCamera.dll` under Windows. So the Java class should be able to
// use this API with only minimal changes.

// Implementation (communication protocol with the camera) is copied from
// [ripe909/NeodenCamera64](https://github.com/ripe909/NeodenCamera64).
// That's because NewNDCamera is built to talk to "new generation"
// cameras with a different VID/PID combination and appareently
// also a different communication protocol.

// TODO: Extract camera-specific implementation into separate C++ class
//       to implement both old and new protocols with auto-switching.

/**
 * @deprecated Not implemented. Use `img_readAsy` instead.
 */
bool img_capture(int id);

/**
 * @deprecated Not implemented.
 */
int img_reset(int id);

/**
 * @deprecated Not implemented.
 */
bool img_led(int id, short mode);

/**
 * @deprecated Not implemented. Use `img_readAsy` instead.
 */
int img_read(int id, unsigned char *buffer, int count, int timeout_md);

/**
 * @brief Initialize the camera driver, enumerating the connected cameras.
 *
 * @return int number of cameras found (>= 0), or a libusb_error code (< 0)
 */
int img_init();

/**
 * @brief Read an image of given byte size into the buffer.
*/
int img_readAsy(int id, unsigned char *buffer, int count, int timeout_ms);

/**
 * @brief Set the image exposure time.
 */
bool img_set_exp(int id, int16_t exposure);

/**
 * @brief Set camera gain.
*/
bool img_set_gain(int id, int16_t gain);

/**
 * @brief Set lt values (no idea what they mean).
*/
bool img_set_lt(int id, int16_t a2, int16_t a3);

/**
 * @brief Set image dimension in pixels.
*/
bool img_set_wh(int id, int16_t w, int16_t h);
