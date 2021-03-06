// Copyright 2014-2015 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_ANALYZER_PUBLIC_ANALYZER_EXPORT_H_
#define AOBA_ANALYZER_PUBLIC_ANALYZER_EXPORT_H_

#if defined(COMPONENT_BUILD)
#if defined(WIN32)

#if defined(AOBA_ANALYZER_IMPLEMENTATION)
#define AOBA_ANALYZER_EXPORT __declspec(dllexport)
#else
#define AOBA_ANALYZER_EXPORT __declspec(dllimport)
#endif  // defined(BASE_IMPLEMENTATION)

#else  // defined(WIN32)
#if defined(AOBA_ANALYZER_IMPLEMENTATION)
#define AOBA_ANALYZER_EXPORT __attribute__((visibility("default")))
#else
#define AOBA_ANALYZER_EXPORT
#endif  // defined(BASE_IMPLEMENTATION)
#endif

#else  // defined(COMPONENT_BUILD)
#define AOBA_ANALYZER_EXPORT
#endif

#endif  // AOBA_ANALYZER_PUBLIC_ANALYZER_EXPORT_H_
