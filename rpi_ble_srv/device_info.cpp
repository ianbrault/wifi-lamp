/*
** device_info.cpp
*/

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <unistd.h>

#include "device_info.hpp"

static bool file_exists(const std::string& path)
{
    return (access(path.c_str(), F_OK) != -1);
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

        save_to_file();
    }
}

const std::string DeviceInfo::filepath() const
{
    return std::string(std::getenv("HOME")) + "/device.info";
}

void DeviceInfo::load_from_file()
{
    std::ifstream file(filepath());
    std::string line;
    getline(file, line);

    size_t pos, pos_prev;

    pos = line.find(',');
    m_name = line.substr(0, pos);
    pos_prev = pos;

    pos = line.find('\n', pos_prev);
    m_mac = line.substr(pos_prev + 1, pos);
}

void DeviceInfo::save_to_file()
{
    std::ofstream file(filepath(), std::ios_base::out | std::ios_base::trunc);
    file << m_name << ","
         << m_mac  << "\n";
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
    char mac_bytes[6];
    for (int i = 0; i < 6; i++)
        mac_bytes[i] = std::stoi(m_mac.substr(i * 2, 2), nullptr, 16);

    return QByteArray::fromRawData(mac_bytes, 6);
}

void DeviceInfo::set_name(std::string&& name)
{
    m_name = name;
}

void DeviceInfo::set_mac(QByteArray&& mac)
{
    char mac_str[18];
    snprintf(
        mac_str, 17, "%02x:%02x:%02x:%02x:%02x:%02x",
        mac.at(0), mac.at(1), mac.at(2), mac.at(3), mac.at(4), mac.at(5));
    mac_str[17] = 0;

    m_mac = std::string(mac_str);
}