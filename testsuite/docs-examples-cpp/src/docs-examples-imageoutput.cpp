// Copyright Contributors to the OpenImageIO project.
// SPDX-License-Identifier: Apache-2.0
// https://github.com/AcademySoftwareFoundation/OpenImageIO


///////////////////////////////////////////////////////////////////////////
// This file contains code examples from the ImageOutput chapter of the
// main OpenImageIO documentation.
//
// To add an additional test, replicate the section below. Change
// "example1" to a helpful short name that identifies the example.

// BEGIN-imageoutput-example1
#include <OpenImageIO/imageio.h>
#include <OpenImageIO/unittest.h>
using namespace OIIO;

void example1()
{
    //
    // Example code fragment from the docs goes here.
    //
    // It probably should generate either some text output (which will show up
    // in "out.txt" that captures each test's output), or it should produce a
    // (small) image file that can be compared against a reference image that
    // goes in the ref/ subdirectory of this test.
    //
}
// END-imageoutput-example1

//
///////////////////////////////////////////////////////////////////////////



// BEGIN-imageoutput-simple
#include <OpenImageIO/imageio.h>
using namespace OIIO;

void simple_write()
{
    const char* filename = "simple.tif";
    const int xres = 320, yres = 240, channels = 3;
    unsigned char pixels[xres * yres * channels] = { 0 };

    std::unique_ptr<ImageOutput> out = ImageOutput::create(filename);
    if (!out)
        return;  // error
    ImageSpec spec(xres, yres, channels, TypeDesc::UINT8);
    out->open(filename, spec);
    out->write_image(TypeDesc::UINT8, pixels);
    out->close();
}
// END-imageoutput-simple



void scanlines_write()
{
    const char* filename = "scanlines.tif";
    const int xres = 320, yres = 240, channels = 3;

    std::unique_ptr<ImageOutput> out = ImageOutput::create(filename);
    if (!out)
        return;  // error
    ImageSpec spec(xres, yres, channels, TypeDesc::UINT8);

// BEGIN-imageoutput-scanlines
    unsigned char scanline[xres * channels] = { 0 };
    out->open (filename, spec);
    int z = 0;   // Always zero for 2D images
    for (int y = 0;  y < yres;  ++y) {
        // ... generate data in scanline[0..xres*channels-1] ...
        out->write_scanline (y, z, TypeDesc::UINT8, scanline);
    }
    out->close();
// END-imageoutput-scanlines
}
// BEGIN-imageoutput-tilewriting
void tiles_write()
{
    const std::string filename = "test_tile_output.tif"; // Assume tiff supports tiles
    const int xres = 128, yres = 128, channels = 3, tilesize = 64;
    std::unique_ptr<ImageOutput> out = ImageOutput::create(filename);
    if (!out->supports("tiles")) {
        std::cerr << "Tiles are not supported for this image format." << std::endl;
        return;
    }

    ImageSpec spec(xres, yres, channels, TypeDesc::UINT8);
    spec.tile_width = tilesize;
    spec.tile_height = tilesize;
    out->open(filename, spec);

    unsigned char tile[tilesize * tilesize * channels];
    int z = 0;   // Always zero for 2D images

    // Generating data in tile, for simplicity filling it with a constant value.
    std::fill_n(tile, tilesize * tilesize * channels, 255);  

    for (int y = 0; y < yres; y += tilesize) {
        for (int x = 0; x < xres; x += tilesize) {
            out->write_tile(x, y, z, TypeDesc::UINT8, tile);
        }
    }
    out->close();

    // Now to verify the data
    std::unique_ptr<ImageInput> in = ImageInput::create(filename);
    in->open(filename, spec);

    unsigned char read_tile[tilesize * tilesize * channels];
    for (int y = 0; y < yres; y += tilesize) {
        for (int x = 0; x < xres; x += tilesize) {
            in->read_tile(x, y, z, TypeDesc::UINT8, read_tile);
            OIIO_CHECK_EQUAL(std::memcmp(tile, read_tile, tilesize * tilesize * channels), 0);
        }
    }
    in->close();

    // Cleanup
    std::remove(filename.c_str());
}
// END-imageoutput-tilewriting



int main(int /*argc*/, char** /*argv*/)
{
    simple_write();
    scanlines_write();
    tiles_write();
    return 0;
}
s