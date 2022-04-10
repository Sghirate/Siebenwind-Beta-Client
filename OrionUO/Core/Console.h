#pragma once

#include "Core/Minimal.h"
#include <string>

namespace Core
{

struct ConsoleVariable;
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
    virtual void OnConsoleVariableRegistered(ConsoleVariable* a_variable) {}
    virtual void OnConsoleVariableChanged(ConsoleVariable* a_variable) {}
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
ConsoleFlags operator|(ConsoleFlags lhs, ConsoleFlags rhs)
{
    return static_cast<ConsoleFlags>(static_cast<u8>(lhs) | static_cast<u8>(rhs));
}
ConsoleFlags operator&(ConsoleFlags lhs, ConsoleFlags rhs)
{
    return static_cast<ConsoleFlags>(static_cast<u8>(lhs) & static_cast<u8>(rhs));
}
typedef void (*ConsoleInit)(void);
typedef void (*ConsoleVariableChanged)(const char* a_oldValue, const char* a_newValue);
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
struct ConsoleCommand : ConsoleInteraction
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
struct ConsoleVariable : ConsoleInteraction
{
    ConsoleVariable(
        const char* a_name, const char* a_defaultValue, ConsoleFlags a_flags = ConsoleFlags::None);
    ConsoleVariable(
        const char* a_name,
        const char* a_defaultValue,
        const char* a_description,
        ConsoleFlags a_flags                     = ConsoleFlags::None,
        ConsoleVariableChanged a_changedCallback = nullptr,
        ConsoleInit a_initCallback               = nullptr);
    virtual ~ConsoleVariable() {}

protected:
    friend struct Console;
    void Create(
        const char* a_name, const char* a_defaultValue, ConsoleFlags a_flags = ConsoleFlags::None);
    void Create(
        const char* a_name,
        const char* a_defaultValue,
        const char* a_description,
        ConsoleFlags a_flags                     = ConsoleFlags::None,
        ConsoleVariableChanged a_changedCallback = nullptr,
        ConsoleInit a_initCallback               = nullptr);
    virtual bool Save(std::string& out_string) { return false; }
    virtual void Load(const std::vector<std::string>& a_tokens) {}

private:
    ConsoleVariableChanged m_changedCallback;
};

} // namespace Core
