#include "Console.h"
#include "Core/Enums.h"
#include "Core/File.h"
#include "Core/Log.h"
#include "Core/TextFileParser.h"
#include "Core/Platform.h"
#include "Core/StringUtils.h"
#include <vector>

namespace Core
{

static const char* CVARS_FILENAME = "cvars.cfg";
// Console State
static ConsoleInteraction* g_coreConsoleRegistry;
static std::vector<IConsoleListener*> g_coreConsoleListeners;
// ~Console State

// Console
void Console::Init()
{
    g_coreConsoleListeners.reserve(8);

    std::filesystem::path path = Platform::GetBinaryPath().parent_path() / CVARS_FILENAME;
    if (!std::filesystem::exists(path))
        path = std::filesystem::current_path() / CVARS_FILENAME;
    if (std::filesystem::exists(path))
    {
        Core::TextFileParser file(path, "=,", "#;", "");
        while (!file.IsEOF())
        {
            auto strings = file.ReadTokens(false); // Trim remove spaces from paths
            if (strings.size() >= 2)
            {
                std::string key         = Core::ToLowerA(strings[0]);
                ConsoleInteraction* cur = g_coreConsoleRegistry;
                while (cur)
                {
                    if (cur->GetInteractionType() == ConsoleInteractionType::Variable)
                    {
                        std::string curName = Core::ToLowerA(cur->GetName());
                        if (curName == key)
                        {
                            ConsoleVariable* var = static_cast<ConsoleVariable*>(cur);
                            var->Load(strings);
                        }
                    }
                    cur = cur->m_next;
                }
            }
        }
    }

    ConsoleInteraction* cur = g_coreConsoleRegistry;
    while (cur)
    {
        if (!cur->HasFlag(ConsoleFlags::Registered))
        {
            cur->Init();
            if (cur->HasFlag(ConsoleFlags::Registered))
            {
                if (cur->GetInteractionType() == ConsoleInteractionType::Command)
                {
                    ConsoleCommand* command = static_cast<ConsoleCommand*>(cur);
                    for (IConsoleListener* listener : g_coreConsoleListeners)
                        listener->OnConsoleCommandRegistered(command);
                }
                else if (cur->GetInteractionType() == ConsoleInteractionType::Variable)
                {
                    ConsoleVariable* variable = static_cast<ConsoleVariable*>(cur);
                    for (IConsoleListener* listener : g_coreConsoleListeners)
                        listener->OnConsoleVariableRegistered(variable);
                }
            }
        }
        cur = cur->m_next;
    }
}

void Console::Shutdown()
{
    std::filesystem::path path = Platform::GetBinaryPath().parent_path() / CVARS_FILENAME;
    Core::File file(path, "w");
    std::string value;

    ConsoleInteraction* cur = g_coreConsoleRegistry;
    while (cur)
    {
        if (cur->HasFlag(ConsoleFlags::Registered))
        {
            if (cur->GetInteractionType() == ConsoleInteractionType::Variable)
            {
                ConsoleVariable* var = static_cast<ConsoleVariable*>(cur);
                if (var->Save(value))
                {
                    file.Print("%s=%s\n", var->GetName(), value.c_str());
                    file.Flush();
                }
            }
            cur->Shutdown();
        }
        cur = cur->m_next;
    }
}
// ~Console

// ConsoleInteraction
ConsoleInteraction::ConsoleInteraction(ConsoleInteractionType a_type)
    : m_type(a_type)
    , m_name(nullptr)
    , m_next(nullptr)
    , m_description(nullptr)
    , m_flags(ConsoleFlags::None)
    , m_initCallback(nullptr)
{
}

ConsoleInteraction::ConsoleInteraction(
    ConsoleInteractionType a_type,
    const char* a_name,
    const char* a_description,
    ConsoleFlags a_flags,
    ConsoleInit a_initCallback)
{
    Create(a_type, a_name, a_description, a_flags, a_initCallback);
}

ConsoleInteraction::~ConsoleInteraction()
{
}

void ConsoleInteraction::Create(
    ConsoleInteractionType a_type,
    const char* a_name,
    const char* a_description,
    ConsoleFlags a_flags,
    ConsoleInit a_initCallback)
{
    m_type         = a_type;
    m_next         = nullptr;
    m_name         = a_name;
    m_description  = a_description;
    m_flags        = a_flags & ~ConsoleFlags::Registered;
    m_initCallback = a_initCallback;
    if (m_type != ConsoleInteractionType::Unknown && !HasFlag(ConsoleFlags::Disabled))
    {
        m_next                = g_coreConsoleRegistry;
        g_coreConsoleRegistry = this;
    }
}

void ConsoleInteraction::Init()
{
    m_flags |= ConsoleFlags::Registered;
    if (m_initCallback)
        m_initCallback();
}
// ~ConsoleInteraction

// ConsoleCommand
ConsoleCommand::ConsoleCommand()
    : ConsoleInteraction(ConsoleInteractionType::Command)
    , m_callback(nullptr)
{
}

ConsoleCommand::ConsoleCommand(
    const char* a_name,
    ConsoleCommandExecuted a_callback,
    const char* a_description,
    ConsoleFlags a_flags,
    ConsoleInit a_initCallback)
{
    Create(a_name, a_callback, a_description, a_flags, a_initCallback);
}

ConsoleCommand::~ConsoleCommand()
{
}

void ConsoleCommand::Create(
    const char* a_name,
    ConsoleCommandExecuted a_callback,
    const char* a_description,
    ConsoleFlags a_flags,
    ConsoleInit a_initCallback)
{
    m_callback = a_callback;
    ConsoleInteraction::Create(
        ConsoleInteractionType::Command, a_name, a_description, a_flags, a_initCallback);
}

void ConsoleCommand::Init()
{
    ERROR_IF_NOT(m_callback, "Core", "Console Command without callback: %s!", GetName());
    if (m_callback)
        ConsoleInteraction::Init();
}

void ConsoleCommand::Execute()
{
    if (m_callback)
        m_callback();
}
// ~ConsoleCommand

// ConsoleVariable
ConsoleVariable::ConsoleVariable(const char* a_name, const char* a_defaultValue, ConsoleFlags a_flags)
{
    Create(a_name, a_defaultValue, a_flags);
}

ConsoleVariable::ConsoleVariable(
    const char* a_name,
    const char* a_defaultValue,
    const char* a_description,
    ConsoleFlags a_flags,
    ConsoleVariableChanged a_changedCallback,
    ConsoleInit a_initCallback)
{
    Create(a_name, a_defaultValue, a_description, a_flags, a_changedCallback, a_initCallback);
}

void ConsoleVariable::Create(const char* a_name, const char* a_defaultValue, ConsoleFlags a_flags)
{
    m_changedCallback = nullptr;
    // TODO: default value!
    ConsoleInteraction::Create(ConsoleInteractionType::Variable, a_name, nullptr, a_flags, nullptr);
}

void ConsoleVariable::Create(
    const char* a_name,
    const char* a_defaultValue,
    const char* a_description,
    ConsoleFlags a_flags,
    ConsoleVariableChanged a_changedCallback,
    ConsoleInit a_initCallback)
{
    m_changedCallback = a_changedCallback;
    // TODO: default value!
    ConsoleInteraction::Create(ConsoleInteractionType::Variable, a_name, a_description, a_flags, a_initCallback);
}
// ~ConsoleVariable

} // namespace Core
