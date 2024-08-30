#include "transpose.h"

/* The naive transpose function as a reference. */
void transpose_naive(int n, int blocksize, int *dst, int *src)
{
  for (int x = 0; x < n; x++)
  {
    for (int y = 0; y < n; y++)
    {
      dst[y + x * n] = src[x + y * n];
    }
  }
}

/* Implement cache blocking below. You should NOT assume that n is a
 * multiple of the block size. */
void transpose_blocking(int n, int blocksize, int *dst, int *src)
{
  // YOUR CODE HERE
  for (int start_x = 0; start_x < n; start_x += blocksize)
  {
    for (int start_y = 0; start_y < n; start_y += blocksize)
    {
      int edge_x = start_x + blocksize;
      if (edge_x > n)
        edge_x = n;
      int edge_y = start_y + blocksize;
      if (edge_y > n)
        edge_y = n;
      for (int x = start_x; x < edge_x; ++x)
      {
        for (int y = start_y; y < edge_y; ++y)
        {
          dst[y + x * n] = src[x + y * n];
        }
      }
    }
  }
}
