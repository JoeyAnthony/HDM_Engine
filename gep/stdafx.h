#pragma once

// prevent min & max macros
#define NOMINMAX

#include <cmath>
#include <fstream>
#include <exception>
#include <locale>
#include <codecvt>
#include <string>
#include <algorithm>
#include <windows.h>


#include <XInput.h>


#include "gep/gepmodule.h"
#include "gep/common.h"
#include "gep/memory/allocator.h"
#include "gep/ArrayPtr.h"
#include "gep/globalManager.h"
#include "include/gep/interfaces/logging.h"
#include "gep/container/dynamicarray.h"
