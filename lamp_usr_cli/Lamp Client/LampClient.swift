//
//  LampClient.swift
//  Lamp Client
//
//  Created by Ian Brault on 12/28/20.
//

import Foundation

// NOTE: this is the debug address
let LampAddress = "ws://127.0.0.1:8182"

let Arni: UInt8 = 0x30
let Ian: UInt8 = 0x31

class LampClient {
    var socket: WebSocket_AutoStream

    init() {
        self.socket = WebSocket_AutoStream(inner: nil)
    }

    deinit {
        // close the websocket connection and free websocket memory
        disconnect_from_lamp(self.socket)
    }

    func open_connection(address: String, owner: String) -> Bool {
        var ownerInt: UInt8
        if owner == "Arni" {
            ownerInt = Arni
        } else if owner == "Ian" {
            ownerInt = Ian
        } else {
            return false
        }

        // open the websocket connection
        self.socket = connect_to_lamp(address)

        if self.socket.inner != nil {
            // declare the client type
            return declare_client_connection(self.socket, ownerInt)
        } else {
            return false
        }
    }

    func turn_on() -> Bool {
        return power_on(self.socket)
    }

    func turn_off() -> Bool {
        return power_off(self.socket)
    }
}
