// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _IMAGE_EFFECTS_H
#define _IMAGE_EFFECTS_H

/* Internal */
#include "Definition.h"
#include "Operator.h"
#include "VoidObjects/VoidObject.h"

VOID_NAMESPACE_OPEN

class VOID_API Effect : public VoidObject
{
public:
    Effect(ImageOp* iop, const std::string& name, QObject* parent = nullptr);
    ~Effect();

    const std::string& Name() const { return m_Name; }
    void SetName(const std::string& name) { m_Name = name; }
    Param* GetParam(const std::string& name) const { return m_Operator->GetParam(name); }
    ImageOp* ImageOperator() { return m_Operator; }

private:
    ImageOp* m_Operator;
    std::string m_Name;
};

VOID_NAMESPACE_CLOSE

#endif // _IMAGE_EFFECTS_H
