# Common
Static library containing shared functionality

## Overview

### BitmapLoader
- Loads image data from bitmap (.bmp) file

### BitStream
- Simplifies access to bits from byte arrays
- Handles moving in stream

### CommandLineParser
- Parses command line arguments
- Creates key-value pairs of command line option and its argument list

### Exception (and RuntimeException)
- Exception for differentiating between project exceptions and standard exceptions

![Demo](media/Exception.png)

### GifLoader
- Loads gif image
- **Limmited support**
    - Animation is not supported
- Referencess:
    - [gif blog](https://www.matthewflickinger.com/lab/whatsinagif/index.html)

### Image
- Contains ability to get and set particular pixel

### ImageLoader
- Loads requested image
- Checks content to determine which image format is used

### [JpegLoader](Common)
- jpeg image decoder
- **Supported features:**
    - baseline DCT is supported (no progressive DCT))
    - 8 bit resolution is supported (no 16bit)
    - 3 color components (no grayscale or CMYK)
    - 4:4:4 and 4:2:0 chroma subsampling

### Pixel
- Represents pixel
- Enables conversion to:
   - RGB 
   - RGBA 
   - YCbCr

![Demo](media/Pixel.png)

### [PngLoader]
- PNG image decoder
- Scroll down for more details

### RawLoader
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

## JpegLoader
- jpeg image decoder
- Referencess:
- - [Anatomy of a JPEG](https://www.ccoderun.ca/programming/2017-01-31_jpeg/)
  - [TODO](TODO) 
- **Supported features:**
    - baseline DCT is supported (no progressive DCT))
    - 8 bit resolution is supported (no 16bit)
    - 3 color components (no grayscale or CMYK)
    - 4:4:4 and 4:2:0 chroma subsampling

TODO: add class model and decoding steps

## PngLoader
- PNG image decoder
- Referencess:
    - [PNG Specification](https://www.w3.org/TR/PNG/)
    - [RFC 1950 "ZLIB Compressed Data Format Specification"](https://datatracker.ietf.org/doc/html/rfc1950)
    - [RFC 1951 "DEFLATE Compressed Data Format Specification"](https://datatracker.ietf.org/doc/html/rfc1951)
- **Unsupported features:**
    - Indexed collor is not supported
    - Interface method [ADAM7](https://en.wikipedia.org/wiki/Adam7_algorithm) is not supported
    - Only 8 bit channels are supported
    - Zlib/Inflate block type 00 is not supported (uncompressed block)

### Class Model

![Demo](media/Png.png)

### Decoding

#### Pre-processing

Pre processing step consist of loading png data chunks, checking for compatibility and re-creating bit stream:
1. Load chunks (until IEND chunk is loaded)
    1. Check Header for compatibility (IHDR chunk)
    2. Append data to bitstream (IDAT chink)
2. Decode bitstream

![Demo](media/PngLoadData.png)

```
open file
LOOP
   load chunk
   IF chunk type is "IHDR"
       process header and check for compatibility
   ELSE IF chunk type is "IDAT"
       append bit stream with chunk data
   ELSE IF chunk type is "IEND"
       BREAK
   ENDIF
ENDLOOP
close file
decode data
```

#### Decoding (ZLIB / INFLATE)

Bit stream starts with two byte header. 
Header is followed by series of encoded blocks.
Each block starts with **3 bit** header:
- 1 bit is last flag (this last block)
- 2 tib type:
    - type 00 - no compression (**note: not yet supported**)
    - type 01 - encoded with fixed "huffman" codes
    - type 10 - encoded with dynamic huffman codes
    - type 11 - error / invalid type

[[DEFLATE specification, section  3.2.3.]](https://datatracker.ietf.org/doc/html/rfc1951#section-3.2.3)

![Demo](media/PngInflate.png)

**Block Type 10** 

Block type 10 start with its own header:
- 5 bit hlit (hlit + 257 literal / length (ll) codes)
- 5 bit hdist (hdist + 1 distance codes)
- 4 bit hclen (hclen + 4 code lengths)

After header follows (hclen + 4) \* 3 bites of code lengths used for decoding two alphabets (literal / length alphabet and distance alphabet).
Folowed by encoded alphabets. 

[[DEFLATE specification, section  3.2.7.]](https://datatracker.ietf.org/doc/html/rfc1951#section-3.2.7)

**Decoding block types 01 and 10**

To decode data, literal / length code is decoded first according to ll alphabet.
- Codes less up to 255 (inclusive) represent raw data
- Code 256 represents end-of-block
- Remaining codes represent length

If length code was decoded, code is decoded into actuall length (length codes represent range of lengths, additional bits may be needed to get length).
Get and decode distance. Copy **\<lengt\>** bytes from end-of-decoded-data minus **\<distance\>** to result.

![Demo](media/PngDecode.png)

#### Post-processing

Last step to get pixels is reverse filterinf step.
Every image line starts with one byte filtration type
(png supports 5 filters - none, sub, up, average, paeth). [[PNG specification, section 9.2.]](https://www.w3.org/TR/PNG/#9Filters)
