//
//  Lamp_ClientApp.swift
//  Lamp Client
//
//  Created by Ian Brault on 12/27/20.
//

import SwiftUI

@main
struct Lamp_ClientApp: App {
    @State private var lampUser = UserDefaults.standard.string(forKey: "lampUser")

    func setLampUser(_ name: String) {
        UserDefaults.standard.set(name, forKey: "lampUser")
        lampUser = name
    }

    var body: some Scene {
        WindowGroup {
            // if the user has not been set, show the selection view
            if lampUser == nil {
                UserSelectView(setLampUser: setLampUser)
            } else {
                ContentView(user: lampUser!)
            }
        }
    }
}
