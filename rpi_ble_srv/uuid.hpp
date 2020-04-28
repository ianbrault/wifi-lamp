/*
** uuid.hpp
*/

#ifndef UUID_HPP
#define UUID_HPP

#include <QBluetoothUuid>

// device information service & characteristics
const auto DeviceInfoService = QBluetoothUuid::DeviceInformation;
const auto DeviceNameChar    = QBluetoothUuid::DeviceName;
const auto DeviceMacChar     = QBluetoothUuid::HardwareRevisionString;
const auto DevicePartnerChar = QBluetoothUuid::Gender;

// device network service & characteristics
const auto DeviceNetworkService = QBluetoothUuid::GenericNetworking;
const auto NetworkStatusChar    = QBluetoothUuid::AlertStatus;
const auto NetworkSSIDChar      = QBluetoothUuid::FirstName;
const auto NetworkPasswordChar  = QBluetoothUuid::LastName;

#endif
