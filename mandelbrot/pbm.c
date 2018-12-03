#include <stdio.h>
#include <stdlib.h>

#include "pbm.h"

int write_pbm(const char *filename,
              void *data, int width, int height,
              int num_channels, int max_val)
{
  FILE *f;
  int bytes_per_channel;

  if (!(f = fopen(filename, "w"))) {
    perror(filename);

    return PBM_BAD_FILE;
  }

  if ((num_channels != 1 && num_channels != 3) ||
      max_val > 65535 || max_val <= 0 ||
      width <= 0 || height <= 0) {
    return PBM_BAD_ARG;
  }

  bytes_per_channel = max_val > 255 ? 2 : 1;

  if (fprintf(f, num_channels == 1 ? "P5\n" : "P6\n") < 0 ||
      fprintf(f, "%d %d\n", width, height) < 0                     ||
      fprintf(f, "%d\n", max_val) < 0) {
    perror("fprintf");

    return PBM_OUTPUT_ERROR;
  }

  if (fwrite(data, 1, width * height * num_channels * bytes_per_channel, f) !=
      width * height * num_channels * bytes_per_channel) {
    perror("fwrite");

    return PBM_OUTPUT_ERROR;
  }

  return PBM_NO_ERROR;
}
