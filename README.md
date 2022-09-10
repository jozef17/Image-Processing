# Image Processing
- CMake based projects related to Image Processing
- C++
- Unit tests written for Google Test framework
- All project support following image formats:
    - RAW
    - Bitmap (.bmp)
    - Portable Network Graphics (.png) 
        - **NOTE:** not whole standard is supported, scroll down for list of unsupported features.

## Common
Static library containing shared functionality

### BitmapImage
- Loads image data from bitmap (.bmp) file

### CommandLineParser
- Parses command line arguments
- Creates key-value pairs of command line option and its argument list

### Exception (and RuntimeException)
- Exception for differentiating between project exceptions and standard exceptions

![Demo](Common/media/Exception.png)

### Image
- Contains ability to get and set particular pixel
- Contains image starting position information:
    - first Pixel being top left or
    - first Pixel being bottom left

![Demo](Common/media/Image.png)

### Pixel
- Represents pixel
- Enables conversion to:
   - RGB 
   - RGBA 
   - YCbCr

![Demo](Common/media/Pixel.png)

### [PngImage](Common)
- PNG image decoder
- Referencess:
    - [PNG Specification](https://www.w3.org/TR/PNG/)
    - [RFC 1950 "ZLIB Compressed Data Format Specification"](https://datatracker.ietf.org/doc/html/rfc1950)
    - [RFC 1951 "DEFLATE Compressed Data Format Specification"](https://datatracker.ietf.org/doc/html/rfc1951)
- **Unsupported features:**
    - Only RGB and RGBA pixel formats are supported
    - Interface method [ADAM7](https://en.wikipedia.org/wiki/Adam7_algorithm) is not supported
    - Only 8 bit channels are supported
    - Zlib/Inflate block type 00 is not supported (uncompressed block)

![Demo](Common/media/Png.png)

### RawImage
- Loads image data from RAW image file

### Window
- Displaying images based on their starting position:
    - First Pixel being top left
    - First Pixel being bottom left
- Arrow keys and WASD for moving image
- \+ and - for zoom in and out

## [Convolution Filter](ConvolutionFilter)
- Application for applying convolution filter on input image
- Parallelised using:
    - [Nvidia CUDA](https://developer.nvidia.com/cuda-zone) (if supported) or
    - C++'s [std::thread](https://en.cppreference.com/w/cpp/thread/thread)
- Displays result in a winfow
- Uses strategy pattern to handle commandline arguments

### Sharpen
![Demo](ConvolutionFilter/media/SHARPEN.png)

### Sobel
![Demo](ConvolutionFilter/media/SOBEL.png)

## [Image Viewer](ImageViewer)
- Application for displaying raw, bmp and png images
- Uses strategy pattern to handle commandline arguments

![Demo](ImageViewer/media/ImageViewer.gif)

