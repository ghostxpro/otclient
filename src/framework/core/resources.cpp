/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include <prerequisites.h>
#include <core/resources.h>

#include <physfs.h>

Resources g_resources;

void Resources::init(const char *argv0)
{
    PHYSFS_init(argv0);
}

void Resources::terminate()
{
    PHYSFS_deinit();
}

bool Resources::setWriteDir(const std::string& path)
{
    bool ret = (bool)PHYSFS_setWriteDir(path.c_str());

    if(!ret)
        flogError("ERROR: Could not set the path \"%s\" as write directory, file write will not work correctly.", path.c_str());
    return ret;
}

bool Resources::addToSearchPath(const std::string& path, bool insertInFront /*= true*/)
{
    if(!PHYSFS_addToSearchPath(path.c_str(), insertInFront ? 0 : 1)) {
        flogError("ERROR: Error while adding \"%s\" to resources search path: %s", path.c_str() % PHYSFS_getLastError());
        return false;
    }
    return true;
}

bool Resources::fileExists(const std::string& filePath)
{
    return PHYSFS_exists(filePath.c_str());
}

uchar *Resources::loadFile(const std::string& fileName, uint *fileSize)
{
    PHYSFS_file *file = PHYSFS_openRead(fileName.c_str());
    if(!file) {
        flogError("ERROR: Failed to load file \"%s\": %s", fileName.c_str() % PHYSFS_getLastError());
        *fileSize = 0;
        return NULL;
    }

    *fileSize = PHYSFS_fileLength(file);
    uchar *buffer = new uchar[*fileSize + 1];
    PHYSFS_read(file, (void*)buffer, 1, *fileSize);
    buffer[*fileSize] = 0;
    PHYSFS_close(file);
    return buffer;
}

std::string Resources::loadTextFile(const std::string& fileName)
{
    std::string text;
    uint fileSize;
    char *buffer = (char *)loadFile(fileName, &fileSize);
    if(buffer) {
        text.assign(buffer);
        delete[] buffer;
    }
    return text;
}

bool Resources::saveFile(const std::string &fileName, const uchar *data, uint size)
{
    PHYSFS_file *file = PHYSFS_openWrite(fileName.c_str());
    if(!file) {
        flogError("ERROR: Failed to save file \"%s\": %s", fileName.c_str() % PHYSFS_getLastError());
        return false;
    }

    PHYSFS_write(file, (void*)data, size, 1);
    PHYSFS_close(file);
    return true;
}

bool Resources::saveTextFile(const std::string &fileName, std::string text)
{
    return saveFile(fileName, (const uchar*)text.c_str(), text.size());
}

std::list<std::string> Resources::getDirectoryFiles(const std::string& directory)
{
    std::list<std::string> files;
    char **rc = PHYSFS_enumerateFiles(directory.c_str());

    for(char **i = rc; *i != NULL; i++)
        files.push_back(*i);

    PHYSFS_freeList(rc);
    return files;
}