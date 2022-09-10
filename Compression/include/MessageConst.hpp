#pragma once

#ifndef MESSAGE_CONST_HPP__
#define MESSAGE_CONST_HPP__

#include <string>

// TODO
const std::string HELP_MESSAGE =
std::string("Usage: ImageViewer [OPTION...] [FILE...]\n") +
std::string("Applyies loosy copmresion algorithm on input image\n") +
std::string("Mandatory arguments to long options are mandatory for short options too.\n") +
std::string("  -f. --file                 display image passed as argument. For raw images, image file is followed\n") +
std::string("                             by image width and image height.\n") +
std::string("  -q. --quality              TODO\n") +
std::string("  -h, --help                 display this help and exit");

const std::string INVALID_INPUT_MESSAGE =
std::string("Usage: Compression [--file] <filepath>\n") +
std::string("       Compression [--file] <filepath> [--quality] <quality>\n") +
std::string("       ImageViewer [--help]");

#endif /* MESSAGE_CONST_HPP__ */