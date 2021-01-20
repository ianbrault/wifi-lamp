//
//  ContentView.swift
//  Lamp Client
//
//  Created by Ian Brault on 12/27/20.
//

import SwiftUI

struct Header: View {
    var user: String

    @State private var time = Date()
    private var dateFormatter = DateFormatter()
    private var timeFormatter = DateFormatter()

    let timer = Timer.publish(every: 1, on: .main, in: .common).autoconnect()

    init(user: String) {
        self.user = user

        dateFormatter.locale = Locale(identifier: "en_US")
        dateFormatter.setLocalizedDateFormatFromTemplate("MMMM d, yyyy")

        timeFormatter.locale = Locale(identifier: "en_US")
        timeFormatter.setLocalizedDateFormatFromTemplate("h:mm a")
    }

    var body: some View {
        VStack {
            Text("\(user)'s Lamp")
                .font(.system(size: 24))
                .bold()
                .foregroundColor(DarkPurple)

            Text("\(timeFormatter.string(from: time))")
                .font(.system(size: 36))
                .bold()
                .foregroundColor(DarkPurple)

            Text("\(dateFormatter.string(from: time))")
                .font(.system(size: 20))
                .foregroundColor(DarkPurple)
        }
        .onReceive(timer, perform: {time in
            self.time = Date()
        })
    }
}

struct ContentView: View {
    var user: String

    var body: some View {
        ZStack {
            // color should fill the entire screen
            LightPurple
                .ignoresSafeArea()

            VStack {
                Header(user: user)
            }
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView(user: "Ian")
    }
}
