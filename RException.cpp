#include "RException.h"

#include <sstream>

RException::RException(int line, const char* file) noexcept
    :
    line(line),
    file(file)
{
}

const char* RException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();

}

const char* RException::GetType() const noexcept
{
    return "R Exception";
}



int RException::GetLine() const noexcept
{
    return line;
}

const std::string& RException::GetFile() const noexcept
{
    return file;
}

std::string RException::GetOriginString() const noexcept
{
    std::ostringstream oss;
    oss << "[File] " << file << std::endl
        << "[Line] " << line;
    return oss.str(); 
}
