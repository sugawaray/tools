//
//  Sh.swift
//  tools
//
//  Created by Yutaka Sugawara on 2016/09/03.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

import Foundation

class Sh {
    init() {
        if cmdinit() != 0 {
            print("cmdinit failed");
        }
    }
    func proc(s: String) -> Int {
        let argv: UnsafeMutablePointer<UnsafeMutablePointer<Int8>> = nil
        ls_main(0, argv)
        return 0
    }
}