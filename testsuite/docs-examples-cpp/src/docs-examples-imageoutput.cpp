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
    const std::string filename = "tile_output.tif"; // Assume tiff supports tiles
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


// BEGIN-imageoutput-cropwindow
void crop_window()
{
    const std::string filename = "crop_window.tif";
    const int fullwidth = 640, fullheight = 480;
    const int cropwidth = 16, cropheight = 16, channels = 3;
    const int xorigin = 32, yorigin = 128;
    unsigned char pixels[cropwidth * cropheight * channels];  // Assume data is already populated
    
    std::unique_ptr<ImageOutput> out = ImageOutput::create(filename);
    ImageSpec spec(cropwidth, cropheight, channels, TypeDesc::UINT8);
    spec.full_x = 0;
    spec.full_y = 0;
    spec.full_width = fullwidth;
    spec.full_height = fullheight;
    spec.x = xorigin;
    spec.y = yorigin;
    out->open(filename, spec);
    
    int z = 0;  // Always zero for 2D images
    for (int y = yorigin; y < yorigin + cropheight; ++y) {
        out->write_scanline(y, z, TypeDesc::UINT8, &pixels[(y - yorigin) * cropwidth * channels]);
    }
    out->close();
    
    // Verification
    std::unique_ptr<ImageInput> in = ImageInput::create(filename);
    in->open(filename, spec);
    unsigned char read_pixels[cropwidth * cropheight * channels];
    for (int y = yorigin; y < yorigin + cropheight; ++y) {
        in->read_scanline(y, z, TypeDesc::UINT8, read_pixels);
        OIIO_CHECK_EQUAL(std::memcmp(&pixels[(y - yorigin) * cropwidth * channels], read_pixels, cropwidth * channels), 0);
    }
    in->close();
    
    // Cleanup
    std::remove(filename.c_str());
}
// END-imageoutput-cropwindow

// BEGIN-imageoutput-metadata
void metadata_write()
    const std::string filename = "test_metadata_output.tif";
    int width = 640, length = 480, channels = 3;
    TypeDesc format = TypeDesc::UINT8;

    // Create ImageSpec and set metadata
    ImageSpec spec(width, length, channels, format);
    spec.channelnames.assign({ "R", "G", "B" });
    spec.alpha_channel = -1;
    spec.z_channel = -1;
    spec.attribute("oiio:ColorSpace", "scene_linear");

    // Open file, write metadata
    std::unique_ptr<ImageOutput> out = ImageOutput::create(filename);
    out->open(filename, spec);
    out->close();

    // Verification
    std::unique_ptr<ImageInput> in = ImageInput::create(filename);
    const ImageSpec &read_spec = in->spec();
    OIIO_CHECK_EQUAL(read_spec.get_string_attribute("oiio:ColorSpace"), "scene_linear");
    OIIO_CHECK_EQUAL(read_spec.channelnames, spec.channelnames);

    // Cleanup
    std::remove(filename.c_str());
}
// END-imageoutput-metadata


// BEGIN-imageoutput-multiimagefile

void multi_image_file_write() {
    const std::string filename = "multi_image_file.tif";
    int nsubimages = 2;
    ImageSpec specs[] = { ImageSpec(640, 480, 3, TypeDesc::UINT8), 
                          ImageSpec(800, 600, 3, TypeDesc::UINT8) };
    unsigned char pixels[][3] = { {255, 0, 0}, {0, 255, 0} };

    auto out = ImageOutput::create(filename);
    if (nsubimages > 1 && (!out->supports("multiimage") || !out->supports("appendsubimage"))) {
        std::cerr << "Does not support appending of subimages\n";
        return;
    }

    out->open(filename, nsubimages, specs);
    for (int s = 0; s < nsubimages; ++s) {
        if (s > 0)
            out->open(filename, specs[s], ImageOutput::AppendSubimage);
        out->write_image(TypeDesc::UINT8, &pixels[s]);
    }
    out->close();

    // Verification
    auto in = ImageInput::create(filename);
    for (int s = 0; s < nsubimages; ++s) {
        const ImageSpec &read_spec = in->spec(s);
        OIIO_CHECK_EQUAL(read_spec.width, specs[s].width);
        OIIO_CHECK_EQUAL(read_spec.height, specs[s].height);
    }

    // Cleanup
    std::remove(filename.c_str());
}


// END-imageoutput-multiimagefile


int main(int /*argc*/, char** /*argv*/)
{
    simple_write();
    scanlines_write();
    tiles_write();
    crop_window();
    metadata_write();
    multi_image_file_write();
    return 0;
}
s