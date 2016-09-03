//
//  Ctl.swift
//  tools
//
//  Created by Yutaka Sugawara on 2016/08/31.
//  Copyright © 2016年 Yutaka Sugawara. All rights reserved.
//

import Foundation

enum Cmdid : Int {
    case Nop    = 1,
    NopFail,
    Savefile,
    Invalid
}

enum Ctlv : Int {
    case StInvalid  = -0x10000
}

struct Cmd {
    var id: Int = Cmdid.Invalid.rawValue
    var astr: String?
}

class Ctl {
    
    init() {
        cmd = nil
        cst = Ctlv.StInvalid.rawValue
    }

    func procc(carg: Cmd) -> Int {
        if cmd != nil {
            return -1
        } else {
            cmd = carg
            cst = Ctlv.StInvalid.rawValue
            return 0
        }
    }
    
    func proc() {
        if cmd == nil {
            return;
        } else if cmd!.id != Cmdid.Invalid.rawValue {
            switch cmd!.id {
            case Cmdid.Nop.rawValue:
                cst = 0
                break
            case Cmdid.NopFail.rawValue:
                cst = -1
                break
            case Cmdid.Savefile.rawValue:
                cst = savefile(cmd!.astr!)
                break
            default:
                cst = Ctlv.StInvalid.rawValue
            }
            cmd = nil
        }
    }
    
    func status() -> (Int, Int) {
        if cst != Ctlv.StInvalid.rawValue {
            return (0, cst)
        } else {
            return (-1, 0)
        }
    }
    
    func savefile(path: String) -> Int {
        return 0
    }
    
    var cmd: Cmd?
    var cst: Int
}