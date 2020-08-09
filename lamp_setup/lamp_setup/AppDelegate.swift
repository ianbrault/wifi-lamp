//
//  AppDelegate.swift
//
//  Created by Ian Brault on 6/7/20.
//  Copyright © 2020 Ian Brault. All rights reserved.
//

import Cocoa
import SwiftUI

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

    var window: NSWindow!

    func applicationDidFinishLaunching(_ aNotification: Notification) {
        let contentView = ContentView(quitAction: quit)

        window = NSWindow(
            contentRect: NSRect(x: 0, y: 0, width: 360, height: 540),
            styleMask: [.titled, .closable, .miniaturizable],
            backing: .buffered, defer: false)

        window.center()
        window.setFrameAutosaveName("Main Window")
        window.contentView = NSHostingView(rootView: contentView)
        window.makeKeyAndOrderFront(nil)

        window.backgroundColor = NSColor.white
        window.titlebarAppearsTransparent = true
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }

    func quit() {
        window.close()
    }

    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
        return true
    }
}

