# Image Convolution

This code sample aims to provide a simple piece of code, which takes an image and produces a visual result as well. For the convolution, LIBXSMM's DNN-domain is used. This sample code cannot use multiple threads (therefore OMP=0) or JIT code generation since parallelization and JIT-vectorization in the DNN-domain are per multiple images and channels respectively. JIT code is vectorized over image channels according to the native vector-width of the processor hence the sample code falls back to a high-level implementation. The code processes only a single image which consists of a single channel (eventually multiple times as per `nrepeat`).

**NOTE**: Multicore and JIT code can be only leveraged with code changes and input data that consists of multiple images or channels (a.k.a. "deep neural networks" or "deep learning"). Please note the collection of [DNN code samples](https://github.com/hfp/libxsmm/tree/master/samples/dnn), which achieves both of which.

The executable can run with the following arguments (all arguments are optional):

> iconv   [&lt;filename-in&gt;  [&lt;nrepeat&gt;  [&lt;kw&gt;  [&lt;kh&gt;]  [&lt;filename-out&gt;]]]]

For stable timing (benchmark), the key operation (convolution) may be repeated (`nrepeat`). Further, `kw` and `kh` can specify the kernel-size of the convolution. The `filename-in` and `filename-out` name MHD-files (see [Meta Image File I/O](https://github.com/hfp/libxsmm/blob/master/documentation/libxsmm_aux.md#meta-image-file-io)) used as input and output respectively. The `filename-in` may not exist, and specify the image resolution (`w`[x`h`] where the file `wxh.mhd` is generated in this case).

To load an image from a familiar format (JPG, PNG, etc.), one may save the raw data using for instance [IrfanView](http://www.irfanview.com/) and rely on a "header-only" MHD-file (plain text). This may look like:

```ini
NDims = 2
DimSize = 202 134
ElementType = MET_UCHAR
ElementNumberOfChannels = 1
ElementDataFile = mhd_image.raw
```

In the above case, a single channel (gray-scale) 202x134-image is described with pixel data stored separately (`mhd_image.raw`). Multi-channel images are expected to interleave the pixel data. The pixel type is per `libxsmm_mhd_elemtype` ([libxsmm_mhd.h](https://github.com/hfp/libxsmm/blob/master/include/libxsmm_mhd.h#L38)).

