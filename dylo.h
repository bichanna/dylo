#ifndef DYLO_H
#define DYLO_H

/*
 * Dylo: Header-only library for loading shared libraries/DLLs
 *
 * Supports Windows, macOS, Linux, and other Unix systems.
 *
 * Usage:
 *   Dylo_Handle handle = dylo_open("library.so");
 *   if (handle) {
 *     void* func = dylo_symbol(handle, "func_name");
 *     if (func) {
 *       // Use function
 *     }
 *     dylo_close(handle);
 *   }
 */

#include <string.h>
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_WIN64)
  #define DYLO_PLATFORM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
  #define DYLO_PLATFORM_MACOS
#elif defined(__unix__) || defined(__unix)
  #define DYLO_PLATFORM_UNIX
#else
  #error "unsupported platform"
#endif

/* Platform-specific includes */
#ifdef DYLO_PLATFORM_WINDOWS
  #include <windows.h>
  typedef HMODULE Dylo_Handle;
#else
  #include <dlfcn.h>
  typedef void* Dylo_Handle;
#endif

#include <string.h>

#ifndef DYLO_MAX_ERROR_SIZE
  #define DYLO_MAX_ERROR_SIZE 512
#endif

static char dylo_last_error[DYLO_MAX_ERROR_SIZE] = {0};

static Dylo_Handle dylo_open(const char* filename);
static void* dylo_symbol(Dylo_Handle handle, const char* symbol);
static int dylo_close(Dylo_Handle handle);
static const char* dylo_error(void);
static void dylo_clear_error(void);

/* Platform-specific implementation */

#ifdef DYLO_PLATFORM_WINDOWS

static Dylo_Handle dylo_open(const char* filename) {
  dylo_clear_error();
if (!filename) {
    strncpy(dylo_last_error, "Filename cannot be NULL", DYLO_MAX_ERROR_SIZE - 1);
    return NULL;
  }

  HMODULE handle = LoadLibraryA(filename);
  if (!handle) {
    DWORD error = GetLastError();
    snprintf(
      dylo_last_error,
      DYLO_MAX_ERROR_SIZE,
      "Failed to load library '%s': Windows error %lu",
      filename,
      error
    );
  }

  return handle;
}

static void* dylo_symbol(Dylo_Handle handle, const char* symbol) {
  dylo_clear_error();

  if (!handle) {
    strncpy(dylo_last_error, "Handle cannot be NULL", DYLO_MAX_ERROR_SIZE - 1);
    return NULL;
  }

  if (!symbol) {
    strncpy(dylo_last_error, "Symbol name cannot be NULL", DYLO_MAX_ERROR_SIZE - 1);
    return NULL;
  }

  FARPROC proc = GetProcAddress(handle, symbol);
  if (!proc) {
    DWORD error = GetLastError();
    snprintf(
      dylo_last_error,
      DYLO_MAX_ERROR,
      "Failed to find symbol '%s': Windows error %lu",
      symbol,
      error
    );
  }

  return (void*)proc;
}

static int dylo_close(Dylo_Handle handle) {
  dylo_clear_error();

  if (!handle) {
    strncpy(dylo_last_error, "Handle cannot be NULL", DYLO_MAX_ERROR_SIZE - 1);
    return -1;
  }

  bool result = FreeLibrary(handle);
  if (!result) {
    DWORD error = GetLastError();
    snprintf(
      dylo_last_error,
      DYLO_MAX_ERROR_SIZE,
      "Failed to close library: Windows error %lu",
      error
    );
    return -1;
  }

  return 0;
}

#else // Unix-like systems

static Dylo_Handle dylo_open(const char* filename) {
  dylo_clear_error();

  if (!filename) {
    strncpy(dylo_last_error, "Filename cannot be NULL", DYLO_MAX_ERROR_SIZE - 1);
    return NULL;
  }

  // Clear any existing error
  dlerror();

  void* handle = dlopen(filename, RTLD_LAZY);
  if (!handle) {
    const char* error = dlerror();
    if (error) {
      snprintf(
        dylo_last_error,
        DYLO_MAX_ERROR_SIZE,
        "Failed to load library '%s': %s",
        filename,
        error
      );
    } else {
      snprintf(
        dylo_last_error,
        DYLO_MAX_ERROR_SIZE,
        "Failed to load library '%s': Unknown error",
        filename
      );
    }
  }

  return handle;
}

static void* dylo_symbol(Dylo_Handle handle, const char* symbol) {
  dylo_clear_error();

  if (!handle) {
    strncpy(dylo_last_error, "Symbol name cannot be NULL", DYLO_MAX_ERROR_SIZE - 1);
    return NULL;
  }

  if (!symbol) {
    strncpy(dylo_last_error, "Symbol name cannot be NULL", DYLO_MAX_ERROR_SIZE - 1);
    return NULL;
  }

  // Clear any existing error
  dlerror();

  void* sym = dlsym(handle, symbol);

  const char* error = dlerror();
  if (error) {
    snprintf(
      dylo_last_error,
      DYLO_MAX_ERROR_SIZE,
      "Failed to find symbol '%s': %s",
      symbol,
      error
    );
    return NULL;
  }

  return sym;
}

static int dylo_close(Dylo_Handle handle) {
  dylo_clear_error();

  if (!handle) {
    strncpy(dylo_last_error, "Handle cannot be NULL", DYLO_MAX_ERROR_SIZE - 1);
    return -1;
  }

  int result = dlclose(handle);
  if (result != 0) {
    const char* error = dlerror();
    if (error) {
      snprintf(
        dylo_last_error,
        DYLO_MAX_ERROR_SIZE,
        "Failed to close library: %s",
        error
      );
    } else {
      strncpy(
        dylo_last_error,
        "Failed to close library: Unknown error",
        DYLO_MAX_ERROR_SIZE - 1
      );
    }
  }

  return result;
}

#endif

// Common functions

static const char* dylo_error(void) {
  return dylo_last_error[0] ? dylo_last_error : NULL;
}

static void dylo_clear_error(void) {
  dylo_last_error[0] = '\0';
}

#ifdef __cplusplus
}
#endif

#endif
