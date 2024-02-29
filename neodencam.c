#include "neodencam.h"

#include "libusb.h"
#include <stdio.h>

// This library uses libusb to communicate with the Neoden cameras.
// It controls two cameras, representing the UP and DOWN views.
#define N_CAMS 2
#define CAM_TOP 0
#define CAM_BOTTOM 1
libusb_device_handle *handles[N_CAMS];

// The Neoden camera's USB vendor ID
const uint16_t VID = 0x0828;
// The Neoden camera's USB product ID
const uint16_t PID = 0x1002;

// Number of the bulk endpoint to read image data from
const int BULK_EP = 0x02;

// Timeout for control transfers in milliseconds
const int control_timeout_ms = 500;

// Our local, driver-specific libusb context
libusb_context *ctx;

libusb_device_handle *get_handle(int id) {
    switch (id) {
    case CAM_TOP:
        return handles[CAM_TOP];
    case CAM_BOTTOM:
        return handles[CAM_BOTTOM];
    default:
        return NULL;
    }
}

// int init_device(libusb_device_handle *handle) {
//     struct libusb_device_descriptor desc;
//     libusb_get_device_descriptor(libusb_get_device(handle), &desc);
//     uint8_t buffer[] = {
//         // clang-format off
//         0x28, 0x74, 0x00, 0x00,
//         0x38, 0x18, 0x00, 0x00,
//         0xb0, 0xd3, 0xb9, 0xdb,
//         0x64, 0x24, 0xcf, 0x77, // clang-format on
//     };
//     int transferred = libusb_control_transfer(
//         handle,
//         LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
//         0xbc,
//         0x0000,
//         0x0000,
//         buffer,
//         sizeof(buffer),
//         control_timeout_ms
//     );
//     if (transferred < sizeof(buffer)) {
//         printf("init_device: control transfer failed: %d\n", transferred);
//         return -1;
//     }
// }

bool img_capture(int id) {
    printf("img_capture(%d) not implemented\n", id);
    return 0;
}

int img_reset(int id) {
    printf("img_reset(%d) not implemented\n", id);
    return 0;
}

bool img_led(int id, short mode) {
    printf("img_led(%d, %d) not implemented\n", id, mode);
    return 0;
}

int img_read(int id, unsigned char *buffer, int count, int timeout_ms) {
    printf("img_read(%d, *buf, %d, %d) not implemented\n", id, count, timeout_ms);
    return 0;
}

int img_init() {
    libusb_device **list;
    int status = libusb_init_context(&ctx, NULL, 0);
    if (status != LIBUSB_SUCCESS) {
        printf("libusb_init_context failed: %d\n", status);
        return -1;
    }

    int count = libusb_get_device_list(ctx, &list);
    if (count < 1) {
        printf("No USB devices found\n");
        return count;
    }

    uint found = 0;
    struct libusb_device_descriptor desc;
    for (int i = 0; i < count; i++) {
        if (found >= N_CAMS) {
            break;
        }

        libusb_device *device = list[i];
        status = libusb_get_device_descriptor(device, &desc);
        if (status != LIBUSB_SUCCESS) {
            printf("libusb_get_device_descriptor failed: %d\n", status);
            continue;
        }

        if (desc.idVendor != VID || desc.idProduct != PID) {
            continue;
        }

        status = libusb_open(device, &handles[found]);
        if (status != LIBUSB_SUCCESS) {
            printf("libusb_open failed: %d\n", status);
            continue;
        }

        // Detach kernel driver and claim interface
        if (libusb_kernel_driver_active(handles[found], 0) == 1) {
            printf("Detaching kernel driver\n");
            status = libusb_detach_kernel_driver(handles[found], 0);
            if (status != LIBUSB_SUCCESS) {
                printf("libusb_detach_kernel_driver failed: %d\n", status);
                continue;
            }
        }
        status = libusb_claim_interface(handles[found], 0);
        if (status != LIBUSB_SUCCESS) {
            printf("libusb_claim_interface failed: %d\n", status);
            continue;
        }

        found++;
    }

    libusb_free_device_list(list, 1);

    return found;
}

int img_readAsy(int id, unsigned char *buffer, int count, int timeout_ms) {
    libusb_device_handle *handle = get_handle(id);
    if (handle == NULL) {
        printf("img_readAsy: invalid camera id: %d\n", id);
        return -1;
    }

    // We send a vendor-specific control transfer to the camera to request an image.
    // Then we receive the image using a bulk transfer.
    int status = libusb_control_transfer(
        handle,
        LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
        0xb3,
        0x0000,
        0x0000,
        NULL,
        0,
        control_timeout_ms
    );
    if (status < 0) {
        printf("img_readAsy: control transfer failed: %d\n", status);
        return status;
    }
    status = libusb_control_transfer(
        handle,
        LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
        0xb1,
        0x0000,
        0x0000,
        NULL,
        0,
        control_timeout_ms
    );
    if (status < 0) {
        printf("img_readAsy: control transfer failed: %d\n", status);
        return status;
    }

    // Now read the image data into the buffer. This is a synchronous, blocking call.
    int transferred = 0;
    status = libusb_bulk_transfer(
        handle, LIBUSB_ENDPOINT_IN | BULK_EP, buffer, count, &transferred, timeout_ms
    );
    if (status < 0 || transferred != count) {
        printf("img_readAsy: bulk transfer failed: %d\n", status);
        return status;
    }

    return 0;
}

bool img_set_exp(int id, int16_t exposure) {
    libusb_device_handle *handle = get_handle(id);
    if (handle == NULL) {
        printf("img_set_exp: invalid camera id: %d\n", id);
        return -1;
    }

    int status = libusb_control_transfer(
        handle,
        LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
        0xb4,
        exposure,
        0x0000,
        NULL,
        0,
        control_timeout_ms
    );
    if (status < 0) {
        printf("img_set_exp: control transfer failed: %d\n", status);
        return false;
    }
    return true;
}

bool img_set_gain(int id, int16_t gain) {
    libusb_device_handle *handle = get_handle(id);
    if (handle == NULL) {
        printf("img_set_gain: invalid camera id: %d\n", id);
        return -1;
    }

    int status = libusb_control_transfer(
        handle,
        LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
        0xb5,
        gain,
        0x0000,
        NULL,
        0,
        control_timeout_ms
    );
    if (status < 0) {
        printf("img_set_gain: control transfer failed: %d\n", status);
        return false;
    }
    return true;
}

bool img_set_lt(int id, int16_t a2, int16_t a3) {
    libusb_device_handle *handle = get_handle(id);
    if (handle == NULL) {
        printf("img_set_lt: invalid camera id: %d\n", id);
        return -1;
    }

    int status = libusb_control_transfer(
        handle,
        LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
        0xb6,
        ((a2 >> 1) << 1) + 12,
        0x0000,
        NULL,
        0,
        control_timeout_ms
    );
    if (status < 0) {
        printf("img_set_lt: control transfer failed: %d\n", status);
        return false;
    }
    return true;
}

bool img_set_wh(int id, int16_t w, int16_t h) {
    libusb_device_handle *handle = get_handle(id);
    if (handle == NULL) {
        printf("img_set_wh: invalid camera id: %d\n", id);
        return -1;
    }

    int status = libusb_control_transfer(
        handle,
        LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
        0xb7,
        h,
        w,
        NULL,
        0,
        control_timeout_ms
    );
    if (status < 0) {
        printf("img_set_wh: control transfer failed: %d\n", status);
        return false;
    }

    return true;
}
