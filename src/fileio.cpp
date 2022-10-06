/*
 * This file is part of FreeRCT.
 * FreeRCT is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * FreeRCT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with FreeRCT. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file fileio.cpp File IO functions. */

/**
 * @defgroup fileio_group File IO
 */

#include <cstdarg>
#include "stdafx.h"
#include "fileio.h"
#include "rev.h"
#ifdef LINUX
	#include "unix/fileio_unix.h"
	#include <dirent.h>
	#include <errno.h>
	#include <unistd.h>
#elif WINDOWS
	#include "windows/fileio_windows.h"
	#include <direct.h> // contains chdir in windows
#endif
#include <sys/types.h>
#include <sys/stat.h>

/**
 * Constructor.
 * @param fmt Error message (may use printf-style placeholders).
 */
LoadingError::LoadingError(const char *fmt, ...)
{
	char buffer[1024];
	va_list va;
	va_start(va, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, va);
	va_end(va);
	this->message = buffer;
}

/**
 * Retrieve the description of the error.
 * @return The error message.
 */
const char* LoadingError::what() const noexcept
{
	return this->message.c_str();
}

/**
 * @fn void DirectoryReader::OpenPath(const char *path)
 * Set up the directory reader object for reading a directory.
 * @param path Path to the directory.
 * @note Failure is not reported here, but #NextEntry will not return anything useful.
 */

/**
 * @fn const char *DirectoryReader::NextEntry()
 * Get next entry of the directory contents.
 * @return Pointer to name of next entry (as a file path suitable for opening a file). Returns \c nullptr if not next entry exists.
 * @note The memory returned is owned by the #DirectoryReader object, and should not be released.
 */

/**
 * @fn void DirectoryReader::ClosePath()
 * Denote no further interest in reading the contents of the current directory.
 */

/**
 * @fn const char *DirectoryReader::MakePath(const char *directory, const char *fname)
 * Construct a path from a directory and a file name.
 * @param directory Directory part of the path.
 * @param fname File name part of the path.
 * @return The combined path to the file.
 */

/**
 * @fn bool DirectoryReader::EntryIsFile()
 * Test whether the last returned entry from #NextEntry is a file.
 * @return Whether the entry is a file.
 */

/**
 * @fn bool DirectoryReader::EntryIsDirectory()
 * Test whether the last returned entry from #NextEntry is a directory.
 * @return Whether the entry is a directory.
 */

/**
 * Get the next file entry.
 * Pulls entries from #NextEntry, until the end is reached or until a file is returned.
 * @return Next file entry, if available.
 */
const char *DirectoryReader::NextFile()
{
	const char *entry;

	do {
		entry = this->NextEntry();
	} while (entry != nullptr && !this->EntryIsFile());
	return entry;
}


/**
 * Construct a directory reader object (specific for the operating system).
 * @return A directory reader.
 * @ingroup fileio_group
 */
DirectoryReader *MakeDirectoryReader()
{
#ifdef LINUX
	return new UnixDirectoryReader();
#elif WINDOWS
	return new WindowsDirectoryReader();
#else
	assert_compile(false);
#endif
}

/**
 * RCD file reader constructor, loading data from a file.
 * @param fname Name of the file to load.
 */
RcdFileReader::RcdFileReader(const char *fname)
: filename(fname), file_pos(0), file_size(0)
{
	this->name[4] = '\0';
	this->fp = fopen(fname, "rb");
	if (this->fp == nullptr) return;

	if (fseek(this->fp, 0L, SEEK_END) != 0) {
		fclose(this->fp);
		return;
	}
	this->file_size = ftell(this->fp);
	if (fseek(this->fp, 0L, SEEK_SET) != 0) {
		fclose(this->fp);
		return;
	}
}

/** Destructor. */
RcdFileReader::~RcdFileReader()
{
	if (this->fp != nullptr) fclose(fp);
}

/**
 * Check whether the version of the current block is supported by this revision of FreeRCT, and throw an exception if this is not the case.
 * @param current_version The currently supported version.
 * @pre Must be inside a block.
 */
void RcdFileReader::CheckVersion(uint32 current_version)
{
	if (this->version != current_version) this->Error("Version mismatch: Found version %u, supported version is %u", this->version, current_version);
}

/**
 * Check whether the remaining length of the block is greater than or equal to a given minimum length, and throw an exception if this is not the case.
 * @param length Remaining length in the block.
 * @param required Required minimum length.
 * @param what Error description if the check fails.
 * @pre Must be inside a block.
 */
void RcdFileReader::CheckMinLength(int length, int required, const char *what)
{
	if (length < required) this->Error("Length too short for %s (at least %d bytes missing)", what, required - length);
}

/**
 * Check whether the remaining length of the block is equal to a given expected length, and throw an exception if this is not the case.
 * @param length Remaining length in the block.
 * @param required Expected remaining length.
 * @param what Error description if the check fails.
 * @pre Must be inside a block.
 */
void RcdFileReader::CheckExactLength(int length, int required, const char *what)
{
	if (length < required) this->Error("Length mismatch at %s (%d bytes missing)", what, required - length);
	if (length > required) this->Error("Length mismatch at %s (%d trailing bytes)", what, length - required);
}


/**
 * Get length of data not yet read.
 * @return Count of remaining data.
 */
size_t RcdFileReader::GetRemaining()
{
	return (this->file_size >= this->file_pos) ? this->file_size - this->file_pos : 0;
}

/**
 * Read an 8 bits unsigned number.
 * @return Loaded number.
 * @pre File must be open, data must be available.
 */
uint8 RcdFileReader::GetUInt8()
{
	this->file_pos++;
	return fgetc(this->fp);
}

/**
 * Read an 8 bits signed number.
 * @return Loaded number.
 * @pre File must be open, data must be available.
 */
int8 RcdFileReader::GetInt8()
{
	return this->GetUInt8();
}

/**
 * Read an 16 bits unsigned number.
 * @return Loaded number.
 * @pre File must be open, data must be available.
 */
uint16 RcdFileReader::GetUInt16()
{
	uint16 val = this->GetUInt8();
	return val | (this->GetUInt8() << 8);
}

/**
 * Read an 16 bits signed number.
 * @return Loaded number.
 * @pre File must be open, data must be available.
 */
int16 RcdFileReader::GetInt16()
{
	uint16 val = this->GetUInt8();
	return val | (this->GetUInt8() << 8);
}

/**
 * Read an 32 bits unsigned number.
 * @return Loaded number.
 * @pre File must be open, data must be available.
 */
uint32 RcdFileReader::GetUInt32()
{
	uint32 val = this->GetUInt16();
	return val | (this->GetUInt16() << 16);
}

/**
 * Read an 32 bits signed number.
 * @return Loaded number.
 * @pre File must be open, data must be available.
 */
int32 RcdFileReader::GetInt32()
{
	uint32 val = this->GetUInt16();
	return val | (this->GetUInt16() << 16);
}

/**
 * Read a nul-terminated string of unknown length.
 * @return Loaded string.
 * @pre File must be open, data must be available.
 */
std::string RcdFileReader::GetText()
{
	std::string result;
	for (;;) {
		char c = this->GetUInt8();
		if (c == '\0') break;
		result += c;
	}
	return result;
}

/**
 * Check whether the file header makes sense, and has the right version.
 * @param hdr_name Header name (should be 4 chars long).
 * @param version Header version.
 * @return The header seems correct.
 */
bool RcdFileReader::CheckFileHeader(const char *hdr_name, uint32 version)
{
	if (this->fp == nullptr) return false;
	if (this->GetRemaining() < 8) return false;

	char name[5];
	this->GetBlob(name, 4);
	name[4] = '\0';
	if (strcmp(name, hdr_name) != 0) return false;
	uint32 val = this->GetUInt32();
	return val == version;
}

/**
 * Starting at the first byte of a block, read the block information, and put it in #name, #version, and #size.
 * @return Whether a block was found. If so, data is in #name, #version, and #size.
 */
bool RcdFileReader::ReadBlockHeader()
{
	if (this->GetRemaining() < 12) return false;
	this->GetBlob(this->name, 4);
	this->version = this->GetUInt32();
	this->size = this->GetUInt32();
	return this->file_pos + (size_t)this->size <= this->file_size;
}

/**
 * Skip a number of bytes in the file.
 * @param count Number of bytes to move forward.
 * @return Skipping was successful.
 */
bool RcdFileReader::SkipBytes(uint32 count)
{
	this->file_pos += count;
	if (this->file_pos > this->file_size) this->file_pos = this->file_size;
	return fseek(this->fp, this->file_pos, SEEK_SET) == 0;
}

/**
 * Get a blob of data from the file.
 * @param address Address to load into.
 * @param length Length of the data.
 * @return Loading was successful.
 */
bool RcdFileReader::GetBlob(void *address, size_t length)
{
	this->file_pos += length;
	return fread(address, length, 1, this->fp) == 1;
}

/**
 * Create a directory and all its parent directories if it did not exist yet.
 * @param path Path of the directory.
 * @todo At the time of writing (2021-06-30) this is tested only on Linux. Before using it anywhere else, test this on all platforms (especially Windows).
 */
void MakeDirectory(std::string path)
{
	if (path.empty() || PathIsDirectory(path.c_str())) return;

	/* Strip trailing path separators. */
	while (StrEndsWith(path.c_str(), DIR_SEP, false)) {
		for (size_t i = strlen(DIR_SEP); i > 0; --i) {
			path.pop_back();
		}
	}

	/* Recursively create parent directories. */
	const size_t sep_pos = path.rfind(DIR_SEP);
	if (sep_pos != std::string::npos) MakeDirectory(path.substr(0, sep_pos));

#ifdef _WIN32
	if (CreateDirectory(path.c_str(), NULL)) return;
	error("Failed creating directory '%s'\n", path.c_str());
#else
	if (mkdir(path.c_str(), 0x1FF) == 0) return;
	error("Failed creating directory '%s' (%s)\n", path.c_str(), strerror(errno));
#endif
}

/**
 * Copy a file.
 * @param src Source file.
 * @param dest Destination file.
 */
void CopyBinaryFile(const char *src, const char *dest)
{
	FILE *in_file = nullptr;
	in_file = fopen(src, "rb");
	if (in_file == nullptr) {
		error("Could not open file for reading: %s\n", src);
	}

	FILE *out_file = nullptr;
	out_file = fopen(dest, "wb");
	if (out_file == nullptr) {
		error("Could not open file for writing: %s\n", dest);
	}

	for (;;) {
		int byte = getc(in_file);
		if (byte == EOF) break;
		putc(byte, out_file);
	}

	fclose(in_file);
	fclose(out_file);
}

/**
 * Locate the user's home directory. Failure is a fatal error.
 * @return User home directory.
 */
const std::string &GetUserHomeDirectory()
{
	static std::string homedir;
	if (!homedir.empty()) return homedir;

	for (const auto& var : {"HOME", "USERPROFILE", "HOMEPATH", "APPDATA"}) {
		const char *environment_variable = getenv(var);
		if (environment_variable != nullptr && environment_variable[0] != '\0') {
			homedir = environment_variable;
			return homedir;
		}
	}

	error("Unable to locate the user home directory. Set the HOME environment variable to fix the problem.\n");
}

/**
 * Locate a data file.
 * @param name Relative path of the file.
 * @return Actual path to the file.
 */
std::string FindDataFile(const std::string &name)
{
	for (std::string path : {std::string("."), std::string(".."), std::string("..") + DIR_SEP + "..", freerct_install_prefix()}) {
		path += DIR_SEP;
		path += name;
		if (PathIsFile(path.c_str())) return path;
	}
	error("Data file %s is missing, the installation seems to be broken!\n", name.c_str());
}

/**
 * Find the directory where the user's savegames are stored.
 * @return The directory path, with trailing directory separator.
 */
const std::string &SavegameDirectory()
{
	static std::string dir;
	if (dir.empty()) {
		dir = freerct_userdata_prefix();
		dir += DIR_SEP;
		dir += SAVEGAME_DIRECTORY;
		dir += DIR_SEP;
	}
	return dir;
}

/**
 * Find the directory where the user's track designs are stored.
 * @return The directory path, with trailing directory separator.
 */
const std::string &TrackDesignDirectory()
{
	static std::string dir;
	if (dir.empty()) {
		dir = freerct_userdata_prefix();
		dir += DIR_SEP;
		dir += TRACK_DESIGN_DIRECTORY;
		dir += DIR_SEP;
	}
	return dir;
}
