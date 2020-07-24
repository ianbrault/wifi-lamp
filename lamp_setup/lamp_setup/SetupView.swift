//
//  SetupView.swift
//
//  Created by Ian Brault on 6/29/20.
//  Copyright © 2020 Ian Brault. All rights reserved.
//

import SwiftUI

enum Owner: String, CaseIterable, Identifiable {
    case Arni
    case Ian

    var id: String { self.rawValue }
}

fileprivate struct NameScreen: View {
    @State var selectedOwner = Owner.Arni

    var body: some View {
        Picker("", selection: $selectedOwner) {
            Text("Arni's Lamp").tag(Owner.Arni)
            Text("Ian's Lamp").tag(Owner.Ian)
        }
        .pickerStyle(RadioGroupPickerStyle())
    }
}

fileprivate struct WifiScreen: View {
    var body: some View {
        Text("WifiScreen")
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

    private var buttonLabel: String {
        switch screen {
        case .Name:
            return "Next"
        case .Wifi:
            return "Done"
        }
    }

    private func onButton() {
        switch screen {
        case .Name:
            // transition to the Wifi screen
            screen = .Wifi
        case .Wifi:
            // TODO
            break
        }
    }

    var body: some View {
        VStack {
            Text(title)
                .font(.headline)
                .padding(.top, 20)

            if screen == .Name {
                NameScreen()
                    .frame(maxWidth: .infinity, maxHeight: .infinity)
            } else if screen == .Wifi {
                WifiScreen()
                    .frame(maxWidth: .infinity, maxHeight: .infinity)
            }

            Button(action: onButton) {
                Text(buttonLabel)
                    .padding(.vertical, 4)
                    .padding(.horizontal, 8)
            }
            .padding(.bottom, 16)
        }
    }
}

struct SetupView_Previews: PreviewProvider {
    static var previews: some View {
        SetupView()
    }
}
