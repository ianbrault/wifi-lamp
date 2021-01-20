//
//  UserSelectView.swift
//  Lamp Client
//
//  Created by Ian Brault on 12/29/20.
//

import SwiftUI

enum User: String, CaseIterable, Identifiable {
    case arni
    case ian

    var id: String { self.rawValue }
}

struct UserSelectView: View {
    let setLampUser: (String) -> Void
    @State private var selectedUser = User.arni

    func onButtonPress() {
        setLampUser(selectedUser.rawValue.capitalized)
    }

    var body: some View {
        ZStack {
            // color should fill the entire screen
            DarkPurple
                .ignoresSafeArea()

            VStack {
                Image("rpi_bare_white")
                    .resizable()
                    .interpolation(.high)
                    .aspectRatio(contentMode: .fill)
                    .frame(width: 32, height: 132)

                Spacer()
                    .frame(height: 32)

                Text("Welcome!")
                    .font(.system(size: 32))
                    .bold()
                    .italic()
                    .foregroundColor(Color.white)

                Spacer()
                    .frame(height: 4)

                Text("Whose lamp is this?")
                    .font(.system(size: 20))
                    .foregroundColor(Color.white)

                Picker("User", selection: $selectedUser) {
                    ForEach(User.allCases) { user in
                        Text(user.rawValue.capitalized)
                            .foregroundColor(Color.white)
                    }
                }

                Spacer()
                    .frame(height: 64)

                Button(action: onButtonPress) {
                    Text("Continue")
                        .font(.system(size: 20))
                        .bold()
                        .foregroundColor(Color.white)
                }
            }
        }
    }
}

struct UserSelectView_Previews: PreviewProvider {
    static var previews: some View {
        UserSelectView(setLampUser: {s in return})
    }
}
