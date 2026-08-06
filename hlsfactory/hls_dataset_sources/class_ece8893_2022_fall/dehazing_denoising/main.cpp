#include "pipeline.h"

int main()
{
    INT_TYPE Input_tb[IMG_HEIGHT][IMG_WIDTH];
    INT_TYPE temp_tb[IMG_HEIGHT][IMG_WIDTH][3];
    INT_TYPE Output_tb[IMG_HEIGHT][IMG_WIDTH][3];

    FILE *ptr = fopen("input_bayer.raw","rb");
    // FILE *ptr = fopen("input2_bayer.raw","rb");
    fread(Input_tb,sizeof(Input_tb),1,ptr);

    img_pipeline(Input_tb, Output_tb);

    FILE *write_ptr;
    write_ptr = fopen("image_out.raw","wb");
    // write_ptr = fopen("image2_out.raw","wb");
    fwrite(Output_tb,sizeof(Output_tb),1,write_ptr);

    system("convert -size 512x384 -depth 8 RGB:image_out.raw result.png");
    // system("convert -size 384x512 -depth 8 RGB:image2_out.raw result2.png");
}
