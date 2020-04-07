/*
** uuid.hpp
*/

#ifndef UUID_HPP
#define UUID_HPP

#include <QBluetoothUuid>
#include <QString>

const static QString BASE_UUID = "dead-fade-cafe-204080160320";
inline QBluetoothUuid get_uuid(QString uuid)
{
    return QBluetoothUuid(QString("0000%1-%2").arg(uuid, BASE_UUID));
}

// BLE server groups all characteristics into a single service
const auto SVC_UUID  = get_uuid("0001");

// BLE characteristics
const auto MAC_UUID      = get_uuid("0002");  // MAC address
const auto NWK_STAT_UUID = get_uuid("0003");  // network status
const auto NWK_SSID_UUID = get_uuid("0004");  // network SSID
const auto NWK_PWD_UUID  = get_uuid("0005");  // network password
const auto PTNR_MAC_UUID = get_uuid("0006");  // MAC address of device partner

#endif
