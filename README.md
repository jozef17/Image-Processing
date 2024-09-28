# Image Processing
- CMake based projects related to Image Processing
- Written in C++
- Parallelised using [std::thread](https://en.cppreference.com/w/cpp/thread/thread) and / or [Nvidia CUDA](https://developer.nvidia.com/cuda-zone)
- Unit tests written for Google Test framework
- All project support following input image formats:
    - RAW
    - Bitmap (.bmp)
    - (WIP)Joint Photographic Experts Group (.jpg)
        - **NOTE:** not the whole standard is supported, scroll down for list of un/supported features.
    - Portable Network Graphics (.png) 
        - **NOTE:** not the whole standard is supported, scroll down for list of un/supported features.
- Projects overview:
    - [Common library (shared functionality)](Common)
    - [DCT Image compression](Compression)
    - [Convolution Filter](ConvolutionFilter)
    - [Image Viewer](ImageViewer)

## [Common](Common)
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

### [(WIP) JpegImage](Common)
- jpeg image decoder
- **Un/supported features:**
    - TODO
 
### Pixel
- Represents pixel
- Enables conversion to:
   - RGB 
   - RGBA 
   - YCbCr

![Demo](Common/media/Pixel.png)

### [PngImage](Common)
- PNG image decoder
- **Un/supported features:**
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
- Keyboard and joystick / gamepad inouts are supported
- Arrow keys, WASD and joystick for moving image
- Zoom in and out:
    - \+ and RT (Right Trigger) to zoom in;
    - - and LT (Left Trigger) to zoom out;

## [Compression](Compression)
- Simplified loosy jpeg compression algorithm (DCT - Discrete Cosine Transform method)
- Parallelised using C++'s [std::thread](https://en.cppreference.com/w/cpp/thread/thread)

### 3% Quality
![Demo](Compression/media/3.PNG)
### 10% Quality
![Demo](Compression/media/10.PNG)
### 95% Quality
![Demo](Compression/media/95.PNG)

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
