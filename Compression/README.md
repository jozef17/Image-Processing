# Image Compression

- Simplified loosy compression algorithm (DCT - Discrete Cosine Transform method)
    - simmilar to what jpeg uses
- Parallelised using C++'s [std::thread](https://en.cppreference.com/w/cpp/thread/thread)

![Demo](media/Demo.png)

## Workflow
![Demo](media/CompressionWorkflow.png)

- TODO Describe workflow
- TODO FORMULA DCT
- TODO FORMULA IDCT
- TODO FORMULA Compression
- TODO FORMULA De-Compression

## Class model
![Demo](media/CompressionClass.png)

## Command line options
### Display help
```cmd
    ./Compression -h
    ./Compression --help
```
### Set Quality
```cmd
    ./Compression -q <quality_from_0_to_100%>
    ./Compression --quality <quality_from_0_to_100%>
```
### Set image
```cmd
    ./Compression -f <image>
    ./Compression --file <image>
    ./Compression -f <raw_image> <image_width> <image_height>
    ./Compression --file <raw_image>  <image_width> <image_height>
```