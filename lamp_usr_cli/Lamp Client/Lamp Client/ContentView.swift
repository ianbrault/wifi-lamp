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
    case OnWaiting
    case OnPaired
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
        case .OnWaiting, .OnPaired:
            return "powered on"
        }
    }

    func bottomText(_ state: LampState) -> String {
        switch state {
        case .NotConnected:
            return "tap to connect to the device"
        case .Off:
            return "tap to power on"
        case .OnWaiting, .OnPaired:
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

struct ContentView: View {
    var user: String

    @State private var state: LampState = .NotConnected

    func buttonPressedDebug() {
        // NOTE: debug implementation, just cycle through the states
        switch state {
        case .NotConnected:
            state = .Off
        case .Off:
            state = .OnPaired
        case .OnWaiting, .OnPaired:
            state = .NotConnected
        }
    }

    func buttonPressed() {
        // FIXME: add a real implementation
        buttonPressedDebug()
    }

    var body: some View {
        ZStack {
            // color should fill the entire screen
            fillColor(state)
                .ignoresSafeArea()

            VStack {
                Header(user, state)

                LampButton(state, onTap: buttonPressed)
                    .padding(.horizontal, 64)

                Footer(state)
            }
            .padding(.vertical, 40)
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView(user: "Ian")
    }
}
