#ifndef PBM_H
# define PBM_H

# ifdef __cplusplus
extern "C" {
# endif

# define PBM_NO_ERROR 0
# define PBM_BAD_FILE 1
# define PBM_BAD_ARG 2
# define PBM_OUTPUT_ERROR 3

/****************************************************************************
 * Use .ppm extension.  Images with max_val > 255 are broken; don't try it. *
 * Returns one of the error codes above.                                    *
 *                                                                          *
 * filename: The name of the image file; should use a ppm extension.  Most  *
 *           Linux/UNIX image readers can display this.  In Windows, the    *
 *           only tool I'm aware of is Irfanview.                           *
 * width: The number of pixels that the image is wide (x dimension).        *
 * height: The number of pixels that the image is high (y dimension).       *
 * num_channels: The number of color channels in the image.  There are only *
 *               two valid values, 3 for RGB and 1 for grayscale.           *
 * max_val: The maximum value of a channel.  Use 255.                       *
 * data: The image data.  Each pixel should have one byte per channel.      *
 *       Data should be tightly packed (no unused bytes in the stream) in   *
 *       row-major order.  Color images should have their channels ordered  *
 *       RGB.                                                               *
 *                                                                          *
 * See pbmtest.c for example usage.                                         *
 ****************************************************************************/
int write_pbm(const char *filename,
              void *data, int width, int height,
              int num_channels, int max_val);

# ifdef __cplusplus
}
# endif

#endif
