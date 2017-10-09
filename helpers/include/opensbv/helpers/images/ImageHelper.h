//
// Created by harut on 8/11/17.
//

#ifndef PB_MAIN_IMAGEWORKER_H
#define PB_MAIN_IMAGEWORKER_H

typedef unsigned char JSAMPLE;

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#include <syslog.h>
#include <iostream>

/*extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/opt.h>
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
}*/

#include "jpeglib.h"
#include "jmorecfg.h"
#include <jerror.h>
#include <setjmp.h>

typedef unsigned short ushort;

/// Compress Data Type
enum h264_data_type {
    YUYV,
};

/// Image color type
enum imageColorType{
    IMAGE_COLOR_BGR,
    IMAGE_COLOR_GRAYSCALE
};

struct jpegErrorManager {
    /* "public" fields */
    struct jpeg_error_mgr pub;
    /* for return to caller */
    jmp_buf setjmp_buffer;
};

/// Image buffer struct for decode and encode
struct buffer_image {
    int height; ///< height of image
    int width; ///< width of image
    unsigned char *buffer; ///< image buffer
    unsigned long buffersize; ///< image buffer size

};

/// Image encoder decoder struct
typedef struct {
    struct jpeg_source_mgr pub; ///< libjpeg image struct
    int *jpegdata; ///< jpeg data
    int jpegsize; ///< jpeg size
} my_source_mgr;


/// Image Helper class
/**
  This class is meant to work with images, encode, decode, etc
*/
class ImageHelper {
public:
    ImageHelper(); ///< Constructor
    ~ImageHelper(); ///< Destructor

    /// Grayscale BGR
    /**
     *
     * @param buffer
     * @param size
     * @param ret
     * @return
     */
    static void BGR2Grayscale(const unsigned char *buffer, size_t size, unsigned char *ret);

    /// Decompress JPEG Image
    /**
     * Decompress JPEG image using libjpeg-turbo
     * @param jpeg
     * @param jpegsize
     * @param image
     * @return
     */
    static bool decompress_jpeg(unsigned char *jpeg, int jpegsize, buffer_image *image, enum imageColorType outSourceType);

    /// Compress JPEG Image
    /**
     * Compress JPEG image using libjpeg-turbo
     * @param data
     * @param image
     * @param width
     * @param height
     * @param quality
     * @return
     */
    static bool compress_jpg(unsigned char *data,
                             enum imageColorType sourceType,
                            buffer_image *image,
                            unsigned int width,
                            unsigned int height,
                            unsigned short quality);

    /// Compress H264 Image
    /**
     * Compress H264 image using ffmpeg
     * @param m_bufferImage
     * @param data
     * @param pixel_fmt
     * @param bit_rate
     * @param cod_name
     * @param width
     * @param height
     * @return
     */
    /*static bool compress_h264(buffer_image *m_bufferImage,
                              unsigned char *data,
                              AVPixelFormat pixel_fmt,
                              int64_t bit_rate,
                              char cod_name[10],
                              unsigned short width,
                              unsigned short height);
*/
    /// Decompress H264 Image
    /**
     * Decompress H254 image using ffmpeg
     * @param m_bufferImage
     * @param input_data
     * @param input_size
     * @return
     */
//    static bool decompress_h264(buffer_image *m_bufferImage, unsigned char *input_data, long input_size);

private:
    /// Jpeg init source
    static void jpeg_init_src(j_decompress_ptr cinfo, int *jpegdata, int jpegsize);

    /// Init source
    static void init_source(j_decompress_ptr cinfo);

    /// Fill input buffer
    static int fill_input_buffer(j_decompress_ptr cinfo);

    /// Skip input data
    static void skip_input_data(j_decompress_ptr cinfo, long num_bytes);

    /// Terminate source
    static void term_source(j_decompress_ptr cinfo);

    /// Error exit
    static void my_error_exit(j_common_ptr cinfo);

    /// Error exit output message
    static void my_error_output_message(j_common_ptr cinfo);

    /// Encode H264
    /**
     * Encode buffer to AVFrame
     * @param m_bufferImage
     * @param enc_ctx
     * @param frame
     * @param pkt
     * @return
     */
//    static bool encode_h264(buffer_image *m_bufferImage, AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt);

    /// Decode H264
    /**
     * Decode buffer to AVFrame
     * @param dec_ctx
     * @param frame
     * @param pkt
     * @return
     */
//    static bool decode_h264(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt);
};


#endif //PB_MAIN_IMAGEWORKER_H