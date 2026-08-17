#pragma once

/**
 * @file extension_api.h
 * @brief Low-level platform macros and callback signature required by the
 *        Arma 3 RV Engine extension API.
 *
 * These definitions are shared by every function exported from this DLL
 * (RVExtension, RVExtensionArgs, RVExtensionFillTextureSource, etc.). They
 * exist so the rest of the codebase never has to deal with platform-specific
 * export/calling-convention syntax directly.
 */

#if defined(_WIN32)
 /// Calling convention required by the Arma 3 engine on Windows.
#define ARMA3IL_CALL __stdcall
/// Marks a function as a DLL export on Windows.
#define ARMA3IL_EXPORT extern "C" __declspec(dllexport)
#else
 /// No specific calling convention required outside Windows.
#define ARMA3IL_CALL
/// Marks a function as a shared library export outside Windows.
#define ARMA3IL_EXPORT extern "C" __attribute__((visibility("default")))
#endif

/**
 * @brief Signature of the callback function registered by the RV Engine via
 *        RVExtensionRegisterCallback.
 *
 * The engine calls this function whenever the extension wants to push
 * asynchronous data back to the mission (e.g. the result of a download that
 * completed on a background thread).
 *
 * @param name     Name of the extension firing the callback (e.g. "arma3il").
 * @param function Logical function name the callback relates to (e.g. "download").
 * @param data     Payload string, format defined by the extension itself.
 */
using RVExtensionCallback = void (ARMA3IL_CALL*)(const char* name, const char* function, const char* data);