/*
** uuid.hpp
*/

#ifndef UUID_HPP
#define UUID_HPP

#include <QBluetoothUuid>
#include <QString>

const static QString BASE_UUID = "-dead-fade-cafe-204080160320";
#define GET_UUID_STR(n) \
    (((n < 0xf) ? "0000000" : "000000") + QString::number(n, 16) + BASE_UUID)
#define GET_UUID(n) QBluetoothUuid(GET_UUID_STR(n))

// group all characteristics into this single service
const auto SVC_UUID  = GET_UUID(1);

// BLE characteristics & descriptors

const auto MAC_UUID     = GET_UUID(2);
const auto MAC_DSC_UUID = GET_UUID(3);

const auto NWK_STAT_UUID     = GET_UUID(4);
const auto NWK_STAT_DSC_UUID = GET_UUID(5);

const auto NWK_SSID_UUID     = GET_UUID(6);
const auto NWK_SSID_DSC_UUID = GET_UUID(7);

const auto NWK_PWD_UUID     = GET_UUID(8);
const auto NWK_PWD_DSC_UUID = GET_UUID(9);

const auto PTNR_UUID     = GET_UUID(10);
const auto PTNR_DSC_UUID = GET_UUID(11);

#endif
