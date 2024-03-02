package main

// #cgo LDFLAGS: -L${SRCDIR}/build -Wl,-rpath,./build -lneodencam
// #include "neodencam.h"
import "C"
import (
	"bytes"
	"fmt"
	"image"
	"image/png"
	"os"
	"time"
	"unsafe"
)

const (
	width  int = 1024
	height int = 1024
)

func main() {
	cams := int(C.img_init())
	fmt.Printf("Found %d cameras\n", cams)

	for i := 0; i < cams; i++ {
		err := setupCamera(i)
		if err != nil {
			fmt.Println(err)
			continue
		}
		time.Sleep(100 * time.Millisecond)

		img, err := readImage(i)
		if err != nil {
			fmt.Println(err)
			continue
		}

		err = saveImage(img, fmt.Sprintf("image-%d.png", i))
	}
}

func setupCamera(cam int) error {
	success := C.img_set_wh(C.int(cam), C.short(width), C.short(height))
	if !success {
		return fmt.Errorf("error during img_set_wh")
	}
	time.Sleep(50 * time.Millisecond)

	success = C.img_set_lt(C.int(cam), C.short(0), C.short(0))
	if !success {
		return fmt.Errorf("error during img_set_lt")
	}
	time.Sleep(50 * time.Millisecond)

	success = C.img_set_exp(C.int(cam), C.short(45))
	if !success {
		return fmt.Errorf("error during img_set_exp")
	}
	time.Sleep(50 * time.Millisecond)

	success = C.img_set_gain(C.int(cam), C.short(36))
	if !success {
		return fmt.Errorf("error during img_set_gain")
	}

	return nil
}

func readImage(cam int) (*image.Gray, error) {
	var buf = make([]byte, width*height, width*height)
	ret := C.img_readAsy(C.int(cam), (*C.uint8_t)(unsafe.Pointer(&buf[0])), C.int(width*height), 2000)
	if ret != 1 {
		return nil, fmt.Errorf("error reading image from camera %d: %d", cam, ret)
	}

	img := image.NewGray(image.Rect(0, 0, width, height))
	img.Pix = buf

	return img, nil
}

func saveImage(img *image.Gray, path string) error {
	buf := bytes.NewBuffer(nil)
	err := png.Encode(buf, img)
	if err != nil {
		return err
	}

	fh, err := os.Create(path)
	if err != nil {
		return err
	}
	defer fh.Close()

	_, err = fh.Write(buf.Bytes())
	if err != nil {
		return err
	}

	fmt.Printf("Image saved as %s\n", path)

	return nil
}
