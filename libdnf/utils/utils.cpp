#include "utils.hpp"

#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <stdexcept>

#include <fcntl.h>
#include <unistd.h>
#include <string.h>

std::vector<std::string> string::split(const std::string &source, const char *delimiter, int maxSplit)
{
    if (source.empty())
        throw std::runtime_error{"Source cannot be empty"};

    std::string::size_type tokenBeginIndex = 0;
    std::vector<std::string> container;

    while ((tokenBeginIndex = source.find_first_not_of(delimiter, tokenBeginIndex)) != std::string::npos) {
        if (maxSplit != -1 && ((int) (container.size() + 1 )) == maxSplit) {
            container.emplace_back(source.substr(tokenBeginIndex));
            break;
        }

        auto tokenEndIndex = source.find_first_of(delimiter, tokenBeginIndex);
        container.emplace_back(source.substr(tokenBeginIndex, tokenEndIndex - tokenBeginIndex));
        tokenBeginIndex = tokenEndIndex;
    }

    if (container.empty()) {
        throw std::runtime_error{"No delimiter found in source: " + source};
    }

    return container;
}

std::vector<std::string> string::rsplit(const std::string &source, const char *delimiter, int maxSplit)
{
    if (source.empty())
        throw std::runtime_error{"Source cannot be empty"};

    std::string sequence = source;
    std::string::size_type tokenBeginIndex = 0;
    std::vector<std::string> container;

    while ((tokenBeginIndex = sequence.find_last_of(delimiter)) != std::string::npos) {
        if (maxSplit != -1 && ((int) (container.size() + 1 )) == maxSplit) {
            container.emplace_back(source.substr(0, tokenBeginIndex));
            break;
        }

        container.emplace(container.begin(), source.substr(tokenBeginIndex + 1));
        sequence = sequence.substr(0, tokenBeginIndex);
    }

    if (container.empty()) {
        throw std::runtime_error{"No delimiter found in source: " + source};
    }

    return container;
}

std::string string::trimSuffix(const std::string &source, const std::string &suffix)
{
    if (source.length() < suffix.length())
        throw std::runtime_error{"Suffix cannot be longer than source"};

    if (string::endsWith(source, suffix))
        return source.substr(0, source.length() - suffix.length());

    throw std::runtime_error{"Suffix '" + suffix + "' not found"};
}

std::string string::trimPrefix(const std::string &source, const std::string &prefix)
{
    if (source.length() < prefix.length())
        throw std::runtime_error{"Prefix cannot be longer than source"};

    if (string::startsWith(source, prefix))
        return source.substr(prefix.length() - 1);

    throw std::runtime_error{"Prefix '" + prefix + "' not found"};
}

bool string::startsWith(const std::string &source, const std::string &toMatch)
{
    return source.find(toMatch) == 0;
}

bool string::endsWith(const std::string &source, const std::string &toMatch)
{
    auto it = toMatch.begin();
    return source.size() >= toMatch.size() &&
           std::all_of(std::next(source.begin(), source.size() - toMatch.size()), source.end(), [&it](const char &c) {
               return c == *(it++);
           });
}

bool haveFilesSameContent(const char * filePath1, const char * filePath2)
{
    static constexpr int BLOCK_SIZE = 4096;
    bool ret = false;
    int fd1 = -1;
    int fd2 = -1;
    do {
        if ((fd1 = open(filePath1, 0)) == -1)
            break;
        if ((fd2 = open(filePath2, 0)) == -1)
            break;
        auto len1 = lseek(fd1, 0, SEEK_END);
        auto len2 = lseek(fd2, 0, SEEK_END);
        if (len1 != len2)
            break;
        ret = true;
        if (len1 == 0)
            break;
        lseek(fd1, 0, SEEK_SET);
        lseek(fd2, 0, SEEK_SET);
        char buf1[BLOCK_SIZE], buf2[BLOCK_SIZE];
        ssize_t readed;
        do {
            readed = read(fd1, buf1, BLOCK_SIZE);
            auto readed2 = read(fd2, buf2, BLOCK_SIZE);
            if (readed2 != readed || memcmp(buf1, buf2, readed) != 0) {
                ret = false;
                break;
            }
        } while (readed > BLOCK_SIZE);
    } while (false);

    if (fd1 != -1)
        close(fd1);
    if (fd2 != -1)
        close(fd2);
    return ret;
}

bool filesystem::exists(const std::string &name)
{
    struct stat buffer;
    return stat(name.c_str(), &buffer) == 0;
}

std::vector<std::string> filesystem::getDirContent(const std::string &dirPath)
{
    std::vector<std::string> content{};
    struct dirent *ent;
    DIR *dir = opendir(dirPath.c_str());

    if (dir != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            if (strcmp(ent->d_name, "..") == 0 ||
                    strcmp(ent->d_name, ".") == 0 )
                continue;

            auto fullPath = dirPath;
            if (!string::endsWith(fullPath, "/"))
                fullPath += "/";
            fullPath += ent->d_name;

            content.emplace_back(fullPath);
        }
        closedir (dir);
    }

    return content;
}
