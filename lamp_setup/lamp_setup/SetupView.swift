//
//  SetupView.swift
//
//  Created by Ian Brault on 6/29/20.
//  Copyright © 2020 Ian Brault. All rights reserved.
//

import SwiftUI

fileprivate struct NextButton: View {
    var onPress: () -> Void

    var body: some View {
        Button(action: onPress) {
            Text("Next")
                .padding(.vertical, 4)
                .padding(.horizontal, 8)
        }
        .padding(.bottom, 16)
    }
}

enum Owner: String, CaseIterable, Identifiable {
    case Arni
    case Ian

    var id: String { self.rawValue }
}

fileprivate struct NameScreen: View {
    @Binding var owner: Owner
    var onButton: () -> Void

    var body: some View {
        VStack {
            Picker("", selection: $owner) {
                Text("Arni's Lamp").tag(Owner.Arni)
                Text("Ian's Lamp").tag(Owner.Ian)
            }
            .pickerStyle(RadioGroupPickerStyle())
            .frame(maxWidth: .infinity, maxHeight: .infinity)

            NextButton(onPress: onButton)
        }
    }
}

fileprivate struct WifiRow: View {
    var ssid: String

    var body: some View {
        Text(ssid)
    }
}

fileprivate struct WifiScreen: View {
    var owner: Owner

    @State private var networks: [String] = []
    @State private var selection = String?.none
    @State private var wifiPassword: String = ""

    private func onButton() {
        // Raspberry Pi SSH key path
        let keyName = (owner == .Arni) ? "arni" : "ian"
        let keyPath = Bundle.main.resourcePath! + "/id_rsa_" + keyName
        print(keyPath)

        // NOTE: SSH key permissions are wiped when copied into the bundle
        let chmod_task = Process()
        chmod_task.executableURL = URL(fileURLWithPath: "/bin/chmod")
        chmod_task.arguments = ["600", keyPath]

        do {
            try chmod_task.run()
        } catch {
            // FIXME
            print("CAUGHT SOMETHING: \(error)")
            return
        }

        // modify the Raspberry Pi WPA supplicant file
        let task = Process()
        task.executableURL = URL(fileURLWithPath: "/usr/bin/ssh")
        task.arguments = [
            "-i", keyPath, "pi@raspberrypi.local", "touch", "/home/pi/sentinel"
        ]

        do {
            try task.run()
        } catch {
            // FIXME
            print("CAUGHT SOMETHING: \(error)")
        }
    }

    private func scanForNetworks() {
        DispatchQueue.global(qos: .background).async {
            if let scanner = WifiScanner() {
                var ssids = scanner.discoverNetworks()
                // sort SSIDS alphabetically (ignoring case)
                ssids.sort {
                    $0.lowercased() <= $1.lowercased()
                }

                DispatchQueue.main.async {
                    self.networks = ssids
                }
            }
        }
    }

    var body: some View {
        VStack {
            if networks.isEmpty {
                SpinnerView()
            } else {
                List(networks, id: \.self, selection: $selection) {ssid in
                    WifiRow(ssid: ssid)
                }
                .border(Color(NSColor.systemGray))

                SecureField("Password", text: $wifiPassword)
                    .padding(.vertical, 8)

                NextButton(onPress: onButton)
                    .disabled(selection == nil || wifiPassword.isEmpty)
            }
        }
        .padding(.horizontal, 40)
        .padding(.vertical, 16)
        .onAppear(perform: scanForNetworks)
    }
}

struct SetupView: View {

    private enum Screen {
        case Name
        case Wifi
    }

    @State private var screen: Screen = .Name
    @State private var owner: Owner = .Arni

    private var title: String {
        switch screen {
        case .Name:
            return "Whose lamp is this?"
        case .Wifi:
            return "Connect to WiFi"
        }
    }

    var body: some View {
        VStack {
            Text(title)
                .font(.headline)
                .padding(.top, 20)

            if screen == .Name {
                NameScreen(owner: $owner, onButton: {
                    self.screen = .Wifi
                })
                .frame(maxWidth: .infinity, maxHeight: .infinity)
            } else if screen == .Wifi {
                WifiScreen(owner: owner)
                    .frame(maxWidth: .infinity, maxHeight: .infinity)
            }
        }
    }
}

struct SetupView_Previews: PreviewProvider {
    static var previews: some View {
        SetupView()
    }
}
