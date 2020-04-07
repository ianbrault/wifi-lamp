//
//  DeviceInfoView.swift
//  RPi Lamp iOS
//
//  Created by Ian Brault on 3/29/20.
//  Copyright © 2020 Ian Brault. All rights reserved.
//

import SwiftUI

struct DeviceInfoView: View {

    let name: String
    let mac: String
    let isConnected: Bool
    let connectToDevice: () -> ()

    private var lampIcon: some View {
        let icon: Image
        let color: Color

        if isConnected {
            icon = Image(systemName: "lightbulb")
            color = Color.green
        } else {
            icon = Image(systemName: "lightbulb.slash")
            color = Color.red
        }

        return icon
            .resizable()
            .aspectRatio(contentMode: .fit)
            .foregroundColor(color)
    }

    private var controlIcon: some View {
        let icon: Image
        let size: CGFloat

        if isConnected {
            icon = Image(systemName: "slider.horizontal.3")
            size = 30
        } else {
            icon = Image(systemName: "arrow.2.circlepath")
            size = 40
        }

        return icon
            .resizable()
            .aspectRatio(contentMode: .fit)
            .foregroundColor(Color(UIColor.systemGray))
            .frame(width: size, height: size)
    }

    private var controlAction: () -> () {
        if isConnected {
            // FIXME: edit mode
            return {}
        } else {
            return connectToDevice
        }
    }

    var body: some View {
        HStack(alignment: .center, spacing: 20) {
            lampIcon
                .frame(width: 40, height: 40)

            VStack(alignment: .leading, spacing: 4) {
                Text(name)
                    .bold()
                    .font(.system(size: 24))
                Text(mac)
                    .foregroundColor(Color(UIColor.systemGray))
            }

            Spacer()

            Button(action: controlAction) {
                controlIcon
            }
        }
    }
}

struct DeviceInfoView_Previews: PreviewProvider {
    static var previews: some View {
        DeviceInfoView(
            name: "Arni's Lamp", mac: "01:23:45:67:89:ab",
            isConnected: true, connectToDevice: {})
    }
}
