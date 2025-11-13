#ifndef CONFIG_TB_H_
#define CONFIG_TB_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "common.h"
#include "bmp.h"

/*
 * Markers for JPEG
 *
 * All markers shall be assigned two-byte codes: a 0xFF byte followed by
 * a second byte which is not 0 or 0xFF. The second byte is defined as following.
 */
#define M_BEGIN 0xFF // beginning of a marker
#define M_PAD   0xFF // consecutive 0xFF are used as fill bytes
                     // for padding purposes
/* Start Of Frame markers */
// Non-differential Huffman coding
#define M_SOF0 0xC0 // Baseline DCT
#define M_SOF1 0xC1 // Extended sequential DCT
#define M_SOF2 0xC2 // Progressive DCT
#define M_SOF3 0xC3 // Spatial (sequential) lossless
// Differential Huffman coding
#define M_SOF5 0xC5 // Differential sequential DCT
#define M_SOF6 0xC6 // Differential Progressive DCT
#define M_SOF7 0xC7 // Differential spatial
// Non-differential arithmetic coding
#define M_JPG  0xC8 // Reserved for JPEG extensions
#define M_SOF9 0xC9 // Extended sequential DCT
#define M_SOFA 0xCA // Progressive DCT
#define M_SOFB 0xCB // Spatial (sequential) lossless
// Differential arithmetic coding
#define M_SOFD 0xCD // Differential sequential DCT
#define M_SOFE 0xCE // Differential Progressive DCT
#define M_SOFF 0xCF // Differential spatial
/* Huffman table specification */
#define M_DHT  0xC4
/* Arithmetic coding conditioning specification */
#define M_DAC  0xCC
/* Restart interval termination: restart with modulo 8 count m ([0..7]) */
#define M_RST0 0xD0
#define M_RST1 0xD1
#define M_RST2 0xD2
#define M_RST3 0xD3
#define M_RST4 0xD4
#define M_RST5 0xD5
#define M_RST6 0xD6
#define M_RST7 0xD7
/* Other markers */
#define M_SOI  0xD8 // start of image
#define M_EOI  0xD9 // end of image
#define M_SOS  0xDA // start of scan (compressed data)
#define M_DQT  0xDB // Quantization tables
#define M_DNL  0xDC // define number of lines
#define M_DRI  0xDD // define restart interval
#define M_DHP  0xDE // define hierarchical progression
#define M_EXP  0xDF // expand reference component(s)
/* Application segments */
#define M_APP0 0xE0 // Jfif marker, used for storing and transmitting images
                    // on the Internet
#define M_APP1 0xE1 // Exif marker, commonly used by digital cameras
#define M_APP2 0xE2
#define M_APP3 0xE3
#define M_APP4 0xE4
#define M_APP5 0xE5
#define M_APP6 0xE6
#define M_APP7 0xE7
#define M_APP8 0xE8
#define M_APP9 0xE9
#define M_APPA 0xEA
#define M_APPB 0xEB
#define M_APPC 0xEC
#define M_APPD 0xED
#define M_APPE 0xEE
#define M_APPF 0xEF
/* Reserved for JPEG extensions (F0 -- FD) */
#define M_JPG0 0xF0
/* Comments */
#define M_COM  0xFE

#define BYTE_STUFFING 0x00 // Within the entropy-coded data, after any 0xFF byte,
                           // a 0x00 byte is inserted by the encoder before the
                           // next byte, so that there does not appear to be a
                           // marker where none is intended, preventing framing
                           // errors. Decoders must skip this 0x00 byte.

// Struct for markers
typedef struct _JpegMarker
{
    char name[32];
    unsigned short int size;
    unsigned char *data;
    struct _JpegMarker *next;
}JpegMarker;
// Linked list for markers
typedef struct _MarkerList
{
    JpegMarker *head;
    JpegMarker *tail;
    JpegMarker *current;
}MarkerList;
// DC & AC table selector
typedef struct _EntropyTableSel
{
    int component;
    int dc;
    int ac;
}EntropyTableSel;
// Huffman table
typedef struct _HuffmanTable
{
    int size[16];
    int val_len;
    int *huffval;
}HuffmanTable;
// Arbitrary length binary code
typedef struct _Bits
{
    int value;
    int length;   
}Bits;
// Huffman code word
typedef struct _HuffmanCode
{
    int length;
    Bits prefix;
    int position;
    int value;
}HuffmanCode;


/*
 * Description: append an element to the end of the linked list
 */
inline void
add_tail(MarkerList *list, char *name,
         unsigned short int size,
         unsigned char *data)
{
    JpegMarker *marker = (JpegMarker *)malloc(sizeof(JpegMarker));

    // Assign the data
    strcpy(marker->name, name);
    marker->size = size;
    marker->data = data;

    // Update the linked list
    if (list->head == NULL)
        list->head = marker;
    else
        list->current->next = marker;

    list->tail = marker;
    marker->next = NULL;
    list->current = marker;
}


/*
 * Description: parse the start of frame
 *
 * Frame header
 *
 * SOFn(2) | Lf(2) | P(1) | Y(2) | X(2) | Nf(1) | component-spec(variable)
 *
 * SOFn: Start of frame marker. The subscript n identifies the encoding process.
 * Lf: frame header length
 * P: sample precision^
 * Y: number of lines
 * X: number of samples per line
 * Nf: number of components in frame
 * components-spec: i = 1, 2, ..., Nf
 *     Ci(1): components identifier
 *     Hi(.5): horizontal sampling factor
 *     Vi(.5): vertical sampling factor
 *     Tqi(1): quantization table selector
 */
inline void
parse_sof0(unsigned char *data, unsigned short int size, int *height, int *width)
{
    int precision = data[0];
    *height = (((int)data[1]) << 8) + data[2];
    *width  = (((int)data[3]) << 8) + data[4];
    int num_component = data[5];
}


/*
 * Description: parse the start of scan
 *
 * Scan header
 *
 * SOS(2) | Ls(2) | Ns(1) | components-spec | Ss(1) | Se(1) | Ah(.5) | Al(.5)
 *
 * SOS: start of scan marker
 * Ls: scan header length
 * Ns: number of image components
 * components-spec: i = 1, 2, ..., Ns
 *     Csi(1): scan component selector
 *     Tdi(.5): DC entropy coding table selector
 *     Tai(.5): AC entropy coding table selector
 * Ss: start of spectral or predictor selection, should be 0 for sequential DCT
 * Se: end or spectral selection, should be 63 for sequential DCT processes
 * Ah: successive approximation bit position high, normally 0
 * Al: successive approximation bit position low or point transform, 0
 */
inline void
parse_sos(unsigned char *data,
          unsigned short int size,
          int *components,
          EntropyTableSel **tables)
{
    *components = data[0];
    *tables = (EntropyTableSel *)malloc(sizeof(EntropyTableSel) * (*components));
    for (int i = 0; i < *components; i++)
    {
        (*tables)[i].component = data[i*2+1];
        (*tables)[i].dc = data[i*2+2] >> 4;
        (*tables)[i].ac = data[i*2+2] & 0xF;
    }
}


/*
 * Description: to be added functions
 */
inline void
parse_dhp();

inline void
parse_dnl();

inline void
parse_exp();

inline void
parse_dri();

inline void
parse_com();

inline void
parse_app();

/*
 * Description: parse quantization table
 *
 * Quantization table
 *
 * DQT(2) | Lq(2) | (Pq, Tq)(1) | Q0, Q1, ..., Q63(1)
 *
 * DQT: define quantization table marker
 * Lq: quantization table definition length
 * Pq: quantization table element precision, specifies the precision of the Qk.
 *     0 for 8-bit, 1 for 16-bit
 * Tq: quantization table selector
 * Qk: quantization table element; specifies the kth element out of 64 elements,
 *     where k is the index in zig-zag ordering
 */
inline void
parse_dqt(unsigned char *data,
          unsigned short int size,
          int *id, unsigned char *table)
{
    if (table == NULL)
        *id = data[0] & 0xF;

    if (id == NULL)
    {
        for (int i = 0; i < BLOCK_SIZE; i++)
            table[i] = data[i + 1];
    }
}


/*
 * Description: parse Huffman table
 *
 * Huffman table
 *
 * DHT(2) | Lh(2) | (Tc, Th)(1) | L1, ..., L16(1) | symbol-length assignment
 *
 * DHT: define Huffman table marker
 * Lh: Huffman table definition length
 * Tc: table class; 0=DC table or lossless table; 1=AC table
 * Th: Huffman table identifier
 * Li: number of Huffman codes of length i
 * symbol-length assignment: HUFFVAL
 */
inline void
parse_dht(unsigned char *data,
          unsigned short int size,
          int *id,
          HuffmanTable *table)
{
    int i = 0;
    int j = 0;

    if (id != NULL)
        *id = data[i];

    if (table != NULL)
    {
        i++;
        for (j = 0; j < 16; i++, j++)
        {
            (table->size)[j] = data[i];
            (table->val_len) += data[i];
        }

        (table->huffval) = (int *)malloc(sizeof(int) * (table->val_len));

        for (j = 0; j < table->val_len; i++, j++)
            (table->huffval)[j] = data[i];
    }
}

/*
 * Description: initialize Huffman table structure
 */
inline void
huffman_table_init(HuffmanTable *table)
{
    for (int i = 0; i < 16; i++)
        (table->size)[i] = 0;

    table->val_len = 0;
    table->huffval = NULL;
}

/*
 * Description: decode Huffman table marker
 */
inline void
decode_dht(HuffmanTable *table, HuffmanCode **codes)
{
    int code_len = 0;
    int code_num = 0;
    int space_availiable = 0;
    int space_remaining = 1;
    int max_val = 0;
    int code_val = 0;
    int code = 0;

    for (int i = 0, ii = 0; i < 16; i++)
    {
        code_len = i + 1;
        code_num = (table->size)[i];
        space_availiable = 2 * space_remaining;
        space_remaining = space_availiable - code_num;
        max_val = pow(2, code_len);
        for (int j = 0; j < code_num; j++, ii++)
        {
            code_val = (table->huffval)[ii];
            code = max_val - space_availiable + j;
            DEBUG_PRINT(("%3d, %4x, %2d\n", code_val, code, code_len));
        }
    }
}

/*
 * Description: Read JPEG image, parse the markers, save the encoded data
 */
inline void
read_jpeg(unsigned char **image_data,
          int *data_size,
          int *width,
          int *height,
          const char* fname)
{
    // open the image
    FILE *fp;
    fp = fopen(fname, "rb");
    if (fp == NULL)
    {
        perror("Couldn't open the image.");
        exit(0);
    }

    // check start of image
    unsigned char marker_header = fgetc(fp);
    unsigned char marker_id     = fgetc(fp);
    if (marker_header != M_BEGIN || marker_id != M_SOI)
    {
        perror("Couldn't find the start of image.");
        exit(0);
    }

    // beginning of the encoded data
    int start_of_data = 0;

    // Variables for markers
    char marker_name[32] = "0";
    unsigned short int marker_size = 0;
    unsigned char *marker_data   = NULL;

    // Linked list for markers
    MarkerList marker_list;
    marker_list.head    = NULL;
    marker_list.tail    = NULL;
    marker_list.current = NULL;

    // Start reading markers and data
    while(!start_of_data)
    {
        // Check if the current position is the header of marker 0xFF
        marker_header = fgetc(fp);
        if (marker_header != M_BEGIN)
        {
            perror("Couldn't find the header of marker.");
            exit(0);
        }
        
        marker_id = fgetc(fp); // Get the identity of marker

        // Find common markers
        switch (marker_id)
        {
            case M_SOF0:
                strcpy(marker_name, "SOF0");
                break;
            case M_SOS:
                strcpy(marker_name, "SOS");
                start_of_data = 1;
                break;
            case M_APP0:
                strcpy(marker_name, "JFIF");
                break;
            case M_APP1:
                strcpy(marker_name, "EXIF");
                break;
            case M_DHT:
                strcpy(marker_name, "Huffman Table");
                break;
            case M_DQT:
                strcpy(marker_name, "Quantization Table");
                break;
            default:
                strcpy(marker_name, "Others");
        }

        // read size
        marker_size = fgetc(fp);
        marker_size = (marker_size << 8) + fgetc(fp);
        // read data
        marker_data = (unsigned char *)malloc(marker_size - 2);
        fread(marker_data, 1, marker_size - 2, fp);
        // add to linked list
        add_tail(&marker_list, marker_name, marker_size, marker_data);

        // read image data
        if (start_of_data)
        {
            int current_pos = ftell(fp); // the start of the encoded data

            fseek(fp, 0, SEEK_END);
            *data_size = ftell(fp) - current_pos - 2; // the size of the data

            // save the encoded data
            fseek(fp, current_pos, SEEK_SET);
            *image_data = (unsigned char *)malloc(*data_size);
            fread(*image_data, 1, (*data_size), fp);
        }
    }
    fclose(fp);

    // For SOS
    int num_component               = 0; // if 1, gray-scale; if 3, RGB
    EntropyTableSel *table_selector = NULL;
    // For quantization tables
    unsigned char table_q_dc[BLOCK_SIZE] = {0};
    unsigned char table_q_ac[BLOCK_SIZE] = {0};
    int qt_id = 0;
    // For Huffman tables
    HuffmanTable table_h_dc_y ;
    HuffmanTable table_h_dc_uv;
    HuffmanTable table_h_ac_y ;
    HuffmanTable table_h_ac_uv;
    int ht_id   = 0;
    int val_len = 0;
    huffman_table_init(&table_h_dc_y);
    huffman_table_init(&table_h_dc_uv);
    huffman_table_init(&table_h_ac_y);
    huffman_table_init(&table_h_ac_uv);
    // For markers
    char *name = NULL;
    unsigned short int size   = 0;
    unsigned char *data   = NULL;

    // Start from the head of the linked list
    JpegMarker *jpeg_marker = marker_list.head;
    while (jpeg_marker != NULL)
    {
        name = jpeg_marker->name;
        size = jpeg_marker->size;
        data = jpeg_marker->data;

        DEBUG_PRINT(("Marker Name: %s\n", name));

        // Parse specific markers
        if (strcmp(name, "SOF0") == 0)
            parse_sof0(data, size, height, width);
        else if (strcmp(name, "SOS") == 0)
            parse_sos(data, size, &num_component, &table_selector);
        else if (strcmp(name, "Quantization Table") == 0)
        {
            parse_dqt(data, size, &qt_id, NULL); // get table ID
            if (qt_id == 0) // for DC
            {
                parse_dqt(data, size, NULL, table_q_dc);
                print_matrix(table_q_dc, 8, 8, 3);
            }
            else // for AC
            {
                parse_dqt(data, size, NULL, table_q_ac);
                print_matrix(table_q_ac, 8, 8, 2);
            }
        }
        else if (strcmp(name, "Huffman Table") == 0)
        {
            parse_dht(data, size, &ht_id, NULL); // get table ID
            switch (ht_id)
            {
                case 0x00: // DC, Y
                    parse_dht(data, size, NULL, &table_h_dc_y);
                    print_matrix(table_h_dc_y.size, 16, 1, 0);
                    print_matrix(table_h_dc_y.huffval, table_h_dc_y.val_len, 1, 0);
                    decode_dht(&table_h_dc_y, NULL);
                    break;
                case 0x01: // DC, UV
                    parse_dht(data, size, NULL, &table_h_dc_uv);
                    print_matrix(table_h_dc_uv.size, 16, 1, 0);
                    print_matrix(table_h_dc_uv.huffval, table_h_dc_uv.val_len, 1, 0);
                    decode_dht(&table_h_dc_uv, NULL);
                    break;
                case 0x10: // AC, Y
                    parse_dht(data, size, NULL, &table_h_ac_y);
                    print_matrix(table_h_ac_y.size, 16, 1, 0);
                    print_matrix(table_h_ac_y.huffval, table_h_ac_y.val_len, 1, 0);
                    decode_dht(&table_h_ac_y, NULL);
                    break;
                case 0x11: // AC, UV
                    parse_dht(data, size, NULL, &table_h_ac_uv);
                    print_matrix(table_h_ac_uv.size, 16, 1, 0);
                    print_matrix(table_h_ac_uv.huffval, table_h_ac_uv.val_len, 1, 0);
                    decode_dht(&table_h_ac_uv, NULL);
                    break;
                default:
                    perror("Invalid Huffman table ID");
                    exit(0);
            }
        }

        // Find next marker
        jpeg_marker = jpeg_marker->next;
    }

    free(marker_data);
    free(table_selector);
}


#endif /* CONFIG_TB_H_ */