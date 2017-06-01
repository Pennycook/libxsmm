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
#include "libxsmm_dnn_dryruns.h"
#include "libxsmm_main.h"
#include <libxsmm.h>
#include <stdlib.h>
#include <string.h>

#if defined(_OPENMP)
# include <omp.h>
#endif


LIBXSMM_API libxsmm_dnn_err_t libxsmm_dnn_perform_fwd_dryrun_direct_custom_custom( libxsmm_dnn_layer* handle ) {
  
  libxsmm_dnn_err_t status = LIBXSMM_DNN_SUCCESS;

  /* check if we have a kernel JITed */
  if (handle->code_fwd[0].xconv.sconv == 0) {
    /* In these case we run fallback code so we do not support thread private jitting */
    status = LIBXSMM_DNN_WARN_FALLBACK;
  }
  else {
    if (handle->datatype == LIBXSMM_DNN_DATATYPE_F32 && handle->datatype_itm == LIBXSMM_DNN_DATATYPE_F32 ) {
      if (handle->desc.N*handle->blocksofm >= handle->desc.threads) {
        if (handle->padding_flag == 1) {
          /* FIXME: For now support only physical padding  */
          status = LIBXSMM_DNN_ERR_INVALID_PADDING;
        } else {
# include "template/libxsmm_dnn_convolve_dryrun_fwd_custom_custom.tpl.c"
        }
      }
      else {
        if (handle->padding_flag == 1) {
          /* FIXME: For now support only physical padding  */
          status = LIBXSMM_DNN_ERR_INVALID_PADDING;
        } else {
# include "template/libxsmm_dnn_convolve_dryrun_fwd_custom_custom_img_par.tpl.c"
        }
      }
    } else {
      status = LIBXSMM_DNN_ERR_UNSUPPORTED_DATATYPE;
      return status;
    }
  }

  return status;
}


LIBXSMM_API libxsmm_dnn_err_t libxsmm_dnn_perform_fwd_dryrun_direct( libxsmm_dnn_layer* handle ) { 

  /* Switch based on the format to use the correct dryrun */
  return libxsmm_dnn_perform_fwd_dryrun_direct_custom_custom( handle );
}