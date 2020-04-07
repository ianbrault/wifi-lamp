//
//  ContentView.swift
//  RPi Lamp iOS
//
//  Created by Ian Brault on 3/29/20.
//  Copyright © 2020 Ian Brault. All rights reserved.
//

import SwiftUI

struct ContentView: View {
    var body: some View {
        VStack {
            TopBanner()
                .frame(height: 160)
            DeviceView()
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
