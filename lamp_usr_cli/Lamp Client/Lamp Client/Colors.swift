//
//  Colors.swift
//  Lamp Client
//
//  Created by Ian Brault on 1/9/21.
//

import SwiftUI

let White = Color(.sRGB, red: 255 / 255, green: 255 / 255, blue: 255 / 255)
let Gray = Color(.sRGB, red: 97 / 255, green: 97 / 255, blue: 97 / 255)

let DarkBlue = Color(.sRGB, red: 55 / 255, green: 91 / 255, blue: 108 / 255)
let LightBlue = Color(.sRGB, red: 204 / 255, green: 239 / 255, blue: 255 / 255)

let DarkGreen = Color(.sRGB, red: 55 / 255, green: 108 / 255, blue: 64 / 255)
let LightGreen = Color(.sRGB, red: 204 / 255, green: 255 / 255, blue: 213 / 255)

let DarkPurple = Color(.sRGB, red: 73 / 255, green: 89 / 255, blue: 154 / 255)
let LightPurple = Color(.sRGB, red: 225 / 255, green: 231 / 255, blue: 255 / 255)

func fillColor(_ state: LampState) -> Color {
    switch state {
    case .NotConnected:
        return White
    case .Off:
        return LightBlue
    case .OnWaiting, .OnPaired:
        return LightGreen
    }
}

func strokeColor(_ state: LampState) -> Color {
    switch state {
    case .NotConnected:
        return Gray
    case .Off:
        return DarkBlue
    case .OnWaiting, .OnPaired:
        return DarkGreen
    }
}
