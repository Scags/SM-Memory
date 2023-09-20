#if defined _smmem_dynlib_included
    #endinput
#endif
#define _smmem_dynlib_included

#include <smmem>

methodmap DynLib < Handle
{
    /**
     *  Load a running dynamic library (.dll/.so)
     *
     *  @param name             Name of the library to open.
     *                          Respective OS DLL file extensions
     *                          (.dll/.so) will be appended automatically,
     *                          so they are not required.
     *                          Linux must have the correct path to the 
     *                          library file. Starts in srcds directory.
     *  @param quietFail        If this is false, an error will be
     *                          thrown if a library is failed to be
     *                          found/loaded.
     *
     *  @return                 Handle to new DynLib object. Must be freed 
     *                          with delete/CloseHandle.
     *                          Upon deletion, the OS' internal DLL
     *                          reference counter is decremented, and if
     *                          it is 0, then the library will be unloaded.
     *
     *                          If for some reason you want to free/delete 
     *                          a DynLib Handle while keeping the library 
     *                          loaded in memory, SDKCall LoadLibrary/dlopen
     *                          on it for Windows and Linux respectively.
     *                          Odds are, this isn't necessary.
    */
    public native DynLib(const char[] name, bool quietFail=true);

    /**
     *  Retrieve the base address of the file in memory.
    */
    property Address BaseAddr
    {
        public native get();
    }

    /**
     *  Get the name of the loaded library.
     *  This will either be the name you passed in the constructor, or it
     *  will be the name appended with the respective OS file extension.
     *
     *  @param buffer           String buffer.
     *  @param len              Buffer length.
    */
    public native void GetName(char[] buffer, int len);

    /**
     *  Find a pattern of bytes in a library AKA sigscan.
     *
     *  @param pattern          String of bytes.
     *  @param len              Length of string.
     *
     *  @return                 Address of the found pattern, Address_Null
     *                          if not found.
    */
    public native Address FindPattern(const char[] pattern, int len);

    /**
     *  Find the address of a symbol in a library
     *
     *  @param sym              Symbol to find, does not need to start with '@'.
     *                          Equivalent to GetProcAddress in Windows.
     *
     *  @return                 Address of the symbol if found, Address_Null
     *                          otherwise.
    */
    public native Address ResolveSymbol(const char[] sym);
};