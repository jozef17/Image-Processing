#pragma once

#ifndef MESSAGE_CONST_HPP__
#define MESSAGE_CONST_HPP__

#include <string>

const std::string HELP_MESSAGE =
std::string("Usage: ImageViewer [OPTION...] [FILE...]\n") +
std::string("Displays image passed as input\n") +
std::string("Mandatory arguments to long options are mandatory for short options too.\n") +
std::string("  -f. --file                 display image passed as argument. For raw images, image file is followed\n") +
std::string("                             by image width and image height.\n") +
std::string("  -h, --help                 display this help and exit");

const std::string INVALID_INPUT_MESSAGE =
std::string("Usage: ImageViewer [--file] <filepath>\n") +
std::string("       ImageViewer [--file] <filepath> <image-width> <image-height>\n") +
std::string("       ImageViewer [--help]");

#endif /* MESSAGE_CONST_HPP__ */