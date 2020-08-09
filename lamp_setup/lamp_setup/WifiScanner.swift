//
//  WifiScanner.swift
//
//  Created by Ian Brault on 7/23/20.
//  Copyright © 2020 Ian Brault. All rights reserved.
//

import Foundation
import CoreWLAN

class WifiScanner {

    var currentInterface: CWInterface
    var interfacesNames: [String] = []

    init?() {
        if let defaultInterface = CWWiFiClient.shared().interface(), let name = defaultInterface.interfaceName {
            self.currentInterface = defaultInterface
            self.interfacesNames.append(name)
        } else {
            return nil
        }
    }

   func discoverNetworks() -> [String] {
        var networks: Set<CWNetwork> = []
        var ssids: Set<String> = []

        do {
            networks = try currentInterface.scanForNetworks(withSSID: nil)
        } catch let error as NSError {
            print("Error: \(error.localizedDescription)")
        }

        // filter SSID from networks
        for network in networks {
            if let ssid = network.ssid {
                ssids.insert(ssid)
            }
        }

        return Array(ssids)
    }
}
