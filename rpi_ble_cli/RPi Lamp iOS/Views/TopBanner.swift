//
//  TopBannerView.swift
//  RPi Lamp iOS
//
//  Created by Ian Brault on 3/29/20.
//  Copyright © 2020 Ian Brault. All rights reserved.
//

import SwiftUI

struct TopBanner: View {

    private let pad: CGFloat = 20

    private let strokeColor = Color(
        red:   Double(0x49) / 255.0,
        green: Double(0x59) / 255.0,
        blue:  Double(0x9a) / 255.0)
    
    private func logoWidth(_ geometry: GeometryProxy) -> CGFloat {
        return geometry.size.width / 5
    }

    private func strokeY(_ geometry: GeometryProxy) -> CGFloat {
        return (geometry.size.height / 2) + 8
    }

    private func leftStrokeStart(_ geometry: GeometryProxy) -> CGPoint {
        return CGPoint(x: self.pad, y: self.strokeY(geometry))
    }

    private func leftStrokeEnd(_ geometry: GeometryProxy) -> CGPoint {
        let center = geometry.size.width / 2
        let logoStart = center - (self.logoWidth(geometry) / 2)

        return CGPoint(x: logoStart - self.pad, y: self.strokeY(geometry))
    }

    private func rightStrokeStart(_ geometry: GeometryProxy) -> CGPoint {
        let center = geometry.size.width / 2
        let logoEnd = center + (self.logoWidth(geometry) / 2)

        return CGPoint(x: logoEnd + self.pad, y: self.strokeY(geometry))
    }

    private func rightStrokeEnd(_ geometry: GeometryProxy) -> CGPoint {
        return CGPoint(
            x: geometry.size.width - self.pad, y: self.strokeY(geometry))
    }

    var body: some View {
        GeometryReader {[self](geometry) in
            ZStack {
                Image("rpi_bare")
                    .resizable()
                    .aspectRatio(contentMode: .fit)
                    .frame(width: self.logoWidth(geometry))
                
                Path { path in
                    path.move(to: self.leftStrokeStart(geometry))
                    path.addLine(to: self.leftStrokeEnd(geometry))

                    path.move(to: self.rightStrokeStart(geometry))
                    path.addLine(to: self.rightStrokeEnd(geometry))
                }
                .stroke(
                    self.strokeColor,
                    style: StrokeStyle(lineWidth: 4, lineCap: .round))
            }
        }
    }

}

struct TopBanner_Previews: PreviewProvider {
    static var previews: some View {
        TopBanner()
    }
}
