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
        if initsh() != 0 {
            print("initsh failed");
        }
    }
    func proc(s: String) -> Int {
        let s2 = s.stringByAppendingString("\n");
        let r = putsonin(s2);
        if r != 0 {
            print("putsonin failed");
        }
        procin();
        /*
        let argv: UnsafeMutablePointer<UnsafeMutablePointer<Int8>> = nil
        ls_main(0, argv)
        */
        return 0
    }
}