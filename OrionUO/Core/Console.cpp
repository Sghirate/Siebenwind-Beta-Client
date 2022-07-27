#include "Console.h"
#include "Core/Enums.h"
#include "Core/File.h"
#include "Core/Log.h"
#include "Core/TextFileParser.h"
#include "Core/Platform.h"
#include "Core/StringUtils.h"
#include <vector>

namespace
{
static const char* CLIENT_VARS_FILENAME = "client.cfg";
static const char* DEBUG_VARS_FILENAME = "debug.cfg";
static const char* USER_VARS_FILENAME = "user.cfg";
static const char* VAR_LOAD_ORDER[] = {
    CLIENT_VARS_FILENAME,
    DEBUG_VARS_FILENAME,
    USER_VARS_FILENAME,
};
static void DetermineVarFilePath(const char* a_fileName, std::filesystem::path& out_path)
{
    out_path = Core::Platform::GetBinaryPath().parent_path() / a_fileName;
    if (!std::filesystem::exists(out_path))
        out_path = std::filesystem::current_path() / a_fileName;
}   
} // namespace

namespace Core
{

// Console State
static ConsoleInteraction* g_coreConsoleRegistry;
static std::vector<IConsoleListener*> g_coreConsoleListeners;
// ~Console State

// Console
void Console::ApplyFileVariables(const std::filesystem::path& a_path)
{
    if (std::filesystem::exists(a_path))
    {
        Core::TextFileParser file(a_path, "=,", "#;", "");
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
                            BaseConsoleVariable* var = static_cast<BaseConsoleVariable*>(cur);
                            var->Load(strings[1]);
                        }
                    }
                    cur = cur->m_next;
                }
            }
        }
    }
}

void Console::Init()
{
    g_coreConsoleListeners.reserve(8);

    for (int i = 0; i < sizeof(VAR_LOAD_ORDER)/sizeof(VAR_LOAD_ORDER[0]); ++i)
    {
        std::filesystem::path path;
        DetermineVarFilePath(VAR_LOAD_ORDER[i], path);
        ApplyFileVariables(path);
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
                    BaseConsoleVariable* variable = static_cast<BaseConsoleVariable*>(cur);
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
    std::filesystem::path userPath;
    DetermineVarFilePath(USER_VARS_FILENAME, userPath);
    Core::File file(userPath, "w");
    std::string value;

    ConsoleInteraction* cur = g_coreConsoleRegistry;
    while (cur)
    {
        if (cur->HasFlag(ConsoleFlags::Registered) && cur->HasFlag(ConsoleFlags::User))
        {
            if (cur->GetInteractionType() == ConsoleInteractionType::Variable)
            {
                BaseConsoleVariable* var = static_cast<BaseConsoleVariable*>(cur);
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
    , m_next(nullptr)
    , m_name(nullptr)
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
    : m_type(a_type)
    , m_next(nullptr)
    , m_name(a_name)
    , m_description(a_description)
    , m_flags(a_flags & ~ConsoleFlags::Registered)
    , m_initCallback(a_initCallback)
{
    Register();
}

ConsoleInteraction::~ConsoleInteraction()
{
}

void ConsoleInteraction::Register()
{
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
    : ConsoleInteraction(ConsoleInteractionType::Command, a_name, a_description, a_flags, a_initCallback)
    , m_callback(a_callback)
{
}

ConsoleCommand::~ConsoleCommand()
{
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
BaseConsoleVariable::BaseConsoleVariable(
    const char* a_name,
    const char* a_description,
    ConsoleFlags a_flags,
    ConsoleVariableChanged a_changedCallback,
    ConsoleInit a_initCallback)
    : ConsoleInteraction(ConsoleInteractionType::Variable, a_name, a_description, a_flags, a_initCallback)
    , m_changedCallback(a_changedCallback)
{
}

std::string BaseConsoleVariable::GetStringValue() const
{
    std::string value;
    Save(value);
    return value;
}
// ~ConsoleVariable

} // namespace Core
