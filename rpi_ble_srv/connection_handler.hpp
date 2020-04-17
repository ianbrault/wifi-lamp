/*
** connection_handler.hpp
*/

#include <QLowEnergyController>

class ConnectionHandler : public QObject
{
    Q_OBJECT

public:
    ConnectionHandler(QObject* parent = nullptr);
    virtual ~ConnectionHandler() {};

public slots:
    void client_connected();
    void client_disconnected();
    void state_changed(QLowEnergyController::ControllerState);
};
