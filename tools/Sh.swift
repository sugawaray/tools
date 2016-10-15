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
        if initbusybox() != 0 {
            print("initbb failed");
        }
        if initsh() != 0 {
            print("initsh failed");
        }
        putsonout("$ ")
    }
    func proc(_ s: String) -> Int {
        let s2 = s + "\n";
        let r = putsonin(s2);
        if r != 0 {
            print("putsonin failed");
        }
        procin();
        putsonout("$ ")
        /*
        let argv: UnsafeMutablePointer<UnsafeMutablePointer<Int8>> = nil
        ls_main(0, argv)
        */
        return 0
    }
}
