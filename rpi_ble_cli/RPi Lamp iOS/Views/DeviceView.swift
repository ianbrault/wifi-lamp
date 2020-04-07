//
//  DeviceView.swift
//  RPi Lamp iOS
//
//  Created by Ian Brault on 3/29/20.
//  Copyright © 2020 Ian Brault. All rights reserved.
//

import SwiftUI

struct DeviceView: View {

    @State var deviceName = "Not Connected"
    @State var deviceMAC  = "00:00:00:00:00:00"
    @State var deviceConnected = false
    @State var searchingForDevice = false

    private func connectToDevice() {
        searchingForDevice = true

        // FIXME: implement
    }

    var body: some View {
        VStack {
            DeviceInfoView(
                name: deviceName,
                mac: deviceMAC,
                isConnected: deviceConnected,
                connectToDevice: connectToDevice)

            Divider()
                .padding([.vertical], 12)

            if !deviceConnected && searchingForDevice {
                Spinner(style: .large)
                    .padding([.top], 24)
                    .padding([.bottom], 8)
                Text("searching for device...")
                    .foregroundColor(Color(UIColor.systemGray))
            }

            Spacer()
        }
        .padding([.horizontal], 32)
    }

}

struct DeviceView_Previews: PreviewProvider {
    static var previews: some View {
        DeviceView()
    }
}
