#pragma once

#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <stdio.h>
#include <string_view>
#include <chrono>
#include <thread>
#include <unordered_map>

#define ALWAYS_INLINE __forceinline

#define l_log( fmt, ... ) printf( " [ lithium ] -> " fmt "\n", ##__VA_ARGS__ )

#include <dependencies/additional/structs.hpp>
#include <core/driver/driver.hpp>
