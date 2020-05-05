/*
** device_info.cpp
*/

#include "device_info.hpp"

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

static bool file_exists(const std::string& path)
{
    struct stat buf;
    return (stat(path.c_str(), &buf) == 0);
}

static std::string get_mac()
{
    std::string mac;
    char buf[128];

    auto pipe = popen("ifconfig wlan0", "r");
    while (fgets(buf, 128, pipe) != nullptr)
    {
        // find the first non-whitespace character
        int i = 0;
        while (isspace(buf[i++])) {}

        if (strncmp(buf + i - 1, "ether", 5) == 0)
        {
            mac = std::string(buf + i + 5, 17);
            break;
        }
    }

    pclose(pipe);
    return mac;
}

DeviceInfo::DeviceInfo()
{
    if (file_exists(filepath()))
        load_from_file();
    else
    {
        m_name = "UNNAMED";
        m_mac = get_mac();
        m_ssid = "";
        m_pwd = "";

        save_to_file();
    }
}

const std::string DeviceInfo::filepath() const
{
    std::stringstream stream;
    stream << std::getenv("HOME") << "/device.info";

    return stream.str();
}

void DeviceInfo::load_from_file()
{
    std::ifstream file(filepath());
    std::string line;
    getline(file, line);

    size_t pos, pos_prev;

    pos = line.find(',');
    m_name = line.substr(0, pos);
    pos_prev = pos + 1;

    pos = line.find(',', pos_prev);
    m_mac = line.substr(pos_prev, pos - pos_prev);
    pos_prev = pos + 1;

    pos = line.find(',', pos_prev);
    m_ssid = line.substr(pos_prev, pos - pos_prev);
    pos_prev = pos + 1;

    pos = line.find('\n', pos_prev);
    m_pwd = line.substr(pos_prev, pos - pos_prev);
}

void DeviceInfo::save_to_file()
{
    std::ofstream file(filepath(), std::ios_base::out | std::ios_base::trunc);
    file << m_name << ","
         << m_mac  << ","
         << m_ssid << ","
         << m_pwd  << "\n";
}

const std::string& DeviceInfo::name() const
{
    return m_name;
}

const std::string& DeviceInfo::mac_string() const
{
    return m_mac;
}

const QByteArray DeviceInfo::mac_bytes() const
{
    QByteArray bytes;

    for (int i = 0; i < 6; i++)
    {
        auto byte = std::stoi(m_mac.substr(i * 3, 2), nullptr, 16);
        bytes.append((char) byte);
    }

    return bytes;
}

const std::string& DeviceInfo::network_ssid() const
{
    return m_ssid;
}

const std::string& DeviceInfo::network_password() const
{
    return m_pwd;
}

void DeviceInfo::set_name(std::string&& name)
{
    m_name = name;
    save_to_file();
}

void DeviceInfo::set_network_ssid(std::string&& ssid)
{
    m_ssid = ssid;
    save_to_file();
}

void DeviceInfo::set_network_password(std::string&& pwd)
{
    m_pwd = pwd;
    save_to_file();
}
