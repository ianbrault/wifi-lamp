//
//  SetupView.swift
//
//  Created by Ian Brault on 6/29/20.
//  Copyright © 2020 Ian Brault. All rights reserved.
//

import SwiftUI

fileprivate struct NameScreen: View {
    var body: some View {
        Text("NameScreen")
    }
}

fileprivate struct WifiScreen: View {
    var body: some View {
        Text("WifiScreen")
    }
}

fileprivate struct PairScreen: View {
    var body: some View {
        Text("PairScreen")
    }
}

struct SetupView: View {

    private enum Screen {
        case Name
        case Wifi
        case Pair
    }

    @State private var screen: Screen = .Name

    private var title: String {
        switch screen {
        case .Name:
            return "Set up your Lamp"
        case .Wifi:
            return "Connect to WiFi"
        case .Pair:
            return "Pair with another Lamp"
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
            } else if screen == .Pair {
                PairScreen()
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
