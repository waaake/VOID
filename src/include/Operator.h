// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _BASE_OPERATOR_H
#define _BASE_OPERATOR_H

/* STD */
#include <memory>

/* Internal */
#include "Definition.h"
#include "PixReader.h"

VOID_NAMESPACE_OPEN

class ImageOp
{
public:
    virtual ~ImageOp() = default;
    virtual bool Evaluate(ImageRow& row) = 0;
};

typedef std::shared_ptr<ImageOp> SharedImageOp;

VOID_NAMESPACE_CLOSE

#endif // _BASE_OPERATOR_H
