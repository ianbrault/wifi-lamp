/*
** device_info.hpp
*/

#include <string>

#include <QByteArray>

class DeviceInfo
{
public:
    DeviceInfo();

public:
    const std::string& name() const;
    const std::string& mac_string() const;
    const QByteArray   mac_bytes() const;

    void set_name(std::string&&);
    void set_mac(QByteArray&&);

private:
    const std::string filepath() const;
    void load_from_file();
    void save_to_file();

private:
    std::string m_name;
    std::string m_mac;
};
