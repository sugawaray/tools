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
        if (s.compare(Sh.dummycmd) == ComparisonResult.orderedSame) {
            fproc = true
            cmdname = Sh.dummycmd
            dmfin = false
            dmstr = ""
            DispatchQueue.global(qos: DispatchQoS.QoSClass.utility).async(execute: dummycmdfn)
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
        if (cmdname?.compare(Sh.dummycmd) == ComparisonResult.orderedSame) {
            dmfin = true
            while (dmfin) {
            }
        }
        fproc = false
        cmdname = nil
    }
    func getdummycmdinput() -> String {
        return dmstr
    }
    
    func dummycmdfn() {
        var li: String? = nil
        var ts = timespec()
        ts.tv_sec = 0
        ts.tv_nsec = 10000000
        Dbgout1("dummycmdfn 1", [])
        while (!dmfin) {
            nanosleep(&ts, nil)
            li = readLine()
            if (li != nil) {
                dmstr += li!
            }
            Dbgout1("dummycmdfn 2", [])
        }
        dmfin = false
    }
    
    static let dummycmd = "ShDummyCommand"
    var fproc = false
    var cmdname: String?
    
    var dmstr = ""
    var dmfin = false
}
