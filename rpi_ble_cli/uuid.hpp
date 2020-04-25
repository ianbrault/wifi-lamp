#ifndef UUID_HPP
#define UUID_HPP

#include <QString>

const static QString BASE_UUID = "-dead-fade-cafe-204080160320";
#define GET_UUID(s) (QString("0000") + s + BASE_UUID)

// group all characteristics into this single service
const auto SVC_UUID = GET_UUID("0000");

// BLE characteristics

const auto DEV_MAC_UUID  = GET_UUID("0001");
const auto DEV_NAME_UUID = GET_UUID("0002");
const auto DEV_PAIR_UUID = GET_UUID("0003");
const auto NWK_STAT_UUID = GET_UUID("0004");
const auto NWK_SSID_UUID = GET_UUID("0005");
const auto NWK_PWD_UUID  = GET_UUID("0006");

#endif // UUID_HPP
