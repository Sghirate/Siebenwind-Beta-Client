#pragma once

#include "Core/Console.h"

struct GameVars
{
static u32 GetClientVersion();
};

extern Core::ConsoleVariable uo_login;
extern Core::ConsoleVariable uo_password;
extern Core::ConsoleVariable uo_client_version;
extern Core::ConsoleVariable uo_game_dir;
extern Core::ConsoleVariable uo_server_address;
extern Core::ConsoleVariable uo_server_port;
extern Core::ConsoleVariable uo_client_flag;