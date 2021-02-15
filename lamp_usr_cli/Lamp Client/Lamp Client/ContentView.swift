//
//  ContentView.swift
//  Lamp Client
//
//  Created by Ian Brault on 12/27/20.
//

import SwiftUI

enum LampState {
    case NotConnected
    case Off
    case On
}

struct Header: View {
    var user: String
    var state: LampState

    @State private var time = Date()
    private var dateFormatter = DateFormatter()
    private var timeFormatter = DateFormatter()

    let timer = Timer.publish(every: 1, on: .main, in: .common).autoconnect()

    init(_ user: String, _ state: LampState) {
        self.user = user
        self.state = state

        dateFormatter.locale = Locale(identifier: "en_US")
        dateFormatter.setLocalizedDateFormatFromTemplate("MMMM d, yyyy")

        timeFormatter.locale = Locale(identifier: "en_US")
        timeFormatter.setLocalizedDateFormatFromTemplate("h:mm a")
    }

    var body: some View {
        VStack {
            Text("\(user)'s Lamp")
                .font(.system(size: 24))
                .fontWeight(.semibold)
                .foregroundColor(strokeColor(state))

            Text("\(timeFormatter.string(from: time))")
                .font(.system(size: 36))
                .bold()
                .foregroundColor(strokeColor(state))

            Text("\(dateFormatter.string(from: time))")
                .font(.system(size: 20))
                .foregroundColor(strokeColor(state))
        }
        .onReceive(timer, perform: {time in
            self.time = Date()
        })
    }
}

struct Footer: View {
    var state: LampState

    init(_ state: LampState) {
        self.state = state
    }

    func topText(_ state: LampState) -> String {
        switch state {
        case .NotConnected:
            return "not connected"
        case .Off:
            return "powered off"
        case .On:
            return "powered on"
        }
    }

    func bottomText(_ state: LampState) -> String {
        switch state {
        case .NotConnected:
            return "tap to connect to the device"
        case .Off:
            return "tap to power on"
        case .On:
            return "tap to power off"
        }
    }

    var body: some View {
        VStack(spacing: 4) {
            Text(topText(state))
                .font(.system(size: 24))
                .fontWeight(.medium)
                .foregroundColor(strokeColor(state))

            Text(bottomText(state))
                .font(.system(size: 18))
                .fontWeight(.light)
                .foregroundColor(strokeColor(state))
        }
    }
}

struct AlertItem: Identifiable {
    var id = UUID()
    var title: Text
    var message: Text?
    var dismissButton: Alert.Button?
}

struct ContentView: View {
    var user: String
    var client = LampClient()

    @State private var isConnectingToServer = true
    @State private var state: LampState = .NotConnected
    @State private var alertItem: AlertItem?

    func connectToServer() {
        let success = client.open_connection(address: LampAddress, owner: user)
        if success {
            isConnectingToServer = false
            // FIXME: wait for the current lamp state to be sent from the server
            state = .Off
        } else {
            // display an alert and re-try (if commanded)
            alertItem = AlertItem(
                title: Text("Connection Failed"),
                message: Text("Would you like to try again?"),
                dismissButton: .default(Text("Retry")) {
                    connectToServer()
                }
            )
        }
    }

    func buttonPressedDebug() {
        // note: debug implementation, just cycle through the states
        switch state {
        case .NotConnected:
            state = .Off
        case .Off:
            state = .On
        case .On:
            state = .NotConnected
        }
    }

    func buttonPressed() {
        // note: uncomment for a debug implementation
        // return buttonPressedDebug()

        switch state {
        case .NotConnected:
            // do nothing and wait for the lamp to connect to the server
            return
        case .Off:
            // turn the lamp on
            if client.turn_on() {
                state = .On
            }
        case .On:
            // turn the lamp off
            if client.turn_off() {
                state = .Off
            }
        }
    }

    var body: some View {
        ZStack {
            // color should fill the entire screen
            // show both colors but animate the opacity in and out
            // note: no need to show .NotConnected, which is white
            LightBlue
                .ignoresSafeArea()
                .opacity(state == .Off ? 1 : 0)
                .animation(.easeOut(duration: 0.32))
            LightGreen
                .ignoresSafeArea()
                .opacity(state == .On ? 1 : 0)
                .animation(.easeOut(duration: 0.32))

            VStack {
                if isConnectingToServer {
                    ProgressView()
                        .padding(.vertical, 12)
                    Text("connecting to the server...")
                        .font(.system(size: 14))
                        .fontWeight(.light)
                } else {
                    Header(user, state)
                    LampButton(state, onTap: buttonPressed)
                        .padding(.horizontal, 64)
                    Footer(state)
                }
            }
            .padding(.vertical, 40)
            .alert(item: $alertItem) { alertItem in
                Alert(title: alertItem.title, message: alertItem.message, dismissButton: alertItem.dismissButton)
            }
        }
        .onAppear {
            DispatchQueue.main.async {
                connectToServer()
            }
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView(user: "Ian")
    }
}
