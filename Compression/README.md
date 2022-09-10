# Image Compression

- - Simplified loosy jpeg compression algorithm (DCT - Discrete Cosine Transform method)
- Parallelised using C++'s [std::thread](https://en.cppreference.com/w/cpp/thread/thread)

![Demo](media/Demo.png)

## TODO???

TODO

## Class model

TODO

## Command line options
### Display help
```
    ./Compression -h
    ./Compression --help
```
### Set Quality
```
    ./Compression -q <quality_from_0_to_100%>
    ./Compression --quality <quality_from_0_to_100%>
```
### Set image
```
    ./Compression -f <image>
    ./Compression --file <image>
    ./Compression -f <raw_image> <image_width> <image_height>
    ./Compression --file <raw_image>  <image_width> <image_height>
```