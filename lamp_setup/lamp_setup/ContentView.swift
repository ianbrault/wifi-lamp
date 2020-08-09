//
//  ContentView.swift
//
//  Created by Ian Brault on 6/7/20.
//  Copyright © 2020 Ian Brault. All rights reserved.
//

import SwiftUI

struct ContentView: View {

    var quitAction: () -> Void

    private enum Status {
        case Searching
        case Connected
    }

    @State private var status: Status = .Searching
    @State var showNotFoundAlert = false

    private let username = "pi"
    private let hostname = "raspberrypi.local"

    func checkForDevice() {
        status = .Searching

        // ping the Raspberry Pi
        let task = Process()
        task.executableURL = URL(fileURLWithPath: "/sbin/ping")
        task.arguments = ["-c", "1", hostname]
        task.terminationHandler = {(process) in
            if process.terminationStatus == 0 {
                self.status = .Connected
            } else {
                // device not found
                // FIXME: self.showNotFoundAlert = true
                self.status = .Connected
            }
        }

        do {
            try task.run()
        } catch {
            showNotFoundAlert = true
        }
    }

    var body: some View {
        VStack {
            LogoView()

            VStack {
                // display the loading indicator if searching (or if search failed)
                if self.status == .Searching {
                    SpinnerView()
                } else if self.status == .Connected {
                    SetupView()
                }
            }.frame(maxWidth: .infinity, maxHeight: .infinity)
        }
        .padding(.vertical, 32)
        .onAppear(perform: checkForDevice)
        .alert(isPresented: self.$showNotFoundAlert) {
            Alert(
                title: Text("Device not found"),
                message: Text("Make sure that your device is connected to your computer via USB."),
                primaryButton: .default(Text("Retry")) {
                    self.checkForDevice()
                },
                secondaryButton: .default(Text("Quit")) {
                    self.quitAction()
                })
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView(quitAction: {})
    }
}
