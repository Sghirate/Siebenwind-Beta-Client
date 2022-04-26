#pragma once

#include "Core/Minimal.h"
#include "Core/StringUtils.h"
#include <string>
#include <vector>

namespace Core
{

struct BaseConsoleVariable;
struct ConsoleCommand;
struct IConsoleListener;
struct Console
{
    static void Init();
    static void Shutdown();
    static void RegisterConsoleListener(IConsoleListener* a_listener);
    static void UnregisterConsoleListener(IConsoleListener* a_listener);
};
struct IConsoleListener
{
    virtual void OnConsoleVariableRegistered(BaseConsoleVariable* a_variable) {}
    virtual void OnConsoleVariableChanged(BaseConsoleVariable* a_variable) {}
    virtual void OnConsoleCommandRegistered(ConsoleCommand* a_command) {}
    virtual void OnConsoleCommandExcecuted(ConsoleCommand* a_command) {}
};

enum class ConsoleInteractionType
{
    Unknown  = 0,
    Command  = 1,
    Variable = 2,
};
enum class ConsoleFlags : u8
{
    None       = 0x00,
    Registered = 0x01,
    Persistent = 0x02,
    Disabled   = 0x04,
    Editor     = 0x08,
    Cheat      = 0x10,
};
inline ConsoleFlags operator|(ConsoleFlags lhs, ConsoleFlags rhs)
{
    return static_cast<ConsoleFlags>(static_cast<u8>(lhs) | static_cast<u8>(rhs));
}
inline ConsoleFlags operator&(ConsoleFlags lhs, ConsoleFlags rhs)
{
    return static_cast<ConsoleFlags>(static_cast<u8>(lhs) & static_cast<u8>(rhs));
}
typedef void (*ConsoleInit)(void);
typedef void (*ConsoleVariableChanged)(void);
typedef void (*ConsoleCommandExecuted)(void);
struct ConsoleInteraction
{
    const char* GetName() const { return m_name; }
    const char* GetDescription() const { return m_description; }
    ConsoleFlags GetFlags() const { return m_flags; }
    bool HasFlag(ConsoleFlags a_flag) const { return (m_flags & a_flag) == a_flag; }
    ConsoleInteractionType GetInteractionType() const { return m_type; }

protected:
    friend struct Console;

    ConsoleInteraction(ConsoleInteractionType a_type = ConsoleInteractionType::Unknown);
    ConsoleInteraction(
        ConsoleInteractionType a_type,
        const char* a_name,
        const char* a_description  = nullptr,
        ConsoleFlags a_flags       = ConsoleFlags::None,
        ConsoleInit a_initCallback = nullptr);
    virtual ~ConsoleInteraction();

    void Register();
    void Create(
        ConsoleInteractionType a_type,
        const char* a_name,
        const char* a_description  = nullptr,
        ConsoleFlags a_flags       = ConsoleFlags::None,
        ConsoleInit a_initCallback = nullptr);
    virtual void Init();
    virtual void Shutdown() {}

private:
    ConsoleInteractionType m_type = ConsoleInteractionType::Unknown;
    ConsoleInteraction* m_next    = nullptr;
    const char* m_name            = nullptr;
    const char* m_description     = nullptr;
    ConsoleFlags m_flags          = ConsoleFlags::None;
    ConsoleInit m_initCallback    = nullptr;
};
struct ConsoleCommand final : public ConsoleInteraction
{
    ConsoleCommand();
    ConsoleCommand(
        const char* a_name,
        ConsoleCommandExecuted a_callback,
        const char* a_description  = nullptr,
        ConsoleFlags a_flags       = ConsoleFlags::None,
        ConsoleInit a_initCallback = nullptr);
    ~ConsoleCommand();

protected:
    friend struct Console;
    void Create(
        const char* a_name,
        ConsoleCommandExecuted a_callback,
        const char* a_description  = nullptr,
        ConsoleFlags a_flags       = ConsoleFlags::None,
        ConsoleInit a_initCallback = nullptr);
    void Init() override;
    void Execute();

private:
    ConsoleCommandExecuted m_callback;
};
struct BaseConsoleVariable : public ConsoleInteraction
{
    std::string GetStringValue() const;

protected:
    friend struct Console;

    BaseConsoleVariable(
        const char* a_name,
        const char* a_description,
        ConsoleFlags a_flags                     = ConsoleFlags::None,
        ConsoleVariableChanged a_changedCallback = nullptr,
        ConsoleInit a_initCallback               = nullptr);
    virtual ~BaseConsoleVariable() {}

    virtual bool Save(std::string& out_string) const { return false; }
    virtual bool Load(const std::string& a_string) { return false; }

protected:
    ConsoleVariableChanged m_changedCallback;
};
template <typename TValue>
struct ConsoleVariable final : public BaseConsoleVariable
{
    ConsoleVariable(
        const char* a_name,
        TValue a_defaultValue,
        ConsoleFlags a_flags                     = ConsoleFlags::None,
        ConsoleVariableChanged a_changedCallback = nullptr,
        ConsoleInit a_initCallback               = nullptr);
    ConsoleVariable(
        const char* a_name,
        const char* a_description,
        TValue a_defaultValue,
        ConsoleFlags a_flags                     = ConsoleFlags::None,
        ConsoleVariableChanged a_changedCallback = nullptr,
        ConsoleInit a_initCallback               = nullptr);
    ~ConsoleVariable() {}

    bool Save(std::string& out_string) const override;
    bool Load(const std::string& a_string) override;

    const TValue& GetValue() const;
    void SetValue(const TValue& a_value);
    void ResetValue();

private:
    TValue m_defaultValue;
    TValue m_currentValue;
};

template <typename TValue>
ConsoleVariable<TValue>::ConsoleVariable(
    const char* a_name,
    TValue a_defaultValue,
    ConsoleFlags a_flags /* = ConsoleFlags::None*/,
    ConsoleVariableChanged a_changedCallback /* = nullptr*/,
    ConsoleInit a_initCallback /* = nullptr*/)
    : BaseConsoleVariable(a_name, nullptr, a_flags, a_changedCallback, a_initCallback)
    , m_defaultValue(a_defaultValue)
    , m_currentValue(a_defaultValue)
{
}
template <typename TValue>
ConsoleVariable<TValue>::ConsoleVariable(
    const char* a_name,
    const char* a_description,
    TValue a_defaultValue,
    ConsoleFlags a_flags /*= ConsoleFlags::None*/,
    ConsoleVariableChanged a_changedCallback /*= nullptr*/,
    ConsoleInit a_initCallback /*= nullptr*/)
    : BaseConsoleVariable(a_name, a_description, a_flags, a_changedCallback, a_initCallback)
    , m_defaultValue(a_defaultValue)
    , m_currentValue(a_defaultValue)
{
}
template <typename TValue>
const TValue& ConsoleVariable<TValue>::GetValue() const
{
    return m_currentValue;
}
template <typename TValue>
void ConsoleVariable<TValue>::SetValue(const TValue& a_value)
{
    m_currentValue = a_value;
    if (m_changedCallback)
        m_changedCallback();
}
template <typename TValue>
void ConsoleVariable<TValue>::ResetValue()
{
    m_currentValue = m_defaultValue;
}
template <typename TData>
bool ConsoleVariable<TData>::Save(std::string& out_string) const
{
    return ToString(m_currentValue, out_string);
}
template <typename TData>
bool ConsoleVariable<TData>::Load(const std::string& a_string)
{
    return FromString(a_string, m_currentValue);
}

} // namespace Core
