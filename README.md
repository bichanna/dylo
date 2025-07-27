# Dylo: Header-only library for loading shared libraries/DLLs

 - A header-only library that provides a cross-platform API for loading and unloading shared libraries/DLLs.
 - Supports Windows, macOS, Linux, and other Unix systems.
 - Supports C90 and later versions.

## Simple Usage

```c
 Dylo_Handle handle = dylo_open("library.so");
 if (handle) {
   void* func = dylo_symbol(handle, "func_name");
   if (func) {
     // Use function
   }
   dylo_close(handle);
 }
```

## API Summary

### Core Functions

`Dylo_Handle dylo_open(const char* filename)`  
**Purpose**: Load a shared library.  
**Parameters**:  
 - `filename`: Path to the shared library file.  
**Returns**:  
 - Valid handle on success
 - `NULL` on failure (check `dylo_error` for error message)


`void* dylo_symbol(Dylo_Handle handle, const char* symbol)`  
**Purpose**: Get a pointer to a function or variable from the loaded library.  
**Parameters**  
 - `handle`: Library handle from `dylo_open`.  
 - `symbol`: Name of the function/variable to find
**Returns**:  
 - Function/variable pointer on success
 - `NULL` on failure (check `dylo_error` for error message)


`int dylo_close(Dylo_Handle handle)`  
**Purpose**: Unload a shared library and free resources.  
**Parameters**:
 - `handle`: Library handle to close
**Returns**:  
 - `0` on success
 - `-1` on failure (check `dylo_error` for error message)

### Error Handling Functions

`const char* dylo_error(void)`  
**Purpose**: Get the last error message.
**Returns**:  
 - Error string if an error occurred
 - `NULL` if no error


`void dylo_clear_error(void)`  
**Purpose**: Clear the current error state.
