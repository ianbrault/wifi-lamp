/*
** connection_handler.cpp
*/

#include <QDebug>

#include "connection_handler.hpp"

ConnectionHandler::ConnectionHandler(QObject* parent)
    : QObject(parent)
{

}

void ConnectionHandler::client_connected()
{
    qDebug() << "Client connected";
}

void ConnectionHandler::client_disconnected()
{
    qDebug() << "Client disconnected";
}

void ConnectionHandler::state_changed(QLowEnergyController::ControllerState state)
{
    switch (state)
    {
    case QLowEnergyController::UnconnectedState:
        qDebug() << "Controller is unconnected";
        break;
    case QLowEnergyController::ConnectingState:
        qDebug() << "Controller is attempting to connect to a device";
        break;
    case QLowEnergyController::ConnectedState:
        qDebug() << "Controller connected to a device";
        break;
    case QLowEnergyController::DiscoveringState:
        break;
    case QLowEnergyController::DiscoveredState:
        break;
    case QLowEnergyController::ClosingState:
        qDebug() << "Controller is about to be disconnected from the device";
        break;
    case QLowEnergyController::AdvertisingState:
        qDebug() << "Controller is advertising data";
        break;
    }
}
