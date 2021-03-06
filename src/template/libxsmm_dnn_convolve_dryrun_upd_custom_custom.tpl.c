/******************************************************************************
 ** Copyright (c) 2016-2017, Intel Corporation                                **
 ** All rights reserved.                                                      **
 **                                                                           **
 ** Redistribution and use in source and binary forms, with or without        **
 ** modification, are permitted provided that the following conditions        **
 ** are met:                                                                  **
 ** 1. Redistributions of source code must retain the above copyright         **
 **    notice, this list of conditions and the following disclaimer.          **
 ** 2. Redistributions in binary form must reproduce the above copyright      **
 **    notice, this list of conditions and the following disclaimer in the    **
 **    documentation and/or other materials provided with the distribution.   **
 ** 3. Neither the name of the copyright holder nor the names of its          **
 **    contributors may be used to endorse or promote products derived        **
 **    from this software without specific prior written permission.          **
 **                                                                           **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       **
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT         **
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR     **
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT      **
 ** HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,    **
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED  **
 ** TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR    **
 ** PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    **
 ** LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING      **
 ** NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS        **
 ** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.              **
 ******************************************************************************/
/* Evangelos Georganas (Intel Corp.)
 ******************************************************************************/
#if !defined(_OPENMP)
int ltid;
#endif
int block_img = 4;

int block_j = 14;

handle->block_upd_ofm = 8;
handle->block_upd_ifm = 8;

block_j = handle->ofh ;

if ( handle->ofh == 56 ) {
  /* Pixel block is 196 Kbytes */
 handle->block_upd_ofm = handle->blocksofm;
 handle->block_upd_ifm = 1;

}

if ( handle->ofh == 28 ) {
  /* Pixel block is 49 Kbytes */
 handle->block_upd_ofm = 3;
 handle->block_upd_ifm = 3;
}

if ( handle->ofh == 14 || handle->ofh == 28 || handle->ofh == 56 ) {
  /* Pixel block is 12.25 Kbytes */
 handle->block_upd_ofm = 8;
 handle->block_upd_ifm = 16;
}

if ( handle->ofh == 7 ) {
  /* Pixel block is 3.06 Kbytes */
 handle->block_upd_ofm = 8;
 handle->block_upd_ifm = 16;
}


if ( handle->ofh == 28 || handle->ofh == 56 ) {
  /* Pixel block is 12.25 Kbytes */
  handle->block_upd_ofm = 32;
  handle->block_upd_ifm = 16;
}

#if defined(_OPENMP)
# pragma omp parallel num_threads(handle->desc.threads)
#else
for (ltid = 0; ltid < handle->desc.threads; ltid++)
#endif
{
#if defined(_OPENMP)
  int ltid = omp_get_thread_num();
#endif
  int img, imgb, ofm1, ifm1, num_ofw_strips, num_ofh_strips, oi_, oj_, oi__, oj__,ii_, ij_, kh, kw, ofm1ifm1, ki, kj, imgifm1, local_entries, stride_w, stride_h ;
  int i, j, ofm1ifm1img;

  /* number of tasks that could be run in parallel */
  const int work = handle->blocksifm*handle->blocksofm;
  /* compute chunck size */
  const int chunksize = (work % handle->desc.threads == 0) ? (work / handle->desc.threads) : (work / handle->desc.threads) + 1;
  /* compute thr_begin and thr_end */
  const int thr_begin = (ltid * chunksize < work) ? (ltid * chunksize) : work;
  const int thr_end = ((ltid + 1) * chunksize < work) ? ((ltid + 1) * chunksize) : work;
  int ifm2;
  /* number of tasks that could be run in parallel */
  const int transpose_work = handle->desc.N*handle->blocksifm;
  /* compute chunck size */
  const int transpose_chunksize = (transpose_work % handle->desc.threads == 0) ? (transpose_work / handle->desc.threads) : (transpose_work / handle->desc.threads) + 1;
  /* compute thr_begin and thr_end */
  const int transpose_thr_begin = (ltid * transpose_chunksize < transpose_work) ? (ltid * transpose_chunksize) : transpose_work;
  const int transpose_thr_end = ((ltid + 1) * transpose_chunksize < transpose_work) ? ((ltid + 1) * transpose_chunksize) : transpose_work;

  /* number of tasks that could be run in parallel */
  const int img_parallel_work = handle->blocksifm*handle->blocksofm;
  /* compute chunck size */
  const int img_parallel_chunksize = (img_parallel_work % handle->desc.threads == 0) ? (img_parallel_work / handle->desc.threads) : (img_parallel_work / handle->desc.threads) + 1;
  /* compute thr_begin and thr_end */
  const int img_parallel_thr_begin = (ltid * img_parallel_chunksize < img_parallel_work) ? (ltid * img_parallel_chunksize) : img_parallel_work;
  const int img_parallel_thr_end = ((ltid + 1) * img_parallel_chunksize < img_parallel_work) ? ((ltid + 1) * img_parallel_chunksize) : img_parallel_work;
  /* number of tasks that could be run in parallel */
  const int reduce_work = handle->blocksofm*handle->blocksifm*handle->desc.R*handle->desc.S*handle->ifmblock*handle->ofmblock;
  /* compute chunck size */
  const int reduce_chunksize = (reduce_work % handle->desc.threads == 0) ? (reduce_work / handle->desc.threads) : (reduce_work / handle->desc.threads) + 1;
  /* compute thr_begin and thr_end */
  const int reduce_thr_begin = (ltid * reduce_chunksize < reduce_work) ? (ltid * reduce_chunksize) : reduce_work;
  const int reduce_thr_end = ((ltid + 1) * reduce_chunksize < reduce_work) ? ((ltid + 1) * reduce_chunksize) : reduce_work;

  const int copywork = handle->desc.N*handle->blocksifm;
  const int copychunksize = (copywork % handle->desc.threads == 0) ? (copywork / handle->desc.threads) : (copywork / handle->desc.threads) + 1;
  const int copy_thr_begin = (ltid * copychunksize < copywork) ? (ltid * copychunksize) : copywork;
  const int copy_thr_end = ((ltid + 1) * copychunksize < copywork) ? ((ltid + 1) * copychunksize) : copywork;

  int total_calls, aux;
  int n_code_segments = 0;
  int mark_ifm_init, mark_ifm_close, mark_img_init;
  int *tmp_expanded_stream, tmp_stream_index;
  segment_t *encoded_code_segments;
  int expanded_size;
  int stretch_of_convs;
  int encoded_stream_index;
  int lookahead_index;

  /* Arrays of stream indices */
  int *compute_indices;
  int *trans_indices;
  char *kernel_variant;
  int *init_indices, *copy_indices;

  int padded_w, padded_h;
  stride_w = handle->desc.v;
  stride_h = handle->desc.u;

  if (handle->padding_flag == 1) {
    padded_h = handle->ifhp + 2 * handle->desc.pad_h;
    padded_w = handle->ifwp + 2 * handle->desc.pad_w + handle->qfma_input_pad;
  } else {
    if (handle->resize_input == 1) {
      padded_h = handle->ifhp_resized;
      padded_w = handle->ifwp_resized + handle->qfma_input_pad;
      stride_w = 1;
      stride_h = 1;
    } else {
      padded_h = handle->ifhp;
      padded_w = handle->ifwp + handle->qfma_input_pad;
    }
  }

  kh = handle->desc.R;
  kw = handle->desc.S;

  num_ofw_strips = 1;
  local_entries = 0;
  int KW, ofmb, ifmb, ojb;

  if ( handle->ifmblock != 1  ) {
    KW = kw;
  } else {
    KW = 1;
  }

  int my_ofm_start;
  int my_ofm_end;
  int my_ifm_start;
  int my_ifm_end;
  int ifmpt, ofmpt;
  int group_size = handle->desc.threads/handle->weight_copies;
  int tile_id = ltid/group_size;
  int tiles = handle->weight_copies;
  int img_per_tile = handle->desc.N/tiles;
  int my_img_start = LIBXSMM_MIN( tile_id * img_per_tile, handle->desc.N);
  int my_img_end = LIBXSMM_MIN( (tile_id+1) * img_per_tile, handle->desc.N);

  int my_in_tile_id = ltid % group_size;
  int ifms_per_thread = handle->blocksifm/group_size;
  my_ifm_start = LIBXSMM_MIN( my_in_tile_id * ifms_per_thread, handle->blocksifm  );
  my_ifm_end = LIBXSMM_MIN( (my_in_tile_id+1) * ifms_per_thread, handle->blocksifm  );
  my_ofm_start = 0;
  my_ofm_end = handle->blocksofm;

  int block_ofm = handle->block_upd_ofm; 
  int block_ifm = handle->block_upd_ifm;

  for (img = my_img_start; img < my_img_end; img += handle->blocksimg_blocking) {
    for (ofmb = my_ofm_start; ofmb < my_ofm_end; ofmb += block_ofm) {
      for (ifmb = my_ifm_start; ifmb < my_ifm_end; ifmb += block_ifm) {
        for (ojb = 0; ojb < handle->ofh; ojb += handle->upd_ofh_rb) {
          for (ofm1 = ofmb; ofm1 < LIBXSMM_MIN(ofmb+block_ofm, my_ofm_end); ofm1++ ) {
            for (ifm1 = ifmb; ifm1 < LIBXSMM_MIN(ifmb+block_ifm, my_ifm_end); ifm1++) {
              for (oj_ = ojb; oj_ < LIBXSMM_MIN(ojb+handle->upd_ofh_rb,handle->ofh); oj_ += handle->upd_ofh_rb) {
                for (oi__=0; oi__<num_ofw_strips; ++oi__) {
                  for (kj=0; kj < kh; ++kj) {
                    for (ki=0; ki < KW; ++ki) {
                      local_entries += 3;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }


  /* Alocate auxiliary data structures for index jitting  */
  handle->n_entries_upd[ltid] = local_entries/3;
  compute_indices = (int*) libxsmm_aligned_malloc( (local_entries+3) * sizeof(int), 64);
  handle->compute_upd_indices_ptrs[ltid] = compute_indices;
  kernel_variant = (char*) libxsmm_aligned_malloc( (local_entries/3) * sizeof(char), 64);
  handle->kernel_upd_variant_ptrs[ltid] = kernel_variant;
  handle->n_upd_code_segments[ltid] = n_code_segments;
  expanded_size = local_entries/3 + n_code_segments;

  local_entries = 0;

  /* Second run to compute actual indices */
  if ( !(((handle->desc.C == 512) && (handle->desc.K == 1024)) || ((handle->desc.C == 1024) &&  (handle->desc.K == 256)))  ) {
    for (ofmb = my_ofm_start; ofmb < my_ofm_end; ofmb += block_ofm) {
      for (ifmb = my_ifm_start; ifmb < my_ifm_end; ifmb += block_ifm) {
        for (ofm1 = ofmb; ofm1 < LIBXSMM_MIN(ofmb+block_ofm, my_ofm_end); ofm1++ ) {
          for (ifm1 = ifmb; ifm1 < LIBXSMM_MIN(ifmb+block_ifm, my_ifm_end); ifm1++) {
            for (img = my_img_start; img < my_img_end; img += handle->blocksimg_blocking) {
              for (ojb = 0; ojb < handle->ofh; ojb += handle->upd_ofh_rb) { 
                for (oj_ = ojb; oj_ < LIBXSMM_MIN(ojb+handle->upd_ofh_rb,handle->ofh); oj_ += handle->upd_ofh_rb) {
                  for (oi__=0; oi__<num_ofw_strips; ++oi__) {
                    for (kj=0; kj < kh; ++kj) {
                      for (ki=0; ki < KW; ++ki) {

                        oi_=oi__*handle->upd_ofw_rb;
                        ii_ = oi_*stride_w;
                        ij_ = oj_*stride_h;

                        compute_indices[local_entries] =  ( ( ( ( ( (img *  handle->blocksifm) +  ifm1) * padded_h )  +  (ij_+kj)) * handle->ifmblock) ) * padded_w  + (ii_ + ki) ;
                        compute_indices[local_entries+1] = (( (ofm1 *  handle->blocksifm )  +  ifm1 ) * handle->desc.R * handle->desc.S *  handle->ifmblock *  handle->ofmblock + kj * handle->desc.S *  handle->ifmblock *  handle->ofmblock + ki * handle->ifmblock *  handle->ofmblock)  * handle->weight_copies;

                        compute_indices[local_entries+2] = ( ( ( ( ( (img *  handle->blocksofm) +  ofm1) *  handle->ofhp )  +  oj_ ) * handle->ofwp)  +  oi_ ) *  handle->ofmblock;
                        local_entries += 3;
                      }

                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  } else {
    for (ifmb = my_ifm_start; ifmb < my_ifm_end; ifmb += block_ifm) {
      for (ofmb = my_ofm_start; ofmb < my_ofm_end; ofmb += block_ofm) {
        for (ifm1 = ifmb; ifm1 < LIBXSMM_MIN(ifmb+block_ifm, my_ifm_end); ifm1++) {
          for (ofm1 = ofmb; ofm1 < LIBXSMM_MIN(ofmb+block_ofm, my_ofm_end); ofm1++ ) {
            for (img = my_img_start; img < my_img_end; img += handle->blocksimg_blocking) {
              for (ojb = 0; ojb < handle->ofh; ojb += handle->upd_ofh_rb) { 
                for (oj_ = ojb; oj_ < LIBXSMM_MIN(ojb+handle->upd_ofh_rb,handle->ofh); oj_ += handle->upd_ofh_rb) {
                  for (oi__=0; oi__<num_ofw_strips; ++oi__) {
                    for (kj=0; kj < kh; ++kj) {
                      for (ki=0; ki < KW; ++ki) {

                        oi_=oi__*handle->upd_ofw_rb;
                        ii_ = oi_*stride_w;
                        ij_ = oj_*stride_h;

                        compute_indices[local_entries] =  ( ( ( ( ( (img *  handle->blocksifm) +  ifm1) * padded_h )  +  (ij_+kj)) * handle->ifmblock) ) * padded_w  + (ii_ + ki) ;
                        compute_indices[local_entries+1] = (( (ofm1 *  handle->blocksifm )  +  ifm1 ) * handle->desc.R * handle->desc.S *  handle->ifmblock *  handle->ofmblock + kj * handle->desc.S *  handle->ifmblock *  handle->ofmblock + ki * handle->ifmblock *  handle->ofmblock)  * handle->weight_copies;
                        compute_indices[local_entries+2] = ( ( ( ( ( (img *  handle->blocksofm) +  ofm1) *  handle->ofhp )  +  oj_ ) * handle->ofwp)  +  oi_ ) *  handle->ofmblock;
                        local_entries += 3;
                      }

                    }
                  }
                }
              }
            }
          }
        }
      }
    }

  }

  compute_indices[local_entries] = 0;
  compute_indices[local_entries+1] = 0;
  compute_indices[local_entries+2] = 0;
  total_calls = local_entries/3;
}

