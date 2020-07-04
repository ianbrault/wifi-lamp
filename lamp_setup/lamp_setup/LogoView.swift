//
//  LogoView.swift
//
//  Created by Ian Brault on 6/9/20.
//  Copyright © 2020 Ian Brault. All rights reserved.
//

import SwiftUI

struct LogoView: View {
    var body: some View {
        Image("rpi_stroke")
            .resizable()
            .aspectRatio(contentMode: .fit)
            .frame(height: 100)
            .shadow(
                color: Color(.sRGBLinear, white: 0, opacity: 0.20),
                radius: 4.0, x: -2, y: 2)
    }
}

struct LogoView_Previews: PreviewProvider {
    static var previews: some View {
        LogoView()
    }
}
