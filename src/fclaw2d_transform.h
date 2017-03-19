/*
Copyright (c) 2012 Carsten Burstedde, Donna Calhoun
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef FCLAW2D_TRANSFORM_H
#define FCLAW2D_TRANSFORM_H

#include <forestclaw2d.h>
#include <fclaw2d_global.h>
#include <fclaw2d_clawpatch_options.h>

#ifdef __cplusplus
extern "C"
{
#if 0
}                               /* need this because indent is dumb */
#endif
#endif

/* Data structure for transformation data at non-aligned boundaries */
typedef struct fclaw2d_transform_data
{
    fclaw2d_patch_t *this_patch;
    fclaw2d_patch_t *neighbor_patch;
    int transform[9];
    int icorner;
    int based;      /* 1 for cell-centered (1 .. mx); 0 for nodes (0 .. mx) */
    int is_block_corner;
    int block_iface;   /* -1 for interior faces or block corners */

    fclaw2d_global_t *glob;

#if 0
    /* These don't seem to be used */
    int is_block_face;    /* not a corner */
    int fine_grid_pos;         /* Fine grid position (0 or 1) */
    int is_block_boundary;     /* is_block_face ? */
#endif
}
fclaw2d_transform_data_t;

#define FCLAW2D_TRANSFORM_FACE \
  FCLAW_F77_FUNC_(fclaw2d_transform_face,FCLAW2D_TRANSFORM_FACE)
void FCLAW2D_TRANSFORM_FACE (const int *i1, const int *j1,
                             int *i2, int *j2,
                             fclaw2d_transform_data_t** ptdata);

#define FCLAW2D_TRANSFORM_FACE_HALF \
  FCLAW_F77_FUNC_(fclaw2d_transform_face_half,FCLAW2D_TRANSFORM_FACE_HALF)
void FCLAW2D_TRANSFORM_FACE_HALF (const int *i1, const int *j1,
                                  int i2[], int j2[],
                                  fclaw2d_transform_data_t** ptdata);

#define FCLAW2D_TRANSFORM_CORNER \
  FCLAW_F77_FUNC_(fclaw2d_transform_corner,FCLAW2D_TRANSFORM_CORNER)
void FCLAW2D_TRANSFORM_CORNER (const int *i1, const int *j1,
                               int *i2, int *j2,
                               fclaw2d_transform_data_t** ptdata);

#define FCLAW2D_TRANSFORM_CORNER_HALF \
  FCLAW_F77_FUNC_(fclaw2d_transform_corner_half,FCLAW2D_TRANSFORM_CORNER_HALF)
void FCLAW2D_TRANSFORM_CORNER_HALF (const int *i1, const int *j1,
                                    int i2[], int j2[],
                                    fclaw2d_transform_data_t** ptdata);

#ifdef __cplusplus
#if 0
{                               /* need this because indent is dumb */
#endif
}
#endif

#endif /* !FCLAW2D_TRANSFORM_H */
