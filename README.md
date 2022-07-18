# Image-Processing
- CMake based projects related to Image Processing
- C++14
- Unit tests written for Google Test framework

## Common
Static library containing shared functionality

### BitmapImage
- Loads image data from .bmp file

### CommandLineOption
- Encapsulates command line option and its parameters

### CommandLineParser
- Parses command line arguments

### Exception
- Exception for differentiating between project exceptions and standard exceptions

![Demo](Common/media/Exception.png)

### Image
- Contains ability to get and set particular pixel

![Demo](Common/media/Image.png)

### Pixel
- Contains conversion to RGB, RGBA or YCbCr

![Demo](Common/media/Pixel.png)

### RawImage
- Loads image data from RAW image file

### RGBAPixel
- Structure for encapsulating RGBA data

### RGBPixel
- Structure for encapsulating RGB data

### RuntimeException
- Exception for differentiating between project exceptions and standard exceptions

### Window
- Displaying images
- Arrow keys and WASD for moving image
- \+ and - for zoom in and out

### YCbCrPixel
- Structure for encapsulating YCbCr data

## Image Viewer
- Application for displaying raw and bmp images
- Uses strategy pattern to handle commandline arguments
    - **-h** or **--help**: displays help
    - **-f** or **--file**: bitmap image file to be displayed
    - **-f *\<width>* *\<height>*** or **--file *\<width>* *\<height>***: raw image file to be displayed followed by its width and height

    ![Demo](ImageViewer/media/ImageViewer.gif)
