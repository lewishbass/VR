#ifndef XRUTILS_HPP
#define XRUTILS_HPP

#include <iostream>

#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

#include <functional>

void dirtyXRErrorPrintInternal(XrResult result, XrInstance instance, const char* file, int line);

#define dirtyXRErrorPrint(result, instance) dirtyXRErrorPrintInternal(result, instance, __FILE__, __LINE__)

// Function pointer type for XR validation
// using PFN_IsXRValid = std::function<bool()>;
// using PFN_IsSDLValid = std::function<bool()>;

#endif // XRUTILS_HPP