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
    var onButton: () -> Void
    @State var selectedOwner = Owner.Arni

    var body: some View {
        VStack {
            Picker("", selection: $selectedOwner) {
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
    var onButton: () -> Void
    @State private var networks: [String] = []
    @State private var selection = String?.none
    @State private var wifiPassword: String = ""

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
                NameScreen(onButton: {
                    self.screen = .Wifi
                })
                .frame(maxWidth: .infinity, maxHeight: .infinity)
            } else if screen == .Wifi {
                WifiScreen(onButton: {

                })
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
