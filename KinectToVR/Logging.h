﻿#pragma once
#include <easylogging++.h>
#include <QApplication>

extern const char* logConfigFileName;
extern const char* logConfigDefault;

#if defined MAKE_KINECTTOVR_LIB
#define KINECTTOVR_LIB Q_DECL_EXPORT
#else
 #define KINECTTOVR_LIB Q_DECL_IMPORT
#endif

extern "C" KINECTTOVR_LIB void initLogging();
