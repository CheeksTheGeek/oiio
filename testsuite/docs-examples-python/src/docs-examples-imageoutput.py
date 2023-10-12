#!/usr/bin/env python

# Copyright Contributors to the OpenImageIO project.
# SPDX-License-Identifier: Apache-2.0
# https://github.com/AcademySoftwareFoundation/OpenImageIO

from __future__ import print_function
from __future__ import absolute_import


############################################################################
# This file contains code examples from the ImageOutput chapter of the
# main OpenImageIO documentation.
#
# To add an additional test, replicate the section below. Change
# "example1" to a helpful short name that identifies the example.


# BEGIN-imageoutput-example1
import OpenImageIO as oiio
import numpy as np

def example1() :
    #
    # Example code fragment from the docs goes here.
    #
    # It probably should generate either some text output (which will show up
    # in "out.txt" that captures each test's output), or it should produce a
    # (small) image file that can be compared against a reference image that
    # goes in the ref/ subdirectory of this test.
    #
    return

# END-imageoutput-example1

#
############################################################################



# BEGIN-imageoutput-simple
import OpenImageIO as oiio
import numpy as np

def simple_write() :
    filename = "simple.tif"
    xres = 320
    yres = 240
    channels = 3  # RGB
    pixels = np.zeros((yres, xres, channels), dtype=np.uint8)

    out = oiio.ImageOutput.create (filename)
    if out:
        spec = oiio.ImageSpec(xres, yres, channels, 'uint8')
        out.open (filename, spec)
        out.write_image (pixels)
        out.close ()
# END-imageoutput-simple


import OpenImageIO as oiio
import numpy as np

def scanlines_write() :
    filename = "scanlines.tif"
    xres = 320
    yres = 240
    channels = 3  # RGB
    spec = oiio.ImageSpec(xres, yres, channels, 'uint8')

    out = oiio.ImageOutput.create (filename)
    if out:
        # BEGIN-imageoutput-scanlines
        z = 0   # Always zero for 2D images
        out.open (filename, spec)
        for y in range(yres) :
            # Generate pixel array for one scanline.
            # As an example, we are just making a zero-filled scanline
            scanline = np.zeros((xres, channels), dtype=np.uint8)
            out.write_scanline (y, z, scanline)
        out.close ()
        # END-imageoutput-scanlines

# BEGIN-imageoutput-tilewriting
import OpenImageIO as oiio
import numpy as np
import os

def tiles_write():
    filename = "tile_output.tif"  # Assume tiff supports tiles
    xres, yres, channels, tilesize = 128, 128, 3, 64
    out = oiio.ImageOutput.create(filename)
    
    if not out.supports("tiles"):
        print("Tiles are not supported for this image format.")
        return
    
    spec = oiio.ImageSpec(xres, yres, channels, oiio.UINT8)
    spec.tile_width = tilesize
    spec.tile_height = tilesize
    out.open(filename, spec)
    
    # Generating data in tile, for simplicity filling it with a constant value.
    tile = np.full((tilesize, tilesize, channels), 255, dtype=np.uint8)
    
    z = 0  # Always zero for 2D images
    for y in range(0, yres, tilesize):
        for x in range(0, xres, tilesize):
            # out.write_tile(x, y, z, oiio.TypeDesc(oiio.UINT8), tile.tobytes())
            out.write_tile(x, y, z, tile.tobytes())
    
    out.close()
    
    # Now to verify the data
    inp = oiio.ImageInput.open(filename)
    for y in range(0, yres, tilesize):
        for x in range(0, xres, tilesize):
            read_tile = inp.read_tile(x, y, z, oiio.UINT8)
            assert np.array_equal(tile, np.array(read_tile))
    
    inp.close()
    
    # Cleanup
    os.remove(filename)
# END-imageoutput-tilewriting


# Import necessary libraries from OpenImageIO and NumPy
import OpenImageIO as oiio
import numpy as np
import os

# BEGIN-imageoutput-cropwindow
def crop_window():
    filename = "crop_window.tif"
    fullwidth, fullheight = 640, 480
    cropwidth, cropheight, channels = 16, 16, 3
    xorigin, yorigin = 32, 128
    pixels = np.zeros((cropheight, cropwidth, channels), dtype="uint8")  # Assume data is already populated
    
    spec = oiio.ImageSpec(cropwidth, cropheight, channels, oiio.UINT8)
    spec.full_x = 0
    spec.full_y = 0
    spec.full_width = fullwidth
    spec.full_height = fullheight
    spec.x = xorigin
    spec.y = yorigin
    
    out = oiio.ImageOutput.create(filename)
    out.open(filename, spec)
    
    z = 0  # Always zero for 2D images
    for y in range(yorigin, yorigin + cropheight):
        out.write_scanline(y, z, oiio.UINT8, pixels[y - yorigin])
    
    out.close()
    
    # Verification
    inp = oiio.ImageInput.open(filename)
    read_pixels = np.empty_like(pixels)
    for y in range(yorigin, yorigin + cropheight):
        inp.read_scanline(y, z, oiio.UINT8, read_pixels[y - yorigin])
    assert np.array_equal(pixels, read_pixels)
    
    inp.close()
    
    # Cleanup
    os.remove(filename)
# END-imageoutput-cropwindow



if __name__ == '__main__':
    # Each example function needs to get called here, or it won't execute
    # as part of the test.
    simple_write()
    scanlines_write()
    tiles_write()
    crop_window()

