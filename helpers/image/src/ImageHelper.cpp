//
// Created by harut on 8/11/17.
//

#include "opensbv/helpers/image/ImageHelper.h"

namespace opensbv {

    namespace helpers {
        namespace image {

            /*
            static int decode_write_frame(AVFrame *framergb, AVCodecContext *avctx,AVFrame *frame,AVPacket *pkt, int last) {
                int len, got_frame;
                char buf[1024];
                struct SwsContext *convert_ctx;


                    int w = avctx->width;
                    int h = avctx->height;
                    convert_ctx = sws_getContext(w, h, avctx->pix_fmt,
                                                 w, h, AV_PIX_FMT_BGR24, SWS_BICUBIC,
                                                 NULL, NULL, NULL);

                    if(convert_ctx == NULL) {
                        fprintf(stderr, "Cannot initialize the conversion context!\n");
                        exit(1);
                    }

                    sws_scale(convert_ctx, (const uint8_t * const *)frame->data,
                              frame->linesize, 0,
                              h,
                              framergb->data, framergb->linesize);

                    */
            /* the picture is allocated by the decoder, no need to free it *//*

        //        snprintf(buf, sizeof(buf), outfilename, *frame_count);

        //        bmp_save(framergb->data[0], framergb->linesize[0],
        //                 avctx->width, avctx->height, buf);
        //        (*frame_count)++;

            if (pkt->data) {
                pkt->size -= len;
                pkt->data += len;
            }
            return 0;
        }
        */

            ImageHelper::ImageHelper() {

            }

            ImageHelper::~ImageHelper() {

            }

            void ImageHelper::compress_jpg_turbo(unsigned char *data,
                                                 enum imageColorType sourceType,
                                                 buffer_image *image,
                                                 unsigned int width,
                                                 unsigned int height,
                                                 unsigned short quality) {
                try {
                    tjhandle _jpegCompressor = tjInitCompress();

                    tjCompress2(_jpegCompressor, data, width, 0, height, TJPF_BGR,
                                &image->buffer, &image->buffersize, TJSAMP_444, quality,
                                TJFLAG_FASTDCT);

                    tjDestroy(_jpegCompressor);

                    //to free the memory allocated by TurboJPEG (either by tjAlloc(),
                    //or by the Compress/Decompress) after you are done working on it:
//                            tjFree(image->buffer);

                } catch (std::exception &e) {
                    tjFree(image->buffer);
                    throw ImageHelperException("compress_jpg_turbo()", e.what());
                } catch (...) {
                    tjFree(image->buffer);
                    throw ImageHelperException("compress_jpg_turbo()", "undefined exception");
                }
            }

            void ImageHelper::decompress_jpg_turbo(unsigned char *data,
                                                 unsigned long jpegSize,
                                                 enum imageColorType sourceType,
                                                 buffer_image *image,
                                                 int width,
                                                int height) {
                try {
                    int jpegSubsamp;

                    int size = 1;
                    if (sourceType == IMAGE_COLOR_BGR)
                        size = 3;

                    delete image->buffer;

                    image->buffer = new unsigned char[width*height*size];

                    tjhandle _jpegDecompressor = tjInitDecompress();

                    tjDecompressHeader2(_jpegDecompressor, data, jpegSize, &width, &height, &jpegSubsamp);

                    tjDecompress2(_jpegDecompressor, data, jpegSize, image->buffer, width, 0/*pitch*/, height, TJPF_BGR, TJFLAG_FASTDCT);

                    tjDestroy(_jpegDecompressor);

                } catch (std::exception &e) {
                    delete image->buffer;
                    throw ImageHelperException("decompress_jpg_turbo()", e.what());
                } catch (...) {
                    delete image->buffer;
                    throw ImageHelperException("decompress_jpg_turbo()", "undefined exception");
                }
            }

            /*
            bool ImageHelper::compress_h264(buffer_image *m_bufferImage,
                                            unsigned char *data,
                                            AVPixelFormat pixel_fmt,
                                            int64_t bit_rate,
                                            char cod_name[10],
                                            unsigned short width,
                                            unsigned short height) {

                const char *filename, *codec_name;
                const AVCodec *codec;
                AVCodecContext *c= NULL;
                int i, ret, x, y;
            //    FILE *f;
                AVFrame *frame;
                AVPacket *pkt;
                uint8_t endcode[] = { 0, 0, 1, 0xb7 };

                codec_name = cod_name; // codec name, example: libx264rgb

                avcodec_register_all();

                */
            /* find the mpeg1video encoder *//*

            codec = avcodec_find_encoder_by_name(codec_name);
            if (!codec) {
                fprintf(stderr, "Codec '%s' not found\n", codec_name);
                return false;
            }

            c = avcodec_alloc_context3(codec);
            if (!c) {
                fprintf(stderr, "Could not allocate video codec context\n");
                return false;
            }

            pkt = av_packet_alloc();
            if (!pkt)
                return false;

            */
            /* put sample parameters *//*

            c->bit_rate = bit_rate;
        //    c->bit_rate = 400000;
            */
            /* resolution must be a multiple of two *//*

            c->width = width;
            c->height = height;
            */
            /* frames per second *//*

            c->time_base = (AVRational){1, 30};
            c->framerate = (AVRational){30, 1};

            */
            /* emit one intra frame every ten frames
                 * check frame pict_type before passing frame
                 * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
                 * then gop_size is ignored and the output of encoder
                 * will always be I frame irrespective to gop_size
                 *//*

            c->gop_size = 10;
            c->max_b_frames = 1;
        //    c->pix_fmt = AV_PIX_FMT_YUV422P;
            c->pix_fmt = pixel_fmt;

            if (codec->id == AV_CODEC_ID_H264)
                av_opt_set(c->priv_data, "preset", "slow", 0);

            */
            /* open it *//*

            ret = avcodec_open2(c, codec, NULL);
            if (ret < 0) {
        //        fprintf(stderr, "Could not open codec: %s\n", av_err2str(ret));
                return false;
            }

            frame = av_frame_alloc();
            if (!frame) {
                fprintf(stderr, "Could not allocate video frame\n");
                return false;
            }
            frame->format = c->pix_fmt;
            frame->width  = c->width;
            frame->height = c->height;

            ret = av_frame_get_buffer(frame, 32);
            if (ret < 0) {
                fprintf(stderr, "Could not allocate the video frame data\n");
                return false;
            }

            fflush(stdout);

            */
            /* make sure the frame data is writable *//*

            ret = av_frame_make_writable(frame);
            if (ret < 0)
                return false;

            frame->data[0] = data;

            frame->pts = 1;

                */
            /* encode the image *//*

            ImageHelper::encode_h264(m_bufferImage, c, frame, pkt);

            */
            /* flush the encoder *//*

            ImageHelper::encode_h264(m_bufferImage, c, NULL, pkt);

            */
            /* add sequence end code to have a real MPEG file *//*

        //    fwrite(endcode, 1, sizeof(endcode), f);

            avcodec_free_context(&c);
            av_frame_free(&frame);
            av_packet_free(&pkt);

            return true;
        }

        bool ImageHelper::encode_h264(buffer_image *m_bufferImage, AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt) {

            int ret;

            */
            /* send the frame to the encoder *//*

        //    if (frame)
        //        printf("Send frame %3"PRId64"\n", frame->pts);

            ret = avcodec_send_frame(enc_ctx, frame);

            if (ret < 0) {
                fprintf(stderr, "Error sending a frame for encoding\n");
                return false;
            }

            while (ret >= 0) {
                ret = avcodec_receive_packet(enc_ctx, pkt);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    return false;
                else if (ret < 0) {
                    fprintf(stderr, "Error during encoding\n");
                    return false;
                }

                m_bufferImage->buffer = (unsigned char *)malloc(pkt->size);

                m_bufferImage->buffersize = pkt->size;
                memcpy( m_bufferImage->buffer, (char *)pkt->data, pkt->size );

        //        printf("Write packet %3"PRId64" (size=%5d)\n", pkt->pts, pkt->size);
        //        fwrite(pkt->data, 1, (size_t)pkt->size, outfile);
                av_packet_unref(pkt);
            }

            return true;
        }

        bool ImageHelper::decompress_h264(buffer_image *m_bufferImage, unsigned char *input_data, long input_size) {
            const AVCodec *codec;
            AVCodecParserContext *parser;
            AVCodecContext *c= NULL;
            AVFrame *frame;
            AVFrame framebgr;
            AVPacket *pkt;

            avcodec_register_all();

            pkt = av_packet_alloc();
            if (!pkt)
                return false;

            */
            /* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) *//*

        //    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

            */
            /* find the MPEG-1 video decoder *//*

            codec = avcodec_find_decoder(AV_CODEC_ID_H264);
            if (!codec) {
                fprintf(stderr, "Codec not found\n");
                return false;
            }

            parser = av_parser_init(codec->id);
            if (!parser) {
                fprintf(stderr, "parser not found\n");
                return false;
            }

            c = avcodec_alloc_context3(codec);
            if (!c) {
                fprintf(stderr, "Could not allocate video codec context\n");
                return false;
            }

            */
            /* For some codecs, such as msmpeg4 and mpeg4, width and height
                   MUST be initialized there because this information is not
                   available in the bitstream. *//*


            */
            /* open it *//*

            if (avcodec_open2(c, codec, NULL) < 0) {
                fprintf(stderr, "Could not open codec\n");
                return false;
            }

            frame = av_frame_alloc();
            if (!frame) {
                fprintf(stderr, "Could not allocate video frame\n");
                return false;
            }

            int bytes = av_image_get_buffer_size(AV_PIX_FMT_BGR24, 640, 480, 1);

            uint8_t *buffer=(uint8_t *)av_malloc(bytes*sizeof(uint8_t));
            av_image_fill_arrays(framebgr.data, framebgr.linesize, buffer, AV_PIX_FMT_BGR24,
                           640, 480, 1);

            */
            /* read raw data from the input file *//*

            framebgr.width = frame->width;
            framebgr.height = frame->height;

            pkt->data = input_data;
            pkt->size = input_size;

            if (pkt->size)
                ImageHelper::decode_h264(c, frame, pkt);

            */
            /* flush the decoder *//*

            ImageHelper::decode_h264(c, frame, NULL);

            avpicture_fill( (AVPicture *)&framebgr, framebgr.data[0], AV_PIX_FMT_BGR24, 640, 480);
        //    av_image_fill_arrays( framebgr.data, framebgr.linesize, frame->data[0], AV_PIX_FMT_BGR24, 640, 480, frame->linesize[0]);

            enum AVPixelFormat src_pixfmt = (enum AVPixelFormat)frame->format;
            enum AVPixelFormat dst_pixfmt = AV_PIX_FMT_BGR24;
            SwsContext *convert_ctx = sws_getContext(640, 480, src_pixfmt, 640, 480, dst_pixfmt,
                                         SWS_DIRECT_BGR, NULL, NULL, NULL);

            if(convert_ctx == NULL) {
                fprintf(stderr, "Cannot initialize the conversion context!\n");
                exit(1);
            }

            sws_scale(convert_ctx, (const uint8_t * const *)frame->data, frame->linesize, 0, 480,
                      framebgr.data, framebgr.linesize);

            m_bufferImage->buffer = framebgr.data[0];
            m_bufferImage->buffersize = bytes;

            av_parser_close(parser);
            avcodec_free_context(&c);
            av_frame_free(&frame);
            av_packet_free(&pkt);

            return 0;
        }

        bool ImageHelper::decode_h264(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt) {
            int ret;
            ret = avcodec_send_packet(dec_ctx, pkt);
            if (ret < 0) {
                fprintf(stderr, "Error sending a packet for decoding\n");
                return false;
            }

            ret = avcodec_receive_frame(dec_ctx, frame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                return false;
            else if (ret < 0) {
                fprintf(stderr, "Error during decoding\n");
                return false;
            }

            return true;
        }

        */

            void ImageHelper::BGR2Grayscale(const unsigned char *buffer, size_t size, unsigned char *ret) {
                if (ret != nullptr) {
                    int i = 0;
                    int k = 0;

                    for (i; i < size; i += 3){
                        int n = ((int)buffer[i] + (int)buffer[i+1] + (int)buffer[i+2])/3;
                        ret[k] = n;
                        k = k+1;
                    }
                }
            }
        }
    }
}