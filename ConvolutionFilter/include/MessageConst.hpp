#pragma once

#ifndef MESSAGE_CONST_HPP__
#define MESSAGE_CONST_HPP__

#include <string>

const std::string HELP_MESSAGE =
std::string("Usage: ConvolutionFilter [OPTION...]\n") +
std::string("Applyies convolution filter on input image\n") +
std::string("Mandatory arguments to long options are mandatory for short options too.\n") +
std::string("  -f, --file                 Apply convolution filter for given image. Image file is specified as option.\n") +
std::string("                             For raw images, image file is followed by image width and image height. Result\n") +
std::string("                             image is displayed in window\n") +
std::string("  -h, --help                 Display this help and exit") +
std::string("  -k, --kernel               Sets convolution kernel. Either by specifying name or passing file as argument.\n") +
std::string("                             If this argument is not specified, the identity kernel is used by default.\n") +
std::string("                             Supported kernels include (case insensitive):\n") +
std::string("                                - Identity\n") +
std::string("                                - Sharpen\n") +
std::string("                                - BoxBlur\n") +
std::string("                                - Gaussian\n") +
std::string("                                - Gaussian5\n") +
std::string("                                - EdgeDetectionH\n") +
std::string("                                - EdgeDetectionV\n") +
std::string("                                - Sobel\n") +
std::string("                                - Laplacian\n") +
std::string("                                - Sharpen\n") +
std::string("  -p, --print                Prints kernel specified by --kernel option");

const std::string INVALID_INPUT_MESSAGE =
std::string("Usage: ConvolutionFilter [--file] <filepath> [--kernel] <kernel>\n") +
std::string("       ConvolutionFilter [--file] <filepath> <image-width> <image-height> [--kernel] <kernel>\n") +
std::string("       ConvolutionFilter [--print] [--kernel] <kernel>\n") +
std::string("       ConvolutionFilter [--help]");

#endif /* MESSAGE_CONST_HPP__ */