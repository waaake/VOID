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
    Q_OBJECT

public:
    Effect(ImageOp* iop, const std::string& name, QObject* parent = nullptr);
    ~Effect();

    const std::string& Name() const { return m_Name; }
    void SetName(const std::string& name) { m_Name = name; }

    /**
     * @brief Set the Value on the param.
     * 
     * @param param Parameter name.
     * @param value The value to be set on the param, could be an std::string, int, float or bool.
     * @return bool true if the value was updated, else false.
     */
    bool SetValue(const std::string& param, ValueType value);

    /**
     * @brief Returns the Value from the parameter, if found with the provided name.
     * Else a Value bearing -1 is returned.
     * 
     * @param param Parameter name.
     * @return const ValueType& Value from the parameter.
     */
    const ValueType& Value(const std::string& param) const;

    [[nodiscard]] bool Enabled() const { return m_Enabled; }
    void SetEnabled(bool enable);

    Param* GetParam(const std::string& name) const { return m_Operator->GetParam(name); }
    ImageOp* ImageOperator() { return m_Operator; }

signals:
    void updated();

private:
    ImageOp* m_Operator;
    std::string m_Name;
    bool m_Enabled;
};

VOID_NAMESPACE_CLOSE

#endif // _IMAGE_EFFECTS_H
