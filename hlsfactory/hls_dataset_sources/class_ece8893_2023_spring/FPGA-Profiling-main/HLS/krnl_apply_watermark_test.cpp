/*
 * Copyright 2021 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <iostream>
#include <vector>
#include "/export/hdd/scratch/ygoyal8/special_problems/Vitis_Accel_Examples/common/includes/bitmap/bitmap.h"
#include "/export/hdd/scratch/ygoyal8/special_problems/Vitis_Accel_Examples/common/includes/cmdparser/cmdlineparser.h"

#define DATA_SIZE 16

typedef struct TYPE { unsigned int data[DATA_SIZE]; } TYPE;

extern "C" {
void apply_watermark(
        TYPE*,
        TYPE*
    );
}

inline void memcpy(int * inImage, int * image, size_t image_size_bytes)
{
	for(size_t i = 0; i < image_size_bytes/sizeof(int); ++i)
	{
		inImage[i] = image[i];
	}
}

int main()
{
    std::string bitmapFilename = "/export/hdd/scratch/ygoyal8/special_problems/Vitis_Accel_Examples/common/data/xilinx_img.bmp";
    std::string goldenFilename = "/export/hdd/scratch/ygoyal8/special_problems/Vitis_Accel_Examples/cpp_kernels/critical_path/data/golden.bmp";

    // Read the input bit map file into memory
    BitmapInterface image(bitmapFilename.data());
    bool result = image.readBitmapFile();
    if (!result) 
    {
        std::cerr << "ERROR:Unable to Read Input Bitmap File " << bitmapFilename.data() << std::endl;
        return EXIT_FAILURE;
    }

    // Read the golden bit map file into memory
    BitmapInterface goldenImage(goldenFilename.data());
    result = goldenImage.readBitmapFile();
    if (!result) {
        std::cerr << "ERROR:Unable to Read Golden Bitmap File " << goldenFilename.data() << std::endl;
        return EXIT_FAILURE;
    }

    int width = image.getWidth();
    int height = image.getHeight();

    // Allocate Memory in Host Memory
    size_t image_size_bytes = image.numPixels() * sizeof(int);
    std::vector<int> inImage(image.numPixels());
    std::vector<int> outImage(image.numPixels());

    // Copying image host buffer
    memcpy(inImage.data(), image.bitmap(), image_size_bytes);


    apply_watermark(
        reinterpret_cast<TYPE*>(inImage.data()),
        reinterpret_cast<TYPE*>(outImage.data())
    );

    // Compare Golden Image with Output image
    bool match = 0;
    if (image.getHeight() != goldenImage.getHeight() || image.getWidth() != goldenImage.getWidth()) 
    {
        match = 1;
    }
    else 
    {
    	int* goldImgPtr = goldenImage.bitmap();
        for (unsigned int i = 0; i < image.numPixels(); i++) 
        {
            if (outImage[i] != goldImgPtr[i]) 
            {
                match = 1;
                printf("Pixel %d Mismatch Output %x and Expected %x \n", i, outImage[i], goldImgPtr[i]);
                break;
            }
        }
    }

    // Write the final image to disk
    image.writeBitmapFile(outImage.data());

    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;
    return (match ? EXIT_FAILURE : EXIT_SUCCESS);
}
