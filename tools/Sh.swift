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
        if (fproc) {
            return -1
        }
        if (s.compare("ShDummyCommand") == ComparisonResult.orderedSame) {
            fproc = true
            cmdname = "ShDummyCommand"
            return 0
        }
        fproc = true
        let s2 = s + "\n";
        let r = putsonin(s2);
        if r != 0 {
            print("putsonin failed");
        }
        procin();
        putsonout("$ ")
        fproc = false
        return 0
    }
    func cmd() -> String? {
        return cmdname
    }
    func kill() {
        fproc = false
        cmdname = nil
    }
    
    var fproc = false
    var cmdname: String?
}
