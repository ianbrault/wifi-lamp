//
//  Spinner.swift
//  RPi Lamp iOS
//
//  Created by Ian Brault on 3/29/20.
//  Copyright © 2020 Ian Brault. All rights reserved.
//

import SwiftUI

struct Spinner: UIViewRepresentable {
    typealias UIViewType = UIActivityIndicatorView

    let style: UIActivityIndicatorView.Style

    func makeUIView(context: Context) -> UIViewType {
        let uiView = UIActivityIndicatorView(style: style)
        uiView.startAnimating()
        return uiView
    }

    func updateUIView(_ uiView: UIViewType, context: Context) {

    }

}

struct Spinner_Previews: PreviewProvider {
    static var previews: some View {
        Spinner(style: .medium)
    }
}
