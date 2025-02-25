#include "filereader.h"

#include <kinc/memory.h>
#include <kinc/string.h>
#include <kinc/system.h>

#ifdef KORE_ANDROID
#include <kinc/backend/Android.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef KORE_WINDOWS
#include <malloc.h>
#include <memory.h>
#endif

#ifndef KORE_CONSOLE

#ifdef KORE_IOS
const char *iphonegetresourcepath(void);
#endif

#ifdef KORE_MACOS
const char *macgetresourcepath(void);
#endif

#if defined(KORE_WINDOWS) || defined(KORE_WINDOWSAPP)
#define NOMINMAX
#include <Windows.h>
#endif

#ifdef KORE_TIZEN
#include <FApp.h>
#endif

#ifdef KORE_PI
#define KORE_LINUX
#endif

static char *fileslocation = NULL;
#ifdef KORE_WINDOWS
static wchar_t wfilepath[1001];
#endif

void kinc_internal_set_files_location(char *dir) {
	fileslocation = dir;
}

char *kinc_internal_get_files_location() {
	return fileslocation;
}

#ifdef KORE_WINDOWSAPP
void kinc_internal_uwp_installed_location_path(char *path);
#endif

#ifndef KORE_ANDROID
bool kinc_file_reader_open(kinc_file_reader_t *reader, const char *filename, int type) {
	kinc_memset(reader, 0, sizeof(kinc_file_reader_t));
	char filepath[1001];
#ifdef KORE_IOS
	kinc_string_copy(filepath, type == KINC_FILE_TYPE_SAVE ? kinc_internal_save_path() : iphonegetresourcepath());
	if (type != KINC_FILE_TYPE_SAVE) {
		kinc_string_append(filepath, "/");
		kinc_string_append(filepath, KORE_DEBUGDIR);
		kinc_string_append(filepath, "/");
	}

	kinc_string_append(filepath, filename);
#endif
#ifdef KORE_MACOS
	kinc_string_copy(filepath, type == KINC_FILE_TYPE_SAVE ? kinc_internal_save_path() : macgetresourcepath());
	if (type != KINC_FILE_TYPE_SAVE) {
		kinc_string_append(filepath, "/");
		kinc_string_append(filepath, KORE_DEBUGDIR);
		kinc_string_append(filepath, "/");
	}
	kinc_string_append(filepath, filename);
#endif
#ifdef KORE_WINDOWS
	if (type == KINC_FILE_TYPE_SAVE) {
		kinc_string_copy(filepath, kinc_internal_save_path());
		kinc_string_append(filepath, filename);
	}
	else {
		kinc_string_copy(filepath, filename);
	}
	size_t filepathlength = kinc_string_length(filepath);
	for (size_t i = 0; i < filepathlength; ++i)
		if (filepath[i] == '/') filepath[i] = '\\';
#endif
#ifdef KORE_WINDOWSAPP
	kinc_internal_uwp_installed_location_path(filepath);
	kinc_string_append(filepath, "\\");
	kinc_string_append(filepath, filename);
#endif
#ifdef KORE_LINUX
	if (type == KINC_FILE_TYPE_SAVE) {
		kinc_string_copy(filepath, kinc_internal_save_path());
		kinc_string_append(filepath, filename);
	}
	else {
		kinc_string_copy(filepath, filename);
	}
#endif
#ifdef KORE_HTML5
	kinc_string_copy(filepath, KORE_DEBUGDIR);
	kinc_string_append(filepath, "/");
	kinc_string_append(filepath, filename);
#endif
#ifdef KORE_TIZEN
	for (int i = 0; i < Tizen::App::App::GetInstance()->GetAppDataPath().GetLength(); ++i) {
		wchar_t c;
		Tizen::App::App::GetInstance()->GetAppDataPath().GetCharAt(i, c);
		filepath[i] = (char)c;
	}
	filepath[Tizen::App::App::GetInstance()->GetAppDataPath().GetLength()] = 0;
	kinc_string_append(filepath, "/");
	kinc_string_append(filepath, filename);
#endif

#ifdef KORE_WINDOWS
	// Drive letter or network
	bool isAbsolute = (filename[1] == ':' && filename[2] == '\\') || (filename[0] == '\\' && filename[1] == '\\');
#else
	bool isAbsolute = filename[0] == '/';
#endif

	if (isAbsolute) {
		kinc_string_copy(filepath, filename);
	}
	else if (fileslocation != NULL && type != KINC_FILE_TYPE_SAVE) {
		kinc_string_copy(filepath, fileslocation);
		kinc_string_append(filepath, "/");
		kinc_string_append(filepath, filename);
	}

#ifdef KORE_WINDOWS
	MultiByteToWideChar(CP_UTF8, 0, filepath, -1, wfilepath, 1000);
	reader->file = CreateFile(wfilepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (reader->file == INVALID_HANDLE_VALUE) {
		return false;
	}
#else
	reader->file = fopen(filepath, "rb");
	if (reader->file == NULL) {
		return false;
	}
#endif

#ifdef KORE_WINDOWS
	reader->size = GetFileSize(reader->file, NULL);
#else
	fseek((FILE *)reader->file, 0, SEEK_END);
	reader->size = (int)ftell((FILE *)reader->file);
	fseek((FILE *)reader->file, 0, SEEK_SET);
#endif
	return true;
}
#endif

int kinc_file_reader_read(kinc_file_reader_t *reader, void *data, size_t size) {
#ifdef KORE_ANDROID
	if (reader->file != NULL) {
		return (int)fread(data, 1, size, reader->file);
	}
	else {
		int read = AAsset_read(reader->asset, data, size);
		reader->pos += read;
		return read;
	}
#elif defined(KORE_WINDOWS)
	DWORD readBytes = 0;
	if (ReadFile(reader->file, data, (DWORD)size, &readBytes, NULL)) {
		return (int)readBytes;
	}
	else {
		return 0;
	}
#else
	return (int)fread(data, 1, size, (FILE *)reader->file);
#endif
}

void kinc_file_reader_seek(kinc_file_reader_t *reader, int pos) {
#ifdef KORE_ANDROID
	if (reader->file != NULL) {
		fseek(reader->file, pos, SEEK_SET);
	}
	else {
		AAsset_seek(reader->asset, pos, SEEK_SET);
		reader->pos = pos;
	}
#elif defined(KORE_WINDOWS)
	SetFilePointer(reader->file, pos, NULL, FILE_BEGIN);
#else
	fseek((FILE *)reader->file, pos, SEEK_SET);
#endif
}

void kinc_file_reader_close(kinc_file_reader_t *reader) {
#ifdef KORE_ANDROID
	if (reader->file != NULL) {
		fclose(reader->file);
		reader->file = NULL;
	}
	if (reader->asset != NULL) {
		AAsset_close(reader->asset);
		reader->asset = NULL;
	}
#elif defined(KORE_WINDOWS)
	CloseHandle(reader->file);
#else
	if (reader->file == NULL) {
		return;
	}
	fclose((FILE *)reader->file);
	reader->file = NULL;
#endif
}

int kinc_file_reader_pos(kinc_file_reader_t *reader) {
#ifdef KORE_ANDROID
	if (reader->file != NULL)
		return (int)ftell(reader->file);
	else
		return reader->pos;
#elif defined(KORE_WINDOWS)
	return (int)SetFilePointer(reader->file, 0, NULL, FILE_CURRENT);
#else
	return (int)ftell((FILE *)reader->file);
#endif
}

size_t kinc_file_reader_size(kinc_file_reader_t *reader) {
	return (size_t)reader->size;
}

#endif

float kinc_read_f32le(uint8_t *data) {
#ifdef KORE_LITTLE_ENDIAN // speed optimization
	return *(float *)data;
#else // works on all architectures
	int i = (data[0] << 0) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
	return *(float *)&i;
#endif
}

float kinc_read_f32be(uint8_t *data) {
#ifdef KORE_BIG_ENDIAN // speed optimization
	return *(float *)data;
#else // works on all architectures
	int i = (data[3] << 0) | (data[2] << 8) | (data[1] << 16) | (data[0] << 24);
	return *(float *)&i;
#endif
}

uint64_t kinc_read_u64le(uint8_t *data) {
#ifdef KORE_LITTLE_ENDIAN
	return *(uint64_t *)data;
#else
	return ((uint64_t)data[0] << 0) | ((uint64_t)data[1] << 8) | ((uint64_t)data[2] << 16) | ((uint64_t)data[3] << 24) | ((uint64_t)data[4] << 32) |
	       ((uint64_t)data[5] << 40) | ((uint64_t)data[6] << 48) | ((uint64_t)data[7] << 56);
#endif
}

uint64_t kinc_read_u64be(uint8_t *data) {
#ifdef KORE_BIG_ENDIAN
	return *(uint64_t *)data;
#else
	return ((uint64_t)data[7] << 0) | ((uint64_t)data[6] << 8) | ((uint64_t)data[5] << 16) | ((uint64_t)data[4] << 24) | ((uint64_t)data[3] << 32) |
	       ((uint64_t)data[2] << 40) | ((uint64_t)data[1] << 48) | ((uint64_t)data[0] << 56);
#endif
}

int64_t kinc_read_s64le(uint8_t *data) {
#ifdef KORE_LITTLE_ENDIAN
	return *(int64_t *)data;
#else
	return ((int64_t)data[0] << 0) | ((int64_t)data[1] << 8) | ((int64_t)data[2] << 16) | ((int64_t)data[3] << 24) | ((int64_t)data[4] << 32) |
	       ((int64_t)data[5] << 40) | ((int64_t)data[6] << 48) | ((int64_t)data[7] << 56);
#endif
}

int64_t kinc_read_s64be(uint8_t *data) {
#ifdef KORE_BIG_ENDIAN
	return *(int64_t *)data;
#else
	return ((int64_t)data[7] << 0) | ((int64_t)data[6] << 8) | ((int64_t)data[5] << 16) | ((int64_t)data[4] << 24) | ((int64_t)data[3] << 32) |
	       ((int64_t)data[2] << 40) | ((int64_t)data[1] << 48) | ((int64_t)data[0] << 56);
#endif
}

uint32_t kinc_read_u32le(uint8_t *data) {
#ifdef KORE_LITTLE_ENDIAN
	return *(uint32_t *)data;
#else
	return (data[0] << 0) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
#endif
}

uint32_t kinc_read_u32be(uint8_t *data) {
#ifdef KORE_BIG_ENDIAN
	return *(uint32_t *)data;
#else
	return (data[3] << 0) | (data[2] << 8) | (data[1] << 16) | (data[0] << 24);
#endif
}

int32_t kinc_read_s32le(uint8_t *data) {
#ifdef KORE_LITTLE_ENDIAN
	return *(int32_t *)data;
#else
	return (data[0] << 0) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
#endif
}

int32_t kinc_read_s32be(uint8_t *data) {
#ifdef KORE_BIG_ENDIAN
	return *(int32_t *)data;
#else
	return (data[3] << 0) | (data[2] << 8) | (data[1] << 16) | (data[0] << 24);
#endif
}

uint16_t kinc_read_u16le(uint8_t *data) {
#ifdef KORE_LITTLE_ENDIAN
	return *(uint16_t *)data;
#else
	return (data[0] << 0) | (data[1] << 8);
#endif
}

uint16_t kinc_read_u16be(uint8_t *data) {
#ifdef KORE_BIG_ENDIAN
	return *(uint16_t *)data;
#else
	return (data[1] << 0) | (data[0] << 8);
#endif
}

int16_t kinc_read_s16le(uint8_t *data) {
#ifdef KORE_LITTLE_ENDIAN
	return *(int16_t *)data;
#else
	return (data[0] << 0) | (data[1] << 8);
#endif
}

int16_t kinc_read_s16be(uint8_t *data) {
#ifdef KORE_BIG_ENDIAN
	return *(int16_t *)data;
#else
	return (data[1] << 0) | (data[0] << 8);
#endif
}

uint8_t kinc_read_u8(uint8_t *data) {
	return *data;
}

int8_t kinc_read_s8(uint8_t *data) {
	return *(int8_t *)data;
}
