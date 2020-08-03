#pragma once

///
/// Platform-specific or system-related utilities.
///

// MARK: - Functions

/// Get the absolute filesystem path of the running program's executable,
/// @return The null-terminated absolute filesystem path of the running program's executable.
/// This pointer is allocated and must be released by the caller.
char *platform_get_path();

/// Get the absolute filesystem path of the given path relative to the directory containing the running program's executable.
/// @return The null-terminated absolute filesystem path of the given relative path.
/// This pointer is allocated and must be released by the caller.
char *platform_get_relative_path(const char *relative_path);
