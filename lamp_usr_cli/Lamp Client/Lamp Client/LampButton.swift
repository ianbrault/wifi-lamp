//
//  LampButton.swift
//  Lamp Client
//
//  Created by Ian Brault on 1/19/21.
//

import SwiftUI

struct LampButtonCenter: View {
    var state: LampState
    
    var body: some View {
        VStack {
            if state == .NotConnected {
                Image(systemName: "lightbulb.slash")
                    .font(.system(size: 36.0))
                    .foregroundColor(DarkPurple)
                Text("not connected")
                    .font(.system(size: 24.0))
                    .foregroundColor(DarkPurple)
            }
        }
    }
}

struct LampButton: View {
    var state: LampState
    var onTap: () -> Void

    init(_ state: LampState, onTap: @escaping () -> Void) {
        self.state = state
        self.onTap = onTap
    }

    var iconName: String {
        switch state {
        case .NotConnected:
            return "wifi.exclamationmark"
        case .Off:
            return "lightbulb.slash"
        case .OnWaiting, .OnPaired:
            return "lightbulb"
        }
    }

    var body: some View {
        GeometryReader {geo in
            ZStack {
                Circle()
                    .strokeBorder(strokeColor(state), lineWidth: 6)
                    .background(Circle().foregroundColor(fillColor(state)))
                    .frame(width: geo.size.width, height: geo.size.height)

                VStack {
                    Image(systemName: iconName)
                        .font(.system(size: 80))
                        .foregroundColor(strokeColor(state))
                }
            }
            .onTapGesture {
                onTap()
            }
        }
    }
}

struct LampButton_Previews: PreviewProvider {
    static var previews: some View {
        LampButton(.OnPaired, onTap: {})
    }
}
