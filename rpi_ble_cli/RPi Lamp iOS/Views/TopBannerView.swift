//
//  TopBannerView.swift
//  RPi Lamp iOS
//
//  Created by Ian Brault on 3/29/20.
//  Copyright © 2020 Ian Brault. All rights reserved.
//

import SwiftUI

struct TopBanner: View {
    var body: some View {
        GeometryReader {geometry in
            ZStack {
                Image("rpi_bare")
                    .resizable()
                    .aspectRatio(contentMode: .fit)
                    .frame(width: geometry.size.width / 5)
            }
        }
    }
}

struct TopBannerView_Previews: PreviewProvider {
    static var previews: some View {
        TopBanner()
    }
}
