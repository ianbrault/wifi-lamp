//
//  SpinnerView.swift
//
//  Created by Ian Brault on 6/9/20.
//  Copyright © 2020 Ian Brault. All rights reserved.
//

import SwiftUI

struct SpinnerView: NSViewRepresentable {
    typealias NSViewType = NSProgressIndicator

    func makeNSView(context: Context) -> NSProgressIndicator {
        let view = NSProgressIndicator()
        view.isIndeterminate = true
        view.style = .spinning
        view.startAnimation(nil)
        return view
    }

    func updateNSView(_ nsView: NSProgressIndicator, context: Context) {

    }
}

struct SpinnerView_Previews: PreviewProvider {
    static var previews: some View {
        SpinnerView()
    }
}
