#pragma once

#include "Core/Console.h"
#include <string>

struct GameVars
{
static u32 GetClientVersion();
static void GetClientVersion(u32 *major, u32 *minor, u32 *rev, u32 *proto);
};

extern Core::ConsoleVariable<std::string> uo_login;
extern Core::ConsoleVariable<std::string> uo_password;
extern Core::ConsoleVariable<std::string> uo_client_version;
extern Core::ConsoleVariable<std::string> uo_game_dir;
extern Core::ConsoleVariable<std::string> uo_server_address;
extern Core::ConsoleVariable<u16> uo_server_port;
extern Core::ConsoleVariable<u8> uo_save_password;
extern Core::ConsoleVariable<u8> uo_auto_login;
extern Core::ConsoleVariable<u8> uo_use_scaling;
extern Core::ConsoleVariable<u8> uo_render_border;
//extern Core::ConsoleVariable uo_client_flag;