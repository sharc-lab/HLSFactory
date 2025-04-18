#include "utils.h"

using namespace std;
int img[H][W];

int isRead = 0;
int num_Chn = 3;
char header[54] = { 0 };
char* header2 = NULL;
int head_size = 0;
int dummy_size = (W % 4 == 0 ? 0 : 4 - W % 4);
int dummy = 0;

bool readImg(string imageName) {
    ifstream image(imageName, ios_base::binary);
    int rgb[3];
    if (image.is_open())
    {
        cout << "opened file:" << imageName << endl;
        //read 54 bit of the image file, to see whether our header is 54 byte
        for (int i = 0; i < 54; i++)
            header[i] = image.get();

        // we obtained the header size, height & width, num_Channel/pixel width
        int w = *(int*)&header[18];
        int h = *(int*)&header[22];
        head_size = *(int*)&header[10];
        num_Chn = *(int*)&header[28] / 8;
        header2 = new char[head_size];

        //if there are more than 54 byte in our header, then we have to get the pallet as well
        //header2 is for storing the new header in case header exceed 54 Byte
        for (int i = 0; i < 54; i++)
            header2[i] = header[i];

        if (head_size > 54)
        {
            for (int i = 54; i < head_size; i++)
            {
                header2[i] = image.get();
            }
        }
        cout << " W:" << w << " H:" << h << " header_size:" << head_size << " channel:" << num_Chn << " pixel_size:" << num_Chn * 8 << " dummy_size:" << dummy_size << endl;

        //read image pixels after header. Images with width cannot divided by 4 will have 0 padding. So I introduce variable dummy sizes to skip through those padding bytes
        for (int i = 0; i < H; i++)
        {
            for (int j = 0; j < (W + dummy_size); j++)
            {
                if (j >= W)
                {
                    dummy = int(image.get());
                }
                else
                {
                    if (num_Chn == 3)
                    {
                        for (int p = 0; p < 3; p++)
                        {
                            rgb[p] = int(image.get());
                        }
                        //if it is a RGB image, make RGB greyscale
                        img[i][j] = 0.3 * rgb[0] + 0.6 * rgb[1] + 0.11 * rgb[2];
                    }
                    else
                    {
                        //if it is not a RGB image, take greyscale directly
                        img[i][j] = int(image.get());
                    }
                }
            }

        }
        cout << "dummy sample: " << dummy << endl;
        image.close();
        isRead = 1;
        return 1;
    }
    else
        return 0;
}

bool writeImg(string imageName, int(&array)[H][W])
{
    if (isRead == 1)
    {
        ofstream image(imageName, ios_base::binary);
        for (int i = 0; i < head_size; i++)
            image.put(header2[i]);
        // if 24 bit pixel width,
        if (num_Chn == 3)
        {
            for (int i = 0; i < H; i++)
            {
                for (int j = 0; j < W + dummy_size; j++)
                {
                    if (j >= W)
                    {
                        image.put(dummy);
                    }
                    else
                    {
                        image.put(array[i][j]);
                        image.put(array[i][j]);
                        image.put(array[i][j]);
                    }
                }
            }
        }
        else
        {
            for (int i = 0; i < H; i++)
                for (int j = 0; j < W + dummy_size; j++)
                {
                    if (j >= W)
                    {
                        image.put(dummy);
                    }
                    else
                    {
                        image.put(array[i][j]);
                    }
                }
        }
        image.close();
        return 1;
    }
    else
    {
        cout << "there is no pinput image yet" << endl;
        return 0;
    }
}

